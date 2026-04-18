#ifndef FASTJSON_H
#define FASTJSON_H

#include <jni.h>
#include <cstdint>
#include <cstddef>

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// JSON value types
#define FJ_TYPE_NULL        0
#define FJ_TYPE_OBJECT      1
#define FJ_TYPE_ARRAY       2
#define FJ_TYPE_STRING      3
#define FJ_TYPE_NUMBER_INT  4
#define FJ_TYPE_NUMBER_LONG 5
#define FJ_TYPE_NUMBER_DOUBLE 6
#define FJ_TYPE_BOOLEAN     7

// SIMD detection
#define FJ_HAS_AVX2  1
#define FJ_HAS_SSE42 2
#define FJ_HAS_NEON  3

// Parse flags
#define FJ_PARSE_LAZY       0x01
#define FJ_PARSE_STRICT     0x02
#define FJ_PARSE_COMMENTS   0x04

// Serialize flags
#define FJ_SERIALIZE_PRETTY 0x01
#define FJ_SERIALIZE_SORTED 0x02

#ifdef __cplusplus
extern "C" {
#endif

// JNI exports
EXPORT JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeParse(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length);

EXPORT JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeParseBuffer(
    JNIEnv* env, jclass clazz, jobject buffer, jint offset, jint length);

EXPORT JNIEXPORT void JNICALL Java_fastjson_FastJSON_nativeFree(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetType(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeGetField(
    JNIEnv* env, jclass clazz, jlong handle, jbyteArray fieldName);

EXPORT JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeGetElement(
    JNIEnv* env, jclass clazz, jlong handle, jint index);

EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetInt(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeGetLong(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jdouble JNICALL Java_fastjson_FastJSON_nativeGetDouble(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jboolean JNICALL Java_fastjson_FastJSON_nativeGetBoolean(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jbyteArray JNICALL Java_fastjson_FastJSON_nativeGetStringBytes(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetArraySize(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetObjectSize(
    JNIEnv* env, jclass clazz, jlong handle);

EXPORT JNIEXPORT jbyteArray JNICALL Java_fastjson_FastJSON_nativeSerialize(
    JNIEnv* env, jclass clazz, jlong handle, jint flags);

// SIMD-accelerated utilities
EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeFindToken(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length, jbyte token);

EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeFindStringEnd(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length);

EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeSkipWhitespace(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length);

#ifdef __cplusplus
}
#endif

// C++ API for internal use
namespace fastjson {

// Forward declarations
class Value;
class Parser;
class Serializer;

// SIMD detection
int detectSimdLevel();
bool hasAvx2();
bool hasSse42();

// SIMD-accelerated scanning
int findTokenSimd(const uint8_t* data, size_t length, uint8_t token);
int findTokenAvx2(const uint8_t* data, size_t length, uint8_t token);
int findTokenSse42(const uint8_t* data, size_t length, uint8_t token);
int findTokenScalar(const uint8_t* data, size_t length, uint8_t token);

int skipWhitespaceSimd(const uint8_t* data, size_t length);
int skipWhitespaceAvx2(const uint8_t* data, size_t length);
int skipWhitespaceSse42(const uint8_t* data, size_t length);
int skipWhitespaceScalar(const uint8_t* data, size_t length);

int findStringEndSimd(const uint8_t* data, size_t length);
int findStringEndAvx2(const uint8_t* data, size_t length);
int findStringEndScalar(const uint8_t* data, size_t length);

// Value handle structure
struct ValueHandle {
    int type;
    union {
        int64_t intValue;
        double doubleValue;
        bool boolValue;
        struct {
            const uint8_t* data;
            size_t length;
        } string;
        struct {
            ValueHandle** elements;
            size_t count;
        } array;
        struct {
            struct Field* fields;
            size_t count;
        } object;
    } data;
    
    // Source reference for lazy parsing
    const uint8_t* sourceData;
    size_t sourceOffset;
    size_t sourceLength;
};

// Parser state
struct ParserState {
    const uint8_t* data;
    size_t length;
    size_t pos;
    int line;
    int column;
    int flags;
};

// Parse functions
ValueHandle* parseJson(ParserState& state);
ValueHandle* parseValue(ParserState& state);
ValueHandle* parseObject(ParserState& state);
ValueHandle* parseArray(ParserState& state);
ValueHandle* parseString(ParserState& state);
ValueHandle* parseNumber(ParserState& state);
ValueHandle* parseLiteral(ParserState& state);

// Memory management
ValueHandle* createValue(int type);
void freeValue(ValueHandle* value);

// Serialization
size_t serializeValue(const ValueHandle* value, uint8_t* buffer, size_t bufferSize, int flags);

} // namespace fastjson

#endif // FASTJSON_H
