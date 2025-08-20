#pragma once

// ============================================================================
// MINIMAL ONNX RUNTIME C API HEADERS FOR COMPILATION
// ============================================================================

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct OrtEnv OrtEnv;
typedef struct OrtSession OrtSession;
typedef struct OrtValue OrtValue;
typedef struct OrtMemoryInfo OrtMemoryInfo;
typedef struct OrtSessionOptions OrtSessionOptions;
typedef struct OrtAllocator OrtAllocator;
typedef struct OrtApi OrtApi;

// Status codes
typedef enum ONNXTensorElementDataType {
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED = 0,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT = 1,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8 = 2,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8 = 3,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16 = 4,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16 = 5,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32 = 6,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64 = 7,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING = 8,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL = 9,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16 = 10,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE = 11,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32 = 12,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64 = 13
} ONNXTensorElementDataType;

typedef enum OrtErrorCode {
    ORT_OK,
    ORT_FAIL,
    ORT_INVALID_ARGUMENT,
    ORT_NO_SUCHFILE,
    ORT_NO_MODEL,
    ORT_ENGINE_ERROR,
    ORT_RUNTIME_EXCEPTION,
    ORT_INVALID_PROTOBUF,
    ORT_MODEL_LOADED,
    ORT_NOT_IMPLEMENTED,
    ORT_INVALID_GRAPH,
    ORT_EP_FAIL,
} OrtErrorCode;

typedef struct OrtStatus OrtStatus;

// Minimal API structure
struct OrtApi {
    OrtStatus* (*CreateEnv)(int log_level, const char* logid, OrtEnv** env);
    OrtStatus* (*CreateSession)(const OrtEnv* env, const char* model_path, 
                               const OrtSessionOptions* options, OrtSession** session);
    void (*ReleaseEnv)(OrtEnv* env);
    void (*ReleaseSession)(OrtSession* session);
    void (*ReleaseStatus)(OrtStatus* status);
    OrtErrorCode (*GetErrorCode)(const OrtStatus* status);
    const char* (*GetErrorMessage)(const OrtStatus* status);
};

// Global API accessor
const OrtApi* OrtGetApiBase(void);

#ifdef __cplusplus
}
#endif