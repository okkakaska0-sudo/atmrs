#!/bin/bash

# Install dependencies for macOS
echo "🔧 Installing macOS dependencies..."

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew not found. Please install Homebrew first:"
    echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

# Install Rubber Band Library (optional but recommended)
echo "📦 Installing Rubber Band Library..."
brew install rubberband

# Install CMake if not present
if ! command -v cmake &> /dev/null; then
    echo "📦 Installing CMake..."
    brew install cmake
fi

echo "✅ Dependencies installed successfully!"
echo ""
echo "Now you can build the plugin:"
echo "   ./build_simple.sh"