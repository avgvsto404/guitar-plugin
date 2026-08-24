#include "PowerAmpSaturation.h"

void PowerAmpSaturation::prepare (const juce::dsp::ProcessSpec&)
{
}

void PowerAmpSaturation::reset()
{
}

void PowerAmpSaturation::setMode (AmpMode newMode)
{
    mode = newMode;
}

void PowerAmpSaturation::process (juce::dsp::AudioBlock<float>& block)
{
    // Sag sutil: atan satura mais liso que tanh, adequado para um segundo estagio pos tone
    // stack. Amount fixo por modo -- Clean quase nao satura, Lead satura mais (mais sustain).
    const float amount = mode == AmpMode::Clean ? 1.2f : (mode == AmpMode::Rhythm ? 2.0f : 3.0f);
    const float normalisation = std::atan (amount);

    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
    {
        auto* data = block.getChannelPointer (ch);
        for (size_t i = 0; i < block.getNumSamples(); ++i)
            data[i] = std::atan (data[i] * amount) / normalisation;
    }
}
