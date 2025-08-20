#!/bin/bash

# AutoTune Plugin Intel Build Script
# For Intel Macs with Homebrew in /usr/local

echo "🍎 AutoTune Plugin - Intel x64 Build"
echo "===================================="

# Make script executable
chmod +x "$0"

# Check for Intel architecture
if [[ $(uname -m) != "x86_64" ]]; then
    echo "❌ This script is for Intel x64 Macs only"
    echo "Your architecture: $(uname -m)"
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
ONNX_PATH=$(find /usr/local -name "libonnxruntime*" 2>/dev/null | head -1)
if [[ -n "$ONNX_PATH" ]]; then
    echo "✅ ONNX Runtime found: $ONNX_PATH"
else
    echo "❌ ONNX Runtime not found. Installing..."
    brew install onnxruntime
fi

# Check for Rubber Band
RUBBER_PATH=$(find /usr/local -name "librubberband*" 2>/dev/null | head -1)
if [[ -n "$RUBBER_PATH" ]]; then
    echo "✅ Rubber Band found: $RUBBER_PATH"
else
    echo "❌ Rubber Band not found. Installing..."
    brew install rubberband
fi

# Check for Eigen
EIGEN_PATH=$(find /usr/local -name "Eigen" -type d 2>/dev/null | head -1)
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
echo "🔧 Configuring for Intel x64..."
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES=x86_64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=/usr/local

if [[ $? -ne 0 ]]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

echo "🔨 Building..."
make -j$(sysctl -n hw.ncpu)

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Build completed successfully!"
    echo "📁 Plugin files created in: build_intel/AutoTunePlugin_artefacts/"
    echo ""
    echo "🎵 Plugin formats built:"
    ls -la AutoTunePlugin_artefacts/Release/ 2>/dev/null || echo "Check build directory for artifacts"
else
    echo "❌ Build failed!"
    exit 1
fi