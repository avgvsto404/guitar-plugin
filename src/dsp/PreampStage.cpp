#include "PreampStage.h"

void PreampStage::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    gainAmount.reset (spec.sampleRate, 0.02);
    preFilter.prepare (spec);
    updatePreFilter();
}

void PreampStage::reset()
{
    preFilter.reset();
}

void PreampStage::setMode (AmpMode newMode)
{
    mode = newMode;
    updatePreFilter();
}

void PreampStage::setGain (float gain01)
{
    gainAmount.setTargetValue (gain01);
}

void PreampStage::updatePreFilter()
{
    // Clean: headroom alto, corte suave. Rhythm/Lead: corte mais alto (voicing "CALI_HI")
    // para manter graves "tight" em palm mute, conforme research.md secao 1.
    const float cutoffHz = mode == AmpMode::Clean ? 60.0f : 110.0f;
    *preFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, cutoffHz);
}

void PreampStage::process (juce::dsp::AudioBlock<float>& block)
{
    juce::dsp::ProcessContextReplacing<float> context (block);
    preFilter.process (context);

    const auto numSamples = block.getNumSamples();
    const auto numChannels = block.getNumChannels();

    // Numero de estagios em cascata e multiplicador de drive por modo -- ver tabela
    // "Voicing Target" em spec.md: Clean é headroom alto (praticamente linear), Rhythm/Lead
    // usam ganho em cascata (voicing "CALI_HI") com presenca cortante extra no Lead
    // (voicing "BRIT_HI").
    const int cascadedStages = mode == AmpMode::Clean ? 1 : (mode == AmpMode::Rhythm ? 2 : 3);
    const float driveMultiplier = mode == AmpMode::Clean ? 3.0f : (mode == AmpMode::Rhythm ? 12.0f : 18.0f);

    for (size_t i = 0; i < numSamples; ++i)
    {
        const auto gain = gainAmount.getNextValue() * driveMultiplier;

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* data = block.getChannelPointer (ch);
            auto sample = data[i] * gain;

            for (int stage = 0; stage < cascadedStages; ++stage)
                sample = std::tanh (sample);

            data[i] = sample;
        }
    }
}
