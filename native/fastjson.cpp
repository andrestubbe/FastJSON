#include "fastjson.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>

// Platform-specific SIMD headers
#ifdef _WIN32
    #include <intrin.h>
    static inline int ctz(unsigned int mask) {
        unsigned long index;
        if (_BitScanForward(&index, mask)) return index;
        return 32;
    }
#else
    #include <x86intrin.h>
    #define ctz(mask) __builtin_ctz(mask)
#endif

namespace fastjson {

// Global SIMD level
static int g_simdLevel = 0;

// Initialize SIMD detection
class SimdInitializer {
public:
    SimdInitializer() {
        g_simdLevel = detectSimdLevel();
    }
};
static SimdInitializer g_simdInitializer;

// SIMD detection using CPUID
int detectSimdLevel() {
#ifdef _WIN32
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 1);
    
    // Check AVX2 (bit 5 of EBX for leaf 7)
    int cpuInfo7[4] = {0};
    __cpuidex(cpuInfo7, 7, 0);
    if (cpuInfo7[1] & (1 << 5)) {
        return FJ_HAS_AVX2;
    }
    
    // Check SSE4.2 (bit 20 of ECX for leaf 1)
    if (cpuInfo[2] & (1 << 20)) {
        return FJ_HAS_SSE42;
    }
#else
    // Linux/Mac detection
    unsigned int eax, ebx, ecx, edx;
    
    // Check for AVX2
    __cpuid(7, eax, ebx, ecx, edx);
    if (ebx & (1 << 5)) {
        return FJ_HAS_AVX2;
    }
    
    // Check for SSE4.2
    __cpuid(1, eax, ebx, ecx, edx);
    if (ecx & (1 << 20)) {
        return FJ_HAS_SSE42;
    }
#endif
    return 0;
}

bool hasAvx2() {
    return g_simdLevel >= FJ_HAS_AVX2;
}

bool hasSse42() {
    return g_simdLevel >= FJ_HAS_SSE42;
}

// ============================================================================
// SIMD-ACCELERATED TOKEN FINDING
// ============================================================================

int findTokenSimd(const uint8_t* data, size_t length, uint8_t token) {
    if (hasAvx2() && length >= 32) {
        return findTokenAvx2(data, length, token);
    } else if (hasSse42() && length >= 16) {
        return findTokenSse42(data, length, token);
    }
    return findTokenScalar(data, length, token);
}

// AVX2: Process 32 bytes at once
int findTokenAvx2(const uint8_t* data, size_t length, uint8_t token) {
    const __m256i target = _mm256_set1_epi8(token);
    size_t i = 0;
    
    // Process 32-byte chunks
    for (; i + 32 <= length; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i cmp = _mm256_cmpeq_epi8(chunk, target);
        int mask = _mm256_movemask_epi8(cmp);
        
        if (mask != 0) {
            return static_cast<int>(i + ctz(mask));
        }
    }
    
    // Handle remaining bytes
    return findTokenScalar(data + i, length - i, token);
}

// SSE4.2: Process 16 bytes at once
int findTokenSse42(const uint8_t* data, size_t length, uint8_t token) {
    const __m128i target = _mm_set1_epi8(token);
    size_t i = 0;
    
    // Process 16-byte chunks
    for (; i + 16 <= length; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        __m128i cmp = _mm_cmpeq_epi8(chunk, target);
        int mask = _mm_movemask_epi8(cmp);
        
        if (mask != 0) {
            return static_cast<int>(i + ctz(mask));
        }
    }
    
    // Handle remaining bytes
    return findTokenScalar(data + i, length - i, token);
}

// Scalar fallback
int findTokenScalar(const uint8_t* data, size_t length, uint8_t token) {
    for (size_t i = 0; i < length; i++) {
        if (data[i] == token) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ============================================================================
// SIMD-ACCELERATED WHITESPACE SKIPPING
// ============================================================================

int skipWhitespaceSimd(const uint8_t* data, size_t length) {
    if (hasAvx2() && length >= 32) {
        return skipWhitespaceAvx2(data, length);
    } else if (hasSse42() && length >= 16) {
        return skipWhitespaceSse42(data, length);
    }
    return skipWhitespaceScalar(data, length);
}

// Create comparison masks for whitespace: ' ', '\t', '\n', '\r'
static inline bool isWhitespace(uint8_t c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int skipWhitespaceAvx2(const uint8_t* data, size_t length) {
    const __m256i space = _mm256_set1_epi8(' ');
    const __m256i tab = _mm256_set1_epi8('\t');
    const __m256i newline = _mm256_set1_epi8('\n');
    const __m256i carriage = _mm256_set1_epi8('\r');
    
    size_t i = 0;
    for (; i + 32 <= length; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        
        __m256i cmp1 = _mm256_cmpeq_epi8(chunk, space);
        __m256i cmp2 = _mm256_cmpeq_epi8(chunk, tab);
        __m256i cmp3 = _mm256_cmpeq_epi8(chunk, newline);
        __m256i cmp4 = _mm256_cmpeq_epi8(chunk, carriage);
        
        __m256i ws = _mm256_or_si256(_mm256_or_si256(cmp1, cmp2), 
                                     _mm256_or_si256(cmp3, cmp4));
        
        int mask = _mm256_movemask_epi8(ws);
        // Invert mask - we want first NON-whitespace
        mask = ~mask & 0xFFFFFFFF;
        
        if (mask != 0) {
            return static_cast<int>(i + ctz(mask));
        }
    }
    
    return skipWhitespaceScalar(data + i, length - i) + static_cast<int>(i);
}

int skipWhitespaceSse42(const uint8_t* data, size_t length) {
    const __m128i space = _mm_set1_epi8(' ');
    const __m128i tab = _mm_set1_epi8('\t');
    const __m128i newline = _mm_set1_epi8('\n');
    const __m128i carriage = _mm_set1_epi8('\r');
    
    size_t i = 0;
    for (; i + 16 <= length; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        
        __m128i cmp1 = _mm_cmpeq_epi8(chunk, space);
        __m128i cmp2 = _mm_cmpeq_epi8(chunk, tab);
        __m128i cmp3 = _mm_cmpeq_epi8(chunk, newline);
        __m128i cmp4 = _mm_cmpeq_epi8(chunk, carriage);
        
        __m128i ws = _mm_or_si128(_mm_or_si128(cmp1, cmp2), 
                                  _mm_or_si128(cmp3, cmp4));
        
        int mask = _mm_movemask_epi8(ws);
        mask = ~mask & 0xFFFF;
        
        if (mask != 0) {
            return static_cast<int>(i + ctz(mask));
        }
    }
    
    return skipWhitespaceScalar(data + i, length - i) + static_cast<int>(i);
}

int skipWhitespaceScalar(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (!isWhitespace(data[i])) {
            return static_cast<int>(i);
        }
    }
    return static_cast<int>(length);
}

// ============================================================================
// FIND STRING END (handles escaped quotes)
// ============================================================================

int findStringEndSimd(const uint8_t* data, size_t length) {
    // String parsing is harder to SIMD-ize due to escape handling
    // Use scalar for now, optimize later
    return findStringEndScalar(data, length);
}

int findStringEndScalar(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (data[i] == '"') {
            // Check for escape
            int backslashes = 0;
            size_t j = i;
            while (j > 0 && data[j - 1] == '\\') {
                backslashes++;
                j--;
            }
            // Odd number of backslashes means quote is escaped
            if (backslashes % 2 == 0) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

} // namespace fastjson

// ============================================================================
// JNI IMPLEMENTATIONS
// ============================================================================

// Note: Field struct is defined in fastjson.h before ValueHandle.

using namespace fastjson;

JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeParse(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length) {
    
    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    if (!bytes) return 0;
    
    // Allocate native copy to prevent dangling pointer issues after JNI release
    uint8_t* nativeCopy = new uint8_t[length];
    std::memcpy(nativeCopy, reinterpret_cast<const uint8_t*>(bytes) + offset, length);
    
    // Create parser state using native copy
    ParserState state;
    state.data = nativeCopy;
    state.length = static_cast<size_t>(length);
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.flags = FJ_PARSE_LAZY;
    
    // Parse
    ValueHandle* result = parseJson(state);
    
    env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
    
    if (result) {
        result->sourceData = nativeCopy;
        result->ownsSourceData = true;
    } else {
        delete[] nativeCopy;
    }
    
    return reinterpret_cast<jlong>(result);
}

JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeParseBuffer(
    JNIEnv* env, jclass clazz, jobject buffer, jint offset, jint length) {
    
    // Get direct buffer address
    void* address = env->GetDirectBufferAddress(buffer);
    if (!address) return 0;
    
    // Allocate native copy to prevent dangling pointer issues
    uint8_t* nativeCopy = new uint8_t[length];
    std::memcpy(nativeCopy, static_cast<const uint8_t*>(address) + offset, length);
    
    ParserState state;
    state.data = nativeCopy;
    state.length = static_cast<size_t>(length);
    state.pos = 0;
    state.line = 1;
    state.column = 1;
    state.flags = FJ_PARSE_LAZY;
    
    ValueHandle* result = parseJson(state);
    
    if (result) {
        result->sourceData = nativeCopy;
        result->ownsSourceData = true;
    } else {
        delete[] nativeCopy;
    }
    
    return reinterpret_cast<jlong>(result);
}

JNIEXPORT void JNICALL Java_fastjson_FastJSON_nativeFree(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    // Sanity check: any real heap pointer on 64-bit Windows/Linux is
    // well above 64KB (the first 64KB of address space is always unmapped).
    // If the handle is suspiciously small, it is a corrupted value - skip it
    // to avoid a guaranteed EXCEPTION_ACCESS_VIOLATION.
    if (handle == 0 || handle < 0x10000LL) return;
    
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    freeValue(value);
}

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetType(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return FJ_TYPE_NULL;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    return value->type;
}

JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeGetField(
    JNIEnv* env, jclass clazz, jlong handle, jbyteArray fieldName) {
    
    if (handle == 0) return 0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    if (value->type != FJ_TYPE_OBJECT) return 0;
    
    // Get field name bytes
    jsize nameLen = env->GetArrayLength(fieldName);
    jbyte* nameBytes = env->GetByteArrayElements(fieldName, nullptr);
    if (!nameBytes) return 0;
    
    // Search for field by comparing key strings
    jlong result = 0;
    for (size_t i = 0; i < value->data.object.count; i++) {
        Field& field = value->data.object.fields[i];
        
        // Compare lengths first
        if (field.keyLength != static_cast<size_t>(nameLen)) {
            continue;
        }
        
        // Compare content
        if (std::memcmp(field.keyData, nameBytes, nameLen) == 0) {
            result = reinterpret_cast<jlong>(field.value);
            break;
        }
    }
    
    env->ReleaseByteArrayElements(fieldName, nameBytes, JNI_ABORT);
    return result;
}

JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeGetElement(
    JNIEnv* env, jclass clazz, jlong handle, jint index) {
    
    if (handle == 0) return 0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    if (value->type != FJ_TYPE_ARRAY) return 0;
    if (index < 0 || static_cast<size_t>(index) >= value->data.array.count) return 0;
    
    return reinterpret_cast<jlong>(value->data.array.elements[index]);
}

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetInt(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return 0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    switch (value->type) {
        case FJ_TYPE_NUMBER_INT:
        case FJ_TYPE_NUMBER_LONG:
            return static_cast<jint>(value->data.intValue);
        case FJ_TYPE_NUMBER_DOUBLE:
            return static_cast<jint>(value->data.doubleValue);
        default:
            return 0;
    }
}

JNIEXPORT jlong JNICALL Java_fastjson_FastJSON_nativeGetLong(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return 0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    switch (value->type) {
        case FJ_TYPE_NUMBER_INT:
        case FJ_TYPE_NUMBER_LONG:
            return static_cast<jlong>(value->data.intValue);
        case FJ_TYPE_NUMBER_DOUBLE:
            return static_cast<jlong>(value->data.doubleValue);
        default:
            return 0;
    }
}

JNIEXPORT jdouble JNICALL Java_fastjson_FastJSON_nativeGetDouble(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return 0.0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    switch (value->type) {
        case FJ_TYPE_NUMBER_INT:
        case FJ_TYPE_NUMBER_LONG:
            return static_cast<jdouble>(value->data.intValue);
        case FJ_TYPE_NUMBER_DOUBLE:
            return value->data.doubleValue;
        default:
            return 0.0;
    }
}

JNIEXPORT jboolean JNICALL Java_fastjson_FastJSON_nativeGetBoolean(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return JNI_FALSE;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    if (value->type == FJ_TYPE_BOOLEAN) {
        return value->data.boolValue ? JNI_TRUE : JNI_FALSE;
    }
    return JNI_FALSE;
}

JNIEXPORT jbyteArray JNICALL Java_fastjson_FastJSON_nativeGetStringBytes(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return nullptr;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    if (value->type != FJ_TYPE_STRING) return nullptr;
    
    jbyteArray result = env->NewByteArray(static_cast<jsize>(value->data.string.length));
    if (result) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(value->data.string.length),
                               reinterpret_cast<const jbyte*>(value->data.string.data));
    }
    
    return result;
}

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetArraySize(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return 0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    if (value->type == FJ_TYPE_ARRAY) {
        return static_cast<jint>(value->data.array.count);
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeGetObjectSize(
    JNIEnv* env, jclass clazz, jlong handle) {
    
    if (handle == 0) return 0;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    if (value->type == FJ_TYPE_OBJECT) {
        return static_cast<jint>(value->data.object.count);
    }
    return 0;
}

JNIEXPORT jbyteArray JNICALL Java_fastjson_FastJSON_nativeSerialize(
    JNIEnv* env, jclass clazz, jlong handle, jint flags) {
    
    if (handle == 0) return nullptr;
    ValueHandle* value = reinterpret_cast<ValueHandle*>(handle);
    
    // Estimate buffer size (will be optimized)
    size_t bufferSize = 65536;
    uint8_t* buffer = new uint8_t[bufferSize];
    
    size_t written = serializeValue(value, buffer, bufferSize, flags);
    
    jbyteArray result = env->NewByteArray(static_cast<jsize>(written));
    if (result) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(written),
                               reinterpret_cast<const jbyte*>(buffer));
    }
    
    delete[] buffer;
    return result;
}

// ============================================================================
// SIMD UTILITY JNI FUNCTIONS
// ============================================================================

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeFindToken(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length, jbyte token) {
    
    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    if (!bytes) return -1;
    
    int result = findTokenSimd(
        reinterpret_cast<const uint8_t*>(bytes) + offset,
        static_cast<size_t>(length),
        static_cast<uint8_t>(token)
    );
    
    env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
    return result;
}

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeFindStringEnd(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length) {
    
    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    if (!bytes) return -1;
    
    int result = findStringEndSimd(
        reinterpret_cast<const uint8_t*>(bytes) + offset,
        static_cast<size_t>(length)
    );
    
    env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
    return result;
}

JNIEXPORT jint JNICALL Java_fastjson_FastJSON_nativeSkipWhitespace(
    JNIEnv* env, jclass clazz, jbyteArray data, jint offset, jint length) {
    
    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    if (!bytes) return 0;
    
    int result = skipWhitespaceSimd(
        reinterpret_cast<const uint8_t*>(bytes) + offset,
        static_cast<size_t>(length)
    );
    
    env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
    return result;
}

// ============================================================================
// PARSER IMPLEMENTATION
// ============================================================================

namespace fastjson {

ValueHandle* createValue(int type) {
    // ValueHandle() constructor now zero-initialises all fields.
    ValueHandle* value = new ValueHandle();
    value->type = type;
    return value;
}

// Minimum address sanity constant (same as in nativeFree).
static const uintptr_t MIN_HEAP_ADDR = 0x10000;

void freeValue(ValueHandle* value) {
    if (!value) return;
    if (reinterpret_cast<uintptr_t>(value) < MIN_HEAP_ADDR) return;
    
    // FREE CHILDREN FIRST before releasing the source buffer they point into.
    switch (value->type) {
        case FJ_TYPE_ARRAY:
            if (value->data.array.elements) {
                for (size_t i = 0; i < value->data.array.count; i++) {
                    freeValue(value->data.array.elements[i]);
                }
                delete[] value->data.array.elements;
                value->data.array.elements = nullptr;
            }
            break;
            
        case FJ_TYPE_OBJECT:
            if (value->data.object.fields) {
                for (size_t i = 0; i < value->data.object.count; i++) {
                    freeValue(value->data.object.fields[i].value);
                    value->data.object.fields[i].value = nullptr;
                }
                delete[] value->data.object.fields;
                value->data.object.fields = nullptr;
            }
            break;
    }
    
    // NOW free the source buffer (after all children that pointed into it are gone).
    if (value->ownsSourceData && value->sourceData) {
        delete[] const_cast<uint8_t*>(value->sourceData);
        value->sourceData = nullptr;
    }
    
    delete value;
}

static inline void skipWhitespace(ParserState& state) {
    while (state.pos < state.length && isWhitespace(state.data[state.pos])) {
        if (state.data[state.pos] == '\n') {
            state.line++;
            state.column = 1;
        } else {
            state.column++;
        }
        state.pos++;
    }
}

static inline bool consumeChar(ParserState& state, char expected) {
    skipWhitespace(state);
    if (state.pos < state.length && state.data[state.pos] == expected) {
        state.pos++;
        state.column++;
        return true;
    }
    return false;
}

ValueHandle* parseJson(ParserState& state) {
    skipWhitespace(state);
    return parseValue(state);
}

ValueHandle* parseValue(ParserState& state) {
    skipWhitespace(state);
    
    if (state.pos >= state.length) return nullptr;
    
    uint8_t c = state.data[state.pos];
    
    switch (c) {
        case '{': return parseObject(state);
        case '[': return parseArray(state);
        case '"': return parseString(state);
        case 't':
        case 'f': return parseLiteral(state);
        case 'n': return parseLiteral(state);
        case '-':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return parseNumber(state);
        default:
            return nullptr;
    }
}

ValueHandle* parseObject(ParserState& state) {
    if (!consumeChar(state, '{')) return nullptr;
    
    ValueHandle* obj = createValue(FJ_TYPE_OBJECT);
    obj->data.object.fields = nullptr;
    obj->data.object.count = 0;
    
    // Temp storage for fields (resize as needed)
    size_t capacity = 8;
    Field* fields = new Field[capacity];
    size_t count = 0;
    
    skipWhitespace(state);
    
    // Empty object
    if (state.pos < state.length && state.data[state.pos] == '}') {
        state.pos++;
        obj->data.object.fields = fields;
        obj->data.object.count = count;
        return obj;
    }
    
    // Parse key-value pairs
    while (state.pos < state.length) {
        skipWhitespace(state);
        
        // Expect string key
        if (state.pos >= state.length || state.data[state.pos] != '"') {
            // Error: expected string key
            break;
        }
        
        // Parse key string
        state.pos++; // Skip opening quote
        const uint8_t* keyStart = state.data + state.pos;
        int keyLen = findStringEndScalar(state.data + state.pos, state.length - state.pos);
        if (keyLen < 0) {
            // Error: unterminated string
            break;
        }
        state.pos += keyLen + 1; // Skip key + closing quote
        
        skipWhitespace(state);
        
        // Expect colon
        if (state.pos >= state.length || state.data[state.pos] != ':') {
            // Error: expected colon
            break;
        }
        state.pos++; // Skip colon
        state.column++;
        
        // Parse value
        ValueHandle* value = parseValue(state);
        if (!value) {
            // Error: invalid value
            break;
        }
        
        // Store field
        if (count >= capacity) {
            capacity *= 2;
            Field* newFields = new Field[capacity];
            for (size_t i = 0; i < count; i++) {
                newFields[i] = fields[i];
            }
            delete[] fields;
            fields = newFields;
        }
        fields[count].keyData = keyStart;
        fields[count].keyLength = keyLen;
        fields[count].value = value;
        count++;
        
        skipWhitespace(state);
        
        // Check for end of object or more fields
        if (state.pos < state.length && state.data[state.pos] == '}') {
            state.pos++; // Skip closing brace
            break;
        }
        
        if (state.pos < state.length && state.data[state.pos] == ',') {
            state.pos++; // Skip comma
            state.column++;
            continue;
        }
        
        // Error: expected , or }
        break;
    }
    
    obj->data.object.fields = fields;
    obj->data.object.count = count;
    return obj;
}

ValueHandle* parseArray(ParserState& state) {
    if (!consumeChar(state, '[')) return nullptr;
    
    ValueHandle* arr = createValue(FJ_TYPE_ARRAY);
    arr->data.array.elements = nullptr;
    arr->data.array.count = 0;
    
    // Temp storage for elements (resize as needed)
    size_t capacity = 8;
    ValueHandle** elements = new ValueHandle*[capacity];
    size_t count = 0;
    
    skipWhitespace(state);
    
    // Empty array
    if (state.pos < state.length && state.data[state.pos] == ']') {
        state.pos++;
        arr->data.array.elements = elements;
        arr->data.array.count = count;
        return arr;
    }
    
    // Parse elements
    while (state.pos < state.length) {
        skipWhitespace(state);
        
        // Parse value
        ValueHandle* value = parseValue(state);
        if (!value) {
            // Error: invalid value
            break;
        }
        
        // Store element
        if (count >= capacity) {
            capacity *= 2;
            ValueHandle** newElements = new ValueHandle*[capacity];
            for (size_t i = 0; i < count; i++) {
                newElements[i] = elements[i];
            }
            delete[] elements;
            elements = newElements;
        }
        elements[count] = value;
        count++;
        
        skipWhitespace(state);
        
        // Check for end of array or more elements
        if (state.pos < state.length && state.data[state.pos] == ']') {
            state.pos++; // Skip closing bracket
            break;
        }
        
        if (state.pos < state.length && state.data[state.pos] == ',') {
            state.pos++; // Skip comma
            state.column++;
            continue;
        }
        
        // Error: expected , or ]
        break;
    }
    
    arr->data.array.elements = elements;
    arr->data.array.count = count;
    return arr;
}

ValueHandle* parseString(ParserState& state) {
    if (!consumeChar(state, '"')) return nullptr;
    
    size_t start = state.pos;
    
    // Find string end using SIMD if available
    int len = findStringEndSimd(state.data + state.pos, state.length - state.pos);
    if (len < 0) return nullptr;
    
    ValueHandle* str = createValue(FJ_TYPE_STRING);
    str->data.string.data = state.data + start;
    str->data.string.length = static_cast<size_t>(len);
    
    state.pos += len + 1; // Skip closing quote
    
    return str;
}

ValueHandle* parseNumber(ParserState& state) {
    size_t start = state.pos;
    bool isDouble = false;
    bool isLong = false;
    
    // Optional minus
    if (state.pos < state.length && state.data[state.pos] == '-') {
        state.pos++;
    }
    
    // Integer part
    while (state.pos < state.length && state.data[state.pos] >= '0' && state.data[state.pos] <= '9') {
        state.pos++;
    }
    
    // Fraction
    if (state.pos < state.length && state.data[state.pos] == '.') {
        isDouble = true;
        state.pos++;
        while (state.pos < state.length && state.data[state.pos] >= '0' && state.data[state.pos] <= '9') {
            state.pos++;
        }
    }
    
    // Exponent
    if (state.pos < state.length && (state.data[state.pos] == 'e' || state.data[state.pos] == 'E')) {
        isDouble = true;
        state.pos++;
        if (state.pos < state.length && (state.data[state.pos] == '+' || state.data[state.pos] == '-')) {
            state.pos++;
        }
        while (state.pos < state.length && state.data[state.pos] >= '0' && state.data[state.pos] <= '9') {
            state.pos++;
        }
    }
    
    // Check if long needed
    size_t len = state.pos - start;
    if (!isDouble && len > 9) {
        isLong = true;
    }
    
    // Parse value
    char* end;
    const char* str = reinterpret_cast<const char*>(state.data + start);
    
    ValueHandle* num = createValue(isDouble ? FJ_TYPE_NUMBER_DOUBLE : 
                                  (isLong ? FJ_TYPE_NUMBER_LONG : FJ_TYPE_NUMBER_INT));
    
    if (isDouble) {
        num->data.doubleValue = std::strtod(str, &end);
    } else if (isLong) {
        num->data.intValue = std::strtoll(str, &end, 10);
    } else {
        num->data.intValue = std::strtol(str, &end, 10);
    }
    
    return num;
}

ValueHandle* parseLiteral(ParserState& state) {
    if (state.pos + 4 <= state.length && 
        state.data[state.pos] == 't' &&
        state.data[state.pos + 1] == 'r' &&
        state.data[state.pos + 2] == 'u' &&
        state.data[state.pos + 3] == 'e') {
        state.pos += 4;
        ValueHandle* val = createValue(FJ_TYPE_BOOLEAN);
        val->data.boolValue = true;
        return val;
    }
    
    if (state.pos + 5 <= state.length && 
        state.data[state.pos] == 'f' &&
        state.data[state.pos + 1] == 'a' &&
        state.data[state.pos + 2] == 'l' &&
        state.data[state.pos + 3] == 's' &&
        state.data[state.pos + 4] == 'e') {
        state.pos += 5;
        ValueHandle* val = createValue(FJ_TYPE_BOOLEAN);
        val->data.boolValue = false;
        return val;
    }
    
    if (state.pos + 4 <= state.length && 
        state.data[state.pos] == 'n' &&
        state.data[state.pos + 1] == 'u' &&
        state.data[state.pos + 2] == 'l' &&
        state.data[state.pos + 3] == 'l') {
        state.pos += 4;
        return createValue(FJ_TYPE_NULL);
    }
    
    return nullptr;
}

// ============================================================================
// SERIALIZER IMPLEMENTATION
// ============================================================================

size_t serializeValue(const ValueHandle* value, uint8_t* buffer, size_t bufferSize, int flags) {
    if (!value || bufferSize == 0) return 0;
    
    char* out = reinterpret_cast<char*>(buffer);
    size_t pos = 0;
    
    switch (value->type) {
        case FJ_TYPE_NULL:
            if (pos + 4 <= bufferSize) {
                std::memcpy(out + pos, "null", 4);
                pos += 4;
            }
            break;
            
        case FJ_TYPE_BOOLEAN:
            if (value->data.boolValue) {
                if (pos + 4 <= bufferSize) {
                    std::memcpy(out + pos, "true", 4);
                    pos += 4;
                }
            } else {
                if (pos + 5 <= bufferSize) {
                    std::memcpy(out + pos, "false", 5);
                    pos += 5;
                }
            }
            break;
            
        case FJ_TYPE_NUMBER_INT:
        case FJ_TYPE_NUMBER_LONG:
            pos += std::snprintf(out + pos, bufferSize - pos, "%lld", 
                                static_cast<long long>(value->data.intValue));
            break;
            
        case FJ_TYPE_NUMBER_DOUBLE:
            pos += std::snprintf(out + pos, bufferSize - pos, "%g", value->data.doubleValue);
            break;
            
        case FJ_TYPE_STRING:
            if (pos < bufferSize) out[pos++] = '"';
            // TODO: Escape special characters
            if (pos + value->data.string.length <= bufferSize) {
                std::memcpy(out + pos, value->data.string.data, value->data.string.length);
                pos += value->data.string.length;
            }
            if (pos < bufferSize) out[pos++] = '"';
            break;
            
        case FJ_TYPE_ARRAY:
            if (pos < bufferSize) out[pos++] = '[';
            for (size_t i = 0; i < value->data.array.count; i++) {
                if (i > 0 && pos < bufferSize) out[pos++] = ',';
                pos += serializeValue(value->data.array.elements[i], 
                                     buffer + pos, bufferSize - pos, flags);
            }
            if (pos < bufferSize) out[pos++] = ']';
            break;
            
        case FJ_TYPE_OBJECT:
            if (pos < bufferSize) out[pos++] = '{';
            // TODO: Serialize object fields
            if (pos < bufferSize) out[pos++] = '}';
            break;
    }
    
    return pos;
}

} // namespace fastjson
