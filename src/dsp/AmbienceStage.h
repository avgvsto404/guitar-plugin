#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>

// Chorus + slapback delay + reverb curta, leves, fixos (sem parametro de UI por ora) --
// usado SO no modo Clean (AmpChain.cpp so chama process() quando currentMode == Clean, pra
// nao gastar CPU nem colorir Rhythm/Lead).
class AmbienceStage
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void process (juce::dsp::AudioBlock<float>& block);

private:
    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float> delayLine { 96000 };
    static constexpr float delayFeedback = 0.16f;
    static constexpr float delayMix = 0.14f;

    juce::Reverb reverb;
};
