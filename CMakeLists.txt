cmake_minimum_required(VERSION 3.15)

project(AutoTunePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")

message(STATUS "Building AutoTune Plugin with available macOS functionality")

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

# Find installed dependencies - only what's actually available
find_package(PkgConfig REQUIRED)

# Find Eigen3 - comprehensive search
find_path(EIGEN3_INCLUDE_DIR Eigen/Dense
    PATHS 
        /usr/local/include/eigen3 
        /usr/local/opt/eigen/include/eigen3
        /usr/local/Cellar/eigen/*/include/eigen3
        /opt/homebrew/include/eigen3
        /opt/homebrew/opt/eigen/include/eigen3
        /opt/homebrew/Cellar/eigen/*/include/eigen3
        /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Headers
)

# Find ONNX Runtime - using correct Homebrew paths
find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
    PATHS 
        /usr/local/opt/onnxruntime/include
        /usr/local/include
        /usr/local/Cellar/onnxruntime/*/include
        /opt/homebrew/opt/onnxruntime/include
        /opt/homebrew/include
        /opt/homebrew/Cellar/onnxruntime/*/include
    NO_DEFAULT_PATH
)
find_library(ONNXRUNTIME_LIB onnxruntime
    NAMES onnxruntime libonnxruntime
    PATHS 
        /usr/local/opt/onnxruntime/lib
        /usr/local/lib
        /usr/local/Cellar/onnxruntime/*/lib
        /opt/homebrew/opt/onnxruntime/lib
        /opt/homebrew/lib
        /opt/homebrew/Cellar/onnxruntime/*/lib
    NO_DEFAULT_PATH
)

# Find Rubber Band - using discovered paths
find_path(RUBBERBAND_INCLUDE_DIR rubberband/RubberBandStretcher.h
    PATHS 
        /usr/local/Cellar/rubberband/4.0.0/include
        /usr/local/include
        /usr/local/opt/rubberband/include
        /usr/local/Cellar/rubberband/*/include
        /opt/homebrew/include
        /opt/homebrew/opt/rubberband/include
        /opt/homebrew/Cellar/rubberband/*/include
)
find_library(RUBBERBAND_LIB rubberband
    NAMES rubberband librubberband
    PATHS 
        /usr/local/Cellar/rubberband/4.0.0/lib
        /usr/local/lib
        /usr/local/opt/rubberband/lib
        /usr/local/Cellar/rubberband/*/lib
        /opt/homebrew/lib
        /opt/homebrew/opt/rubberband/lib
        /opt/homebrew/Cellar/rubberband/*/lib
)

# Report what was found
if(EIGEN3_INCLUDE_DIR)
    message(STATUS "Found Eigen3: ${EIGEN3_INCLUDE_DIR}")
    set(HAS_EIGEN3 TRUE)
else()
    message(WARNING "Eigen3 not found - mathematical operations will be limited")
    set(HAS_EIGEN3 FALSE)
endif()

if(ONNXRUNTIME_INCLUDE_DIR AND ONNXRUNTIME_LIB)
    message(STATUS "Found ONNX Runtime: ${ONNXRUNTIME_INCLUDE_DIR}")
    message(STATUS "ONNX Runtime library: ${ONNXRUNTIME_LIB}")
    set(HAS_ONNX TRUE)
else()
    message(WARNING "ONNX Runtime not found - AI features will be disabled")
    set(HAS_ONNX FALSE)
endif()

if(RUBBERBAND_INCLUDE_DIR AND RUBBERBAND_LIB)
    message(STATUS "Found Rubber Band: ${RUBBERBAND_INCLUDE_DIR}")
    message(STATUS "Rubber Band library: ${RUBBERBAND_LIB}")
    set(HAS_RUBBERBAND TRUE)
else()
    message(WARNING "Rubber Band not found - pitch shifting will use basic algorithms")
    set(HAS_RUBBERBAND FALSE)
endif()

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

# Compiler definitions based on available libraries
target_compile_definitions(AutoTunePlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_STRICT_REFCOUNTEDPTR=1
        USE_EIGEN=$<BOOL:${HAS_EIGEN3}>
        USE_RUBBERBAND=$<BOOL:${HAS_RUBBERBAND}>
        USE_ONNX=$<BOOL:${HAS_ONNX}>
        USE_CREPE=$<BOOL:${HAS_ONNX}>
        USE_DDSP=$<BOOL:${HAS_ONNX}>
    PRIVATE
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AutoTunePlugin,JUCE_PLUGIN_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AutoTunePlugin,JUCE_PLUGIN_VERSION>"
)

# Include directories for available libraries
target_include_directories(AutoTunePlugin PRIVATE Source)

if(HAS_EIGEN3)
    target_include_directories(AutoTunePlugin PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()

if(HAS_ONNX)
    target_include_directories(AutoTunePlugin PRIVATE ${ONNXRUNTIME_INCLUDE_DIR})
endif()

if(HAS_RUBBERBAND)
    target_include_directories(AutoTunePlugin PRIVATE ${RUBBERBAND_INCLUDE_DIR})
endif()

# Link libraries
target_link_libraries(AutoTunePlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_core
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

if(HAS_ONNX)
    target_link_libraries(AutoTunePlugin PRIVATE ${ONNXRUNTIME_LIB})
endif()

if(HAS_RUBBERBAND)
    target_link_libraries(AutoTunePlugin PRIVATE ${RUBBERBAND_LIB})
endif()

message(STATUS "AutoTune Plugin configured with available macOS functionality")