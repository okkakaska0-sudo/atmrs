#!/bin/bash

# Full macOS build script for AutoTune Plugin
# Preserves ALL functionality - no simplification

echo "🍎 Building COMPLETE AutoTune Plugin for macOS..."
echo "📌 PRESERVING ALL FUNCTIONALITY - No simplification!"

# Clean and setup
rm -rf build_full
mkdir -p build_full
cd build_full

# Copy the full macOS CMakeLists
cp ../CMakeLists_macos_full.txt ./CMakeLists.txt

echo "📦 Configuring FULL AutoTune Plugin build..."
echo "🎵 Preserving: Audio processing, AI models, math libraries"
echo "🤖 Preserving: ONNX Runtime, CREPE, DDSP capabilities"
echo "🔧 Preserving: All utilities and advanced algorithms"

# Configure with all features
cmake . -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON

if [ $? -ne 0 ]; then
    echo "❌ Configuration failed!"
    exit 1
fi

echo "🔨 Building COMPLETE AutoTune Plugin..."
echo "⏳ This may take time due to downloading all libraries..."
cmake --build . --config Release --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ COMPLETE AutoTune Plugin built successfully!"
echo ""
echo "📍 Plugin locations:"
echo "   VST3: build_full/AutoTunePlugin_artefacts/Release/VST3/ProAutoTune.vst3"
echo ""
echo "🎉 FULL functionality preserved!"
echo "🎵 Ready with complete audio processing capabilities!"
echo "🤖 AI models integration ready!"
echo "🔧 All advanced features intact!"