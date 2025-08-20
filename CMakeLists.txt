cmake_minimum_required(VERSION 3.15)

project(AutoTunePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration for Replit Linux environment
message(STATUS "Building AutoTune Plugin for Replit Linux")

# Add JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    URL https://github.com/juce-framework/JUCE/archive/refs/tags/7.0.9.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

# Configure JUCE for Linux build
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_HELPER_TOOLS OFF CACHE BOOL "" FORCE)
set(JUCE_COPY_PLUGIN_AFTER_BUILD OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(JUCE)

# Find system packages
find_package(PkgConfig REQUIRED)

# Find Eigen3 in Nix store
find_path(EIGEN3_INCLUDE_DIR Eigen/Dense
    PATHS 
        /nix/store/*/include/eigen3
        /nix/store/*/include
        /home/runner/.nix-profile/include/eigen3
        /nix/var/nix/profiles/default/include/eigen3
    PATH_SUFFIXES eigen3
)

# Find ONNX Runtime in Nix store  
find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
    PATHS 
        /nix/store/*/include
        /home/runner/.nix-profile/include
        /nix/var/nix/profiles/default/include
)

find_library(ONNXRUNTIME_LIB onnxruntime
    NAMES onnxruntime libonnxruntime
    PATHS 
        /nix/store/*/lib
        /home/runner/.nix-profile/lib
        /nix/var/nix/profiles/default/lib
)

# Report findings
if(EIGEN3_INCLUDE_DIR)
    message(STATUS "Found Eigen3: ${EIGEN3_INCLUDE_DIR}")
    set(HAS_EIGEN3 TRUE)
else()
    message(WARNING "Eigen3 not found - mathematical operations will use fallback")
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

# Define source files
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

# Add plugin target
juce_add_plugin(AutoTunePlugin
    COMPANY_NAME "ReplicaStudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "Repl"
    PLUGIN_CODE "ATun"
    FORMATS VST3 Standalone
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
        USE_ONNX=$<BOOL:${HAS_ONNX}>
        USE_CREPE=$<BOOL:${HAS_ONNX}>
        USE_DDSP=$<BOOL:${HAS_ONNX}>
        USE_RUBBERBAND=0
    PRIVATE
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AutoTunePlugin,JUCE_PLUGIN_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AutoTunePlugin,JUCE_PLUGIN_VERSION>"
)

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

# Include directories
target_include_directories(AutoTunePlugin PRIVATE Source)

if(HAS_EIGEN3)
    target_include_directories(AutoTunePlugin PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()

if(HAS_ONNX)
    target_include_directories(AutoTunePlugin PRIVATE ${ONNXRUNTIME_INCLUDE_DIR})
    target_link_libraries(AutoTunePlugin PRIVATE ${ONNXRUNTIME_LIB})
endif()

message(STATUS "AutoTune Plugin configured for Replit Linux with available libraries")