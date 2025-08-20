#!/bin/bash

echo "🎵 Building AutoTune Plugin for Replit Linux environment..."
echo "📦 Using Replit-optimized configuration..."

# Clean previous builds
rm -rf build_replit build CMakeCache.txt

# Use the Replit configuration
cp CMakeLists_replit.txt CMakeLists.txt

# Create build directory
mkdir -p build_replit
cd build_replit

# Configure with CMake for Replit
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DJUCE_BUILD_EXTRAS=OFF \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=OFF \
    -DJUCE_BUILD_HELPER_TOOLS=OFF \
    -DCMAKE_VERBOSE_MAKEFILE=ON

# Build the plugin
echo "🔨 Building plugin..."
cmake --build . --config Release --parallel $(nproc) || {
    echo "❌ Build failed!"
    echo "📋 Showing error details..."
    exit 1
}

if [ $? -eq 0 ]; then
    echo "✅ AutoTune Plugin built successfully for Replit!"
    echo "📂 Build artifacts:"
    find . -name "*.so" -o -name "*.vst3" -o -name "AutoTunePlugin*" -type f | head -10
else
    echo "❌ Build failed!"
    exit 1
fi