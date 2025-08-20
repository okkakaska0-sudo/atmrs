#!/bin/bash

echo "🔍 Searching for all dependencies on macOS..."

# Find Eigen3
echo "📐 Searching for Eigen3..."
EIGEN_PATHS=(
    "/usr/local/include/eigen3"
    "/usr/local/opt/eigen/include/eigen3"
    "/usr/local/Cellar/eigen/*/include/eigen3"
    "/opt/homebrew/include/eigen3"
    "/opt/homebrew/opt/eigen/include/eigen3"
    "/opt/homebrew/Cellar/eigen/*/include/eigen3"
)

for path in "${EIGEN_PATHS[@]}"; do
    if [ -f "$path/Eigen/Dense" ] || [ -f $path/Eigen/Dense ]; then
        echo "  ✅ Found Eigen3: $path"
    fi
done

# Find ONNX Runtime
echo "🤖 Searching for ONNX Runtime..."
ONNX_PATHS=(
    "/usr/local/include"
    "/usr/local/opt/onnxruntime/include"
    "/usr/local/Cellar/onnxruntime/*/include"
    "/opt/homebrew/include"
    "/opt/homebrew/opt/onnxruntime/include"
    "/opt/homebrew/Cellar/onnxruntime/*/include"
)

for path in "${ONNX_PATHS[@]}"; do
    if [ -f "$path/onnxruntime_cxx_api.h" ] || [ -f $path/onnxruntime_cxx_api.h ]; then
        echo "  ✅ Found ONNX Runtime headers: $path"
    fi
done

# Find ONNX Runtime libraries
ONNX_LIB_PATHS=(
    "/usr/local/lib"
    "/usr/local/opt/onnxruntime/lib"
    "/usr/local/Cellar/onnxruntime/*/lib"
    "/opt/homebrew/lib"
    "/opt/homebrew/opt/onnxruntime/lib"
    "/opt/homebrew/Cellar/onnxruntime/*/lib"
)

for path in "${ONNX_LIB_PATHS[@]}"; do
    if [ -f "$path/libonnxruntime.dylib" ] || [ -f $path/libonnxruntime.* ]; then
        echo "  ✅ Found ONNX Runtime library: $path"
    fi
done

# Find Rubber Band
echo "🎵 Searching for Rubber Band..."
RUBBERBAND_PATHS=(
    "/usr/local/include"
    "/usr/local/opt/rubberband/include"
    "/usr/local/Cellar/rubberband/*/include"
    "/opt/homebrew/include"
    "/opt/homebrew/opt/rubberband/include"
    "/opt/homebrew/Cellar/rubberband/*/include"
)

for path in "${RUBBERBAND_PATHS[@]}"; do
    if [ -f "$path/rubberband/RubberBandStretcher.h" ] || [ -f $path/rubberband/RubberBandStretcher.h ]; then
        echo "  ✅ Found Rubber Band headers: $path"
    fi
done

# Show all installed Homebrew packages
echo ""
echo "📦 All Homebrew packages:"
if command -v brew &> /dev/null; then
    brew list | grep -E "(eigen|onnx|rubber)" || echo "  No matching packages found"
else
    echo "  Homebrew not found"
fi

# Try to find via find command
echo ""
echo "🔍 Using find command for comprehensive search..."
echo "Eigen3:"
find /usr/local /opt/homebrew -name "Eigen" -type d 2>/dev/null | head -5

echo "ONNX Runtime:"
find /usr/local /opt/homebrew -name "*onnx*" -type f 2>/dev/null | head -5

echo "Rubber Band:"
find /usr/local /opt/homebrew -name "*rubberband*" -type f 2>/dev/null | head -5