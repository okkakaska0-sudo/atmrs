#!/bin/bash

echo "🍎 Installing macOS dependencies for AutoTune Plugin..."
echo "📦 This will install all required libraries via Homebrew"

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew not found! Please install Homebrew first:"
    echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

echo "🔧 Installing dependencies..."

# Install core dependencies
brew install eigen onnxruntime rubberband pkg-config cmake

echo "✅ Dependencies installed!"
echo ""
echo "📍 Verifying installations:"

# Verify installations
if brew list | grep -q eigen; then
    echo "  ✅ Eigen3 installed at: $(brew --prefix eigen)"
else
    echo "  ❌ Eigen3 not found - reinstalling..."
    brew install eigen
fi

if brew list | grep -q onnxruntime; then
    echo "  ✅ ONNX Runtime installed at: $(brew --prefix onnxruntime)"
else
    echo "  ❌ ONNX Runtime not found - reinstalling..."
    brew install onnxruntime
fi

if brew list | grep -q rubberband; then
    echo "  ✅ Rubber Band installed at: $(brew --prefix rubberband)"
else
    echo "  ❌ Rubber Band not found - reinstalling..."
    brew install rubberband
fi

echo ""
echo "🎵 All dependencies installed! Now run:"
echo "   cd /path/to/project"  
echo "   ./build_simple.sh"
echo ""
echo "📋 To verify library locations, run: ./find_dependencies.sh"