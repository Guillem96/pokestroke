#!/bin/bash

# Configuration
REPO="Guillem96/pokestroke"
INSTALL_DIR="$HOME/.pokestroke"
BIN_NAME="pokestroke" 
BIN_PATH="$INSTALL_DIR/$BIN_NAME"

echo "🚀 Installing Pokestroke..."

# 1. Detect Platform
OS_TYPE=$(uname -s)
if [ "$OS_TYPE" == "Darwin" ]; then
    PACKAGE="pokestroke-macos.zip"
    IS_MAC=true
elif [[ "$OS_TYPE" == "Linux" ]]; then
    PACKAGE="pokestroke-linux.zip"
    IS_MAC=false
else
    echo "❌ Unsupported OS: $OS_TYPE"
    exit 1
fi

# 2. Get Release URLs
echo "🔍 Fetching latest release..."
RELEASE_DATA=$(curl -s https://api.github.com/repos/$REPO/releases/latest)
RELEASE_URL=$(echo "$RELEASE_DATA" | grep "browser_download_url.*$PACKAGE" | cut -d '"' -f 4)
RES_URL=$(echo "$RELEASE_DATA" | grep "browser_download_url.*resources.zip" | cut -d '"' -f 4)

# 3. Setup Directory
mkdir -p "$INSTALL_DIR"
cd /tmp || exit

# 4. Download and Extract
echo "📦 Downloading assets..."
curl -L -o resources.zip "$RES_URL"
unzip -o resources.zip -d "$INSTALL_DIR"

echo "👾 Downloading binary..."
curl -L -o app.zip "$RELEASE_URL"
unzip -o app.zip -d "$INSTALL_DIR"
chmod +x "$INSTALL_DIR/$BIN_NAME"

# --- NEW SECTION: INTERACTIVE SETUP ---

# We use < /dev/tty to force bash to listen to the keyboard even if piped
echo ""
read -p "❓ Do you want to create a desktop shortcut? (y/n): " confirm_shortcut < /dev/tty

if [[ $confirm_shortcut =~ ^[yY](es)?$ ]]; then
    if [ "$IS_MAC" = true ]; then
        SHORTCUT_PATH="$HOME/Desktop/Pokestroke.app"
        # Create a small AppleScript app that runs the binary silently
        mkdir -p "$SHORTCUT_PATH/Contents/MacOS"
        mkdir -p "$SHORTCUT_PATH/Contents/Resources"

        cp "$INSTALL_DIR/resources/icon.icns" "$SHORTCUT_PATH/Contents/Resources/icon.icns"
        cp "$BIN_PATH" "$SHORTCUT_PATH/Contents/MacOS/pokestroke"
        cp "$INSTALL_DIR/resources/macos.plist.xml" "$SHORTCUT_PATH/Contents/Info.plist"
        echo "✅ macOS App Shortcut created on Desktop (No Terminal)."
        echo "✅ macOS App Shortcut created on Desktop (No Terminal)."
    else
        DESKTOP_FILE="$HOME/Desktop/pokestroke.desktop"
        cat <<EOF > "$DESKTOP_FILE"
[Desktop Entry]
Type=Application
Name=Pokestroke
Exec=$INSTALL_DIR/$BIN_NAME
Path=$INSTALL_DIR
Icon=$INSTALL_DIR/resources/icon.png
Terminal=false
EOF
        chmod +x "$DESKTOP_FILE"
        echo "✅ Desktop shortcut created."
    fi
fi

echo ""
read -p "❓ Do you want Pokestroke to run automatically on session start? (y/n): " confirm_auto < /dev/tty

if [[ $confirm_auto =~ ^[yY](es)?$ ]]; then
    if [ "$IS_MAC" = true ]; then
        PLIST="$HOME/Library/LaunchAgents/com.guillem96.pokestroke.plist"
        cat <<EOF > "$PLIST"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.guillem96.pokestroke</string>
    <key>ProgramArguments</key>
    <array>
        <string>$INSTALL_DIR/$BIN_NAME</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
</dict>
</plist>
EOF
        launchctl load "$PLIST"
        echo "✅ LaunchAgent created and loaded."
    else
        AUTOSTART_DIR="$HOME/.config/autostart"
        mkdir -p "$AUTOSTART_DIR"
        cat <<EOF > "$AUTOSTART_DIR/pokestroke.desktop"
[Desktop Entry]
Type=Application
Name=Pokestroke
Exec=$INSTALL_DIR/$BIN_NAME
Path=$INSTALL_DIR
EOF
        echo "✅ Added to autostart."
    fi
fi

echo ""
echo "🎉 Installation complete! You can find Pokestroke in $INSTALL_DIR"