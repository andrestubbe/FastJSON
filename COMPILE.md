# FastJSON Build Instructions

## Prerequisites

### Required

- **Java**: 17 or higher
- **Maven**: 3.9 or higher
- **Visual Studio**: 2022 or Build Tools (for native compilation)
- **Windows**: 10/11 x64

### Optional

- **CPU with AVX2**: For maximum performance (Intel Haswell+ / AMD Zen+)
- **CPU with SSE4.2**: Minimum requirement (Intel Nehalem+ / AMD Bulldozer+)

---

## Quick Build

### 1. Compile Native Library

Open **x64 Native Tools Command Prompt for VS 2022** (not regular cmd):

```cmd
cd C:\Users\andre\Documents\FastJava\2026-04-18-Work-FastJSON-v1.0
compile.bat
```

This creates `build\fastjson.dll` with AVX2 SIMD optimizations.

### 2. Build JAR

```cmd
mvn clean package
```

Output: `target\fastjson-1.0.0.jar`

### 3. Run Tests

```cmd
mvn test
```

### 4. Run Benchmarks

```cmd
mvn exec:java
```

---

## Manual Compilation

If `compile.bat` fails, compile manually:

### 1. Set Environment

```cmd
set JAVA_HOME=C:\Program Files\Java\jdk-17
set MSVC_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.35.32215
```

### 2. Compile C++

```cmd
cl /O2 /arch:AVX2 /EHsc /W3 /MD /DNDEBUG ^
   /I "%JAVA_HOME%\include" ^
   /I "%JAVA_HOME%\include\win32" ^
   /c /Fo build\fastjson.obj ^
   native\fastjson.cpp
```

### 3. Link DLL

```cmd
link /DLL /OUT:build\fastjson.dll ^
     build\fastjson.obj ^
     /DEF:native\fastjson.def ^
     /MACHINE:X64
```

---

## Maven Profiles

### Development (with tests)

```cmd
mvn clean package -Pdev
```

### Production (skip tests)

```cmd
mvn clean package -Pprod
```

### Release (sign artifacts)

```cmd
mvn clean package -Prelease
```

---

## Troubleshooting

### "cl is not recognized"

Run from **Visual Studio Developer Command Prompt**, not regular cmd.

### "jni.h not found"

Set JAVA_HOME:

```cmd
set JAVA_HOME=C:\Program Files\Java\jdk-17
```

### "AVX2 not supported"

Edit `compile.bat`, change `/arch:AVX2` to `/arch:SSE2` for older CPUs.

### Runtime error: "Can't find dependent libraries"

Install Visual C++ Redistributable:
https://aka.ms/vs/17/release/vc_redist.x64.exe

---

## Cross-Platform (Future)

### Linux

```bash
# Install dependencies
sudo apt-get install build-essential openjdk-17-jdk

# Compile
mkdir -p build
g++ -O3 -march=native -fPIC -shared \
    -I "$JAVA_HOME/include" \
    -I "$JAVA_HOME/include/linux" \
    -o build/libfastjson.so \
    native/fastjson.cpp

# Build JAR
mvn clean package
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Compile
mkdir -p build
clang++ -O3 -march=native -fPIC -shared \
    -I "$JAVA_HOME/include" \
    -I "$JAVA_HOME/include/darwin" \
    -o build/libfastjson.dylib \
    native/fastjson.cpp

# Build JAR
mvn clean package
```

---

## Verification

Check that native library is loaded:

```java
import fastjson.FastJSON;

System.out.println("Native loaded: " + FastJSON.isNativeLoaded());
System.out.println("SIMD Level: " + FastCore.getSimdLevel());
```

Expected output:
```
Native loaded: true
SIMD Level: AVX2
```

---

## JitPack Build

JitPack builds automatically from Git tags:

```bash
# Tag release
git tag -a v1.0.0 -m "Release 1.0.0"
git push origin v1.0.0
```

JitPack will build and host the JAR at:
`https://jitpack.io/#andrestubbe/fastjson/v1.0.0`

---

## Performance Tuning

### Verify AVX2 is used

Run with verbose output:

```cmd
mvn exec:java -Dexec.args="--verbose"
```

### Benchmark comparison

```cmd
mvn exec:java -Dexec.mainClass=fastjson.Benchmark
```

Results show Jackson vs FastJSON side-by-side.

---

## Release Checklist

- [ ] Version updated in `pom.xml`
- [ ] `CHANGELOG.md` updated
- [ ] Native libs compiled for all platforms
- [ ] `mvn clean test` passes
- [ ] Benchmarks run
- [ ] Git tag created: `git tag -a v1.0.0 -m "Release 1.0.0"`
- [ ] GitHub Release created with JAR

---

**Part of the FastJava Ecosystem**
