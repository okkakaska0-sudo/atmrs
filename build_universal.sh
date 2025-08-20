#!/bin/bash

# AutoTune Plugin Universal Build Script
# Auto-detects architecture and uses appropriate paths

echo "🍎 AutoTune Plugin - Universal Build"
echo "==================================="

# Make script executable
chmod +x "$0"

# Auto-detect architecture
ARCH=$(uname -m)
echo "🔍 Detected architecture: $ARCH"

if [[ "$ARCH" == "arm64" ]]; then
    echo "🍎 Apple Silicon (ARM64) Mac detected"
    HOMEBREW_PREFIX="/opt/homebrew"
    CMAKE_ARCH="arm64"
elif [[ "$ARCH" == "x86_64" ]]; then
    echo "🍎 Intel x64 Mac detected"
    HOMEBREW_PREFIX="/usr/local"
    CMAKE_ARCH="x86_64"
else
    echo "❌ Unsupported architecture: $ARCH"
    exit 1
fi

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew not found. Install it first:"
    echo "/bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

echo "✅ Intel Mac detected"
echo "✅ Homebrew found at: $(which brew)"

# Check dependencies
echo ""
echo "🔍 Checking dependencies..."

# Check for ONNX Runtime
ONNX_PATH=$(find "$HOMEBREW_PREFIX" -name "libonnxruntime*" 2>/dev/null | head -1)
if [[ -n "$ONNX_PATH" ]]; then
    echo "✅ ONNX Runtime found: $ONNX_PATH"
else
    echo "❌ ONNX Runtime not found. Installing..."
    brew install onnxruntime
fi

# Check for Rubber Band
RUBBER_PATH=$(find "$HOMEBREW_PREFIX" -name "librubberband*" 2>/dev/null | head -1)
if [[ -n "$RUBBER_PATH" ]]; then
    echo "✅ Rubber Band found: $RUBBER_PATH"
else
    echo "❌ Rubber Band not found. Installing..."
    brew install rubberband
fi

# Check for Eigen
EIGEN_PATH=$(find "$HOMEBREW_PREFIX" -name "Eigen" -type d 2>/dev/null | head -1)
if [[ -n "$EIGEN_PATH" ]]; then
    echo "✅ Eigen found: $EIGEN_PATH"
else
    echo "❌ Eigen not found. Installing..."
    brew install eigen
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found. Installing..."
    brew install cmake
else
    echo "✅ CMake found: $(which cmake)"
fi

echo ""
echo "🏗️  Starting build..."

# Use architecture-specific CMakeLists
if [[ "$ARCH" == "arm64" && -f "CMakeLists_arm64.txt" ]]; then
    echo "📋 Using ARM64 configuration..."
    cp CMakeLists_arm64.txt CMakeLists.txt
elif [[ "$ARCH" == "x86_64" && -f "CMakeLists_intel.txt" ]]; then
    echo "📋 Using Intel configuration..."
    cp CMakeLists_intel.txt CMakeLists.txt
else
    echo "❌ No suitable CMakeLists found for $ARCH!"
    exit 1
fi

# Create build directory
mkdir -p "build_$ARCH"
cd "build_$ARCH"

# Configure with detected architecture
echo "🔧 Configuring for $ARCH..."
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES="$CMAKE_ARCH" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$HOMEBREW_PREFIX"

if [[ $? -ne 0 ]]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

echo "🔨 Building..."
make -j$(sysctl -n hw.ncpu)

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Build completed successfully!"
    echo "📁 Plugin files created in: build_$ARCH/AutoTunePlugin_artefacts/"
    echo ""
    echo "🎵 Plugin formats built:"
    ls -la AutoTunePlugin_artefacts/Release/ 2>/dev/null || echo "Check build directory for artifacts"
else
    echo "❌ Build failed!"
    exit 1
fi