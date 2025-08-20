#!/bin/bash

echo "🔧 Installing macOS dependencies for AutoTune Plugin..."

# Install Homebrew if not present
if ! command -v brew &> /dev/null; then
    echo "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install dependencies
echo "Installing dependencies..."
brew install eigen
brew install onnxruntime
brew install rubberband
brew install pkg-config
brew install cmake

echo "✅ macOS dependencies installed successfully!"
echo ""
echo "📋 Installed libraries:"
echo "   - Eigen3: $(brew --prefix eigen)"
echo "   - ONNX Runtime: $(brew --prefix onnxruntime)" 
echo "   - Rubber Band: $(brew --prefix rubberband)"
echo ""
echo "🎯 Ready to build with full functionality on macOS!"