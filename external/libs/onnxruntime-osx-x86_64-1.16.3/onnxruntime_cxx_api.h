#pragma once

// ============================================================================
// MINIMAL ONNX RUNTIME C++ API HEADERS FOR COMPILATION
// ============================================================================
// This is a minimal header to allow compilation when full ONNX Runtime 
// headers are not available. For production use, proper headers are needed.

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace Ort {
    
    // Environment
    struct Env {
        static Env& Default() { 
            static Env instance; 
            return instance; 
        }
    };
    
    // Memory info
    struct MemoryInfo {
        static MemoryInfo CreateCpu(int arena_type, int mem_type) {
            return MemoryInfo{};
        }
    };
    
    // Value (tensor)
    template<typename T>
    struct Value {
        static Value CreateTensor(const MemoryInfo& memory_info, T* data, 
                                 size_t data_length, const int64_t* shape, 
                                 size_t shape_length) {
            return Value{};
        }
        
        T* GetTensorMutableData() { 
            static T dummy{};
            return &dummy; 
        }
        
        const T* GetTensorData() const { 
            static T dummy{};
            return &dummy; 
        }
    };
    
    // Session options
    struct SessionOptions {
        void SetIntraOpNumThreads(int num_threads) {}
        void SetGraphOptimizationLevel(int level) {}
        void EnableCpuMemArena() {}
    };
    
    // Session
    struct Session {
        Session(Env& env, const char* model_path, const SessionOptions& options) {}
        Session(Env& env, const void* model_data, size_t model_data_length, 
               const SessionOptions& options) {}
        
        std::vector<Value> Run(const std::vector<const char*>& input_names,
                              const std::vector<Value>& input_values,
                              const std::vector<const char*>& output_names) {
            return std::vector<Value>{};
        }
        
        size_t GetInputCount() const { return 0; }
        size_t GetOutputCount() const { return 0; }
        char* GetInputName(size_t index, void* allocator) const { return nullptr; }
        char* GetOutputName(size_t index, void* allocator) const { return nullptr; }
    };
    
    // Allocator
    struct AllocatorWithDefaultOptions {};
    
    // Run options
    struct RunOptions {};
    
} // namespace Ort

// C API compatibility
extern "C" {
    typedef struct OrtEnv OrtEnv;
    typedef struct OrtSession OrtSession;
    typedef struct OrtValue OrtValue;
    typedef struct OrtMemoryInfo OrtMemoryInfo;
    typedef struct OrtSessionOptions OrtSessionOptions;
    typedef struct OrtAllocator OrtAllocator;
}