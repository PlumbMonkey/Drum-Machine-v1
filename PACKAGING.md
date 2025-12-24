# Drum Machine - Cross-Platform Packaging Guide

This guide explains how to build and package Drum Machine for Windows, macOS, and Linux for distribution.

## Table of Contents
1. [Windows Installer](#windows-installer)
2. [macOS App Bundle](#macos-app-bundle)
3. [Linux AppImage](#linux-appimage)
4. [Gumroad Distribution](#gumroad-distribution)

---

## Windows Installer

### Prerequisites
- NSIS (Nullsoft Scriptable Install System)
  - Download: https://nsis.sourceforge.io/Download
  - Install to default location

### Build Steps

1. **Build Release Binary**
   ```powershell
   cd "d:\Dev Projects 2025\Drum Machine-v1\build"
   $cmake = 'C:\Program Files\CMake\bin\cmake.exe'
   & $cmake --build . --config Release
   ```

2. **Create Installer**
   ```powershell
   # Navigate to NSIS installer directory
   cd "d:\Dev Projects 2025\Drum Machine-v1\installer"
   
   # Compile with NSIS
   & "C:\Program Files (x86)\NSIS\makensis.exe" DrumMachine.nsi
   ```

3. **Output**
   - Generated file: `DrumMachine-0.1.0-Windows.exe`
   - Ready for distribution on Gumroad
   - Includes desktop shortcut and Start Menu shortcuts
   - Automatic uninstaller

### Customization
Edit `DrumMachine.nsi` to:
- Change `OutFile` for different version numbers
- Modify `InstallDir` for different installation location
- Add/remove bundled assets
- Customize desktop shortcut behavior

---

## macOS App Bundle

### Prerequisites
- macOS 10.13 or later
- Xcode Command Line Tools: `xcode-select --install`
- CMake: `brew install cmake`
- SDL2: `brew install sdl2`

### Build Steps

1. **Clean build directory**
   ```bash
   rm -rf build
   mkdir build && cd build
   ```

2. **Configure for macOS**
   ```bash
   cmake .. -G "Xcode" -DCMAKE_BUILD_TYPE=Release
   # Or for Unix Makefiles:
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build Release Binary**
   ```bash
   cmake --build . --config Release
   ```

4. **Create App Bundle**
   ```bash
   cd ..
   chmod +x installer/create_macos_bundle.sh
   installer/create_macos_bundle.sh
   ```

5. **Create DMG for Distribution**
   ```bash
   cd build
   hdiutil create -volname "Drum Machine" \
     -srcfolder DrumMachine.app \
     -ov -format UDZO \
     DrumMachine-0.1.0-macOS.dmg
   ```

### Output
- Generated file: `DrumMachine-0.1.0-macOS.dmg`
- Ready for distribution on Gumroad
- Double-click to mount and drag to Applications folder

### Code Signing (Optional but Recommended for Distribution)
```bash
# Sign the app bundle
codesign --deep --force --verify --verbose \
  --sign - build/DrumMachine.app

# Notarize for Gatekeeper (Apple requirement for Mac App Store):
# See: https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution
```

---

## Linux AppImage

### Prerequisites
- Linux x86_64 system
- GCC/Clang with C++17 support
- CMake 3.20+
- linuxdeploy: https://github.com/linuxdeploy/linuxdeploy
- AppImage tools

### Install Dependencies (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake git \
  libsdl2-dev \
  libasound2-dev \
  libjack-dev

# Get linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
```

### Build Steps

1. **Clean build directory**
   ```bash
   rm -rf build
   mkdir build && cd build
   ```

2. **Configure for Linux**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build Release Binary**
   ```bash
   cmake --build . --config Release
   ```

4. **Create AppImage**
   ```bash
   cd ..
   chmod +x installer/create_linux_appimage.sh
   installer/create_linux_appimage.sh
   
   # Build AppImage (from build directory)
   cd build
   ../linuxdeploy-x86_64.AppImage \
     --appimage-extract-and-run \
     --appdir=DrumMachine.AppDir \
     --output=appimage
   ```

5. **Output**
   - Generated file: `DrumMachine-0.1.0-x86_64.AppImage`
   - Single executable, no installation needed
   - Ready for distribution on Gumroad

### Making AppImage Executable
```bash
chmod +x DrumMachine-0.1.0-x86_64.AppImage
./DrumMachine-0.1.0-x86_64.AppImage  # Run directly
```

---

## Gumroad Distribution

### Preparation Checklist
- [ ] Windows: `DrumMachine-0.1.0-Windows.exe` (installer)
- [ ] macOS: `DrumMachine-0.1.0-macOS.dmg` (disk image)
- [ ] Linux: `DrumMachine-0.1.0-x86_64.AppImage` (AppImage)
- [ ] Create README with installation instructions
- [ ] Set version to 0.1.0 (or current version)
- [ ] Test each installer on target OS

### Gumroad Setup
1. Create new product on Gumroad
2. Upload all three installers as variant downloads:
   - **Windows**: `DrumMachine-0.1.0-Windows.exe`
   - **macOS**: `DrumMachine-0.1.0-macOS.dmg`
   - **Linux**: `DrumMachine-0.1.0-x86_64.AppImage`
3. Set price/free status
4. Write description mentioning:
   - 8-track drum sequencer
   - MIDI controller support
   - Pattern save/load (JSON + MIDI)
   - Cross-platform (Windows, macOS, Linux)
   - Supported audio interfaces
   - System requirements per platform

### README Template for Distribution
```
# Drum Machine v0.1.0

## Features
- 8 tracks × 16-step pattern grid
- Real-time audio playback with sample-accurate timing
- MIDI controller support with CC automation
- Swing control (0-60%)
- Pattern persistence (JSON & MIDI formats)
- Per-track volume, pan, mute controls
- Cross-platform support

## Installation

### Windows
1. Download `DrumMachine-0.1.0-Windows.exe`
2. Run installer
3. Desktop shortcut will be created automatically
4. Requires Windows 10/11

### macOS
1. Download `DrumMachine-0.1.0-macOS.dmg`
2. Double-click to mount
3. Drag DrumMachine.app to Applications folder
4. Requires macOS 10.13 or later

### Linux
1. Download `DrumMachine-0.1.0-x86_64.AppImage`
2. Make executable: `chmod +x DrumMachine-0.1.0-x86_64.AppImage`
3. Run: `./DrumMachine-0.1.0-x86_64.AppImage`
4. Tested on Ubuntu 20.04+

## System Requirements

### Windows
- Windows 10 or later
- 100 MB disk space
- Any audio interface supported by Windows

### macOS
- macOS 10.13 or later
- 100 MB disk space
- Any audio interface supported by macOS

### Linux
- x86_64 processor
- 100 MB disk space
- ALSA or PulseAudio for audio

## Usage
[Add quick start guide here]

## Support
[Add contact/support info]
```

---

## Automated Build Script (Optional)

Create a `build_all.sh` for automated multi-platform builds:

```bash
#!/bin/bash
VERSION="0.1.0"

echo "Building Drum Machine v$VERSION for all platforms..."

# Windows (requires NSIS on Windows)
echo "Build Windows installer from Windows machine using:"
echo "  makensis.exe installer/DrumMachine.nsi"

# macOS (requires macOS)
echo "Build macOS DMG from macOS machine using:"
echo "  ./installer/create_macos_bundle.sh"

# Linux (requires Linux)
echo "Build Linux AppImage from Linux machine using:"
echo "  ./installer/create_linux_appimage.sh"

echo "All builds complete!"
```

---

## Version Management

When releasing new versions:

1. Update `CMakeLists.txt`:
   ```cmake
   project(DrumMachine VERSION 0.2.0 LANGUAGES CXX)
   ```

2. Update installer scripts:
   - `DrumMachine.nsi`: Change version in `OutFile` and `VIProductVersion`
   - `create_macos_bundle.sh`: Update `VERSION="0.2.0"`

3. Rebuild all platforms
4. Test installers on fresh systems
5. Upload to Gumroad with release notes

---

## Troubleshooting

### Windows Installer
- **Missing DLLs**: Ensure SDL2.dll, rtaudio.dll, rtmidi.dll are in build/bin/Release/
- **NSIS not found**: Install NSIS from https://nsis.sourceforge.io/

### macOS
- **Code signing issues**: Use `codesign --remove-signature` to test locally
- **Library not found**: Check that dylib dependencies are in MACOS directory

### Linux
- **Permission denied**: Run `chmod +x DrumMachine-0.1.0-x86_64.AppImage`
- **Library errors**: AppImage bundles most deps, but ALSA may need system package

