#include "CabinetLoader.h"
#include "BinaryData.h"
#include <juce_audio_formats/juce_audio_formats.h>

void CabinetLoader::prepare (const juce::dsp::ProcessSpec& spec)
{
    currentSpec = spec;
    convolution.prepare (spec);
    loadDefaultIr();
}

void CabinetLoader::reset()
{
    convolution.reset();
}

void CabinetLoader::loadDefaultIr()
{
    convolution.loadImpulseResponse (
        BinaryData::default_cab_PLACEHOLDER_wav,
        static_cast<size_t> (BinaryData::default_cab_PLACEHOLDER_wavSize),
        juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::yes,
        0,
        juce::dsp::Convolution::Normalise::yes);

    usingDefaultIr = true;
    currentIrName = "default_cab_PLACEHOLDER (embutido -- ver T090 em tasks.md)";
}

bool CabinetLoader::loadIrFromFile (const juce::File& file)
{
    if (! file.existsAsFile())
        return false;

    // juce::dsp::Convolution nao expoe um retorno de sucesso direto em todas as versoes --
    // validamos o arquivo com um AudioFormatManager antes de tentar carregar, para poder
    // aplicar o fallback do edge case da spec (IR invalido -> mantem o IR anterior).
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));

    if (reader == nullptr || reader->numChannels == 0 || reader->sampleRate <= 0.0)
        return false;

    convolution.loadImpulseResponse (
        file,
        juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::yes,
        0,
        juce::dsp::Convolution::Normalise::yes);

    usingDefaultIr = false;
    currentIrName = file.getFileName();
    return true;
}

void CabinetLoader::process (juce::dsp::AudioBlock<float>& block)
{
    juce::dsp::ProcessContextReplacing<float> context (block);
    convolution.process (context);
}
