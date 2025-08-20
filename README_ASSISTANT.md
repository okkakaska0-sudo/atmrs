# 🤖 AutoTune Plugin - AI Assistant Quick Reference

## 🚨 CRITICAL: macOS-Only Project

This project builds EXCLUSIVELY on macOS. **NEVER attempt Linux builds.**

## 📋 Current Project Status

### ✅ Completed Setup
- All macOS dependencies installed via Homebrew
- Eigen3, ONNX Runtime, Rubber Band libraries configured
- Build system optimized for macOS paths
- VST3, AU, and Standalone formats supported

### 🎯 Working Configuration Files
- **Primary CMake**: `CMakeLists_macos_working.txt` (auto-detects libraries)
- **Build Script**: `build_simple.sh` (macOS-optimized)
- **Dependency Finder**: `find_dependencies.sh` (locates Homebrew libraries)

## 🔧 Dependency Locations (Found via Homebrew)
```
Eigen3:      /usr/local/Cellar/eigen/3.4.0_1/include/eigen3
ONNX Runtime: /usr/local/Cellar/onnxruntime/1.22.2_1/
Rubber Band:  /usr/local/Cellar/rubberband/4.0.0/
```

## 🚀 User Instructions
1. Install dependencies: `brew install eigen onnxruntime rubberband pkg-config cmake`
2. Build plugin: `./build_simple.sh`
3. Use built VST3/AU plugins in DAW

## 🛡️ AI Assistant Guidelines

### DO:
- Use existing `CMakeLists_macos_working.txt` configuration
- Preserve ALL functionality (AI, math libraries, GUI)
- Focus on macOS-native builds
- Use discovered Homebrew library paths

### DON'T:
- Attempt Linux/Replit compilation
- Simplify or remove features
- Use generic library paths
- Modify core architecture without consultation

## 📁 Key Files for AI Reference
- `replit.md` - Main project documentation and context
- `Source/` - All plugin source code
- `CMakeLists_macos_working.txt` - Working build configuration
- `build_simple.sh` - macOS build script

## 🎵 Plugin Features (ALL PRESERVED)
- Real-time pitch correction
- Classic, Hard, and AI correction modes
- ONNX Runtime integration for AI models
- Eigen3 for mathematical operations
- Rubber Band for pitch shifting
- Professional GUI with JUCE framework
- VST3, AU, and Standalone formats

---
**Last Updated**: August 20, 2025
**Status**: Ready for macOS builds with full functionality