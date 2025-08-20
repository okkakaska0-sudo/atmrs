#!/bin/bash

# AutoTune Plugin FORCE Intel x64 ONLY Build
echo "🍎 AutoTune Plugin - FORCE Intel x64 Build"
echo "=========================================="

# Force Intel architecture
ARCH=$(uname -m)
if [[ "$ARCH" != "x86_64" ]]; then
    echo "❌ This script requires Intel x64 Mac"
    exit 1
fi

echo "✅ Intel x64 Mac confirmed"

# Intel Homebrew paths
HOMEBREW_PREFIX="/usr/local"
export PATH="/usr/local/bin:$PATH"

echo ""
echo "🏗️  Starting AGGRESSIVE Intel-only build..."

# Copy Intel-only CMakeLists
cp CMakeLists_intel.txt CMakeLists.txt

# Clean everything
rm -rf build_force
mkdir -p build_force
cd build_force

# Configure with MAXIMUM Intel forcing
echo "🔧 Configuring with MAXIMUM Intel x64 enforcement..."
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$HOMEBREW_PREFIX" \
    -DJUCE_BUILD_UNIVERSAL_BINARY=OFF \
    -DCMAKE_APPLE_SILICON_PROCESSOR=OFF \
    -DCMAKE_C_FLAGS="-arch x86_64" \
    -DCMAKE_CXX_FLAGS="-arch x86_64" \
    -DCMAKE_EXE_LINKER_FLAGS="-arch x86_64" \
    -DCMAKE_MODULE_LINKER_FLAGS="-arch x86_64" \
    -DCMAKE_SHARED_LINKER_FLAGS="-arch x86_64" \
    -DCMAKE_VERBOSE_MAKEFILE=ON

if [[ $? -ne 0 ]]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

echo ""
echo "🔨 Building with forced Intel x64..."
make -j$(sysctl -n hw.ncpu) VERBOSE=1

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Build completed!"
    echo "📁 Plugins in: build_force/AutoTunePlugin_artefacts/"
    
    echo ""
    echo "🔍 Verifying architectures:"
    find AutoTunePlugin_artefacts -name "AutoTune Plugin*" -exec file {} \;
else
    echo "❌ Build failed!"
    exit 1
fi