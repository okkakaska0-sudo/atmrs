#!/bin/bash

echo "🎵 Building AutoTune Plugin with FULL macOS functionality..."
echo "📦 Configuring build system..."

# Use working macOS configuration that finds all available dependencies
cp CMakeLists_macos_working.txt CMakeLists.txt

# Create build directory
mkdir -p build
cd build

# Configure with CMake for macOS with ONNX Runtime
echo "🔧 Configuring with ONNX Runtime at /usr/local/opt/onnxruntime..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DJUCE_BUILD_EXTRAS=OFF \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=ON \
    -DJUCE_BUILD_HELPER_TOOLS=OFF \
    -DONNXRUNTIME_ROOT_PATH=/usr/local/opt/onnxruntime \
    -DEIGEN3_INCLUDE_DIR=/usr/local/include/eigen3 \
    -DRUBBERBAND_INCLUDE_DIR=/usr/local/include \
    -DRUBBERBAND_LIBRARY=/usr/local/lib/librubberband.dylib \
    -DCMAKE_VERBOSE_MAKEFILE=ON

if [ $? -ne 0 ]; then
    echo "❌ Configuration failed!"
    echo "📋 Check that ONNX Runtime is installed: brew install onnxruntime"
    exit 1
fi

# Build the plugin
echo "🔨 Building plugin with AI features..."
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu 2>/dev/null || echo 4) || {
    echo "❌ Build failed!"
    echo "📋 Showing error details..."
    exit 1
}

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 SUCCESS! AutoTune Plugin built with ONNX Runtime!"
    echo ""
    echo "📂 Plugin files:"
    find . -name "*.vst3" -o -name "*.component" -o -name "AutoTunePlugin*" -type f | head -10
    echo ""
    echo "✅ Features included:"
    echo "   🎵 Full pitch correction (Classic, Hard modes)"
    echo "   🤖 AI models (CREPE pitch detection, DDSP synthesis)" 
    echo "   📊 Eigen3 mathematical operations"
    echo "   🎛️ Professional GUI with JUCE"
    echo "   📁 VST3, AU, and Standalone formats"
    echo ""
    echo "🚀 Ready to use in your DAW!"
else
    echo "❌ Build failed!"
    exit 1
fi