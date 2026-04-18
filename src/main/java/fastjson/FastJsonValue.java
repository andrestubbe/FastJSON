package fastjson;

import faststring.FastString;

import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * Represents a JSON value in the FastJSON parse tree.
 * 
 * This is a lazy, zero-copy view on the original JSON buffer.
 * Fields are parsed on-demand when accessed, not upfront.
 * 
 * @author FastJava Team
 * @version 1.0.0
 */
public class FastJsonValue implements AutoCloseable {
    
    // JSON type constants
    public static final int TYPE_NULL = 0;
    public static final int TYPE_OBJECT = 1;
    public static final int TYPE_ARRAY = 2;
    public static final int TYPE_STRING = 3;
    public static final int TYPE_NUMBER_INT = 4;
    public static final int TYPE_NUMBER_LONG = 5;
    public static final int TYPE_NUMBER_DOUBLE = 6;
    public static final int TYPE_BOOLEAN = 7;
    
    private final long handle;
    private final byte[] sourceData;
    private final int sourceOffset;
    private final int sourceLength;
    private volatile boolean closed = false;
    
    FastJsonValue(long handle, byte[] sourceData, int sourceOffset, int sourceLength) {
        this.handle = handle;
        this.sourceData = sourceData;
        this.sourceOffset = sourceOffset;
        this.sourceLength = sourceLength;
    }
    
    long getHandle() {
        checkClosed();
        return handle;
    }
    
    private void checkClosed() {
        if (closed) {
            throw new IllegalStateException("FastJsonValue has been closed");
        }
    }
    
    /**
     * Get the type of this JSON value.
     * 
     * @return type constant (TYPE_OBJECT, TYPE_ARRAY, etc.)
     */
    public int getType() {
        return FastJSON.nativeGetValueType(handle);
    }
    
    /**
     * Check if this value is a JSON object.
     * 
     * @return true if object
     */
    public boolean isObject() {
        return getType() == TYPE_OBJECT;
    }
    
    /**
     * Check if this value is a JSON array.
     * 
     * @return true if array
     */
    public boolean isArray() {
        return getType() == TYPE_ARRAY;
    }
    
    /**
     * Check if this value is a string.
     * 
     * @return true if string
     */
    public boolean isString() {
        return getType() == TYPE_STRING;
    }
    
    /**
     * Check if this value is a number (any type).
     * 
     * @return true if number
     */
    public boolean isNumber() {
        int type = getType();
        return type == TYPE_NUMBER_INT || type == TYPE_NUMBER_LONG || type == TYPE_NUMBER_DOUBLE;
    }
    
    /**
     * Check if this value is an integer.
     * 
     * @return true if int or long
     */
    public boolean isInt() {
        int type = getType();
        return type == TYPE_NUMBER_INT || type == TYPE_NUMBER_LONG;
    }
    
    /**
     * Check if this value is a boolean.
     * 
     * @return true if boolean
     */
    public boolean isBoolean() {
        return getType() == TYPE_BOOLEAN;
    }
    
    /**
     * Check if this value is null.
     * 
     * @return true if null
     */
    public boolean isNull() {
        return getType() == TYPE_NULL;
    }
    
    /**
     * Get a field from an object.
     * Lazy parsing - field is extracted on access.
     * 
     * @param name field name
     * @return FastJsonValue for field, or null if not found
     * @throws IllegalStateException if not an object
     */
    public FastJsonValue get(String name) {
        if (!isObject()) {
            throw new IllegalStateException("Not an object: " + getTypeName());
        }
        long fieldHandle = FastJSON.nativeGetField(handle, name);
        return fieldHandle != 0 ? new FastJsonValue(fieldHandle, sourceData, sourceOffset, sourceLength) : null;
    }
    
    /**
     * Get element from array by index.
     * 
     * @param index element index
     * @return FastJsonValue at index
     * @throws IllegalStateException if not an array
     * @throws IndexOutOfBoundsException if index invalid
     */
    public FastJsonValue get(int index) {
        if (!isArray()) {
            throw new IllegalStateException("Not an array: " + getTypeName());
        }
        long elementHandle = FastJSON.nativeGetElementValue(handle, index);
        if (elementHandle == 0) {
            throw new IndexOutOfBoundsException("Index: " + index);
        }
        return new FastJsonValue(elementHandle, sourceData, sourceOffset, sourceLength);
    }
    
    /**
     * Get field as int.
     * 
     * @param name field name
     * @return int value
     */
    public int getInt(String name) {
        FastJsonValue field = get(name);
        return field != null ? field.asInt() : 0;
    }
    
    /**
     * Get field as int with default.
     * 
     * @param name field name
     * @param defaultValue default if field missing or null
     * @return int value
     */
    public int getInt(String name, int defaultValue) {
        FastJsonValue field = get(name);
        return field != null && !field.isNull() ? field.asInt() : defaultValue;
    }
    
    /**
     * Get field as long.
     * 
     * @param name field name
     * @return long value
     */
    public long getLong(String name) {
        FastJsonValue field = get(name);
        return field != null ? field.asLong() : 0L;
    }
    
    /**
     * Get field as long with default.
     * 
     * @param name field name
     * @param defaultValue default if field missing or null
     * @return long value
     */
    public long getLong(String name, long defaultValue) {
        FastJsonValue field = get(name);
        return field != null && !field.isNull() ? field.asLong() : defaultValue;
    }
    
    /**
     * Get field as double.
     * 
     * @param name field name
     * @return double value
     */
    public double getDouble(String name) {
        FastJsonValue field = get(name);
        return field != null ? field.asDouble() : 0.0;
    }
    
    /**
     * Get field as double with default.
     * 
     * @param name field name
     * @param defaultValue default if field missing or null
     * @return double value
     */
    public double getDouble(String name, double defaultValue) {
        FastJsonValue field = get(name);
        return field != null && !field.isNull() ? field.asDouble() : defaultValue;
    }
    
    /**
     * Get field as boolean.
     * 
     * @param name field name
     * @return boolean value
     */
    public boolean getBoolean(String name) {
        FastJsonValue field = get(name);
        return field != null && field.asBoolean();
    }
    
    /**
     * Get field as boolean with default.
     * 
     * @param name field name
     * @param defaultValue default if field missing or null
     * @return boolean value
     */
    public boolean getBoolean(String name, boolean defaultValue) {
        FastJsonValue field = get(name);
        return field != null && !field.isNull() ? field.asBoolean() : defaultValue;
    }
    
    /**
     * Get field as String.
     * 
     * @param name field name
     * @return string value, or null if missing/null
     */
    public String getString(String name) {
        FastJsonValue field = get(name);
        return field != null && !field.isNull() ? field.asString() : null;
    }
    
    /**
     * Get field as String with default.
     * 
     * @param name field name
     * @param defaultValue default if field missing or null
     * @return string value
     */
    public String getString(String name, String defaultValue) {
        FastJsonValue field = get(name);
        String result = field != null && !field.isNull() ? field.asString() : null;
        return result != null ? result : defaultValue;
    }
    
    /**
     * Get field as FastString (zero-copy).
     * 
     * @param name field name
     * @return FastString value, or null if missing/null
     */
    public FastString getFastString(String name) {
        FastJsonValue field = get(name);
        return field != null && !field.isNull() ? field.asFastString() : null;
    }
    
    /**
     * Get this value as int.
     * 
     * @return int value
     * @throws IllegalStateException if not a number
     */
    public int asInt() {
        return FastJSON.nativeGetIntValue(handle);
    }
    
    /**
     * Get this value as long.
     * 
     * @return long value
     */
    public long asLong() {
        return FastJSON.nativeGetLongValue(handle);
    }
    
    /**
     * Get this value as double.
     * 
     * @return double value
     */
    public double asDouble() {
        return FastJSON.nativeGetDoubleValue(handle);
    }
    
    /**
     * Get this value as boolean.
     * 
     * @return boolean value
     */
    public boolean asBoolean() {
        return FastJSON.nativeGetBooleanValue(handle);
    }
    
    /**
     * Get this value as String.
     * 
     * @return string value
     */
    public String asString() {
        return FastJSON.nativeGetStringValue(handle);
    }
    
    /**
     * Get this value as FastString (zero-copy).
     * 
     * @return FastString value
     */
    public FastString asFastString() {
        return FastJSON.nativeGetFastStringValue(handle);
    }
    
    /**
     * Get array size.
     * 
     * @return number of elements
     * @throws IllegalStateException if not an array
     */
    public int size() {
        if (isArray()) {
            return FastJSON.nativeGetArraySizeValue(handle);
        } else if (isObject()) {
            return FastJSON.nativeGetObjectSizeValue(handle);
        }
        throw new IllegalStateException("Not an array or object: " + getTypeName());
    }
    
    /**
     * Check if object has field.
     * 
     * @param name field name
     * @return true if field exists and not null
     */
    public boolean has(String name) {
        if (!isObject()) return false;
        FastJsonValue field = get(name);
        return field != null && !field.isNull();
    }
    
    /**
     * Get iterator over array elements.
     * 
     * @return iterator over FastJsonValue elements
     */
    public Iterable<FastJsonValue> elements() {
        if (!isArray()) {
            throw new IllegalStateException("Not an array: " + getTypeName());
        }
        return () -> new Iterator<FastJsonValue>() {
            private int index = 0;
            private final int size = size();
            
            @Override
            public boolean hasNext() {
                return index < size;
            }
            
            @Override
            public FastJsonValue next() {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                return get(index++);
            }
        };
    }
    
    /**
     * Get type as human-readable string.
     * 
     * @return type name
     */
    public String getTypeName() {
        switch (getType()) {
            case TYPE_NULL: return "null";
            case TYPE_OBJECT: return "object";
            case TYPE_ARRAY: return "array";
            case TYPE_STRING: return "string";
            case TYPE_NUMBER_INT: return "int";
            case TYPE_NUMBER_LONG: return "long";
            case TYPE_NUMBER_DOUBLE: return "double";
            case TYPE_BOOLEAN: return "boolean";
            default: return "unknown";
        }
    }
    
    /**
     * Serialize this value to JSON bytes.
     * 
     * @return UTF-8 JSON bytes
     */
    public byte[] toBytes() {
        return FastJSON.serialize(this);
    }
    
    /**
     * Serialize this value to JSON string.
     * 
     * @return JSON string
     */
    @Override
    public String toString() {
        return FastJSON.serializeToString(this);
    }
    
    /**
     * Release native resources.
     * Call when value is no longer needed to free native memory.
     */
    @Override
    public void close() {
        if (!closed) {
            closed = true;
            FastJSON.nativeFreeValue(handle);
        }
    }
    
    @Override
    protected void finalize() throws Throwable {
        try {
            if (!closed) {
                close();
            }
        } finally {
            super.finalize();
        }
    }
}
