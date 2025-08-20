#!/bin/bash

# Fix ARM64 library architecture mismatch
echo "🔧 Fixing ARM64 library architecture mismatch"
echo "=============================================="

# Check current architecture
ARCH=$(uname -m)
echo "🔍 Current architecture: $ARCH"

if [[ "$ARCH" != "arm64" ]]; then
    echo "❌ This script requires ARM64 (Apple Silicon) Mac"
    exit 1
fi

echo ""
echo "🗑️  Uninstalling Intel libraries..."

# Uninstall current libraries (likely Intel versions)
brew uninstall --ignore-dependencies onnxruntime 2>/dev/null || true
brew uninstall --ignore-dependencies rubberband 2>/dev/null || true
brew uninstall --ignore-dependencies eigen 2>/dev/null || true

echo ""
echo "🧹 Cleaning up old installations..."

# Clean up any Intel remnants in /usr/local
sudo rm -rf /usr/local/lib/libonnxruntime* 2>/dev/null || true
sudo rm -rf /usr/local/lib/librubberband* 2>/dev/null || true
sudo rm -rf /usr/local/include/onnxruntime* 2>/dev/null || true
sudo rm -rf /usr/local/include/rubberband* 2>/dev/null || true
sudo rm -rf /usr/local/include/eigen3* 2>/dev/null || true

echo ""
echo "📦 Installing ARM64 native libraries..."

# Ensure we're using ARM64 Homebrew
export PATH="/opt/homebrew/bin:$PATH"

# Install ARM64 versions
arch -arm64 brew install onnxruntime
arch -arm64 brew install rubberband
arch -arm64 brew install eigen
arch -arm64 brew install pkg-config
arch -arm64 brew install cmake

echo ""
echo "🔍 Verifying ARM64 installations..."

# Check architectures of installed libraries
echo "📋 ONNX Runtime architecture:"
find /opt/homebrew -name "libonnxruntime*" -exec file {} \; 2>/dev/null | head -3

echo ""
echo "📋 Rubber Band architecture:"
find /opt/homebrew -name "librubberband*" -exec file {} \; 2>/dev/null | head -3

echo ""
echo "📋 Installation paths:"
echo "ONNX Runtime: $(find /opt/homebrew -name "libonnxruntime.dylib" 2>/dev/null | head -1)"
echo "Rubber Band: $(find /opt/homebrew -name "librubberband.dylib" 2>/dev/null | head -1)"
echo "Eigen: $(find /opt/homebrew -name "Eigen" -type d 2>/dev/null | head -1)"

echo ""
echo "✅ ARM64 libraries installation completed!"
echo "🎯 Now run: ./build_universal.sh"