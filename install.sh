#!/bin/bash

# Configuration
REPO="Guillem96/pokestroke"
INSTALL_DIR="$HOME/.pokestroke"
BIN_NAME="pokestroke" # Ensure this matches your actual binary name

echo "🚀 Installing Pokestroke..."

# 1. Detect Platform
OS_TYPE=$(uname -s)
if [ "$OS_TYPE" == "Darwin" ]; then
    PACKAGE="pokestroke-macos.zip"
    IS_MAC=true
elif [[ "$OS_TYPE" == "MINGW"* || "$OS_TYPE" == "MSYS"* || "$OS_TYPE" == "Linux" ]]; then
    PACKAGE="pokestroke-windows.zip" # Assuming Linux users use the windows-dist or similar logic
    IS_MAC=false
else
    echo "❌ Unsupported OS."
    exit 1
fi

# 2. Get Release URLs
echo "🔍 Fetching latest release..."
RELEASE_DATA=$(curl -s https://api.github.com/repos/$REPO/releases/latest)
RELEASE_URL=$(echo "$RELEASE_DATA" | grep "browser_download_url.*$PACKAGE" | cut -d '"' -f 4)
RES_URL=$(echo "$RELEASE_DATA" | grep "browser_download_url.*resources.zip" | cut -d '"' -f 4)

# 3. Setup Directory
mkdir -p "$INSTALL_DIR"
cd /tmp

# 4. Download and Extract
echo "📦 Downloading assets..."
curl -L -o resources.zip "$RES_URL"
unzip -o resources.zip -d "$INSTALL_DIR"

echo "👾 Downloading binary..."
curl -L -o app.zip "$RELEASE_URL"
unzip -o app.zip -d "$INSTALL_DIR"
chmod +x "$INSTALL_DIR/$BIN_NAME"

# --- NEW SECTION: INTERACTIVE SETUP ---

echo ""
read -p "❓ Do you want to create a desktop shortcut? (y/n): " confirm_shortcut
if [[ $confirm_shortcut == [yY] || $confirm_shortcut == [yY][eE][sS] ]]; then
    if [ "$IS_MAC" = true ]; then
        # Create a macOS .app wrapper or a simple alias
        ln -s "$INSTALL_DIR/$BIN_NAME" "$HOME/Desktop/Pokestroke"
        echo "✅ Alias created on Desktop."
    else
        # Create a Linux .desktop file
        DESKTOP_FILE="$HOME/Desktop/pokestroke.desktop"
        echo "[Desktop Entry]
Type=Application
Name=Pokestroke
Exec=$INSTALL_DIR/$BIN_NAME
Path=$INSTALL_DIR
Icon=$INSTALL_DIR/resources/icon.png
Terminal=false" > "$DESKTOP_FILE"
        chmod +x "$DESKTOP_FILE"
        echo "✅ Desktop shortcut created."
    fi
fi

echo ""
read -p "❓ Do you want Pokestroke to run automatically on session start? (y/n): " confirm_auto
if [[ $confirm_auto == [yY] || $confirm_auto == [yY][eE][sS] ]]; then
    if [ "$IS_MAC" = true ]; then
        # macOS LaunchAgent
        PLIST="$HOME/Library/LaunchAgents/com.guillem96.pokestroke.plist"
        echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
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
</plist>" > "$PLIST"
        launchctl load "$PLIST"
        echo "✅ LaunchAgent created and loaded."
    else
        # Linux Autostart
        AUTOSTART_DIR="$HOME/.config/autostart"
        mkdir -p "$AUTOSTART_DIR"
        cp "$HOME/Desktop/pokestroke.desktop" "$AUTOSTART_DIR/" 2>/dev/null || \
        echo "[Desktop Entry]
Type=Application
Name=Pokestroke
Exec=$INSTALL_DIR/$BIN_NAME
Path=$INSTALL_DIR" > "$AUTOSTART_DIR/pokestroke.desktop"
        echo "✅ Added to autostart."
    fi
fi

echo ""
echo "🎉 Installation complete! You can find Pokestroke in $INSTALL_DIR"