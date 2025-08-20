#!/bin/bash

echo "🎵 Building AutoTune Plugin WITHOUT ONNX Runtime (for testing)..."
echo "📦 This version will work without AI features but with full audio processing"

# Create a temporary CMakeLists.txt without ONNX
cp CMakeLists_macos_working.txt CMakeLists_temp.txt

# Disable ONNX in the temporary config
sed -i.bak 's/USE_ONNX=\$<BOOL:\${HAS_ONNX}>/USE_ONNX=0/g' CMakeLists_temp.txt
sed -i.bak 's/USE_CREPE=\$<BOOL:\${HAS_ONNX}>/USE_CREPE=0/g' CMakeLists_temp.txt  
sed -i.bak 's/USE_DDSP=\$<BOOL:\${HAS_ONNX}>/USE_DDSP=0/g' CMakeLists_temp.txt

# Use the modified config
cp CMakeLists_temp.txt CMakeLists.txt

# Create build directory
mkdir -p build_no_onnx
cd build_no_onnx

# Configure and build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DJUCE_BUILD_EXTRAS=OFF \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=ON \
    -DJUCE_BUILD_HELPER_TOOLS=OFF \
    -DCMAKE_VERBOSE_MAKEFILE=ON

echo "🔨 Building plugin..."
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -eq 0 ]; then
    echo "✅ AutoTune Plugin built successfully without ONNX!"
    echo "📂 Build artifacts:"
    find . -name "*.vst3" -o -name "*.component" -o -name "AutoTunePlugin*" | head -10
    echo ""
    echo "🎯 Plugin built with:"
    echo "   ✅ Eigen3 mathematical operations"
    echo "   ✅ Full pitch correction algorithms" 
    echo "   ✅ Classic and Hard correction modes"
    echo "   ✅ Professional GUI with JUCE"
    echo "   ❌ AI features disabled (no ONNX Runtime)"
    echo ""
    echo "📋 To add AI features, install ONNX Runtime:"
    echo "   brew install onnxruntime"
else
    echo "❌ Build failed!"
    exit 1
fi

# Cleanup
cd ..
rm -f CMakeLists_temp.txt CMakeLists_temp.txt.bak