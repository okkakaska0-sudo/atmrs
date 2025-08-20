#!/bin/bash

echo "🔍 SYSTEM ARCHITECTURE DIAGNOSIS"
echo "================================="

echo ""
echo "📋 System Information:"
echo "Architecture: $(uname -m)"
echo "System: $(uname -s)"
echo "Kernel: $(uname -r)"

echo ""
echo "📋 Hardware Information:"
sysctl -n machdep.cpu.brand_string 2>/dev/null || echo "CPU info not available"

echo ""
echo "📋 Compiler Information:"
echo "Default compiler: $(which clang++)"
clang++ --version | head -1

echo ""
echo "📋 Architecture Support:"
echo "Available architectures:"
lipo -archs /usr/bin/clang++ 2>/dev/null || echo "lipo check failed"

echo ""
echo "📋 Homebrew Installation:"
echo "Homebrew location: $(which brew)"
brew --version | head -1

echo ""
echo "📋 Library Architecture Check:"

# Check ONNX Runtime
ONNX_LIB=$(find /usr/local -name "libonnxruntime*.dylib" 2>/dev/null | head -1)
if [[ -n "$ONNX_LIB" ]]; then
    echo ""
    echo "ONNX Runtime Library:"
    echo "Location: $ONNX_LIB"
    file "$ONNX_LIB"
    lipo -archs "$ONNX_LIB" 2>/dev/null || echo "lipo failed"
fi

# Check Rubber Band
RUBBER_LIB=$(find /usr/local -name "librubberband*.dylib" 2>/dev/null | head -1)
if [[ -n "$RUBBER_LIB" ]]; then
    echo ""
    echo "Rubber Band Library:"
    echo "Location: $RUBBER_LIB"
    file "$RUBBER_LIB"
    lipo -archs "$RUBBER_LIB" 2>/dev/null || echo "lipo failed"
fi

echo ""
echo "📋 Conclusion:"
if [[ "$(uname -m)" == "x86_64" ]]; then
    echo "✅ This is an Intel x64 Mac"
    echo "✅ Should use /usr/local Homebrew libraries"
    echo "✅ Must build ONLY for x86_64 architecture"
else
    echo "⚠️  This appears to be Apple Silicon Mac"
    echo "⚠️  Should use /opt/homebrew libraries"
    echo "⚠️  Can build for arm64 architecture"
fi