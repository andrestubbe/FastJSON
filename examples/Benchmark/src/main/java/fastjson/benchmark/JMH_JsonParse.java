package fastjson.benchmark;

import fastjson.FastJSON;
import fastjson.FastJsonValue;
import org.openjdk.jmh.annotations.*;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.Throughput)
@OutputTimeUnit(TimeUnit.SECONDS)
@State(Scope.Thread)
@Fork(1)
@Warmup(iterations = 2, time = 1)
@Measurement(iterations = 3, time = 1)
public class JMH_JsonParse {

    private byte[] jsonBytes;

    @Setup
    public void setup() {
        String jsonStr = "{\"name\":\"FastJava\",\"version\":\"0.1.2\",\"status\":\"active\",\"metrics\":[10,20,30,40,50],\"settings\":{\"simd\":true,\"threads\":8}}";
        jsonBytes = jsonStr.getBytes(StandardCharsets.UTF_8);
    }

    @Benchmark
    public FastJsonValue testFastJsonParse() {
        return FastJSON.parse(jsonBytes);
    }
}
