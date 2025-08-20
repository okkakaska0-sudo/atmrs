#!/bin/bash

# Check ARM64 library locations for debugging
echo "🔍 Checking ARM64 Homebrew libraries..."
echo "======================================="

HOMEBREW_PREFIX="/opt/homebrew"

echo "📁 Homebrew prefix: $HOMEBREW_PREFIX"
echo ""

echo "🔎 ONNX Runtime locations:"
find $HOMEBREW_PREFIX -name "*onnxruntime*" 2>/dev/null | head -10
echo ""

echo "🔎 Rubber Band locations:"
find $HOMEBREW_PREFIX -name "*rubberband*" 2>/dev/null | head -10
echo ""

echo "🔎 Eigen locations:"
find $HOMEBREW_PREFIX -name "Eigen" -type d 2>/dev/null | head -5
echo ""

echo "🔎 ONNX Runtime headers:"
find $HOMEBREW_PREFIX -name "onnxruntime_cxx_api.h" 2>/dev/null
find $HOMEBREW_PREFIX -name "onnxruntime_c_api.h" 2>/dev/null
echo ""

echo "🔎 Rubber Band headers:"
find $HOMEBREW_PREFIX -name "RubberBandStretcher.h" 2>/dev/null
echo ""

echo "🔎 ONNX Runtime dylibs:"
find $HOMEBREW_PREFIX -name "*onnxruntime*.dylib" 2>/dev/null
echo ""

echo "🔎 Rubber Band dylibs:"
find $HOMEBREW_PREFIX -name "*rubberband*.dylib" 2>/dev/null
echo ""

echo "🏗️ Checking brew installations:"
brew list | grep -E "(onnxruntime|rubberband|eigen)" || echo "None found via brew list"
echo ""

echo "📋 Brew info:"
brew info onnxruntime 2>/dev/null | grep "Installed" || echo "ONNX Runtime not installed"
brew info rubberband 2>/dev/null | grep "Installed" || echo "Rubber Band not installed"
brew info eigen 2>/dev/null | grep "Installed" || echo "Eigen not installed"