cmake_minimum_required(VERSION 3.15)

project(AutoTunePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect platform
if(APPLE)
    message(STATUS "Building for macOS")
    set(MACOS_BUILD TRUE)
else()
    message(STATUS "Building for other platform")
    set(MACOS_BUILD FALSE)
endif()

# Add JUCE - download as archive
include(FetchContent)
FetchContent_Declare(
    JUCE
    URL https://github.com/juce-framework/JUCE/archive/refs/tags/7.0.9.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

# Configure JUCE build - disable all extras and tools
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_HELPER_TOOLS OFF CACHE BOOL "" FORCE)
set(JUCE_COPY_PLUGIN_AFTER_BUILD OFF CACHE BOOL "" FORCE)

# Disable juceaide completely
if(NOT COMMAND juce_add_plugin)
    macro(juce_add_plugin)
        # Override to prevent juceaide calls
    endmacro()
endif()

FetchContent_MakeAvailable(JUCE)

# Rubber Band Library - use system installation or disable
# Since Rubber Band doesn't use CMake, we'll use system installation
find_path(RUBBERBAND_INCLUDE_DIR rubberband/RubberBandStretcher.h
    PATHS /usr/local/include /opt/homebrew/include
)
find_library(RUBBERBAND_LIBRARY NAMES rubberband
    PATHS /usr/local/lib /opt/homebrew/lib
)

if(RUBBERBAND_INCLUDE_DIR AND RUBBERBAND_LIBRARY)
    message(STATUS "Rubber Band Library found: ${RUBBERBAND_LIBRARY}")
    set(USE_RUBBERBAND ON)
else()
    message(STATUS "Rubber Band Library not found. Install with: brew install rubberband")
    set(USE_RUBBERBAND OFF)
endif()

message(STATUS "Rubber Band Library integrated into project")
set(USE_RUBBERBAND ON)

# Add ONNX Runtime for AI models
FetchContent_Declare(
    onnxruntime
    URL https://github.com/microsoft/onnxruntime/releases/download/v1.16.3/onnxruntime-osx-universal2-1.16.3.tgz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_GetProperties(onnxruntime)
if(NOT onnxruntime_POPULATED AND APPLE)
    message(STATUS "Fetching ONNX Runtime for macOS...")
    FetchContent_MakeAvailable(onnxruntime)
    set(ONNXRUNTIME_ROOT_PATH ${onnxruntime_SOURCE_DIR})
    set(USE_ONNX ON)
else()
    message(STATUS "ONNX Runtime disabled for this platform")
    set(USE_ONNX OFF)
endif()

# Add Eigen3 for mathematical operations - header only library
FetchContent_Declare(
    eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
)

# Configure Eigen3 - disable everything except headers
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE) 
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_BLAS OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_LAPACK OFF CACHE BOOL "" FORCE)
set(EIGEN_LEAVE_TEST_IN_ALL_TARGET OFF CACHE BOOL "" FORCE)

# Get Eigen3 source without building anything
FetchContent_GetProperties(eigen3)
if(NOT eigen3_POPULATED)
    FetchContent_Populate(eigen3)
    # Add only the header-only target manually to avoid BLAS/LAPACK issues
    add_library(eigen3_headers INTERFACE)
    target_include_directories(eigen3_headers INTERFACE ${eigen3_SOURCE_DIR})
    add_library(Eigen3::Eigen ALIAS eigen3_headers)
endif()

# Use Kiss FFT instead of FFTW3 for simplicity
message(STATUS "Using Kiss FFT for FFT operations")

# Add Kiss FFT - lightweight FFT library
FetchContent_Declare(
    kissfft
    GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
    GIT_TAG 131.1.0
    GIT_SHALLOW TRUE
)

# Configure Kiss FFT build
set(KISSFFT_STATIC ON CACHE BOOL "" FORCE)
set(KISSFFT_TEST OFF CACHE BOOL "" FORCE)
set(KISSFFT_TOOLS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(kissfft)

# Add libsamplerate for high-quality sample rate conversion
FetchContent_Declare(
    libsamplerate
    GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
    GIT_TAG 0.2.3
    GIT_SHALLOW TRUE
)

# Configure libsamplerate build
set(LIBSAMPLERATE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIBSAMPLERATE_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

FetchContent_MakeAvailable(libsamplerate)

message(STATUS "All libraries integrated into project")
set(USE_EIGEN ON)
set(USE_KISSFFT ON)
set(USE_LIBSAMPLERATE ON)

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

# Create the plugin target with platform-specific formats
if(APPLE)
    set(PLUGIN_FORMATS AU VST3)
else()
    set(PLUGIN_FORMATS VST3)
endif()

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
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "ProAutoTune"
    BUNDLE_ID "com.proaudio.proautotune"
)

# Add sources to the plugin
target_sources(AutoTunePlugin PRIVATE ${PLUGIN_SOURCES})

# Platform-specific compiler options
if(APPLE)
    # macOS build with full GUI support
    target_compile_definitions(AutoTunePlugin
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
            JUCE_DISPLAY_SPLASH_SCREEN=0
            JUCE_REPORT_APP_USAGE=0
    )
else()
    # Linux/other platforms - headless build
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
            JUCE_HEADLESS=1
        PRIVATE
            JUCE_MODULE_AVAILABLE_juce_opengl=0
    )
endif()

# Link integrated libraries

# Rubber Band Library (optional)
if(USE_RUBBERBAND)
    target_compile_definitions(AutoTunePlugin PRIVATE USE_RUBBERBAND=1)
    target_include_directories(AutoTunePlugin PRIVATE ${RUBBERBAND_INCLUDE_DIR})
    target_link_libraries(AutoTunePlugin PRIVATE ${RUBBERBAND_LIBRARY})
endif()

# Eigen3 mathematics library (header-only)
target_compile_definitions(AutoTunePlugin PRIVATE USE_EIGEN=1)
target_link_libraries(AutoTunePlugin PRIVATE Eigen3::Eigen)

# Kiss FFT library
target_compile_definitions(AutoTunePlugin PRIVATE USE_KISSFFT=1)
target_link_libraries(AutoTunePlugin PRIVATE kissfft)

# libsamplerate
target_compile_definitions(AutoTunePlugin PRIVATE USE_LIBSAMPLERATE=1)
target_link_libraries(AutoTunePlugin PRIVATE samplerate)

# ONNX Runtime (macOS only)
if(USE_ONNX)
    target_compile_definitions(AutoTunePlugin PRIVATE USE_ONNX=1)
    target_include_directories(AutoTunePlugin PRIVATE ${ONNXRUNTIME_ROOT_PATH}/include)
    target_link_libraries(AutoTunePlugin PRIVATE ${ONNXRUNTIME_ROOT_PATH}/lib/libonnxruntime.dylib)
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
