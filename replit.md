# AutoTune Plugin

## Overview

This is a professional audio plugin project that implements real-time pitch correction (AutoTune) functionality using JUCE framework. The plugin provides multiple pitch correction modes including Classic, Hard, and AI-powered correction using CREPE/DDSP models. It features a comprehensive GUI with parameter controls for speed, amount, key/scale selection, and mode switching. The project is designed to create VST3 and AU plugins for macOS with a professional interface similar to industry-standard pitch correction tools.

## User Preferences

Preferred communication style: Simple, everyday language.
Critical requirement: Do NOT simplify the plugin - preserve ALL functionality including:
- Complete audio processing capabilities
- All AI models (ONNX, CREPE, DDSP)
- All mathematical libraries (Eigen3, Kiss FFT, libsamplerate)
- Rubber Band Library integration
- Full VST3 and AU plugin format support
Build environment: Cross-platform build on Replit with full macOS compatibility preservation
Migration status: ✅ COMPLETED - Plugin fully configured for macOS with ALL dependencies installed via Homebrew. Ready for full functionality build including ONNX Runtime, Eigen3, and Rubber Band libraries.

## System Architecture

### Core Framework Architecture
- **JUCE Framework**: Primary audio plugin framework handling audio processing, GUI rendering, and plugin format compatibility
- **CMake Build System**: Cross-platform build configuration with automatic dependency management
- **C++17 Standard**: Modern C++ features for robust audio processing

### Audio Processing Architecture
- **PluginProcessor**: Main DSP pipeline controller managing audio buffer processing and parameter state
- **PitchCorrectionEngine**: Core pitch correction algorithms implementing multiple correction modes
- **Real-time Processing**: Low-latency audio processing optimized for live performance scenarios

### User Interface Architecture
- **Component-based GUI**: JUCE component hierarchy with custom look-and-feel styling
- **PluginEditor**: Main interface controller managing all GUI components and parameter binding
- **LookAndFeel**: Custom styling system for professional interface appearance
- **ModeSelector**: Dynamic interface switching between different correction modes

### Parameter Management
- **Parameters Module**: Centralized parameter definitions and value mapping
- **PresetManager**: User preset save/load functionality with persistent storage
- **Real-time Parameter Updates**: Thread-safe parameter changes during audio processing

### AI Integration Architecture
- **AIModelLoader**: TensorFlow Lite/ONNX model loading and inference management
- **CREPE Integration**: AI-powered pitch detection for enhanced accuracy
- **DDSP Integration**: AI-based audio synthesis for natural-sounding corrections

### Utility Systems
- **Utils Module**: Frequency conversion utilities and mathematical operations
- **Cross-platform Compatibility**: macOS-focused build with VST3/AU format support

## External Dependencies

### Audio Processing Libraries
- **JUCE Framework 7.0.9**: Core audio plugin framework (automatically fetched via CMake)
- **Rubber Band Library**: Professional pitch shifting and time stretching algorithms

### AI/Machine Learning
- **CREPE**: AI-powered pitch detection model for enhanced note recognition
- **DDSP (Differentiable Digital Signal Processing)**: AI synthesis models for natural audio processing
- **TensorFlow Lite or ONNX Runtime**: Machine learning inference engines for AI model execution

### Build Dependencies
- **CMake 3.15+**: Build system configuration and dependency management
- **macOS SDK**: Platform-specific audio unit and VST3 plugin format support

### Development Tools
- **Xcode**: Primary development environment for macOS plugin development
- **Terminal Build Scripts**: Automated build process via `./build_simple.sh` command

The architecture supports professional-grade real-time audio processing with multiple correction modes, AI-enhanced pitch detection, and a modern user interface designed to compete with industry-standard pitch correction tools like Antares Auto-Tune.