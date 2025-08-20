#!/bin/bash

# Build script for AutoTune Plugin on Replit
# Adapted for Linux environment with macOS compatibility mode

echo "🎵 Building ProAutoTune Plugin on Replit..."

# Create build directory
mkdir -p build
cd build

# Set environment variables for macOS compatibility
export MACOS_BUILD=1
export APPLE=1

# Configure with CMake for Linux build with macOS compatibility
echo "📦 Configuring build system..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
         -DCMAKE_INSTALL_PREFIX=../install \
         -DJUCE_BUILD_EXTRAS=OFF \
         -DJUCE_BUILD_EXAMPLES=OFF

# Check if configuration was successful
if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

# Build the plugin
echo "🔨 Building plugin..."
cmake --build . --config Release --parallel $(nproc)

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build completed successfully!"
echo ""
echo "📍 Plugin built in build/ directory"
echo "🎉 Ready for testing!"