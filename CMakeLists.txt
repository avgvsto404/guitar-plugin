cmake_minimum_required(VERSION 3.22)

project(NikolayevskDarkDsp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# NOTA: fixe esta tag na versao estavel mais recente do JUCE 8.x disponivel no momento do
# build (T001) -- este valor e um ponto de partida, nao uma garantia de compatibilidade com
# o restante do CMakeLists (em especial a API de WebBrowserComponent usada em
# src/plugin/PluginEditor.cpp, que evoluiu entre releases pontuais do JUCE 8. Ver
# specs/001-nikolayevsk-dark-dsp/research.md secao 4).
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# ui/dist/index.html (React/TS via vite-plugin-singlefile, um unico HTML com JS/CSS inline)
# precisa ser buildado ANTES de configurar este CMakeLists -- roda "npm install && npm run
# build" dentro de ui/. Isso e feito como passo explicito (README.md / .github/workflows/
# ci.yml), nao como custom command do CMake: um add_custom_command chamando npm dentro do
# build MSVC multi-config se mostrou nao-confiavel no CI do Windows (o rule reportava sucesso
# sem o arquivo de saida realmente existir, quebrando a geracao do BinaryData). Falhar aqui
# com uma mensagem clara e mais simples e mais confiavel do que depender dessa integracao.
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/ui/dist/index.html)
    message(FATAL_ERROR
        "ui/dist/index.html nao encontrado. Rode antes de configurar:\n"
        "  cd ui && npm install && npm run build")
endif()

# Embutido aqui como BinaryData::index_html -- o plugin final e autocontido, sem depender de
# arquivos soltos ao lado do binario (ver PluginEditor.cpp).
juce_add_binary_data(DarkDspBinaryData
    SOURCES
        ${CMAKE_SOURCE_DIR}/assets/ir/default_cab_PLACEHOLDER.wav
        ${CMAKE_SOURCE_DIR}/ui/dist/index.html
    # NOTA (T090): substitua o IR acima por um IR de cabinet real antes de qualquer release.
)

juce_add_plugin(NikolayevskDarkDsp
    COMPANY_NAME "Nikolayevsk"
    PRODUCT_NAME "Nikolayevsk DARK DSP"
    PLUGIN_MANUFACTURER_CODE Nklv
    PLUGIN_CODE Ndd1
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Fx Distortion
)

target_sources(NikolayevskDarkDsp
    PRIVATE
        src/plugin/PluginProcessor.cpp
        src/plugin/PluginEditor.cpp
        src/dsp/AmpChain.cpp
        src/dsp/TubeScreamerStage.cpp
        src/dsp/PreampStage.cpp
        src/dsp/ToneStack.cpp
        src/dsp/PowerAmpSaturation.cpp
        src/dsp/CabinetLoader.cpp
)

target_compile_definitions(NikolayevskDarkDsp
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        # No Windows, o backend default do WebBrowserComponent e o Internet Explorer, que nao
        # suporta withResourceProvider (so compila -- e so existe -- com o backend WebView2
        # habilitado). Sem isso o build do Windows falha com "'withResourceProvider': is not a
        # member of 'juce::WebBrowserComponent::Options'". Carregamento dinamico (sem
        # JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING), entao nao precisa do pacote NuGet do
        # WebView2 pra compilar -- so precisa do WebView2Loader.dll existir do lado do binario
        # em tempo de execucao no Windows real (ainda nao validado, ver PluginEditor.cpp).
        JUCE_USE_WIN_WEBVIEW2=1
)

target_link_libraries(NikolayevskDarkDsp
    PRIVATE
        DarkDspBinaryData
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

enable_testing()
add_subdirectory(tests)
