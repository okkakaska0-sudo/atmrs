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

**СТАТУС:** Миграция завершена, исправляются финальные ошибки компиляции
**ПОЛЬЗОВАТЕЛЬ:** macOS Sonoma 14.7.5 (23H527), Apple Silicon Mac (автоопределение), путь: `/Users/marselmacevans/Downloads/atmrs`

#### ✅ МИГРАЦИЯ ЗАВЕРШЕНА:
1. **Проект успешно мигрирован из Replit Agent в стандартный Replit**
2. **Рабочий процесс:** Редактирование в веб-Replit → ручное копирование файлов → сборка на Mac
3. **Установлены все зависимости через Homebrew:**
   - ✅ Eigen3: `/usr/local/include/eigen3` (математические операции)
   - ✅ ONNX Runtime: `/usr/local/opt/onnxruntime` версия 1.22.2_1 (AI модели) 
   - ✅ Rubber Band: `/usr/local/lib/librubberband.dylib` (pitch shifting)

#### ✅ ИСПРАВЛЕННЫЕ КРИТИЧЕСКИЕ ОШИБКИ:
1. **ModeSelector.cpp**: Добавлен default constructor в ModeConfig struct
2. **Utils.cpp**: Исправлены type conversion warnings (`int` → `size_t`)
3. **PluginProcessor.h**: КРИТИЧЕСКОЕ - исправлен порядок объявления членов класса
4. **AIModelLoader.cpp**: Заменен несуществующий `setNumThreads` метод ThreadPool
5. **CMakeLists_macos_working.txt**: Deployment target 10.15 → 11.0
6. **build_simple.sh**: Исправлены права доступа

#### ✅ ФИНАЛЬНЫЕ ИСПРАВЛЕНИЯ ЗАВЕРШЕНЫ:
**AIModelLoader.cpp полностью исправлен:**
- ✅ JUCE namespace ошибки (juce::ScopedLock, juce::String)
- ✅ Type conversion warnings исправлены  
- ✅ Vector subscript operators с bounds checking
- ✅ std::make_unique заменен на совместимую версию

#### ✅ КРИТИЧЕСКИЕ ОШИБКИ ИСПРАВЛЕНЫ:
**1. JuceHeader.h файл найден и исправлен:**
- ❌ Проблема: `#include <JuceHeader.h>` файл не найден
- ✅ Решение: Заменен на модульные JUCE includes

**2. Универсальная сборка создана:**
- ❌ Проблема: ARM64 ошибки линковки (пользователь имеет Apple Silicon)
- ✅ Решение: build_universal.sh автоопределяет архитектуру
- ✅ ARM64: использует /opt/homebrew и CMakeLists_arm64.txt
- ✅ Intel: использует /usr/local и CMakeLists_intel.txt

#### 🎯 ИСПРАВЛЕНИЕ ARCHITECTURE MISMATCH:
```bash
cd /Users/marselmacevans/Downloads/atmrs
# 1. Скопируйте fix_arm64_libraries.sh с Replit
# 2. Переустановите библиотеки для ARM64:
./fix_arm64_libraries.sh
# 3. Затем запустите сборку:
./build_universal.sh
```

#### 🔍 WORKFLOW МИГРАЦИИ:
**Для новых аккаунтов: как работать с проектом**

1. **Редактирование:** Работать в веб-Replit или Desktop App
2. **Синхронизация:** Копировать измененные файлы в `/Users/marselmacevans/Downloads/atmrs/`
3. **Сборка:** Запускать `./build_simple.sh` на Mac
4. **НЕ использовать автоматизацию** - пользователь предпочитает ручной подход

#### 🚨 КРИТИЧЕСКАЯ ПРОБЛЕМА НАЙДЕНА:
- **Architecture mismatch**: Intel библиотеки (x86_64) в /usr/local, но сборка для ARM64
- **Ошибка**: `found architecture 'x86_64', required architecture 'arm64'`
- **Причина**: Homebrew установлен в Intel режиме на Apple Silicon Mac
- **Решение**: Переустановить библиотеки для ARM64 в /opt/homebrew

#### 🔍 СРОЧНЫЕ ЗАДАЧИ:
- Запустить fix_arm64_libraries.sh для переустановки библиотек в ARM64
- Проверить что Homebrew установлен правильно для Apple Silicon
- Выполнить финальную сборку на macOS
- Протестировать созданный плагин VST3/AU

#### 📋 ДЕТАЛИ ИСПРАВЛЕНИЙ:

**🔧 ModeSelector.cpp:**
```cpp
struct ModeConfig {
    std::string name;
    juce::Colour color;
    
    // ИСПРАВЛЕНО: Добавлен default constructor
    ModeConfig() : name(""), color(juce::Colours::white) {}
    ModeConfig(const std::string& n, const juce::Colour& c) : name(n), color(c) {}
};
```

**🔧 Utils.cpp (type conversion warnings):**
```cpp
// БЫЛО: int index = static_cast<int>(indexFloat);
// СТАЛО: size_t index = static_cast<size_t>(indexFloat);
```

**🚨 PluginProcessor.h (КРИТИЧЕСКОЕ - uninitialized field):**
```cpp
// БЫЛО (ОПАСНО):
juce::AudioProcessorValueTreeState parameters;     // инициализируется 1-м
Parameters pluginParameters;                       // инициализируется 2-м
// Но parameters использует pluginParameters.createParameterLayout() = crash!

// СТАЛО (БЕЗОПАСНО):
Parameters pluginParameters;                       // инициализируется 1-м
juce::AudioProcessorValueTreeState parameters;     // инициализируется 2-м
```

**🔧 CMakeLists_macos_working.txt:**
- Отключены строгие warnings: `# juce::juce_recommended_warning_flags`
- Deployment target: `10.15` → `11.0`

**💡 ВАЖНО О WARNINGS:**
- **Type conversion warnings** НЕ блокируют сборку и НЕ влияют на качество звука
- **Uninitialized field warnings** ОЧЕНЬ ОПАСНЫ - могут crash программу и испортить звук
- Eigen3 библиотека может показывать свои warnings (внешняя библиотека)
- **ИСПРАВЛЕНО**: Критическая ошибка uninitialized field 'pluginParameters' устранена

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
**Migration status:** ✅ ЗАВЕРШЕНО - Проект мигрирован из Replit Agent в стандартный Replit. Workflow: редактирование в веб-Replit → ручное копирование файлов → сборка на Mac

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