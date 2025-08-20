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

# Configure JUCE build - disable all extras
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_HELPER_TOOLS OFF CACHE BOOL "" FORCE)

# Force disable juceaide and other tools
set(JUCE_TOOL_INSTALL_DIR "" CACHE STRING "" FORCE)
set(JUCE_COPY_PLUGIN_AFTER_BUILD OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(JUCE)

# Add Rubber Band Library - download and build automatically
FetchContent_Declare(
    rubberband
    GIT_REPOSITORY https://github.com/breakfastquay/rubberband.git
    GIT_TAG v3.3.0
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(rubberband)
if(NOT rubberband_POPULATED)
    message(STATUS "Fetching Rubber Band Library...")
    FetchContent_Populate(rubberband)
    
    # Configure Rubber Band build
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(RUBBERBAND_PROGRAM OFF CACHE BOOL "" FORCE)
    set(RUBBERBAND_VAMP OFF CACHE BOOL "" FORCE)
    set(RUBBERBAND_LADSPA OFF CACHE BOOL "" FORCE)
    
    add_subdirectory(${rubberband_SOURCE_DIR} ${rubberband_BINARY_DIR})
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
    FetchContent_Populate(onnxruntime)
    set(ONNXRUNTIME_ROOT_PATH ${onnxruntime_SOURCE_DIR})
    set(USE_ONNX ON)
else()
    message(STATUS "ONNX Runtime disabled for this platform")
    set(USE_ONNX OFF)
endif()

# Add Eigen3 for mathematical operations
FetchContent_Declare(
    eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(eigen3)
if(NOT eigen3_POPULATED)
    message(STATUS "Fetching Eigen3 mathematics library...")
    FetchContent_Populate(eigen3)
    add_subdirectory(${eigen3_SOURCE_DIR} ${eigen3_BINARY_DIR})
endif()

# Add FFTW3 for fast Fourier transforms
FetchContent_Declare(
    fftw3
    URL http://www.fftw.org/fftw-3.3.10.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_GetProperties(fftw3)
if(NOT fftw3_POPULATED)
    message(STATUS "Fetching FFTW3 library...")
    FetchContent_Populate(fftw3)
    
    # Configure FFTW3 build
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ENABLE_THREADS ON CACHE BOOL "" FORCE)
    set(ENABLE_FLOAT ON CACHE BOOL "" FORCE)
    set(DISABLE_FORTRAN ON CACHE BOOL "" FORCE)
    
    add_subdirectory(${fftw3_SOURCE_DIR} ${fftw3_BINARY_DIR})
endif()

# Add Kiss FFT as lightweight alternative
FetchContent_Declare(
    kissfft
    GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
    GIT_TAG v131.1.0
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(kissfft)
if(NOT kissfft_POPULATED)
    message(STATUS "Fetching Kiss FFT library...")
    FetchContent_Populate(kissfft)
    
    # Configure Kiss FFT build
    set(KISSFFT_STATIC ON CACHE BOOL "" FORCE)
    set(KISSFFT_TEST OFF CACHE BOOL "" FORCE)
    set(KISSFFT_TOOLS OFF CACHE BOOL "" FORCE)
    
    add_subdirectory(${kissfft_SOURCE_DIR} ${kissfft_BINARY_DIR})
endif()

# Add libsamplerate for high-quality sample rate conversion
FetchContent_Declare(
    libsamplerate
    GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
    GIT_TAG 0.2.2
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(libsamplerate)
if(NOT libsamplerate_POPULATED)
    message(STATUS "Fetching libsamplerate...")
    FetchContent_Populate(libsamplerate)
    
    # Configure libsamplerate build
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(LIBSAMPLERATE_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(LIBSAMPLERATE_TESTS OFF CACHE BOOL "" FORCE)
    
    add_subdirectory(${libsamplerate_SOURCE_DIR} ${libsamplerate_BINARY_DIR})
endif()

message(STATUS "All libraries integrated into project")
set(USE_EIGEN ON)
set(USE_FFTW ON)
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

# Link all integrated libraries

# Rubber Band Library
target_compile_definitions(AutoTunePlugin PRIVATE USE_RUBBERBAND=1)
target_link_libraries(AutoTunePlugin PRIVATE rubberband)

# Eigen3 mathematics library
target_compile_definitions(AutoTunePlugin PRIVATE USE_EIGEN=1)
target_link_libraries(AutoTunePlugin PRIVATE Eigen3::Eigen)

# FFTW3 library
target_compile_definitions(AutoTunePlugin PRIVATE USE_FFTW=1)
target_link_libraries(AutoTunePlugin PRIVATE fftw3f)

# ONNX Runtime (macOS only)
if(USE_ONNX)
    target_compile_definitions(AutoTunePlugin PRIVATE USE_ONNX=1)
    target_include_directories(AutoTunePlugin PRIVATE ${ONNXRUNTIME_ROOT_PATH}/include)
    target_link_libraries(AutoTunePlugin PRIVATE ${ONNXRUNTIME_ROOT_PATH}/lib/libonnxruntime.dylib)
endif()

# Kiss FFT library
target_compile_definitions(AutoTunePlugin PRIVATE USE_KISSFFT=1)
target_link_libraries(AutoTunePlugin PRIVATE kissfft)

# libsamplerate
target_compile_definitions(AutoTunePlugin PRIVATE USE_LIBSAMPLERATE=1)
target_link_libraries(AutoTunePlugin PRIVATE samplerate)

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
