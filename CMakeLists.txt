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

# ============================================================================
# USE BUNDLED LIBRARIES - NO EXTERNAL DEPENDENCIES
# ============================================================================

# Set paths to our bundled libraries
set(EXTERNAL_LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/libs")

# Eigen3 - header-only library
set(EIGEN3_INCLUDE_DIR "${EXTERNAL_LIBS_DIR}/eigen-3.4.0")

# ONNX Runtime - pre-compiled for macOS x64 (headers might be embedded)
set(ONNXRUNTIME_BASE_DIR "${EXTERNAL_LIBS_DIR}/onnxruntime-osx-x86_64-1.16.3")
set(ONNXRUNTIME_LIB "${ONNXRUNTIME_BASE_DIR}/lib/libonnxruntime.1.16.3.dylib")

# Rubber Band - use source headers and compile needed source files
set(RUBBERBAND_BASE_DIR "${EXTERNAL_LIBS_DIR}/rubberband-3.3.0")
set(RUBBERBAND_INCLUDE_DIR "${RUBBERBAND_BASE_DIR}")
set(RUBBERBAND_SRC_DIR "${RUBBERBAND_BASE_DIR}/src")

# ============================================================================
# FORCE ALL LIBRARIES TO BE ENABLED - FOR BOTH REPLIT AND MAC BUILDS
# ============================================================================

# Enable all bundled libraries
set(USE_EIGEN ON CACHE BOOL "Enable Eigen3 library" FORCE)
set(USE_ONNX ON CACHE BOOL "Enable ONNX Runtime library" FORCE) 
set(USE_RUBBERBAND ON CACHE BOOL "Enable Rubber Band library" FORCE)

# Verify libraries exist
if(EXISTS "${EIGEN3_INCLUDE_DIR}/Eigen/Dense")
    message(STATUS "✅ Bundled Eigen3: ${EIGEN3_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "❌ Eigen3 not found at ${EIGEN3_INCLUDE_DIR}")
endif()

if(EXISTS "${ONNXRUNTIME_LIB}")
    message(STATUS "✅ Bundled ONNX Runtime library: ${ONNXRUNTIME_LIB}")
else()
    message(FATAL_ERROR "❌ ONNX Runtime library not found at ${ONNXRUNTIME_LIB}")
endif()

if(EXISTS "${RUBBERBAND_SRC_DIR}")
    message(STATUS "✅ Bundled Rubber Band: ${RUBBERBAND_SRC_DIR}")
else()
    message(FATAL_ERROR "❌ Rubber Band not found at ${RUBBERBAND_SRC_DIR}")
endif()

message(STATUS "🚀 ALL BUNDLED LIBRARIES READY FOR COMPILATION")

# Source files
file(GLOB_RECURSE SOURCE_FILES
    "Source/*.cpp"
    "Source/*.h"
)

# Create plugin
juce_add_plugin(AutoTunePlugin
    COMPANY_NAME "Marsi Sound Studio"
    BUNDLE_ID "com.marsisoundstudio.autotunepro"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Mrsi
    PLUGIN_CODE MsAt
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Marsi AutoTune Pro"
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

# Include directories for bundled libraries
target_include_directories(AutoTunePlugin PRIVATE 
    ${EIGEN3_INCLUDE_DIR}
    ${RUBBERBAND_INCLUDE_DIR}
    ${ONNXRUNTIME_BASE_DIR}  # ONNX headers might be with the library
)

message(STATUS "✅ Including all bundled library headers")

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

# Link bundled libraries  
target_link_libraries(AutoTunePlugin PRIVATE ${ONNXRUNTIME_LIB})

# Add Rubber Band source files to compile
file(GLOB_RECURSE RUBBERBAND_SOURCES 
    "${RUBBERBAND_SRC_DIR}/*.cpp"
    "${RUBBERBAND_SRC_DIR}/*.c"
)

# Filter out unnecessary files
list(FILTER RUBBERBAND_SOURCES EXCLUDE REGEX ".*test.*")
list(FILTER RUBBERBAND_SOURCES EXCLUDE REGEX ".*main.*")
list(FILTER RUBBERBAND_SOURCES EXCLUDE REGEX ".*program.*")

if(RUBBERBAND_SOURCES)
    target_sources(AutoTunePlugin PRIVATE ${RUBBERBAND_SOURCES})
    list(LENGTH RUBBERBAND_SOURCES RUBBERBAND_COUNT)
    message(STATUS "✅ Added ${RUBBERBAND_COUNT} Rubber Band source files")
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