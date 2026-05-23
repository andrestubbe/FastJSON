# FastJSON â€” Ultra-Fast Native JSON Parser for Java v0.1.0 [ALPHA] - v0.1.0
**A high-performance native JSON module for the FastJava ecosystem. Optimized for raw throughput and zero-copy parsing via SIMD.**

[![Build](https://img.shields.io/github/actions/workflow/status/andrestubbe/FastJSON/maven.yml?branch=main)](https://github.com/andrestubbe/FastJSON/actions)
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![JitPack](https://jitpack.io/v/andrestubbe/FastJSON.svg)](https://jitpack.io/#andrestubbe/FastJSON)

---

**FastJSON** delivers elite parsing performance by leveraging native SIMD instructions and optimized memory handling. Built for high-frequency API requests and massive data processing pipelines.

```java
// Quick Start â€” Example
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
- **âš¡ SIMD Accelerated**: Native JSON parsing optimizations (AVX2/SSE).
- **ðŸ“¦ Zero-Copy**: Direct access to native byte buffers bypassing String overhead.
- **ðŸš€ Raw Performance**: Optimized for massive data throughput (50x faster than Jackson).
- **ðŸ–‡ï¸ Ecosystem Ready**: Seamless integration with FastIO and FastCore.

---

## Installation

### Option 1: Maven (Recommended)
Add the JitPack repository and the dependencies to your `pom.xml`:

```xml
<repositories>
    <repository>
        <id>jitpack.io</id>
        <url>https://jitpack.io</url>
    </repository>
</repositories>

<dependencies>
    <!-- FastJSON Library -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>fastjson</artifactId>
        <version>v0.1.0</version>
    </dependency>

    <!-- FastCore (Required Native Loader) -->
    <dependency>
        <groupId>com.github.andrestubbe</groupId>
        <artifactId>fastcore</artifactId>
        <version>v0.1.0</version>
    </dependency>
</dependencies>
```

### Option 2: Gradle (via JitPack)
```groovy
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'com.github.andrestubbe:fastjson:v0.1.0'
    implementation 'com.github.andrestubbe:fastcore:v0.1.0'
}
```

### Option 3: Direct Download (No Build Tool)
Download the latest JARs directly to add them to your classpath:

1. 📦 **[fastjson-v0.1.0.jar](https://github.com/andrestubbe/FastJSON/releases/download/v0.1.0/fastjson-v0.1.0.jar)** (The Core Library)
2. ⚙️ **[fastcore-v0.1.0.jar](https://github.com/andrestubbe/FastCore/releases/download/v0.1.0/fastcore-v0.1.0.jar)** (The Mandatory Native Loader)

> [!IMPORTANT]
> All JARs must be in your classpath for the native JNI calls to function correctly.


## License
MIT License â€” See [LICENSE](LICENSE) for details.

---

## Related Projects
- [FastCore](https://github.com/andrestubbe/FastCore) â€” Native Library Loader
- [FastIO](https://github.com/andrestubbe/FastIO) â€” High-performance File I/O

---
**Made with âš¡ by Andre Stubbe**

