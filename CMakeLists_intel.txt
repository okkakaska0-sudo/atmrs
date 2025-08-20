cmake_minimum_required(VERSION 3.15)

project(AutoTunePlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version")

# FORCE Intel x64 ONLY - aggressive architecture enforcement
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "macOS architecture" FORCE)
set(CMAKE_APPLE_SILICON_PROCESSOR OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_UNIVERSAL_BINARY OFF CACHE BOOL "" FORCE)

# Force compiler and linker flags for Intel x64
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch x86_64" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -arch x86_64" CACHE STRING "" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -arch x86_64" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -arch x86_64" CACHE STRING "" FORCE)

message(STATUS "Building AutoTune Plugin for Intel x64")

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

# AGGRESSIVE JUCE universal binary prevention
set(JUCE_BUILD_UNIVERSAL_BINARY OFF CACHE BOOL "" FORCE)
set(JUCE_TARGET_ARCHITECTURE "x86_64" CACHE STRING "" FORCE)

FetchContent_MakeAvailable(JUCE)

# Find installed dependencies for Intel Homebrew
find_package(PkgConfig REQUIRED)

# Intel Homebrew paths (/usr/local)
set(HOMEBREW_PREFIX "/usr/local")

# Find Eigen3 - Intel paths
find_path(EIGEN3_INCLUDE_DIR Eigen/Dense
    PATHS 
        ${HOMEBREW_PREFIX}/include/eigen3
        ${HOMEBREW_PREFIX}/opt/eigen/include/eigen3
        ${HOMEBREW_PREFIX}/Cellar/eigen/*/include/eigen3
        /opt/homebrew/include/eigen3
        /opt/homebrew/opt/eigen/include/eigen3
)

# Find ONNX Runtime - Intel paths with verbose output
find_path(ONNXRUNTIME_INCLUDE_DIR 
    NAMES onnxruntime_cxx_api.h onnxruntime_c_api.h
    PATHS 
        ${HOMEBREW_PREFIX}/opt/onnxruntime/include
        ${HOMEBREW_PREFIX}/include/onnxruntime
        ${HOMEBREW_PREFIX}/include
        ${HOMEBREW_PREFIX}/Cellar/onnxruntime/*/include
    NO_DEFAULT_PATH
)

find_library(ONNXRUNTIME_LIB 
    NAMES libonnxruntime.dylib onnxruntime libonnxruntime
    PATHS 
        ${HOMEBREW_PREFIX}/opt/onnxruntime/lib
        ${HOMEBREW_PREFIX}/lib
        ${HOMEBREW_PREFIX}/Cellar/onnxruntime/*/lib
    NO_DEFAULT_PATH
)

# Find Rubber Band - Intel paths with verbose output  
find_path(RUBBERBAND_INCLUDE_DIR 
    NAMES rubberband/RubberBandStretcher.h
    PATHS 
        ${HOMEBREW_PREFIX}/include
        ${HOMEBREW_PREFIX}/opt/rubberband/include
        ${HOMEBREW_PREFIX}/Cellar/rubberband/*/include
    NO_DEFAULT_PATH
)

find_library(RUBBERBAND_LIB 
    NAMES librubberband.dylib rubberband librubberband
    PATHS 
        ${HOMEBREW_PREFIX}/lib
        ${HOMEBREW_PREFIX}/opt/rubberband/lib
        ${HOMEBREW_PREFIX}/Cellar/rubberband/*/lib
    NO_DEFAULT_PATH
)

# Report found libraries
if(EIGEN3_INCLUDE_DIR)
    message(STATUS "Found Eigen3: ${EIGEN3_INCLUDE_DIR}")
    set(USE_EIGEN ON)
else()
    message(WARNING "Eigen3 not found - some features will be disabled")
    set(USE_EIGEN OFF)
endif()

if(ONNXRUNTIME_INCLUDE_DIR AND ONNXRUNTIME_LIB)
    message(STATUS "Found ONNX Runtime: ${ONNXRUNTIME_INCLUDE_DIR}, ${ONNXRUNTIME_LIB}")
    set(USE_ONNX ON)
else()
    message(WARNING "ONNX Runtime not found - AI features will be disabled")
    set(USE_ONNX OFF)
endif()

if(RUBBERBAND_INCLUDE_DIR AND RUBBERBAND_LIB)
    message(STATUS "Found Rubber Band: ${RUBBERBAND_INCLUDE_DIR}, ${RUBBERBAND_LIB}")
    set(USE_RUBBERBAND ON)
else()
    message(WARNING "Rubber Band not found - pitch shifting will use basic algorithms")
    set(USE_RUBBERBAND OFF)
endif()

# Source files
file(GLOB_RECURSE SOURCE_FILES
    "Source/*.cpp"
    "Source/*.h"
)

# Create plugin
juce_add_plugin(AutoTunePlugin
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Yco
    PLUGIN_CODE Atpl
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "AutoTune Plugin"
)

# Set target properties
juce_generate_juce_header(AutoTunePlugin)

target_sources(AutoTunePlugin PRIVATE ${SOURCE_FILES})

# Compile definitions
target_compile_definitions(AutoTunePlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_ALSA=0
        JUCE_DIRECTSOUND=0
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        $<$<BOOL:${USE_EIGEN}>:USE_EIGEN=1>
        $<$<BOOL:${USE_ONNX}>:USE_ONNX=1>
        $<$<BOOL:${USE_RUBBERBAND}>:USE_RUBBERBAND=1>
)

# Include directories
target_include_directories(AutoTunePlugin PRIVATE Source)

if(USE_EIGEN)
    target_include_directories(AutoTunePlugin PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()

if(USE_ONNX)
    target_include_directories(AutoTunePlugin PRIVATE ${ONNXRUNTIME_INCLUDE_DIR})
endif()

if(USE_RUBBERBAND)
    target_include_directories(AutoTunePlugin PRIVATE ${RUBBERBAND_INCLUDE_DIR})
endif()

# Link libraries
target_link_libraries(AutoTunePlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
)

# Link external libraries if available
if(USE_ONNX AND ONNXRUNTIME_LIB)
    target_link_libraries(AutoTunePlugin PRIVATE ${ONNXRUNTIME_LIB})
endif()

if(USE_RUBBERBAND AND RUBBERBAND_LIB)
    target_link_libraries(AutoTunePlugin PRIVATE ${RUBBERBAND_LIB})
endif()

# Compiler-specific settings
if(APPLE)
    target_compile_options(AutoTunePlugin PRIVATE -Wno-deprecated-declarations)
    
    # Intel specific optimizations
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
        target_compile_options(AutoTunePlugin PRIVATE -march=native)
    endif()
endif()

# Installation
install(TARGETS AutoTunePlugin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)