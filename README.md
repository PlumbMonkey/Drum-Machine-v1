# Drum Machine v1

A production-grade drum machine and step sequencer. The first module in a modular music software ecosystem.

**Version:** 0.1.0  
**Status:** MVP / Development  
**Platforms:** Windows, macOS

## Quick Start

### Prerequisites
- CMake 3.20+
- C++17 compiler (MSVC, Clang, or GCC)
- Git

### Build

```bash
cd Drum Machine-v1
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Run

```bash
./bin/DrumMachine
```

## Project Structure

```
Drum Machine-v1/
├── src/
│   ├── audio/          # RtAudio + sample playback
│   ├── sequencer/      # Transport, pattern, sequencer logic
│   ├── ui/             # ImGui + custom graphics
│   ├── data/           # JSON serialization
│   └── main.cpp        # Entry point
├── include/            # Public headers
├── assets/             # Graphical and audio assets
├── tests/              # Unit tests (Catch2)
└── build/              # CMake build output
```

## Architecture

**Layered design for real-time audio:**

1. **Audio Engine** (RtAudio)
   - Real-time audio callback
   - Lock-free communication with UI
   - Sample-accurate scheduling

2. **Sequencer Logic**
   - Pattern data model
   - Transport state (tempo, swing, time signature)
   - Step scheduling with timing

3. **UI Layer** (ImGui + SDL2)
   - Immediate-mode GUI
   - Custom graphical assets
   - Real-time feedback

4. **Data Layer** (nlohmann/json)
   - Project serialization
   - Save/load functionality

## Tech Stack

| Component | Library | License |
|-----------|---------|---------|
| Audio I/O | RtAudio | MIT |
| Sample Loading | dr_wav | Public Domain |
| UI | Dear ImGui | MIT |
| Graphics | SDL2 | zlib |
| JSON | nlohmann/json | MIT |
| Build | CMake | BSD |
| Testing | Catch2 | Boost |

**Total cost:** $0  
**Total licensing:** All permissive (MIT/Public Domain/BSD)

## Features (v1)

### Sequencer
- 16-step grid per bar
- 1–5 bars per pattern
- 8 GM-compatible drum tracks
- Step on/off toggles
- Playhead visualization

### Transport
- Global tempo (60–180 BPM)
- Swing (0–60%)
- Time signatures: 4/4, 3/4, 6/8
- Sample-accurate timing (no drift)

### Audio Engine
- WAV sample playback
- Auto-resampling to engine sample rate
- Per-track retrigger behavior
- No artificial limits

### UI
- Custom-designed GUI
- Real-time control feedback
- Functional, intentional design

## Milestones

1. **Audio Proof-of-Concept** — Play samples at tempo
2. **Sequencer Logic** — 8 tracks, 16 steps, patterns
3. **Minimal UI** — Basic ImGui controls
4. **Custom Graphics** — Brand-identity visuals
5. **Full Features** — Swing, time signatures, metronome
6. **Data Persistence** — Save/load projects
7. **Release** — Cross-platform packaging

## Not in v1 (Out of Scope)

- Per-step velocity or probability
- Automation lanes
- Pattern chaining or MIDI out
- Effects processing
- Preset browser
- Plugin builds (VST3/AU)
- Waveform/spectrum visualization

These belong to v2+.

## Development Notes

- **Audio engine is real-time safe.** No allocations in callback.
- **UI and audio are decoupled.** Lock-free state updates.
- **Parameter IDs are stable.** Foundation for presets and automation.
- **No backward-breaking changes.** v1 data must work in v2+.

## License

Proprietary. All rights reserved.

---

**"I am building tools that I own, understand, and evolve — not renting creativity from others."**
