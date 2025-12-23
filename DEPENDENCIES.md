# Dependencies

Drum Machine v1 uses the following libraries. All are permissive-licensed (MIT/Public Domain/BSD).

## External Dependencies

### RtAudio (Audio I/O)
- **URL:** http://www.music.mcgill.ca/~gary/rtaudio/
- **License:** MIT
- **Status:** Needs integration into CMakeLists.txt
- **Note:** Cross-platform real-time audio, will be integrated via CMake find_package() or FetchContent

### dr_wav (WAV Loading)
- **URL:** https://github.com/mackron/dr_libs
- **License:** Public Domain
- **Status:** Single-header library, place in `include/dr_libs/`
- **Integration:** Copy header file, include and use

### Dear ImGui (UI Framework)
- **URL:** https://github.com/ocornut/imgui
- **License:** MIT
- **Status:** Needs integration
- **Note:** Immediate-mode GUI, will be integrated via CMake

### SDL2 (Window + Graphics Context)
- **URL:** https://www.libsdl.org/
- **License:** zlib
- **Status:** System or FetchContent integration
- **Note:** Creates window and OpenGL context for ImGui

### nlohmann/json (JSON Serialization)
- **URL:** https://github.com/nlohmann/json
- **License:** MIT
- **Status:** Single-header library, place in `include/nlohmann/`
- **Integration:** Copy header, include and use

### Catch2 (Testing Framework)
- **URL:** https://github.com/catchorg/Catch2
- **License:** Boost
- **Status:** For unit tests, will be integrated via CMake

## Current Status

These dependencies are noted in CMakeLists.txt but not yet integrated. Integration plan:

1. **dr_wav** — Copy header to `include/dr_libs/`
2. **nlohmann/json** — Copy header to `include/nlohmann/`
3. **RtAudio** — Integrate via FetchContent or find_package()
4. **SDL2** — Find or fetch
5. **ImGui** — Fetch and integrate with SDL2 + OpenGL
6. **Catch2** — Add to tests subdirectory

All dependencies will maintain permissive licensing (100% ownership, no restrictions).
