#include "PluginEditor.h"
#include "BinaryData.h"

PluginEditor::PluginEditor (PluginProcessor& processorRef)
    : juce::AudioProcessorEditor (processorRef), pluginProcessor (processorRef)
{
    webView = std::make_unique<juce::WebBrowserComponent> (
        juce::WebBrowserComponent::Options {}
            .withNativeIntegrationEnabled()
            .withResourceProvider ([this] (const juce::String& url) { return provideResource (url); })
            .withNativeFunction ("setParam",
                [this] (const juce::Array<juce::var>& args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                {
                    completion (handleSetParam (args));
                })
            .withNativeFunction ("getState",
                [this] (const juce::Array<juce::var>&, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                {
                    completion (buildStateVar());
                })
            .withNativeFunction ("loadIR",
                [this] (const juce::Array<juce::var>& args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                {
                    completion (handleLoadIr (args));
                })
            .withNativeFunction ("browseForIR",
                [this] (const juce::Array<juce::var>&, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                {
                    fileChooser = std::make_unique<juce::FileChooser> (
                        "Selecione um Impulse Response (.wav)", juce::File {}, "*.wav");

                    fileChooser->launchAsync (
                        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, completion] (const juce::FileChooser& fc)
                        {
                            const auto file = fc.getResult();
                            if (file != juce::File {})
                                pluginProcessor.loadIrFromFile (file); // false = mantem IR anterior (edge case da spec)

                            completion (buildIrStateVar());
                        });
                }));

    addAndMakeVisible (*webView);
    // getResourceProviderRoot() e o unico jeito correto de apontar o WebBrowserComponent para
    // o resourceProvider registrado acima -- uma URL arbitraria (mesmo com dominio .invalid)
    // tenta resolver DNS de verdade e falha ("A server with the specified hostname could not
    // be found"). Confirmado lendo o header real: modules/juce_gui_extra/misc/
    // juce_WebBrowserComponent.h em build/_deps/juce-src apos o primeiro build.
    webView->goToURL (juce::WebBrowserComponent::getResourceProviderRoot());

    setResizable (true, true);
    setSize (760, 420);
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::resized()
{
    if (webView != nullptr)
        webView->setBounds (getLocalBounds());
}

std::optional<juce::WebBrowserComponent::Resource> PluginEditor::provideResource (const juce::String&)
{
    // App React e uma SPA de arquivo unico (ui/vite.config.ts usa vite-plugin-singlefile) --
    // qualquer URL pedida recebe o mesmo index.html embutido como BinaryData.
    return juce::WebBrowserComponent::Resource {
        std::vector<std::byte> (
            reinterpret_cast<const std::byte*> (BinaryData::index_html),
            reinterpret_cast<const std::byte*> (BinaryData::index_html) + BinaryData::index_htmlSize),
        "text/html"
    };
}

juce::var PluginEditor::handleSetParam (const juce::Array<juce::var>& args)
{
    if (args.size() < 2)
        return false;

    const auto id = args[0].toString();

    if (id == ParamId::ampMode)
    {
        const auto modeStr = args[1].toString();
        const int index = modeStr == "rhythm" ? 1 : (modeStr == "lead" ? 2 : 0);
        if (auto* param = pluginProcessor.apvts.getParameter (ParamId::ampMode))
            param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (index)));
        return true;
    }

    if (auto* param = pluginProcessor.apvts.getParameter (id))
    {
        param->setValueNotifyingHost (static_cast<float> (static_cast<double> (args[1])));
        return true;
    }

    return false;
}

juce::var PluginEditor::handleLoadIr (const juce::Array<juce::var>& args)
{
    if (args.isEmpty())
        return buildIrStateVar();

    const juce::File file (args[0].toString());
    pluginProcessor.loadIrFromFile (file); // false = mantem IR anterior (edge case da spec)
    return buildIrStateVar();
}

juce::var PluginEditor::ampModeToVar (AmpMode mode)
{
    switch (mode)
    {
        case AmpMode::Clean:  return "clean";
        case AmpMode::Rhythm: return "rhythm";
        case AmpMode::Lead:   return "lead";
    }
    return "clean";
}

juce::var PluginEditor::buildStateVar() const
{
    auto* obj = new juce::DynamicObject();

    const auto modeIndex = static_cast<int> (pluginProcessor.apvts.getRawParameterValue (ParamId::ampMode)->load());
    obj->setProperty ("ampMode", ampModeToVar (static_cast<AmpMode> (modeIndex)));
    obj->setProperty ("gain", pluginProcessor.apvts.getRawParameterValue (ParamId::gain)->load());
    obj->setProperty ("bass", pluginProcessor.apvts.getRawParameterValue (ParamId::bass)->load());
    obj->setProperty ("mid", pluginProcessor.apvts.getRawParameterValue (ParamId::mid)->load());
    obj->setProperty ("treble", pluginProcessor.apvts.getRawParameterValue (ParamId::treble)->load());
    obj->setProperty ("presence", pluginProcessor.apvts.getRawParameterValue (ParamId::presence)->load());
    obj->setProperty ("master", pluginProcessor.apvts.getRawParameterValue (ParamId::master)->load());
    obj->setProperty ("tsDrive", pluginProcessor.apvts.getRawParameterValue (ParamId::tsDrive)->load());
    obj->setProperty ("tsTone", pluginProcessor.apvts.getRawParameterValue (ParamId::tsTone)->load());
    obj->setProperty ("tsLevel", pluginProcessor.apvts.getRawParameterValue (ParamId::tsLevel)->load());
    obj->setProperty ("tsBypass", pluginProcessor.apvts.getRawParameterValue (ParamId::tsBypass)->load() > 0.5f);

    return juce::var (obj);
}

juce::var PluginEditor::buildIrStateVar() const
{
    auto* obj = new juce::DynamicObject();
    obj->setProperty ("loaded", true);
    obj->setProperty ("name", pluginProcessor.getCurrentIrName());
    obj->setProperty ("isDefault", pluginProcessor.isUsingDefaultIr());
    return juce::var (obj);
}
