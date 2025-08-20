#!/bin/bash

echo "🎵 Building AutoTune Plugin with FULL macOS functionality..."
echo "📦 Configuring build system..."

# Use working macOS configuration that finds all available dependencies
cp CMakeLists_macos_working.txt CMakeLists.txt

# Create build directory
mkdir -p build
cd build

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
cmake --build . --config Release --parallel $(sysctl -n hw.ncpu 2>/dev/null || echo 4) || {
    echo "❌ Build failed!"
    echo "📋 Showing error details..."
    exit 1
}

if [ $? -eq 0 ]; then
    echo "✅ AutoTune Plugin built successfully!"
    echo "📂 Build artifacts:"
    find . -name "*.so" -o -name "*.vst3" -o -name "AutoTunePlugin*" -type f | head -10
else
    echo "❌ Build failed!"
    exit 1
fi
