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
    # So tem efeito no Windows (JUCEUtils.cmake so le isso dentro de um bloco elseif(WIN32)) --
    # sem isso juce_gui_extra.cpp nao acha WebView2.h ao compilar com JUCE_USE_WIN_WEBVIEW2=1.
    # Precisa do pacote NuGet Microsoft.Web.WebView2 disponivel via JUCE_WEBVIEW2_PACKAGE_LOCATION
    # (ver .github/workflows/ci.yml e README.md para o passo manual local no Windows).
    NEEDS_WEBVIEW2 TRUE
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
        src/dsp/AmbienceStage.cpp
)

target_compile_definitions(NikolayevskDarkDsp
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        # No Windows, o backend default do WebBrowserComponent e o Internet Explorer, que nao
        # suporta withResourceProvider (so compila -- e so existe -- com o backend WebView2
        # habilitado). Sem isso o build do Windows falha com "'withResourceProvider': is not a
        # member of 'juce::WebBrowserComponent::Options'".
        JUCE_USE_WIN_WEBVIEW2=1
        # CONFIRMADO EM MAQUINA REAL (log de PluginEditor.cpp): com carregamento dinamico (sem
        # esta flag), o runtime faz LoadLibraryA("WebView2Loader.dll") e nao acha o arquivo (a
        # gente nunca empacotou essa DLL do lado do binario) -- a criacao do WebView2 falha
        # silenciosamente e o JUCE cai de volta pro Internet Explorer (log mostrou literalmente
        # "res://ieframe.dll/navcancl.htm"). Linkagem estatica usa direto o
        # WebView2LoaderStatic.lib que o NEEDS_WEBVIEW2 abaixo ja linka -- sem precisar
        # distribuir DLL nenhuma junto do plugin.
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
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
