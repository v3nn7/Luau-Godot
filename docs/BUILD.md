# Build Instructions - Godot Luau Extension

## Environment Setup

### Windows

1. **Visual Studio 2019 or newer**
   ```bash
   # Download Visual Studio Community (free)
   # Install "Desktop development with C++" workload
   ```

2. **Python 3.6+**
   ```bash
   # Download from https://python.org
   pip install scons
   ```

3. **Git**
   ```bash
   # Download from https://git-scm.com
   ```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential scons pkg-config libx11-dev libxcursor-dev \
    libxinerama-dev libgl1-mesa-dev libglu-dev libasound2-dev libpulse-dev \
    libudev-dev libxi-dev libxrandr-dev python3-pip

pip3 install scons
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if you don't have it)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install SCons
brew install scons python3
pip3 install scons
```

## Cloning the Project

```bash
git clone --recursive https://github.com/your-username/godot-luau-extension.git
cd godot-luau-extension

# If you forgot --recursive
git submodule update --init --recursive
```

## Step-by-Step Compilation

### 1. Environment Check

```bash
# Check SCons availability
scons --version

# Check compiler
# Windows: 
cl
# Linux/macOS:
gcc --version
```

### 2. Debug Build (Development)

```bash
# Windows
scons target=template_debug platform=windows arch=x86_64

# Linux  
scons target=template_debug platform=linux arch=x86_64

# macOS
scons target=template_debug platform=osx
```

### 3. Release Build (Production)

```bash
# Windows
scons target=template_release platform=windows arch=x86_64

# Linux
scons target=template_release platform=linux arch=x86_64

# macOS 
scons target=template_release platform=osx
```

### 4. Editor Build (Optional)

```bash
# If you want to use in Godot editor
scons target=editor platform=windows arch=x86_64
```

## Build Options

### Basic Options

```bash
# Architecture (32-bit vs 64-bit)
scons arch=x86_32    # 32-bit
scons arch=x86_64    # 64-bit (default)
scons arch=arm64     # Apple Silicon (M1/M2)

# Target
scons target=template_debug     # Debug with optimizations
scons target=template_release   # Release with full optimizations  
scons target=editor             # For use in editor

# Platform
scons platform=windows
scons platform=linux
scons platform=osx
```

### Advanced Options

```bash
# Parallel compilation (speeds up build)
scons -j8  # uses 8 cores

# Verbose output (detailed logs)
scons verbose=yes

# Static C++ linking
scons use_static_cpp=yes

# Use LLVM/clang instead of GCC
scons use_llvm=yes

# Generate compile_commands.json (for IDEs)
scons compiledb=yes
```

## Troubleshooting

### Windows

**Issue**: `'cl' is not recognized as an internal or external command`
```bash
# Solution: Run "x64 Native Tools Command Prompt" from Visual Studio
# or set environment variables:
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
```

**Issue**: `Python was not found`
```bash
# Solution: Add Python to PATH or use py launcher
py -m pip install scons
```

### Linux

**Issue**: `fatal error: 'godot_cpp/core/defs.hpp' file not found`
```bash
# Solution: Check submodules
git submodule update --init --recursive
```

**Issue**: `ld: cannot find -lpulse`
```bash
# Solution: Install missing libraries
sudo apt install libpulse-dev libasound2-dev
```

### macOS

**Issue**: `xcrun: error: invalid active developer path`
```bash
# Solution: Reinstall Xcode Command Line Tools
sudo xcode-select --reset
xcode-select --install
```

**Issue**: Linker errors with Apple Silicon
```bash
# Solution: Use native architecture
scons arch=arm64 platform=osx
```

## Testing the Build

### 1. Check Results

```bash
# Library should be in bin/ folder
ls bin/
# Windows: libluau-windows.template_debug.x86_64.dll
# Linux:   libluau-linux.template_debug.x86_64.so  
# macOS:   libluau-osx.template_debug.dylib
```

### 2. Test in Godot

```bash
# Copy files to Godot test project
mkdir test_project
cp luau.gdextension test_project/
cp -r bin/ test_project/
cp -r examples/ test_project/

# Launch Godot with this project
godot test_project/
```

## Development Mode

### Quick Development Loop

```bash
# 1. Edit C++ code
# 2. Quick debug compilation  
scons -j8 target=template_debug

# 3. Test in Godot (F5 in editor)
# 4. Repeat
```

### Debugging

```bash
# Compile with debug symbols
scons target=template_debug debug_symbols=yes

# Run Godot through debugger
# Windows: Visual Studio debugger
# Linux: gdb godot
# macOS: lldb godot
```

## Clean Build

```bash
# Clean previous builds
scons -c

# Or manually
rm -rf bin/
rm -rf extern/godot-cpp/bin/
```

## Cross-Compilation

### From Linux to Windows

```bash
# Install MinGW
sudo apt install mingw-w64

# Compile
scons platform=windows arch=x86_64 use_mingw=yes
```

### From macOS to Linux (Docker)

```bash
# Use Docker with Linux image
docker run -v $(pwd):/project ubuntu:20.04
# Install dependencies in container and compile
```

## Performance Tuning

### Compilation Optimizations

```bash
# Maximum optimizations
scons target=template_release optimize=speed

# Link Time Optimization
scons lto=yes

# Profile Guided Optimization  
scons pgo=yes
```

### Size Reduction

```bash
# Remove debug symbols from release
scons debug_symbols=no

# Static linking (but larger size)
scons use_static_cpp=yes
```

## CI/CD

### GitHub Actions Example

```yaml
name: Build
on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
        
    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive
        
    - name: Setup Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
        
    - name: Install SCons
      run: pip install scons
      
    - name: Build Debug
      run: scons target=template_debug
      
    - name: Build Release
      run: scons target=template_release
```

## Build Verification

### Automated Tests

```bash
# Run after successful build
cd test_project
godot --headless --quit-after 100

# Check for errors in logs
grep -i error godot.log
```

### Manual Verification

1. **Library Loading**: Check that `.gdextension` file loads without errors
2. **Script Recognition**: Verify `.luau` files are recognized as scripts
3. **API Access**: Test basic Godot API calls from Luau
4. **Performance**: Run performance tests for critical paths

This completes the build instructions for the Godot Luau Extension!
