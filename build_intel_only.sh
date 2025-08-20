#!/bin/bash

# AutoTune Plugin Intel ONLY Build Script
echo "🍎 AutoTune Plugin - Intel x64 ONLY Build"
echo "=========================================="

# Make script executable
chmod +x "$0"

# Force Intel architecture check
ARCH=$(uname -m)
echo "🔍 Detected architecture: $ARCH"

if [[ "$ARCH" != "x86_64" ]]; then
    echo "❌ This script requires Intel x64 Mac"
    echo "Your architecture: $ARCH"
    exit 1
fi

echo "✅ Intel x64 Mac confirmed"

# Intel Homebrew paths
HOMEBREW_PREFIX="/usr/local"
export PATH="/usr/local/bin:$PATH"

echo ""
echo "🔍 Checking Intel x64 library architectures..."

# Check architectures of installed libraries
ONNX_LIB=$(find /usr/local -name "libonnxruntime*.dylib" 2>/dev/null | head -1)
RUBBER_LIB=$(find /usr/local -name "librubberband*.dylib" 2>/dev/null | head -1)

echo "📋 Checking library architectures:"
if [[ -n "$ONNX_LIB" ]]; then
    echo "ONNX Runtime:"
    file "$ONNX_LIB"
fi

if [[ -n "$RUBBER_LIB" ]]; then
    echo "Rubber Band:"
    file "$RUBBER_LIB"
fi

echo ""
echo "🏗️  Starting Intel-only build..."

# Use Intel specific CMakeLists
if [[ ! -f "CMakeLists_intel.txt" ]]; then
    echo "❌ CMakeLists_intel.txt not found!"
    exit 1
fi

# Copy Intel config
cp CMakeLists_intel.txt CMakeLists.txt

# Clean previous build
rm -rf build_intel_only
mkdir -p build_intel_only
cd build_intel_only

# Configure with Intel ONLY settings
echo "🔧 Configuring for Intel x64 ONLY (no universal binary)..."
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$HOMEBREW_PREFIX" \
    -DJUCE_BUILD_UNIVERSAL_BINARY=OFF \
    -DCMAKE_APPLE_SILICON_PROCESSOR=OFF \
    -DCMAKE_VERBOSE_MAKEFILE=ON

if [[ $? -ne 0 ]]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

echo ""
echo "🔨 Building with Intel x64 ONLY target..."
make -j$(sysctl -n hw.ncpu) VERBOSE=1

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Build completed successfully!"
    echo "📁 Plugin files created in: build_intel_only/AutoTunePlugin_artefacts/"
    echo ""
    echo "🎵 Plugin formats built:"
    ls -la AutoTunePlugin_artefacts/Release/ 2>/dev/null || echo "Check build directory for artifacts"
    
    echo ""
    echo "🔍 Verifying Intel x64 binaries:"
    find AutoTunePlugin_artefacts -name "AutoTune Plugin*" -exec file {} \; 2>/dev/null
else
    echo "❌ Build failed! Check logs above for errors."
    exit 1
fi