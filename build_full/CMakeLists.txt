cmake_minimum_required(VERSION 3.15)

project(AutoTunePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")

message(STATUS "Building AutoTune Plugin with FULL functionality for macOS")

# Add JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    URL https://github.com/juce-framework/JUCE/archive/refs/tags/7.0.9.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_HELPER_TOOLS OFF CACHE BOOL "" FORCE)
set(JUCE_COPY_PLUGIN_AFTER_BUILD ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(JUCE)

# Find installed dependencies
find_package(PkgConfig REQUIRED)

# Find Eigen3
find_path(EIGEN3_INCLUDE_DIR Eigen/Dense
    PATHS 
        /usr/local/include/eigen3 
        /usr/local/opt/eigen/include/eigen3
        /usr/local/Cellar/eigen/*/include/eigen3
    REQUIRED
)

# Find ONNX Runtime
find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
    PATHS 
        /usr/local/opt/onnxruntime/include
        /usr/local/include
        /usr/local/Cellar/onnxruntime/*/include
    REQUIRED
)
find_library(ONNXRUNTIME_LIB onnxruntime
    NAMES onnxruntime libonnxruntime
    PATHS 
        /usr/local/opt/onnxruntime/lib
        /usr/local/lib
        /usr/local/Cellar/onnxruntime/*/lib
    REQUIRED
)

# Find Rubber Band
find_path(RUBBERBAND_INCLUDE_DIR rubberband/RubberBandStretcher.h
    PATHS 
        /usr/local/opt/rubberband/include
        /usr/local/include
        /usr/local/Cellar/rubberband/*/include
    REQUIRED
)
find_library(RUBBERBAND_LIB rubberband
    NAMES rubberband librubberband
    PATHS 
        /usr/local/opt/rubberband/lib
        /usr/local/lib
        /usr/local/Cellar/rubberband/*/lib
    REQUIRED
)

message(STATUS "Found Eigen3: ${EIGEN3_INCLUDE_DIR}")
message(STATUS "Found ONNX Runtime: ${ONNXRUNTIME_INCLUDE_DIR}")
message(STATUS "Found Rubber Band: ${RUBBERBAND_INCLUDE_DIR}")

# Define ALL source files
set(SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PitchCorrectionEngine.cpp
    Source/Parameters.cpp
    Source/PresetManager.cpp
    Source/ModeSelector.cpp
    Source/Utils.cpp
    Source/LookAndFeel.cpp
    Source/AIModelLoader.cpp
)

# Create the plugin with ALL formats
juce_add_plugin(AutoTunePlugin
    COMPANY_NAME "ReplicaStudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE "Repl"
    PLUGIN_CODE "ATun"
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "AutoTune Plugin"
)

# Add source files
target_sources(AutoTunePlugin PRIVATE ${SOURCES})

# Full compiler definitions with all features enabled
target_compile_definitions(AutoTunePlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_STRICT_REFCOUNTEDPTR=1
        USE_ONNX=1
        USE_CREPE=1
        USE_DDSP=1
        USE_EIGEN=1
        USE_RUBBERBAND=1
    PRIVATE
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AutoTunePlugin,JUCE_PLUGIN_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AutoTunePlugin,JUCE_PLUGIN_VERSION>"
)

# Include directories for all libraries
target_include_directories(AutoTunePlugin PRIVATE 
    Source
    ${EIGEN3_INCLUDE_DIR}
    ${ONNXRUNTIME_INCLUDE_DIR}
    ${RUBBERBAND_INCLUDE_DIR}
)

# Link ALL libraries
target_link_libraries(AutoTunePlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_core
        ${ONNXRUNTIME_LIB}
        ${RUBBERBAND_LIB}
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

message(STATUS "AutoTune Plugin configured with FULL macOS functionality")