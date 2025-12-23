# Assets Directory

This directory contains all non-code resources for the Drum Machine.

## Structure

### images/
Custom graphical assets for the UI:
- **buttons/** — Button states (normal, hover, pressed, disabled)
- **sliders/** — Slider tracks and thumbs
- **icons/** — Play, stop, mute, load, metronome icons

### fonts/
TrueType fonts for the UI. Default: `primary.ttf`

### samples/
Example WAV files and project templates:
- Drum samples for demonstration
- `example_pattern.json` — Sample project file

## Asset Guidelines

### Buttons
- Create 3–4 states per button: normal, hover, pressed, disabled
- PNG format with transparency
- Recommended size: 64×64 px or higher
- Consistent visual style across all buttons

### Sliders
- Track: horizontal bar (e.g., 200×20 px)
- Thumb: draggable knob (e.g., 20×20 px)
- PNG with transparency

### Icons
- 32×32 or 64×64 px
- SVG or high-res PNG
- Consistent line weight and style

### Fonts
- TrueType (.ttf) or OpenType (.otf)
- License permissive or open-source
- Recommended: Inter, IBM Plex Mono, Roboto

## Integration

Assets are loaded at runtime via `AssetManager`:

```cpp
AssetManager assets("assets/");
assets.loadTexture("button_play", "images/buttons/play_normal.png");
assets.loadFont("primary", "fonts/primary.ttf", 14.0f);
```

ImGui can then reference these assets by name for rendering.
