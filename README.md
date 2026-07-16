# FastJSON 0.1.0 [ALPHA-2026-06-14] — Ultra-Fast Native JSON Parser for Java

[![Status](https://img.shields.io/badge/status-0.1.0-brightgreen.svg)](https://github.com/andrestubbe/FastJSON/releases/tag/0.1.0)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Java](https://img.shields.io/badge/Java-17+-blue.svg)](https://www.java.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010+-lightgrey.svg)]()
[![JitPack](https://img.shields.io/badge/JitPack-ready-green.svg)](https://jitpack.io/#andrestubbe/FastJSON)

**💡 A high-performance native JSON module for the FastJava ecosystem. Optimized for raw throughput and zero-copy parsing
via SIMD.**

FastJSON delivers elite parsing performance by leveraging native SIMD instructions and optimized memory handling. Built
for high-frequency API requests and massive data processing pipelines.

[![FastKeyboard Showcase](docs/screenshot.png)](https://www.youtube.com/watch?v=BZsqQl7WqWk)


---

## Table of Contents

- [Features](#features)
- [Performance](#performance)
- [Installation](#installation)
- [License](#license)
- [Related Projects](#related-projects)

---

## Quick Start

```java
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

## Features

- ⚡ **SIMD Accelerated**: Native JSON parsing via AVX2/SSE — no JVM overhead.
- 🚀 **Zero-Copy**: Direct access to native byte buffers, bypassing String allocation.
- 📈 **Raw Performance**: Optimized for massive data throughput (up to 50× faster than Jackson).
- 🔗 **Ecosystem Ready**: Seamless integration with FastIO, FastBytes, FastString and FastCore.

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
    <artifactId>FastJSON</artifactId>
    <version>0.1.0</version>
</dependency>

<!-- FastCore (Required Native Loader) -->
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>fastcore</artifactId>
    <version>0.1.0</version>
</dependency>

<!-- FastString (Required Dependency) -->
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>FastString</artifactId>
    <version>0.1.0</version>
</dependency>

<!-- FastBytes (Required Dependency) -->
<dependency>
    <groupId>com.github.andrestubbe</groupId>
    <artifactId>FastBytes</artifactId>
    <version>0.1.0</version>
</dependency>
</dependencies>
```

### Option 2: Gradle (via JitPack)

```groovy
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'com.github.andrestubbe:FastJSON:0.1.0'
    implementation 'com.github.andrestubbe:fastcore:0.1.0'
    implementation 'com.github.andrestubbe:FastString:0.1.0'
    implementation 'com.github.andrestubbe:FastBytes:0.1.0'
}
```

### Option 3: Direct Download (No Build Tool)

Download the latest JARs directly to add them to your classpath:

1. 🚀 **[fastjson-0.1.0.jar](https://github.com/andrestubbe/FastJSON/releases/download/0.1.0/fastjson-0.1.0.jar)** (Core Library)
2. ⚙️ **[fastcore-0.1.0.jar](https://github.com/andrestubbe/FastCore/releases/download/0.1.0/fastcore-0.1.0.jar)** (Mandatory Native Loader)
3. 📦 **[FastString-0.1.0.jar](https://github.com/andrestubbe/FastString/releases/download/0.1.0/FastString-0.1.0.jar)** (Required String Dependency)
4. 📦 **[FastBytes-0.1.0.jar](https://github.com/andrestubbe/FastBytes/releases/download/0.1.0/FastBytes-0.1.0.jar)** (Required Bytes Dependency)

> [!IMPORTANT]
> All JARs must be in your classpath for the JNI calls to function correctly.

---

## Documentation

* **[COMPILE.md](docs/COMPILE.md)**: Full compilation guide (MSVC C++17 build chain + JNI Setup).
* **[REFERENCE.md](docs/REFERENCE.md)**: Full API descriptions, border configurations, and codepoint index.
* **[PHILOSOPHY.md](docs/PHILOSOPHY.md)**: The engineering rationale for zero-allocation performance.
* **[ROADMAP.md](docs/ROADMAP.md)**: Future milestones and planned features.

---

## Platform Support

| Platform      | Status            |
|---------------|-------------------|
| Windows 10/11 | ✅ Fully Supported |
| Linux         | 🚧 Planned        |
| macOS         | 🚧 Planned        |

---

## License

MIT License  See [LICENSE](LICENSE) file for details.

---

## Related Projects

- [FastCore](https://github.com/andrestubbe/FastCore) — Native Library Loader for Java
- [FastIO](https://github.com/andrestubbe/FastIO) — Ultra-fast native file I/O for Java
- [FastBytes](https://github.com/andrestubbe/FastBytes) — High-performance byte buffer engine
- [FastString](https://github.com/andrestubbe/FastString) — Zero-allocation String processing

---
**Part of the FastJava Ecosystem** — *Making the JVM faster. Small package. Maximum speed. Zero bloat. ⚡*




