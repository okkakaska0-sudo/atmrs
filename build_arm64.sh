#!/bin/bash

# AutoTune Plugin ARM64 Build Script
# For Apple Silicon Macs with Homebrew in /opt/homebrew

echo "🍎 AutoTune Plugin - ARM64 (Apple Silicon) Build"
echo "================================================="

# Make script executable
chmod +x "$0"

# Check for ARM64 architecture
if [[ $(uname -m) != "arm64" ]]; then
    echo "❌ This script is for ARM64 (Apple Silicon) Macs only"
    echo "Your architecture: $(uname -m)"
    exit 1
fi

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew not found. Install it first:"
    echo "/bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

echo "✅ ARM64 Mac detected"
echo "✅ Homebrew found at: $(which brew)"

# Check dependencies
echo ""
echo "🔍 Checking dependencies..."

# Check for ONNX Runtime
ONNX_PATH=$(find /opt/homebrew -name "libonnxruntime*" 2>/dev/null | head -1)
if [[ -n "$ONNX_PATH" ]]; then
    echo "✅ ONNX Runtime found: $ONNX_PATH"
else
    echo "❌ ONNX Runtime not found. Installing..."
    brew install onnxruntime
fi

# Check for Rubber Band
RUBBER_PATH=$(find /opt/homebrew -name "librubberband*" 2>/dev/null | head -1)
if [[ -n "$RUBBER_PATH" ]]; then
    echo "✅ Rubber Band found: $RUBBER_PATH"
else
    echo "❌ Rubber Band not found. Installing..."
    brew install rubberband
fi

# Check for Eigen
EIGEN_PATH=$(find /opt/homebrew -name "Eigen" -type d 2>/dev/null | head -1)
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

# Use ARM64 specific CMakeLists
if [[ ! -f "CMakeLists_arm64.txt" ]]; then
    echo "❌ CMakeLists_arm64.txt not found!"
    exit 1
fi

# Copy ARM64 config
cp CMakeLists_arm64.txt CMakeLists.txt

# Create build directory
mkdir -p build_arm64
cd build_arm64

# Configure with ARM64 specific settings
echo "🔧 Configuring for ARM64..."
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=/opt/homebrew

if [[ $? -ne 0 ]]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

echo "🔨 Building..."
make -j$(sysctl -n hw.ncpu)

if [[ $? -eq 0 ]]; then
    echo ""
    echo "✅ Build completed successfully!"
    echo "📁 Plugin files created in: build_arm64/AutoTunePlugin_artefacts/"
    echo ""
    echo "🎵 Plugin formats built:"
    ls -la AutoTunePlugin_artefacts/Release/ 2>/dev/null || echo "Check build directory for artifacts"
else
    echo "❌ Build failed!"
    exit 1
fi