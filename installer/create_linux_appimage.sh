#!/bin/bash
# Linux AppImage creator for Drum Machine
# Requirements: linuxdeploy, linuxdeploy-plugin-appimage
# Install: https://github.com/linuxdeploy/linuxdeploy

set -e

APP_DIR="DrumMachine.AppDir"
VERSION="0.1.0"

echo "Creating Linux AppImage for Drum Machine v$VERSION"

# Create AppDir structure
mkdir -p "$APP_DIR/usr/bin"
mkdir -p "$APP_DIR/usr/lib"
mkdir -p "$APP_DIR/usr/share/applications"
mkdir -p "$APP_DIR/usr/share/icons/hicolor/256x256/apps"

# Copy executable
cp bin/Release/DrumMachine "$APP_DIR/usr/bin/" || cp bin/Debug/DrumMachine "$APP_DIR/usr/bin/"
chmod +x "$APP_DIR/usr/bin/DrumMachine"

# Copy shared libraries
if [ -f bin/Release/libSDL2.so ]; then
    cp bin/Release/libSDL2.so* "$APP_DIR/usr/lib/" 2>/dev/null || true
fi
if [ -f bin/Release/librtaudio.so ]; then
    cp bin/Release/librtaudio.so* "$APP_DIR/usr/lib/" 2>/dev/null || true
fi
if [ -f bin/Release/librtmidi.so ]; then
    cp bin/Release/librtmidi.so* "$APP_DIR/usr/lib/" 2>/dev/null || true
fi

# Copy assets
mkdir -p "$APP_DIR/usr/share/drummachine/assets"
cp -r ../assets/samples "$APP_DIR/usr/share/drummachine/assets/" 2>/dev/null || true
cp -r ../assets/fonts "$APP_DIR/usr/share/drummachine/assets/" 2>/dev/null || true
cp -r ../assets/images "$APP_DIR/usr/share/drummachine/assets/" 2>/dev/null || true

# Create desktop entry
cat > "$APP_DIR/usr/share/applications/DrumMachine.desktop" << 'EOF'
[Desktop Entry]
Type=Application
Name=Drum Machine
Comment=Step Sequencer and Sample Player
Exec=DrumMachine
Icon=drummachine
Categories=Audio;Utility;
Terminal=false
EOF

# Create AppRun script
cat > "$APP_DIR/AppRun" << 'EOF'
#!/bin/bash
HERE="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
export PATH="${HERE}/usr/bin:${PATH}"
exec "${HERE}/usr/bin/DrumMachine" "$@"
EOF
chmod +x "$APP_DIR/AppRun"

echo "AppDir structure created: $APP_DIR"
echo ""
echo "To create AppImage (requires linuxdeploy):"
echo "  linuxdeploy --appimage-extract-and-run \\"
echo "    --appdir=$APP_DIR \\"
echo "    --output=appimage \\"
echo "    --custom-apprun=$APP_DIR/AppRun"
echo ""
echo "Or manually package with:"
echo "  appimagetool $APP_DIR DrumMachine-${VERSION}-x86_64.AppImage"
