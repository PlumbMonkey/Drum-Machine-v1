# AI Agent Instructions for Drum Machine v1

## Project Overview

**Drum Machine v1** is a production-grade step sequencer and sample player (v0.1.0, MVP phase). Architecture is layered: real-time audio engine (RtAudio) → sequencer logic (timing/patterns) → immediate-mode UI (ImGui) → JSON serialization. This is milestone 1, focused on audio proof-of-concept.

## Critical Architecture

### Layered Real-Time Design
- **Audio Thread** (RtAudio callback, lock-free): `AudioEngine::processAudio()` runs on dedicated thread, must not block. Uses atomic counters for lock-free communication with UI.
- **Sequencer Logic**: `Sequencer`, `Pattern`, `Transport` coordinate sample-accurate timing. Transport tracks tempo (60–180 BPM), swing (0–60%), time signatures (4/4, 3/4, 6/8). No assumptions about bar length—driven by `Transport::setBarCount()`.
- **Sample Playback**: `SamplePlayer` handles WAV loading (dr_wav), multi-channel support, interpolation resampling. Loops samples at configured position without drift.
- **UI Layer** (TODO): ImGui planned for immediate-mode interface. Not yet integrated; current codebase is CLI-only.

### Key Data Flows
1. **Timing**: `Transport` calculates sample offsets from BPM → `Sequencer::shouldTrigger()` checks per-track per-step → `SamplePlayer::play()` triggers at exact sample.
2. **Scheduling**: `Sequencer::advanceFrame()` called by audio callback every buffer; swing delay computed by `getSwingDelayedSample()`.
3. **Drift Prevention**: Audio callback tracks absolute frame count (`totalFramesProcessed_`); sequencer verifies no samples missed across 5+ minute runs.

## Build & Test Workflow

### Build Command (Windows)
```powershell
cd "d:\Dev Projects 2025\Drum Machine-v1\build"
cmake .. -G "Visual Studio 17 2022"  # One-time configure
cmake --build . --config Release      # Incremental compile
```

### Dependencies (auto-fetched via CMake FetchContent)
- **RtAudio** (audio I/O, MIT)
- **dr_wav** (WAV loading, public domain)
- **nlohmann/json** (serialization, MIT)
- **Catch2** (testing, Boost)

**Note**: No manual installation required; CMake pulls all via git URLs in `CMakeLists.txt`.

### Running & Testing
- **Main executable**: `./bin/Release/DrumMachine.exe` runs 5-minute audio timing validation (Milestone 1 proof-of-concept).
- **Unit tests**: `ctest` (requires Catch2 integration in test suite; see `tests/CMakeLists.txt`).
- **Test sample**: `assets/samples/test_kick.wav` (100ms kick, auto-generated if missing).

### Debugging Audio Issues
- Check frame counts: `audioEngine.getTotalFramesProcessed()` should advance by `sampleRate * seconds` (e.g., 220.5M frames in 5 min at 44.1 kHz).
- Verify sample loading: `SamplePlayer::getDurationSeconds()` confirms correct metadata.
- Inspect timing drift: compare expected vs. actual frame deltas in 30-second checkpoints (main.cpp logs this).

## Patterns & Conventions

### C++17 Codebase
- **Namespaces**: All code in `DrumMachine::` namespace.
- **Headers**: Public APIs in `include/DrumMachine.h`; implementation headers in `src/*/`. No `#pragma once`; use include guards.
- **Memory**: Use `std::unique_ptr`, `std::atomic<>` for thread-safe types. No raw pointers except transient references.
- **Callbacks**: RtAudio callback wraps via static method + userData pointer (see `AudioEngine::audioCallback()`). Audio thread never allocates/deallocates.

### Real-Time Constraints
- **Audio thread must be lock-free**: No mutexes, `new`/`delete`, or blocking I/O in `processAudio()`.
- **Main thread**: Loads samples, configures transport. UI changes should queue commands for audio thread (not yet implemented; focus on atomic flags if needed).
- **Sample timing**: All offsets are 64-bit sample counts (`uint64_t`), not floating-point, to avoid rounding drift over hours.

### File Organization
- `src/audio/`: RtAudio wrapper, sample loading, frame processing.
- `src/sequencer/`: Transport (tempo/time signature), Pattern (step data), Sequencer (scheduling logic).
- `src/ui/`: UI scaffolding (ImGui integration TBD).
- `src/data/`: JSON serialization (DataManager, not yet integrated).
- `tests/`: Catch2 unit tests (directory structure exists; tests TBD).
- `assets/samples/`: WAV files for testing (test_kick.wav provided).

### Error Handling
- **Critical failures**: `initialize()` returns `bool`; check before proceeding (see main.cpp pattern).
- **File I/O**: `SamplePlayer::loadSample()` returns false if WAV invalid or unreadable.
- **Audio startup**: If RtAudio fails, gracefully shutdown and report device list.

## Development Priorities (Milestone 2+)

- **UI Integration**: Add ImGui rendering and event handling. Immediate-mode pattern: draw UI each frame, read input from SDL2.
- **Step Editor**: Implement UI grid for 16-step patterns; toggle steps on/off.
- **Pattern Persistence**: Save/load `.json` patterns via DataManager (library ready, not wired to UI yet).
- **Swing Visualization**: Plot step timings showing swing offset to validate logic.
- **Cross-Platform Testing**: Verify audio callback works on macOS (CoreAudio) and Linux (ALSA).

## References

- **README.md**: High-level overview, tech stack, features.
- **MILESTONE_1_BUILD.md**: Detailed build steps and proof-of-concept validation.
- **BUILD.md**: Cross-platform build setup.
- **CMakeLists.txt**: Dependency fetching, compiler flags, target definitions.
- **main.cpp**: Reference implementation for initialization sequence and timing validation.
