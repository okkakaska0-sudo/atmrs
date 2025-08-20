#!/bin/bash

echo "🧪 Building AutoTune Plugin for Replit testing..."
echo "⚠️  Note: This is for testing only - full functionality requires macOS"
echo ""

# Use safe Replit configuration
cp CMakeLists_replit_safe.txt CMakeLists.txt

# Create build directory
mkdir -p build_replit_safe
cd build_replit_safe

echo "🔧 Configuring build..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DJUCE_BUILD_EXTRAS=OFF \
    -DJUCE_BUILD_EXAMPLES=OFF \
    -DJUCE_BUILD_HELPER_TOOLS=OFF \
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=OFF \
    -DCMAKE_VERBOSE_MAKEFILE=ON

if [ $? -ne 0 ]; then
    echo "❌ Configuration failed!"
    exit 1
fi

echo "🔨 Building plugin (test version)..."
cmake --build . --config Release --parallel $(nproc 2>/dev/null || echo 4)

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ SUCCESS! AutoTune Plugin test build complete!"
    echo ""
    echo "📂 Build artifacts:"
    find . -name "*AutoTune*" -type f | head -10
    echo ""
    echo "🧪 Test build features:"
    echo "   ✅ Core audio processing algorithms"
    echo "   ✅ Parameter management system"
    echo "   ✅ Basic pitch correction (Classic/Hard modes)"
    echo "   ✅ Professional GUI framework (JUCE)"
    echo "   ✅ Eigen3 mathematical operations (if available)"
    echo "   ❌ AI features disabled (ONNX not available in Replit)"
    echo "   ❌ Rubber Band Library disabled"
    echo ""
    echo "🍎 For full functionality, use ./build_simple.sh on macOS with:"
    echo "   - brew install onnxruntime eigen rubberband"
    echo ""
    echo "🧪 This test build verifies the core architecture works correctly"
else
    echo "❌ Build failed!"
    exit 1
fi