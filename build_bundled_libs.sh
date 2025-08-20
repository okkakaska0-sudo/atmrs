#!/bin/bash

# ============================================================================
# BUILD SCRIPT FOR AUTOTUNE PLUGIN WITH BUNDLED LIBRARIES
# ============================================================================
# This script builds the AutoTune plugin using bundled libraries
# for maximum compatibility and no external dependencies

set -e

echo "🚀 Building AutoTune Plugin with Bundled Libraries"
echo "=================================================="

# Clean previous build
if [ -d "build" ]; then
    echo "🧹 Cleaning previous build..."
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

echo "⚙️  Configuring CMake with bundled libraries..."
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=x86_64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DJUCE_BUILD_UNIVERSAL_BINARY=OFF \
    -DUSE_EIGEN=ON \
    -DUSE_ONNX=ON \
    -DUSE_RUBBERBAND=ON \
    -f ../CMakeLists_intel.txt \
    ..

echo "🔨 Building plugin..."
make -j$(sysctl -n hw.ncpu) AutoTunePlugin

echo ""
echo "✅ BUILD COMPLETE!"
echo "=================="
echo "📁 Plugin files created:"

# Check for built plugins
if [ -d "AutoTunePlugin_artefacts" ]; then
    find AutoTunePlugin_artefacts -name "*.vst3" -o -name "*.component" -o -name "*.app" | while read file; do
        echo "   ✓ $file"
    done
else
    echo "   ⚠️  Plugin artefacts directory not found"
fi

echo ""
echo "🎵 Ready to use in DAW!"
echo "Copy .vst3 files to: ~/Library/Audio/Plug-Ins/VST3/"
echo "Copy .component files to: ~/Library/Audio/Plug-Ins/Components/"