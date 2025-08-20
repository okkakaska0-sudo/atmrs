# AutoTune Plugin

## Overview

This is a professional audio plugin project that implements real-time pitch correction (AutoTune) functionality using JUCE framework. The plugin provides multiple pitch correction modes including Classic, Hard, and AI-powered correction using CREPE/DDSP models. It features a comprehensive GUI with parameter controls for speed, amount, key/scale selection, and mode switching. The project is designed to create VST3 and AU plugins for macOS with a professional interface similar to industry-standard pitch correction tools.

## 🚀 Quick Start Instructions for New Repository Clone

**CRITICAL: This project is macOS-only. Do NOT attempt Linux builds.**

### Step 1: Install macOS Dependencies
```bash
# Install Homebrew dependencies
brew install eigen onnxruntime rubberband pkg-config cmake
```

### Step 2: Build Plugin
```bash
cd /path/to/project
./build_simple.sh
```

### 🚀 ТЕКУЩИЙ СТАТУС ПРОЕКТА (20 августа 2025)

**СТАТУС:** Готов к финальной сборке на macOS с полной функциональностью

#### ✅ ВЫПОЛНЕННЫЕ ЭТАПЫ:
1. **Установлены все зависимости через Homebrew:**
   - ✅ Eigen3: `/usr/local/include/eigen3` (математические операции)
   - ✅ ONNX Runtime: `/usr/local/opt/onnxruntime` (AI модели) 
   - ✅ Rubber Band: `/usr/local/lib/librubberband.dylib` (pitch shifting)

2. **Исправлены ошибки компиляции:**
   - ✅ Исправлены предупреждения типов в `PitchCorrectionEngine.cpp` (sign conversion)
   - ✅ Обновлены пути к ONNX Runtime в `CMakeLists_macos_working.txt`
   - ✅ Добавлены условные включения в `AIModelLoader.h` для ONNX
   - ✅ Исправлены JUCE namespace ошибки в `AIModelLoader.cpp`

3. **Настроена сборка:**
   - ✅ Обновлен `build_simple.sh` для работы с ONNX Runtime
   - ✅ Добавлены правильные пути к библиотекам в CMake
   - ✅ Настроена universal binary сборка (x86_64 + ARM64)

#### 🎯 ТЕКУЩИЙ МОМЕНТ:
**ГОТОВ К ФИНАЛЬНОЙ СБОРКЕ НА MACOS:**

Выполните следующие команды на вашем Mac:
```bash
cd /Users/marselmacevans/Downloads/atmrs
brew install onnxruntime  # (если еще не установлен)
./build_simple.sh
```

#### 🔍 ОСТАВШИЕСЯ ЗАДАЧИ:
- Выполнить финальную сборку на macOS
- Протестировать созданный плагин
- Убедиться что VST3/AU файлы работают в DAW

#### 📋 ИСПРАВЛЕННЫЕ ПРОБЛЕМЫ:
- ✅ Исправлены ошибки конструктора в `ModeSelector.cpp`
- ✅ Исправлены права доступа к `build_simple.sh`
- ✅ Добавлена поддержка ONNX Runtime
- ✅ Создана инструкция по установке зависимостей
- ✅ Подтверждено что сборка работает только на macOS (как задумано)

#### 💻 КОМАНДА ДЛЯ ЗАПУСКА:
```bash
cd /Users/marselmacevans/Downloads/atmrs
./build_simple.sh
```

## User Preferences

Preferred communication style: Simple, everyday language.
Critical requirement: Do NOT simplify the plugin - preserve ALL functionality including:
- Complete audio processing capabilities
- All AI models (ONNX, CREPE, DDSP)
- All mathematical libraries (Eigen3, Kiss FFT, libsamplerate)
- Rubber Band Library integration
- Full VST3 and AU plugin format support
Build environment: macOS ONLY - never attempt Linux/Replit builds
**Migration status:** 🔄 ПОЧТИ ЗАВЕРШЕНО - Все зависимости установлены, код исправлен, готов к финальной сборке

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

## 🎯 For AI Assistant: Project Context

**IMPORTANT CONTEXT:**
- This project builds ONLY on macOS with native tools
- All dependencies are installed via Homebrew at specific paths
- Use `CMakeLists_macos_working.txt` configuration (auto-detects libraries)
- Run `./find_dependencies.sh` to verify library locations
- Build script: `./build_simple.sh` (fixes macOS-specific paths)

**NEVER:**
- Attempt Linux builds or Replit compilation
- Simplify functionality or remove AI features
- Use generic dependency paths - use discovered Homebrew paths

**ТЕКУЩИЕ ФАЙЛЫ СБОРКИ:**
- Основная конфигурация: `CMakeLists_macos_working.txt` (с ONNX путями)
- Скрипт сборки: `build_simple.sh` (обновлен для ONNX)
- Поиск зависимостей: `find_dependencies.sh`
- Альтернативная сборка без AI: `build_without_onnx.sh`

**ИСТОРИЯ ИЗМЕНЕНИЙ:**
- 20.08.2025: Исправлены ошибки типов в PitchCorrectionEngine.cpp
- 20.08.2025: Обновлены пути ONNX Runtime в CMake
- 20.08.2025: Исправлены JUCE namespace ошибки
- 20.08.2025: Обновлен build_simple.sh для полной функциональности

The architecture supports professional-grade real-time audio processing with multiple correction modes, AI-enhanced pitch detection, and a modern user interface designed to compete with industry-standard pitch correction tools like Antares Auto-Tune.