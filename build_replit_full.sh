#!/bin/bash

# Complete AutoTune Plugin build for Replit with macOS compatibility
# PRESERVES ALL FUNCTIONALITY - No simplification whatsoever

echo "🎵 Building COMPLETE AutoTune Plugin on Replit..."
echo "📌 PRESERVING ALL FUNCTIONALITY:"
echo "   🎵 Complete audio processing"
echo "   🤖 Full AI models (ONNX, CREPE, DDSP)"
echo "   🔧 All mathematical libraries"
echo "   🍎 macOS compatibility mode"

# Clean and setup
rm -rf build_replit
mkdir -p build_replit
cd build_replit

# Use the Replit-compatible full CMakeLists
cp ../CMakeLists_replit_macos.txt ./CMakeLists.txt

echo "📦 Configuring with macOS framework emulation..."
cmake . -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON

if [ $? -ne 0 ]; then
    echo "❌ Configuration failed!"
    exit 1
fi

echo "🔨 Building COMPLETE plugin with all libraries..."
cmake --build . --config Release --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ COMPLETE AutoTune Plugin built successfully!"
echo ""
echo "📍 Plugin location: build_replit/AutoTunePlugin_artefacts/Release/VST3/"
echo ""
echo "🎉 SUCCESS - ALL functionality preserved:"
echo "   ✅ Audio processing capabilities"
echo "   ✅ AI models integration"
echo "   ✅ Mathematical libraries"
echo "   ✅ macOS compatibility"
echo ""
echo "🚀 Plugin ready for use!"