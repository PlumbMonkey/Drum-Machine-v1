# Milestone 1: Audio Proof-of-Concept — BUILD & TEST GUIDE

## Prerequisites

Before building, ensure you have:

1. **CMake 3.20+** — Download from https://cmake.org/download/
   - Add to PATH during installation
   
2. **Visual Studio 2022** with C++ workload
   - Or use Clang or another C++17 compiler

3. **Git** (already in use)

## What's Implemented

✅ **AudioEngine.cpp** — Full RtAudio integration
  - Device detection and initialization
  - Real-time audio callback (lock-free frame counting)
  - Platform-specific audio APIs (WASAPI/Windows, CoreAudio/macOS, ALSA/Linux)
  - Proper error handling

✅ **SamplePlayer.cpp** — Complete dr_wav integration
  - WAV file loading with metadata detection
  - Multi-channel support (mono/stereo)
  - Linear interpolation resampling to engine sample rate
  - Loop playback with position tracking

✅ **CMakeLists.txt** — Full dependency management
  - FetchContent for RtAudio, dr_wav, nlohmann/json
  - Platform-specific compiler flags
  - Proper linking and includes

✅ **main.cpp** — Proof-of-concept application
  - 5-minute timing test with 30-second checkpoints
  - Real-time drift measurement
  - Detailed statistics output
  - Clean shutdown

✅ **Test WAV Sample** — Generated 100ms kick drum at 44.1 kHz
  - Frequency sweep (150 Hz → 50 Hz)
  - Exponential decay envelope
  - Located at: `assets/samples/test_kick.wav`

## Build Steps (Windows)

### Step 1: Install CMake
```powershell
# Option A: Download from https://cmake.org/download/
# Check "Add CMake to system PATH" during installation

# Option B: Using Winget
winget install Kitware.CMake
```

Verify installation:
```powershell
cmake --version
```

### Step 2: Configure
```powershell
cd "d:\Dev Projects 2025\Drum Machine-v1\build"
cmake .. -G "Visual Studio 17 2022"
```

This will:
- Fetch RtAudio, dr_wav, nlohmann/json dependencies
- Generate Visual Studio solution file
- Configure project

### Step 3: Build
```powershell
cmake --build . --config Release
```

Or open the Visual Studio solution and build from IDE.

### Step 4: Run
```powershell
.\bin\Release\DrumMachine.exe
```

## Expected Output

When running, you should see:

```
======================================
Drum Machine v0.1.0
Milestone 1: Audio Proof-of-Concept
======================================

[1/5] Initializing audio engine...
      Audio engine OK

[2/5] Initializing sequencer...
      Sequencer OK
      Tempo: 120 BPM

[3/5] Loading sample...
      Sample OK (0.100 seconds)

[4/5] Starting audio playback...
      Playback OK

[5/5] Monitoring playback for timing drift...

   Time (s) |   Frames Processed |  Expected Frames |  Drift (ms)
----------------------------------------------------------------------
      30.0 |           1323000 |           1323000 |       0.00 ms
      60.0 |           2646000 |           2646000 |       0.00 ms
      90.0 |           3969000 |           3969000 |       0.00 ms
     120.0 |           5292000 |           5292000 |       0.00 ms
     150.0 |           6615000 |           6615000 |       0.00 ms
     180.0 |           7938000 |           7938000 |       0.00 ms
     210.0 |           9261000 |           9261000 |       0.00 ms
     240.0 |          10584000 |          10584000 |       0.00 ms
     270.0 |          11907000 |          11907000 |       0.00 ms
     300.0 |          13230000 |          13230000 |       0.00 ms

Test complete!

Final Statistics:
================
Total elapsed time: 300.00 seconds
Frames processed:   13230000
Expected frames:    13230000
Total drift:        0.000 ms (0.0000 ms/sec)

✓ SUCCESS: Timing is accurate (< 1ms drift)

Drum Machine shutdown cleanly
```

## Success Criteria (Milestone 1)

✓ Timing does not drift (< 1ms over 5 minutes)
✓ Audio engine initializes without errors
✓ Sample loads and plays correctly
✓ No crashes with valid WAV files
✓ Cross-platform code ready (Windows/macOS/Linux)

## Troubleshooting

### CMake configuration fails
- Ensure CMake 3.20+ is installed and in PATH
- Delete `build/` folder and retry
- Check Visual Studio installation is complete

### RtAudio linking errors
- RtAudio will be fetched automatically during CMake configuration
- Check internet connection during `cmake ..`
- Verify no build directory corruption

### Audio device not found
- Check if audio devices are available in Windows Audio settings
- Try plugging in USB audio device or enabling default device
- Some VMs may not support audio

### Timing drift detected
- This is unlikely on a properly configured system
- Drift > 10ms may indicate system load or audio buffer issues
- Check system audio settings for buffer size

## Next Steps After Milestone 1

Once timing is verified as accurate:

1. **Commit results to GitHub**
   ```powershell
   cd "d:\Dev Projects 2025\Drum Machine-v1"
   git add .
   git commit -m "Milestone 1: Audio POC - Verified sample-accurate timing at 120 BPM"
   git push
   ```

2. **Proceed to Milestone 2: Sequencer Logic**
   - 8 tracks, 16 steps per bar
   - Pattern data model
   - Transport with swing implementation

## Reference

- **RtAudio Documentation:** http://www.music.mcgill.ca/~gary/rtaudio/
- **dr_wav Documentation:** https://github.com/mackron/dr_libs
- **CMake Documentation:** https://cmake.org/cmake/help/latest/
