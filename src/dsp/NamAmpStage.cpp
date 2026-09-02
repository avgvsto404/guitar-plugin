#include "NamAmpStage.h"
#include "BinaryData.h"

#include <algorithm>

#include "dsp.h"
#include "get_dsp.h"
#include "slimmable.h"
#include "json.hpp"

namespace
{
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
    catch (const std::exception&)
    {
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

    for (juce::uint32 ch = 0; ch < spec.numChannels; ++ch)
    {
        auto model = createModel();
        if (model != nullptr)
            model->Reset (sampleRate, maxBlockSize);

        models.push_back (std::move (model));
        scratchBuffers.emplace_back (static_cast<size_t> (maxBlockSize), 0.0f);
    }
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
    }
}
