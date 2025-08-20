#!/bin/bash

# Build script for macOS AutoTune Plugin
# This script builds VST3 and AU formats for macOS

echo "🎵 Building ProAutoTune Plugin..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "📦 Configuring build system..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
         -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15

# Check if configuration was successful
if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

# Build the plugin
echo "🔨 Building plugin..."
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu)

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build completed successfully!"
echo ""
echo "📍 Plugin locations:"
echo "   VST3: ~/Library/Audio/Plug-Ins/VST3/ProAutoTune.vst3"
echo "   AU:   ~/Library/Audio/Plug-Ins/Components/ProAutoTune.component"
echo ""
echo "🎉 Ready to use in your DAW!"
