# Drum Machine v0.1.0

A professional-grade step sequencer and sample player with real-time audio processing, MIDI support, and cross-platform compatibility.

## Features

- **8-Track Sequencer**: 16-step drum patterns with real-time playback
- **Sample Accurate Timing**: Drift-free audio synchronization (tested for 5+ minute sessions)
- **MIDI Controller Support**: Connect external hardware controllers with CC automation
- **Pattern Management**: Save/load patterns in JSON and MIDI formats
- **Swing Control**: 0-60% swing with visual offset feedback
- **Per-Track Controls**: Independent mute, volume, and pan for each track
- **Cross-Platform**: Windows, macOS, and Linux support
- **Professional Installer**: One-click installation on all platforms

## System Requirements

### Windows
- Windows 10 or later
- Any audio interface with Windows audio driver support
- 100 MB disk space

### macOS
- macOS 10.13 or later
- Any audio interface with macOS audio driver support
- 100 MB disk space

### Linux
- x86_64 processor
- Ubuntu 20.04+ or equivalent
- ALSA or PulseAudio for audio
- 100 MB disk space

## Installation

### Windows
1. Download `DrumMachine-0.1.0-Windows.exe`
2. Run the installer
3. A desktop shortcut will be created automatically
4. Launch from the shortcut or Start Menu

**Uninstall**: Use Windows Add/Remove Programs

### macOS
1. Download `DrumMachine-0.1.0-macOS.dmg`
2. Double-click to mount the disk image
3. Drag `DrumMachine.app` to your Applications folder
4. Launch from Applications or Spotlight

**Uninstall**: Drag the app to Trash

### Linux
1. Download `DrumMachine-0.1.0-x86_64.AppImage`
2. Make it executable:
   ```bash
   chmod +x DrumMachine-0.1.0-x86_64.AppImage
   ```
3. Run it:
   ```bash
   ./DrumMachine-0.1.0-x86_64.AppImage
   ```

**Uninstall**: Delete the AppImage file

## Quick Start

1. **Launch the application** via desktop shortcut or app launcher
2. **Load samples** using the Sample Browser (File menu or UI panel)
3. **Click steps** in the 16-step grid to activate notes
4. **Press Play** to start the sequencer
5. **Adjust tempo** with the Transport slider (60-180 BPM)
6. **Save patterns** using the Pattern Manager

## Controls

- **Play/Stop Button**: Start/stop audio playback
- **Tempo Slider**: Adjust BPM (60-180)
- **Swing Slider**: Add swing offset to even-numbered steps (0-60%)
- **Time Signature**: Select 4/4, 3/4, or 6/8 time
- **Step Grid**: Click to toggle steps on/off
- **Track Buttons**: Select which track to edit
- **Mute Button**: Mute individual tracks
- **Volume/Pan**: Control track levels

## File Formats

### Pattern Files
Patterns are saved as JSON in the `patterns/` directory:
```json
{
  "tempo": 120,
  "timeSignature": "4/4",
  "tracks": [
    {"muted": false, "volume": 1.0, "pan": 0.0, "sample": "path/to/sample.wav"}
  ],
  "steps": [[true, false, true, ...], ...]
}
```

### MIDI Export
Patterns can be exported to Standard MIDI Files (.mid) for use in other DAWs:
- Drum track mapping: MIDI notes 36-43 (standard drum kit)
- Tempo preserved as MIDI Set Tempo event
- Compatible with any DAW

## MIDI Controller

### Port Selection
1. Open MIDI Controller panel
2. Click "Connect" next to your device
3. Listen for incoming messages

### CC Mapping
- **CC 7** (Volume) → Track Volume
- **CC 10** (Pan) → Track Pan
- Monitor current CC values in the UI

## Troubleshooting

### No Audio Output
1. Check that your audio interface is connected
2. Verify Windows/macOS audio settings recognize the device
3. Load a sample and ensure a step is activated
4. Press Play

### MIDI Not Detected
1. Ensure your controller is connected before launching
2. Click "Refresh" in MIDI Controller panel
3. Check device manufacturer's driver installation

### Pattern Won't Save
1. Ensure you have write permissions in the patterns/ directory
2. Try a simpler filename (avoid special characters)
3. Check available disk space

## Audio Engine Details

- **Sample Rate**: 44.1 kHz
- **Buffer Size**: Configurable
- **Latency**: <10 ms typical
- **Audio API**: WASAPI (Windows), CoreAudio (macOS), ALSA (Linux)
- **Maximum Tracks**: 8
- **Maximum Pattern Length**: 16 steps

## Development & Source Code

Source code available at: https://github.com/PlumbMonkey/Drum-Machine-v1

Built with:
- C++17
- CMake 3.20+
- RtAudio for cross-platform audio
- RtMidi for MIDI input
- SDL2 + OpenGL for UI
- Dear ImGui for immediate-mode GUI
- nlohmann/json for serialization

## License

MIT License - See LICENSE file for details

## Support

For issues, feature requests, or questions:
- GitHub Issues: https://github.com/PlumbMonkey/Drum-Machine-v1/issues
- Contact: [Your contact info here]

## Changelog

### v0.1.0 (Initial Release)
- Core drum sequencer with 8 tracks × 16 steps
- Real-time audio playback with sample loading
- MIDI controller support
- Pattern save/load (JSON + MIDI)
- Swing visualization
- Cross-platform installers (Windows, macOS, Linux)
- Professional UI with Dear ImGui

---

**Enjoy making music!** 🎵
