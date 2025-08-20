#!/bin/bash

echo "🍎 Building AutoTune Plugin with ONNX Runtime on macOS..."
echo "📦 This will create VST3, AU, and Standalone formats"

# Copy the working configuration
cp CMakeLists_macos_working.txt CMakeLists.txt

# Remove old build directory and create fresh one
rm -rf build_macos
mkdir -p build_macos
cd build_macos

# Configure with proper macOS settings and ONNX paths
echo "🔧 Configuring project..."
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
    -DRUBBERBAND_LIBRARY=/usr/local/lib/librubberband.dylib

if [ $? -ne 0 ]; then
    echo "❌ Configuration failed!"
    exit 1
fi

echo "🔨 Building plugin..."
# Build with all CPU cores
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 SUCCESS! AutoTune Plugin built with ONNX Runtime!"
    echo ""
    echo "📂 Plugin files created:"
    find . -name "*.vst3" -o -name "*.component" -o -name "AutoTunePlugin*" 2>/dev/null | grep -v build | head -10
    echo ""
    echo "✅ Features included:"
    echo "   🎵 Full pitch correction (Classic, Hard modes)"
    echo "   🤖 AI models (CREPE pitch detection, DDSP synthesis)"
    echo "   📊 Eigen3 mathematical operations"
    echo "   🎛️ Professional GUI with JUCE"
    echo "   🔧 All audio processing algorithms"
    echo "   📁 VST3, AU, and Standalone formats"
    echo ""
    echo "🚀 Ready to use in your DAW!"
else
    echo "❌ Build failed!"
    echo "📋 Check the error messages above for details"
    exit 1
fi

cd ..