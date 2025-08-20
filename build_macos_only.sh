#!/bin/bash

# macOS-only build script for AutoTune Plugin on Replit
# Completely bypasses Linux dependencies

echo "🍎 Building AutoTune Plugin in macOS-ONLY mode..."

# Clean previous builds
rm -rf build_macos
mkdir -p build_macos
cd build_macos

# Use the macOS-only CMakeLists.txt
cp ../CMakeLists_macos_only.txt ./CMakeLists.txt

# Configure with minimal dependencies
echo "📦 Configuring macOS-only build..."
cmake . -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON

# Check configuration
if [ $? -ne 0 ]; then
    echo "❌ macOS-only configuration failed!"
    exit 1
fi

# Build the static library
echo "🔨 Building macOS-compatible library..."
cmake --build . --config Release

# Check build result
if [ $? -ne 0 ]; then
    echo "❌ macOS-only build failed!"
    exit 1
fi

echo "✅ macOS-only build completed successfully!"
echo "📍 Library built: build_macos/libAutoTunePluginLib.a"
echo "🎉 Ready for macOS integration!"