package fastjson;

import faststring.FastString;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

/**
 * Builder for constructing JSON objects and arrays.
 * 
 * Uses FastString internally for efficient string building.
 * Zero-allocation for repeated operations.
 * 
 * @author FastJava Team
 * @version 1.0.0
 */
public class FastJsonBuilder {
    
    private enum Mode { OBJECT, ARRAY }
    
    private final Mode mode;
    private final List<Field> fields;
    private final List<Value> elements;
    private boolean built = false;
    
    private static class Field {
        final String name;
        final Value value;
        
        Field(String name, Value value) {
            this.name = name;
            this.value = value;
        }
    }
    
    private static class Value {
        final int type;
        final Object data;
        
        // Type constants
        static final int TYPE_NULL = 0;
        static final int TYPE_STRING = 1;
        static final int TYPE_INT = 2;
        static final int TYPE_LONG = 3;
        static final int TYPE_DOUBLE = 4;
        static final int TYPE_BOOLEAN = 5;
        static final int TYPE_OBJECT = 6;
        static final int TYPE_ARRAY = 7;
        static final int TYPE_FASTSTRING = 8;
        static final int TYPE_BYTES = 9;
        
        Value(int type, Object data) {
            this.type = type;
            this.data = data;
        }
    }
    
    private FastJsonBuilder(Mode mode) {
        this.mode = mode;
        this.fields = mode == Mode.OBJECT ? new ArrayList<>() : null;
        this.elements = mode == Mode.ARRAY ? new ArrayList<>() : null;
    }
    
    /**
     * Create a new object builder.
     * 
     * @return FastJsonBuilder for object
     */
    public static FastJsonBuilder object() {
        return new FastJsonBuilder(Mode.OBJECT);
    }
    
    /**
     * Create a new array builder.
     * 
     * @return FastJsonBuilder for array
     */
    public static FastJsonBuilder array() {
        return new FastJsonBuilder(Mode.ARRAY);
    }
    
    /**
     * Add string field to object.
     * 
     * @param name field name
     * @param value string value
     * @return this builder
     * @throws IllegalStateException if not building an object
     */
    public FastJsonBuilder add(String name, String value) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_STRING, value)));
        return this;
    }
    
    /**
     * Add FastString field to object (zero-copy).
     * 
     * @param name field name
     * @param value FastString value
     * @return this builder
     */
    public FastJsonBuilder add(String name, FastString value) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_FASTSTRING, value)));
        return this;
    }
    
    /**
     * Add int field to object.
     * 
     * @param name field name
     * @param value int value
     * @return this builder
     */
    public FastJsonBuilder add(String name, int value) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_INT, value)));
        return this;
    }
    
    /**
     * Add long field to object.
     * 
     * @param name field name
     * @param value long value
     * @return this builder
     */
    public FastJsonBuilder add(String name, long value) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_LONG, value)));
        return this;
    }
    
    /**
     * Add double field to object.
     * 
     * @param name field name
     * @param value double value
     * @return this builder
     */
    public FastJsonBuilder add(String name, double value) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_DOUBLE, value)));
        return this;
    }
    
    /**
     * Add boolean field to object.
     * 
     * @param name field name
     * @param value boolean value
     * @return this builder
     */
    public FastJsonBuilder add(String name, boolean value) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_BOOLEAN, value)));
        return this;
    }
    
    /**
     * Add null field to object.
     * 
     * @param name field name
     * @return this builder
     */
    public FastJsonBuilder addNull(String name) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_NULL, null)));
        return this;
    }
    
    /**
     * Add nested object field.
     * 
     * @param name field name
     * @param builder nested object builder
     * @return this builder
     */
    public FastJsonBuilder add(String name, FastJsonBuilder builder) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_OBJECT, builder)));
        return this;
    }
    
    /**
     * Add nested array field.
     * 
     * @param name field name
     * @param builder nested array builder
     * @return this builder
     */
    public FastJsonBuilder addArray(String name, FastJsonBuilder builder) {
        checkObjectMode();
        fields.add(new Field(name, new Value(Value.TYPE_ARRAY, builder)));
        return this;
    }
    
    /**
     * Add string element to array.
     * 
     * @param value string value
     * @return this builder
     * @throws IllegalStateException if not building an array
     */
    public FastJsonBuilder add(String value) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_STRING, value));
        return this;
    }
    
    /**
     * Add FastString element to array.
     * 
     * @param value FastString value
     * @return this builder
     */
    public FastJsonBuilder add(FastString value) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_FASTSTRING, value));
        return this;
    }
    
    /**
     * Add int element to array.
     * 
     * @param value int value
     * @return this builder
     */
    public FastJsonBuilder add(int value) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_INT, value));
        return this;
    }
    
    /**
     * Add long element to array.
     * 
     * @param value long value
     * @return this builder
     */
    public FastJsonBuilder add(long value) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_LONG, value));
        return this;
    }
    
    /**
     * Add double element to array.
     * 
     * @param value double value
     * @return this builder
     */
    public FastJsonBuilder add(double value) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_DOUBLE, value));
        return this;
    }
    
    /**
     * Add boolean element to array.
     * 
     * @param value boolean value
     * @return this builder
     */
    public FastJsonBuilder add(boolean value) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_BOOLEAN, value));
        return this;
    }
    
    /**
     * Add null element to array.
     * 
     * @return this builder
     */
    public FastJsonBuilder addNull() {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_NULL, null));
        return this;
    }
    
    /**
     * Add nested object element to array.
     * 
     * @param builder nested object builder
     * @return this builder
     */
    public FastJsonBuilder addObject(FastJsonBuilder builder) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_OBJECT, builder));
        return this;
    }
    
    /**
     * Add nested array element to array.
     * 
     * @param builder nested array builder
     * @return this builder
     */
    public FastJsonBuilder addArray(FastJsonBuilder builder) {
        checkArrayMode();
        elements.add(new Value(Value.TYPE_ARRAY, builder));
        return this;
    }
    
    /**
     * Build and serialize to JSON bytes.
     * 
     * @return UTF-8 JSON bytes
     */
    public byte[] build() {
        if (built) {
            throw new IllegalStateException("Builder can only be used once");
        }
        built = true;
        
        FastString result = new FastString(1024);
        
        if (mode == Mode.OBJECT) {
            buildObject(result);
        } else {
            buildArray(result);
        }
        
        return result.getBytes();
    }
    
    /**
     * Build and serialize to JSON string.
     * 
     * @return JSON string
     */
    public String buildString() {
        byte[] bytes = build();
        return new String(bytes, StandardCharsets.UTF_8);
    }
    
    /**
     * Build and serialize to FastString.
     * 
     * @return FastString containing JSON
     */
    public FastString buildFastString() {
        byte[] bytes = build();
        return new FastString(bytes, 0, bytes.length);
    }
    
    private void buildObject(FastString out) {
        out.append("{");
        
        for (int i = 0; i < fields.size(); i++) {
            if (i > 0) out.append(",");
            
            Field field = fields.get(i);
            appendString(out, field.name);
            out.append(":");
            appendValue(out, field.value);
        }
        
        out.append("}");
    }
    
    private void buildArray(FastString out) {
        out.append("[");
        
        for (int i = 0; i < elements.size(); i++) {
            if (i > 0) out.append(",");
            appendValue(out, elements.get(i));
        }
        
        out.append("]");
    }
    
    private void appendValue(FastString out, Value value) {
        switch (value.type) {
            case Value.TYPE_NULL:
                out.append("null");
                break;
            case Value.TYPE_STRING:
                appendString(out, (String) value.data);
                break;
            case Value.TYPE_FASTSTRING:
                appendFastString(out, (FastString) value.data);
                break;
            case Value.TYPE_INT:
                out.append(String.valueOf((Integer) value.data));
                break;
            case Value.TYPE_LONG:
                out.append(String.valueOf((Long) value.data));
                break;
            case Value.TYPE_DOUBLE:
                out.append(String.valueOf((Double) value.data));
                break;
            case Value.TYPE_BOOLEAN:
                out.append((Boolean) value.data ? "true" : "false");
                break;
            case Value.TYPE_OBJECT:
                ((FastJsonBuilder) value.data).buildObject(out);
                break;
            case Value.TYPE_ARRAY:
                ((FastJsonBuilder) value.data).buildArray(out);
                break;
        }
    }
    
    private void appendString(FastString out, String s) {
        out.append("\"");
        
        // Escape special characters
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            switch (c) {
                case '"': out.append("\\\""); break;
                case '\\': out.append("\\\\"); break;
                case '\b': out.append("\\b"); break;
                case '\f': out.append("\\f"); break;
                case '\n': out.append("\\n"); break;
                case '\r': out.append("\\r"); break;
                case '\t': out.append("\\t"); break;
                default:
                    if (c < 0x20) {
                        out.append(String.format("\\u%04x", (int) c));
                    } else {
                        out.append(String.valueOf(c));
                    }
            }
        }
        
        out.append("\"");
    }
    
    private void appendFastString(FastString out, FastString fs) {
        out.append("\"");
        
        byte[] bytes = fs.getBytes();
        int offset = 0;
        int length = fs.byteLength();
        
        for (int i = 0; i < length; i++) {
            byte b = bytes[offset + i];
            if (b >= 0) {
                // ASCII
                char c = (char) b;
                switch (c) {
                    case '"': out.append("\\\""); break;
                    case '\\': out.append("\\\\"); break;
                    case '\b': out.append("\\b"); break;
                    case '\f': out.append("\\f"); break;
                    case '\n': out.append("\\n"); break;
                    case '\r': out.append("\\r"); break;
                    case '\t': out.append("\\t"); break;
                    default:
                        if (c < 0x20) {
                            out.append(String.format("\\u%04x", (int) c));
                        } else {
                            out.append(String.valueOf(c));
                        }
                }
            } else {
                // UTF-8 continuation byte - append as-is
                out.append(String.valueOf((char) (b & 0xFF)));
            }
        }
        
        out.append("\"");
    }
    
    private void checkObjectMode() {
        if (mode != Mode.OBJECT) {
            throw new IllegalStateException("Not in object mode");
        }
    }
    
    private void checkArrayMode() {
        if (mode != Mode.ARRAY) {
            throw new IllegalStateException("Not in array mode");
        }
    }
}
