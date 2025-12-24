#!/bin/bash
# macOS app bundle creator for Drum Machine
# Run this from the build directory after cmake --build . --config Release

APP_NAME="DrumMachine"
APP_BUNDLE="${APP_NAME}.app"
CONTENTS="$APP_BUNDLE/Contents"
MACOS="$CONTENTS/MacOS"
RESOURCES="$CONTENTS/Resources"

echo "Creating macOS app bundle: $APP_BUNDLE"

# Create directory structure
mkdir -p "$MACOS"
mkdir -p "$RESOURCES"
mkdir -p "$RESOURCES/assets/samples"
mkdir -p "$RESOURCES/assets/fonts"
mkdir -p "$RESOURCES/assets/images"
mkdir -p "$RESOURCES/patterns"
mkdir -p "$RESOURCES/midi"

# Copy executable
cp bin/Release/DrumMachine "$MACOS/" || cp bin/Debug/DrumMachine "$MACOS/"
chmod +x "$MACOS/DrumMachine"

# Copy assets
cp -r ../assets/samples/* "$RESOURCES/assets/samples/" 2>/dev/null || true
cp -r ../assets/fonts/* "$RESOURCES/assets/fonts/" 2>/dev/null || true
cp -r ../assets/images/* "$RESOURCES/assets/images/" 2>/dev/null || true

# Copy dylib dependencies (they should be in same directory as executable after build)
# If not, copy from build output:
if [ -f bin/Release/libSDL2.dylib ]; then
    cp bin/Release/libSDL2.dylib "$MACOS/"
fi
if [ -f bin/Release/librtaudio.dylib ]; then
    cp bin/Release/librtaudio.dylib "$MACOS/"
fi
if [ -f bin/Release/librtmidi.dylib ]; then
    cp bin/Release/librtmidi.dylib "$MACOS/"
fi

# Create Info.plist (macOS app metadata)
cat > "$CONTENTS/Info.plist" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>DrumMachine</string>
    <key>CFBundleIdentifier</key>
    <string>com.drummachine.app</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>Drum Machine</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>0.1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSMainNibFile</key>
    <string></string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

echo "App bundle created: $APP_BUNDLE"
echo ""
echo "To test locally:"
echo "  open $APP_BUNDLE"
echo ""
echo "To create DMG for distribution:"
echo "  hdiutil create -volname 'Drum Machine' -srcfolder . -ov -format UDZO DrumMachine-0.1.0-macOS.dmg"
