#!/bin/bash
set -e

APP_NAME="Pokestroke"
BIN_NAME="pokestroke"
APP_BUNDLE="$APP_NAME.app"

# 1. Create Folder Structure
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources"

# 2. Copy Binary
cp "bin/Release/$BIN_NAME" "$APP_BUNDLE/Contents/MacOS/$BIN_NAME"
chmod +x "$APP_BUNDLE/Contents/MacOS/$BIN_NAME"

# 3. Copy Resources
# Note: Copying the folder content, not the folder itself, into Resources
cp -r resources/* "$APP_BUNDLE/Contents/Resources/"

# 4. Copy Metadata
if [ -f "resources/macos.plist.xml" ]; then
    cp "resources/macos.plist.xml" "$APP_BUNDLE/Contents/Info.plist"
else
    echo "Warning: Info.plist not found, macOS might not launch the app correctly."
fi

# 5. Handle Icon if it exists
if [ -f "resources/icon.icns" ]; then
    cp "resources/icon.icns" "$APP_BUNDLE/Contents/Resources/icon.icns"
fi

echo "✅ macOS App Bundle created at $APP_BUNDLE"
