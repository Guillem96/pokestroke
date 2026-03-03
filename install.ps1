# Configuration
$Repo = "YOUR_GITHUB_USERNAME/YOUR_REPO_NAME"
$InstallDir = Join-Path $HOME ".pokestroke"
$TempDir = Join-Path $env:TEMP "pokestroke_install"

# 1. Create Directories
if (!(Test-Path $InstallDir)) { New-Item -ItemType Directory -Path $InstallDir }
if (Test-Path $TempDir) { Remove-Item -Recurse -Force $TempDir }
New-Item -ItemType Directory -Path $TempDir

Write-Host "Fetching latest release info..." -ForegroundColor Cyan

# 2. Get Release Metadata
$Release = Invoke-RestMethod -Uri "https://api.github.com/repos/$Repo/releases/latest"
$WinAsset = $Release.assets | Where-Object { $_.name -like "*windows.zip" }
$ResAsset = $Release.assets | Where-Object { $_.name -like "resources.zip" }

if (!$WinAsset -or !$ResAsset) {
    Write-Error "Could not find the required release assets on GitHub."
    exit
}

# 3. Download Assets
Write-Host "Downloading Pokestroke and Resources..." -ForegroundColor Cyan
Invoke-WebRequest -Uri $WinAsset.browser_download_url -OutFile "$TempDir\app.zip"
Invoke-WebRequest -Uri $ResAsset.browser_download_url -OutFile "$TempDir\resources.zip"

# 4. Extract
Write-Host "Extracting files to $InstallDir..." -ForegroundColor Cyan
Expand-Archive -Path "$TempDir\resources.zip" -DestinationPath "$InstallDir" -Force
Expand-Archive -Path "$TempDir\app.zip" -DestinationPath "$TempDir\bin_extract" -Force

# 5. Move Binary (Assumes the zip contains a folder or files)
Move-Item -Path "$TempDir\bin_extract\*" -Destination "$InstallDir" -Force

# Clean up
Remove-Item -Recurse -Force $TempDir

Write-Host "Installation Complete! Files are located in $InstallDir" -ForegroundColor Green
