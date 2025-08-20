cmake_minimum_required(VERSION 3.15)

project(AutoTunePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory (assumes JUCE is in the project root or as a submodule)
# If JUCE is not present, it will be downloaded
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.9
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Find Rubber Band Library
find_path(RUBBERBAND_INCLUDE_DIR rubberband/RubberBandStretcher.h)
find_library(RUBBERBAND_LIBRARY NAMES rubberband)

if(NOT RUBBERBAND_INCLUDE_DIR OR NOT RUBBERBAND_LIBRARY)
    message(STATUS "Rubber Band Library not found, using basic implementation")
    set(USE_RUBBERBAND OFF)
else()
    message(STATUS "Rubber Band Library found: ${RUBBERBAND_LIBRARY}")
    set(USE_RUBBERBAND ON)
endif()

# Plugin sources
set(PLUGIN_SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PitchCorrectionEngine.cpp
    Source/Parameters.cpp
    Source/Utils.cpp
    Source/LookAndFeel.cpp
    Source/PresetManager.cpp
    Source/ModeSelector.cpp
    Source/AIModelLoader.cpp
)

# Create the plugin target
juce_add_plugin(AutoTunePlugin
    COMPANY_NAME "ProAudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Paud
    PLUGIN_CODE Auto
    FORMATS VST3
    PRODUCT_NAME "ProAutoTune"
)

# Add sources to the plugin
target_sources(AutoTunePlugin PRIVATE ${PLUGIN_SOURCES})

# Compiler-specific options
target_compile_definitions(AutoTunePlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_USE_XRANDR=0
        JUCE_USE_XINERAMA=0
        JUCE_USE_XCURSOR=0
        JUCE_PLUGINHOST_VST3=0
        JUCE_PLUGINHOST_AU=0
)

if(USE_RUBBERBAND)
    target_compile_definitions(AutoTunePlugin PRIVATE USE_RUBBERBAND=1)
    target_include_directories(AutoTunePlugin PRIVATE ${RUBBERBAND_INCLUDE_DIR})
    target_link_libraries(AutoTunePlugin PRIVATE ${RUBBERBAND_LIBRARY})
endif()

# Link required JUCE modules
target_link_libraries(AutoTunePlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Additional compiler flags for optimization
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(AutoTunePlugin PRIVATE -O3 -ffast-math)
endif()
