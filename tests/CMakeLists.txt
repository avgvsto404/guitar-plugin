include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Catch2)

add_executable(DarkDspTests
    dsp/TubeScreamerStageTests.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/TubeScreamerStage.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/PreampStage.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/ToneStack.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/PowerAmpSaturation.cpp
)

target_include_directories(DarkDspTests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/dsp)

# NOTA: um executavel JUCE fora de juce_add_plugin/juce_add_gui_app normalmente precisa de
# JUCE_STANDALONE_APPLICATION=1 para linkar sem um main() gerado pelo framework (o Catch2
# fornece o main via Catch2::Catch2WithMain). Se o link falhar por simbolo faltante
# relacionado a JUCEApplicationBase, esse e o primeiro lugar a ajustar.
target_compile_definitions(DarkDspTests PRIVATE JUCE_STANDALONE_APPLICATION=1)

target_link_libraries(DarkDspTests
    PRIVATE
        Catch2::Catch2WithMain
        juce::juce_dsp
        juce::juce_audio_processors
        juce::juce_recommended_config_flags
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(DarkDspTests)
