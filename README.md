# FastJSON — Ultra-Fast Native JSON Parser for Java v0.1.0 [ALPHA] - v0.1.0
**A high-performance native JSON module for the FastJava ecosystem. Optimized for raw throughput and zero-copy parsing via SIMD.**

[![Build](https://img.shields.io/github/actions/workflow/status/andrestubbe/FastJSON/maven.yml?branch=main)](https://github.com/andrestubbe/FastJSON/actions)
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![JitPack](https://jitpack.io/v/andrestubbe/FastJSON.svg)](https://jitpack.io/#andrestubbe/FastJSON)

---

**FastJSON** delivers elite parsing performance by leveraging native SIMD instructions and optimized memory handling. Built for high-frequency API requests and massive data processing pipelines.

```java
// Quick Start — Example
import fastjson.FastJSON;
import fastjson.FastJsonValue;

public class Demo {
    public static void main(String[] args) {
        FastJsonValue doc = FastJSON.parse("{\"status\":\"ok\", \"latency\": 120}");
        System.out.println("Status: " + doc.getString("status"));
    }
}
```

---

## Table of Contents
- [Features](#features)
- [Performance](#performance)
- [Installation](#installation)
- [License](#license)
- [Related Projects](#related-projects)

---

## Features
- **⚡ SIMD Accelerated**: Native JSON parsing optimizations (AVX2/SSE).
- **📦 Zero-Copy**: Direct access to native byte buffers bypassing String overhead.
- **🚀 Raw Performance**: Optimized for massive data throughput (50x faster than Jackson).
- **🖇️ Ecosystem Ready**: Seamless integration with FastIO and FastCore.

---

## Installation

FastJSON requires **two** dependencies: the module itself and `FastCore` (the native loader).

### Maven (JitPack)
```xml
<repositories>
    <repository>
        <id>jitpack.io</id>
        <url>https://jitpack.io</url>
    </repository>
</repositories>

<dependencies>
    <!-- 1. FastJSON Library -->
    <dependency>
        <groupId>io.github.andrestubbe</groupId>
        <artifactId>fastjson</artifactId>
        <version>0.1.0</version>
    </dependency>

    <!-- 2. FastCore (Mandatory Native Loader) -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>fastcore</artifactId>
        <version>0.1.0</version>
    </dependency>
</dependencies>
```

### Gradle (JitPack)
```groovy
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'io.github.andrestubbe:fastjson:0.1.0'
    implementation 'com.github.andrestubbe:fastcore:0.1.0'
}
```

### Option 3: Direct Download (No Build Tool)
Download the latest JARs directly to add them to your classpath:

1. 📦 [**fastjson-v0.1.0.jar**](https://github.com/andrestubbe/FastJSON/releases/download/v0.1.0/fastjson-0.1.0.jar)
2. ⚙️ [**fastcore-v0.1.0.jar**](https://github.com/andrestubbe/fastcore/releases/download/v0.1.0/fastcore-0.1.0.jar)

---

## License
MIT License — See [LICENSE](LICENSE) for details.

---

## Related Projects
- [FastCore](https://github.com/andrestubbe/FastCore) — Native Library Loader
- [FastIO](https://github.com/andrestubbe/FastIO) — High-performance File I/O

---
**Made with ⚡ by Andre Stubbe**

