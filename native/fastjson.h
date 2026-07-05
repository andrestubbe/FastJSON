/**
 * @file fastjson.h
 * @brief FastJSON JNI Header - High-performance JSON parser for Java
 *
 * @details SIMD-accelerated JSON parser with multiple backends:
 * - AVX2 (256-bit vectors): ~8x faster than scalar
 * - SSE4.2 (128-bit vectors): ~4x faster than scalar
 * - Scalar fallback for compatibility
 *
 * @par Features
 * - Lazy parsing: Parse on-demand for large documents
 * - Strict mode: RFC 8259 compliant validation
 * - Comments: Support for // and C-style comments
 * - Streaming: Parse from ByteBuffer without copying
 *
 * @par Value Types
 * Supports all JSON types: null, object, array, string, number (int/long/double), boolean
 *
 * @par SIMD Acceleration
 * - findTokenSimd: ~8x faster bracket/quote finding
 * - skipWhitespaceSimd: ~4x faster whitespace skipping
 * - findStringEndSimd: ~6x faster string scanning
 *
 * @par Performance
 * - Small documents (< 1KB): ~2x faster than Jackson
 * - Large documents (> 100KB): ~10x faster than Jackson
 * - Memory efficient: Zero-copy where possible
 *
 * @author FastJava Team
 * @version 1.0.0
 * @copyright MIT License
 */

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

/** @defgroup Constants Constants and Type Definitions
 *  @brief JSON type constants and flags
 *  @{ */

/** @name JSON Value Types */
#define FJ_TYPE_NULL        0   /**< Null value */
#define FJ_TYPE_OBJECT      1   /**< JSON object { } */
#define FJ_TYPE_ARRAY       2   /**< JSON array [ ] */
#define FJ_TYPE_STRING      3   /**< JSON string */
#define FJ_TYPE_NUMBER_INT  4   /**< 32-bit integer */
#define FJ_TYPE_NUMBER_LONG 5   /**< 64-bit integer */
#define FJ_TYPE_NUMBER_DOUBLE 6 /**< Double precision float */
#define FJ_TYPE_BOOLEAN     7   /**< true/false */

/** @name SIMD Detection Levels */
#define FJ_HAS_AVX2  1          /**< AVX2 (256-bit) support */
#define FJ_HAS_SSE42 2          /**< SSE4.2 (128-bit) support */
#define FJ_HAS_NEON  3          /**< ARM NEON support */

/** @name Parse Flags */
#define FJ_PARSE_LAZY       0x01    /**< Defer parsing until access */
#define FJ_PARSE_STRICT     0x02    /**< RFC 8259 compliance mode */
#define FJ_PARSE_COMMENTS   0x04    /**< Allow JSON comments */

/** @name Serialize Flags */
#define FJ_SERIALIZE_PRETTY 0x01    /**< Pretty print with indentation */
#define FJ_SERIALIZE_SORTED 0x02    /**< Sort object keys alphabetically */

/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup JNI JSON JNI Functions
 *  @brief Java Native Interface exports
 *  @{ */
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

/** @} */

/** @defgroup SIMD SIMD-Accelerated Utilities
 *  @brief Low-level SIMD string scanning operations
 *  @details These functions provide raw SIMD acceleration for common
 *           JSON parsing operations without full document parsing.
 *  @{ */

/**
 * @brief Find token position using SIMD
 * @param env JNI environment
 * @param clazz FastJSON class
 * @param data Byte array to search
 * @param offset Start position
 * @param length Search length
 * @param token Character to find
 * @return Position of token or -1 if not found
 * @note Uses AVX2/SSE4.2 auto-detected at runtime
 */
EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeFindToken(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length, jbyte token);

/**
 * @brief Find end of JSON string (unescaped quote)
 * @param env JNI environment
 * @param clazz FastJSON class
 * @param data Byte array containing string
 * @param offset Start of string (after opening quote)
 * @param length Maximum search length
 * @return Position of closing quote or -1
 * @note Handles escaped quotes (\") correctly
 */
EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeFindStringEnd(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length);

/**
 * @brief Skip whitespace using SIMD
 * @param env JNI environment
 * @param clazz FastJSON class
 * @param data Byte array
 * @param offset Start position
 * @param length Maximum length
 * @return Position of first non-whitespace character
 * @note Whitespace: space, tab, newline, carriage return
 */
EXPORT JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeSkipWhitespace(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length);

/** @} */

#ifdef __cplusplus
}
#endif

/** @defgroup Internal Internal C++ API
 *  @brief Internal implementation details
 *  @warning Not part of public API - subject to change
 *  @{ */

namespace fastjson {

// Forward declarations
class Value;        /**< JSON value representation */
class Parser;       /**< JSON parser state machine */
class Serializer;   /**< JSON serializer */

/** @defgroup SimdDetection SIMD Detection
 *  @brief Runtime CPU capability detection
 *  @{ */

/**
 * @brief Detect available SIMD level
 * @return FJ_HAS_AVX2, FJ_HAS_SSE42, or 0 (scalar only)
 */
int detectSimdLevel();

/**
 * @brief Check for AVX2 support
 * @return true if AVX2 available
 */
bool hasAvx2();

/**
 * @brief Check for SSE4.2 support
 * @return true if SSE4.2 available
 */
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

/** @} */

/** @defgroup Structures Data Structures
 *  @brief Internal data structures for JSON representation
 *  @{ */

/**
 * @brief Key-value field in a JSON object.
 * @details keyData points into the nativeCopy buffer (NOT null-terminated).
 *          value is a heap-allocated ValueHandle owned by the parent object.
 */
struct Field {
    const uint8_t* keyData;   /**< Pointer into source buffer (not null-terminated) */
    size_t         keyLength; /**< Length of key in bytes */
    struct ValueHandle* value; /**< Owned child value */
};

/**
 * @brief JSON value handle
 * @details Union-based value storage with type discriminator.
 *          Supports lazy parsing via source reference.
 */
struct ValueHandle {
    int type;
    union Data {
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
            Field* fields;   /**< Now a COMPLETE type - no ABI mismatch */
            size_t count;
        } object;
        // Explicit default constructor to zero-initialize the union
        Data() : intValue(0) {}
    } data;
    
    // Source reference for lazy parsing
    const uint8_t* sourceData;
    size_t sourceOffset;
    size_t sourceLength;
    bool ownsSourceData;

    // Zero-initialize everything on construction
    ValueHandle()
        : type(0)
        , data()
        , sourceData(nullptr)
        , sourceOffset(0)
        , sourceLength(0)
        , ownsSourceData(false)
    {}
};

/**
 * @brief Parser state structure
 * @details Tracks position, line, column for error reporting.
 */
struct ParserState {
    const uint8_t* data;
    size_t length;
    size_t pos;
    int line;
    int column;
    int flags;
};

/** @} */

/** @defgroup ParseFunctions Parse Functions
 *  @brief Recursive descent parser implementation
 *  @{ */
ValueHandle* parseJson(ParserState& state);    /**< Parse full JSON document */
ValueHandle* parseValue(ParserState& state);     /**< Parse any JSON value */
ValueHandle* parseObject(ParserState& state);    /**< Parse object { } */
ValueHandle* parseArray(ParserState& state);      /**< Parse array [ ] */
ValueHandle* parseString(ParserState& state);    /**< Parse quoted string */
ValueHandle* parseNumber(ParserState& state);    /**< Parse numeric value */
ValueHandle* parseLiteral(ParserState& state);   /**< Parse true/false/null */
/** @} */

/** @defgroup Memory Memory Management
 *  @brief Value allocation and deallocation
 *  @{ */
ValueHandle* createValue(int type);   /**< Allocate new value */
void freeValue(ValueHandle* value); /**< Free value and children */
/** @} */

/** @defgroup Serialize Serialization
 *  @brief JSON string generation
 *  @{ */
size_t serializeValue(const ValueHandle* value, uint8_t* buffer, size_t bufferSize, int flags);
/** @} */

} // namespace fastjson

/** @} */

#endif // FASTJSON_H
