#!/bin/bash

# AutoTune Plugin Intel Build Script - Fixed for x64
echo "🍎 AutoTune Plugin - Intel x64 Build (Fixed)"
echo "============================================="

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

# Check for Homebrew in correct location
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew not found. Installing..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

echo "✅ Intel x64 Mac confirmed"
echo "✅ Homebrew found at: $(which brew)"

# Intel Homebrew paths
HOMEBREW_PREFIX="/usr/local"
export PATH="/usr/local/bin:$PATH"

echo ""
echo "🔍 Checking and installing dependencies for Intel..."

# Install/update dependencies for Intel
echo "📦 Installing ONNX Runtime..."
brew install onnxruntime || brew upgrade onnxruntime

echo "📦 Installing Rubber Band..."
brew install rubberband || brew upgrade rubberband

echo "📦 Installing Eigen..."
brew install eigen || brew upgrade eigen

echo "📦 Installing build tools..."
brew install cmake pkg-config || brew upgrade cmake pkg-config

echo ""
echo "🔍 Verifying Intel x64 installations..."

# Check architectures of installed libraries
echo "📋 ONNX Runtime architecture:"
find /usr/local -name "libonnxruntime*" -exec file {} \; 2>/dev/null | head -3

echo ""
echo "📋 Rubber Band architecture:"
find /usr/local -name "librubberband*" -exec file {} \; 2>/dev/null | head -3

echo ""
echo "📋 Library locations:"
ONNX_LIB=$(find /usr/local -name "libonnxruntime*.dylib" 2>/dev/null | head -1)
RUBBER_LIB=$(find /usr/local -name "librubberband*.dylib" 2>/dev/null | head -1)
EIGEN_DIR=$(find /usr/local -name "Eigen" -type d 2>/dev/null | head -1)

echo "ONNX Runtime: $ONNX_LIB"
echo "Rubber Band: $RUBBER_LIB"
echo "Eigen: $EIGEN_DIR"

if [[ -z "$ONNX_LIB" || -z "$RUBBER_LIB" || -z "$EIGEN_DIR" ]]; then
    echo "❌ Some libraries not found! Check installation."
    exit 1
fi

echo ""
echo "🏗️  Starting build..."

# Use Intel specific CMakeLists
if [[ ! -f "CMakeLists_intel.txt" ]]; then
    echo "❌ CMakeLists_intel.txt not found!"
    exit 1
fi

# Copy Intel config
cp CMakeLists_intel.txt CMakeLists.txt

# Create build directory
mkdir -p build_intel
cd build_intel

# Configure with Intel specific settings
echo "🔧 Configuring for Intel x64 ONLY..."
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES=x86_64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$HOMEBREW_PREFIX" \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DJUCE_BUILD_UNIVERSAL_BINARY=OFF

if [[ $? -ne 0 ]]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

echo "🔨 Building with verbose output..."
make -j$(sysctl -n hw.ncpu) VERBOSE=1

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Build completed successfully!"
    echo "📁 Plugin files created in: build_intel/AutoTunePlugin_artefacts/"
    echo ""
    echo "🎵 Plugin formats built:"
    ls -la AutoTunePlugin_artefacts/Release/ 2>/dev/null || echo "Check build directory for artifacts"
else
    echo "❌ Build failed! Check logs above for linker errors."
    exit 1
fi