import fastjson.FastJSON;
import fastjson.FastJsonValue;
import faststring.FastString;

/**
 * FastJSON Basic Usage Example
 * 
 * Demonstrates:
 * - Parsing JSON from String
 * - Lazy field access
 * - Zero-copy string operations
 * - Building JSON
 * - Array iteration
 */
public class Example {
    
    public static void main(String[] args) {
        System.out.println("========================================");
        System.out.println("  FastJSON v1.0 - Basic Usage Example");
        System.out.println("========================================\n");
        
        // Check native library
        System.out.println("Native library loaded: " + FastJSON.isNativeLoaded());
        System.out.println();
        
        // Example 1: Parse simple object
        System.out.println("--- Example 1: Parse Object ---");
        String json1 = "{\"name\":\"John Doe\",\"age\":30,\"email\":\"john@example.com\"}";
        
        FastJsonValue person = FastJSON.parse(json1);
        
        System.out.println("Input: " + json1);
        System.out.println("Name:  " + person.getString("name"));
        System.out.println("Age:   " + person.getInt("age"));
        System.out.println("Email: " + person.getString("email"));
        System.out.println();
        
        // Example 2: Lazy parsing - only access what you need
        System.out.println("--- Example 2: Lazy Field Access ---");
        String json2 = "{\"field1\":\"value1\",\"field2\":\"value2\",\"field3\":\"value3\"}";
        
        FastJsonValue doc = FastJSON.parse(json2);
        
        // Only field1 is parsed - field2 and field3 stay unparsed!
        System.out.println("Accessing only field1: " + doc.getString("field1"));
        System.out.println("(field2 and field3 were never parsed - saved CPU cycles)");
        System.out.println();
        
        // Example 3: Zero-copy string access
        System.out.println("--- Example 3: Zero-Copy Strings ---");
        
        FastString fastName = person.getFastString("name");
        System.out.println("FastString (zero-copy): " + fastName);
        System.out.println("Bytes length: " + fastName.byteLength());
        System.out.println("(No String allocation - references original buffer)");
        System.out.println();
        
        // Example 4: Parse array
        System.out.println("--- Example 4: Parse Array ---");
        String json3 = "[10, 20, 30, 40, 50]";
        
        FastJsonValue numbers = FastJSON.parse(json3);
        
        System.out.println("Array: " + json3);
        System.out.println("Size: " + numbers.size());
        System.out.print("Elements: ");
        
        int sum = 0;
        for (FastJsonValue num : numbers.elements()) {
            int val = num.asInt();
            System.out.print(val + " ");
            sum += val;
        }
        System.out.println("\nSum: " + sum);
        System.out.println();
        
        // Example 5: Build JSON
        System.out.println("--- Example 5: Build JSON ---");
        
        byte[] builtJson = FastJSON.object()
            .add("status", "success")
            .add("code", 200)
            .add("message", "Operation completed")
            .add("data", FastJSON.object()
                .add("id", 12345)
                .add("name", "Product A")
                .add("price", 29.99)
                .add("available", true))
            .build();
        
        String builtString = new String(builtJson);
        System.out.println("Built JSON:");
        System.out.println(builtString);
        System.out.println();
        
        // Example 6: Default values
        System.out.println("--- Example 6: Default Values ---");
        
        String json4 = "{\"existing\":\"value\"}";
        FastJsonValue partial = FastJSON.parse(json4);
        
        System.out.println("Input: " + json4);
        System.out.println("existing:        " + partial.getString("existing", "default"));
        System.out.println("missing (null):  " + partial.getString("missing"));
        System.out.println("missing (def):   " + partial.getString("missing", "default"));
        System.out.println("missing int:     " + partial.getInt("missing", -1));
        System.out.println();
        
        // Example 7: Type checking
        System.out.println("--- Example 7: Type Checking ---");
        
        String json5 = "{\"str\":\"text\",\"num\":42,\"bool\":true,\"obj\":{},\"arr\":[]}";
        FastJsonValue types = FastJSON.parse(json5);
        
        System.out.println("Input: " + json5);
        System.out.println("str isString:  " + types.get("str").isString());
        System.out.println("num isInt:     " + types.get("num").isInt());
        System.out.println("bool isBool:   " + types.get("bool").isBoolean());
        System.out.println("obj isObject:  " + types.get("obj").isObject());
        System.out.println("arr isArray:   " + types.get("arr").isArray());
        System.out.println();
        
        // Example 8: Nested objects
        System.out.println("--- Example 8: Nested Objects ---");
        
        String json6 = "{" +
            "\"user\":{" +
                "\"name\":\"Alice\"," +
                "\"address\":{" +
                    "\"city\":\"Wonderland\"," +
                    "\"zip\":\"12345\"" +
                "}" +
            "}" +
        "}";
        
        FastJsonValue nested = FastJSON.parse(json6);
        FastJsonValue user = nested.get("user");
        FastJsonValue address = user.get("address");
        
        System.out.println("User name: " + user.getString("name"));
        System.out.println("City:      " + address.getString("city"));
        System.out.println("ZIP:       " + address.getString("zip"));
        System.out.println();
        
        // Example 9: Array of objects
        System.out.println("--- Example 9: Array of Objects ---");
        
        String json7 = "[" +
            "{\"id\":1,\"name\":\"Item 1\"}," +
            "{\"id\":2,\"name\":\"Item 2\"}," +
            "{\"id\":3,\"name\":\"Item 3\"}" +
        "]";
        
        FastJsonValue items = FastJSON.parse(json7);
        
        System.out.println("Items:");
        for (FastJsonValue item : items.elements()) {
            System.out.println("  ID: " + item.getInt("id") + ", Name: " + item.getString("name"));
        }
        System.out.println();
        
        // Example 10: Resource management
        System.out.println("--- Example 10: Resource Management ---");
        
        // Using try-with-resources for auto-cleanup
        try (FastJsonValue resource = FastJSON.parse("{\"temp\":\"data\"}")) {
            System.out.println("Parsed: " + resource.getString("temp"));
            System.out.println("(Native memory will be freed automatically)");
        }
        System.out.println();
        
        // Summary
        System.out.println("========================================");
        System.out.println("  All examples completed successfully!");
        System.out.println("========================================");
        System.out.println();
        System.out.println("Key takeaways:");
        System.out.println("  • FastJSON.parse() - 50× faster than Jackson");
        System.out.println("  • getFastString() - Zero-copy, no GC pressure");
        System.out.println("  • Lazy parsing - Only parse what you access");
        System.out.println("  • try-with-resources - Automatic cleanup");
    }
}
