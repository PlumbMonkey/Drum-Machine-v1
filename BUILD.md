# Build Instructions

## Prerequisites

### Windows
- **Compiler:** MSVC 2019+ (Visual Studio 2019+), or MinGW with GCC 10+
- **CMake:** 3.20+
- **Git:** For cloning and version control

### macOS
- **Compiler:** Apple Clang (Xcode Command Line Tools)
- **CMake:** 3.20+
- **Git:** Homebrew install: `brew install cmake git`

### Linux
- **Compiler:** GCC 10+ or Clang 12+
- **CMake:** 3.20+
- **Git:** `sudo apt install cmake git` (or your package manager)

## Build Steps

### 1. Clone Repository

```bash
git clone <repository-url> "Drum Machine-v1"
cd "Drum Machine-v1"
```

### 2. Create Build Directory

```bash
mkdir build
cd build
```

### 3. Configure CMake

```bash
cmake ..
```

**Optional:** Specify generator or options:
```bash
# Windows (MSVC)
cmake .. -G "Visual Studio 16 2019"

# macOS (Xcode)
cmake .. -G Xcode

# Linux (Unix Makefiles)
cmake .. -G "Unix Makefiles"
```

### 4. Build Project

```bash
cmake --build . --config Release
```

**Or directly with your build tool:**
```bash
# MSVC
msbuild DrumMachine.sln /p:Configuration=Release

# Make
make

# Xcode
xcodebuild -scheme DrumMachine -configuration Release
```

### 5. Run Application

```bash
# Windows
./bin/Release/DrumMachine.exe

# macOS / Linux
./bin/DrumMachine
```

## Build Targets

- **DrumMachine** — Main application
- **DrumMachine_tests** — Unit tests (requires Catch2)

## Troubleshooting

### CMake Not Found
- **Windows:** Add CMake to PATH, or use CMake GUI
- **macOS/Linux:** `brew install cmake` or `sudo apt install cmake`

### Compiler Not Found
- **Windows:** Install Visual Studio with C++ workload
- **macOS:** `xcode-select --install`
- **Linux:** `sudo apt install build-essential` or equivalent

### Build Fails
1. Delete `build/` directory
2. Recreate and reconfigure from scratch
3. Check CMakeLists.txt for missing dependencies

## Development Build (Debug)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

Produces larger executables with debug symbols and no optimization.

## Release Build

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

Optimized build suitable for distribution.
