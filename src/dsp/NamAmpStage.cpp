#include "NamAmpStage.h"
#include "BinaryData.h"
#include "../util/DiagnosticLog.h"

#include <algorithm>

#include "dsp.h"
#include "get_dsp.h"
#include "slimmable.h"
#include "json.hpp"

namespace
{
// Diagnostico temporario -- createModel() cai pra passthrough silenciosamente se o parse do
// .nam falhar, e sem log nenhuma maquina consegue distinguir "modelo carregou e soa fraco" de
// "modelo nunca carregou". Ver relato "som capenga em Rhythm/Lead, como se nao tivesse
// processado nada".
std::unique_ptr<nam::DSP> createModel()
{
    try
    {
        const auto json = nlohmann::json::parse (BinaryData::nikolayevsk_nam,
                                                   BinaryData::nikolayevsk_nam + BinaryData::nikolayevsk_namSize);
        auto model = nam::get_dsp (json);

        // assets/nam/nikolayevsk.nam e um SlimmableContainer com 2 sub-modelos (leve/full
        // quality) -- por padrao a lib ativa o mais leve (ContainerModel::_active_index{0});
        // forcamos sempre o de melhor qualidade.
        if (auto* slimmable = dynamic_cast<nam::SlimmableModel*> (model.get()))
            slimmable->SetSlimmableSize (1.0);

        return model;
    }
    catch (const std::exception& e)
    {
        logDiagnosticEvent ("NAM", juce::String ("createModel falhou -- ") + e.what());
        return nullptr;
    }
}
}

NamAmpStage::NamAmpStage() = default;
NamAmpStage::~NamAmpStage() = default;

void NamAmpStage::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    maxBlockSize = static_cast<int> (spec.maximumBlockSize);

    models.clear();
    scratchBuffers.clear();
    outputGainLinear = 1.0f;

    bool outputGainComputed = false;
    int loadedCount = 0;

    for (juce::uint32 ch = 0; ch < spec.numChannels; ++ch)
    {
        auto model = createModel();
        if (model != nullptr)
        {
            ++loadedCount;
            model->Reset (sampleRate, maxBlockSize);

            // So precisa calcular uma vez -- todos os canais carregam o mesmo asset, logo a
            // mesma loudness de treino.
            if (! outputGainComputed && model->HasLoudness())
            {
                constexpr double targetLoudnessDb = -18.0;
                const auto gainDb = static_cast<float> (targetLoudnessDb - model->GetLoudness());
                outputGainLinear = juce::Decibels::decibelsToGain (gainDb);
                outputGainComputed = true;
            }
        }

        models.push_back (std::move (model));
        scratchBuffers.emplace_back (static_cast<size_t> (maxBlockSize), 0.0f);
    }

    // Resumo de uma linha so, cobrindo os dois jeitos de "deu errado": nenhum canal carregou
    // (loadedCount == 0, ve o log de createModel acima pro motivo), ou carregou mas sem
    // metadata de loudness (outputGainComputed == false -> compensacao fica em 1.0x/0dB, som
    // sai baixo igual ao bug original).
    logDiagnosticEvent ("NAM", "prepare -- canais=" + juce::String (spec.numChannels)
                                   + " carregados=" + juce::String (loadedCount)
                                   + " sampleRate=" + juce::String (sampleRate)
                                   + " loudnessCompensada=" + (outputGainComputed ? "sim" : "nao")
                                   + " outputGainDb=" + juce::String (juce::Decibels::gainToDecibels (outputGainLinear), 2));
}

void NamAmpStage::reset()
{
    for (auto& model : models)
        if (model != nullptr)
            model->Reset (sampleRate, maxBlockSize);
}

void NamAmpStage::process (juce::dsp::AudioBlock<float>& block)
{
    const auto numSamples = block.getNumSamples();
    const auto numChannels = juce::jmin (block.getNumChannels(), models.size());

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto& model = models[ch];
        if (model == nullptr)
            continue;

        auto* data = block.getChannelPointer (ch);
        auto& scratch = scratchBuffers[ch];

        std::copy (data, data + numSamples, scratch.data());

        float* inputPtr = scratch.data();
        float* outputPtr = data;
        model->process (&inputPtr, &outputPtr, static_cast<int> (numSamples));

        for (int i = 0; i < static_cast<int> (numSamples); ++i)
            data[i] *= outputGainLinear;
    }
}
