# Setup Paths
$BuildDir = "bin/Release"
$LayoutDir = "msix_layout"
$PackageName = "Pokestroke.msix"

# 1. Find the Windows SDK installation path
$sdkRoot = "${env:ProgramFiles(x86)}\Windows Kits\10\bin"
# Get the latest version folder (e.g., 10.0.22621.0)
$latestSdk = Get-ChildItem -Path $sdkRoot -Directory | Sort-Object Name -Descending | Select-Object -First 1

if ($null -eq $latestSdk) {
    throw "Windows SDK not found in $sdkRoot"
}

# 2. Specifically target the x64 binaries
$toolPath = Join-Path $latestSdk.FullName "x64"
$MakeAppx = Join-Path $toolPath "makeappx.exe"
$SignTool = Join-Path $toolPath "signtool.exe"

# 2. Prepare Layout Folder
if (Test-Path $LayoutDir) { Remove-Item -Recurse -Force $LayoutDir }
New-Item -ItemType Directory -Path "$LayoutDir/Assets"
New-Item -ItemType Directory -Path "$LayoutDir/resources"

# 3. Copy Binary and Resources
Copy-Item "$BuildDir/pokestroke.exe" -Destination "$LayoutDir/"
Copy-Item -Recurse "resources/*" -Destination "$LayoutDir/resources/"
Copy-Item "resources/AppxManifest.xml" -Destination "$LayoutDir/"
Copy-Item "resources/icon*.png" -Destination "$LayoutDir/Assets/"
Copy-Item "resources/wide.png" -Destination "$LayoutDir/Assets/"

# 4. Create the MSIX Package
# MakeAppx is usually in the Windows SDK path
& $MakeAppx pack /d $LayoutDir /p $PackageName /o

# 5. Create a Self-Signed Certificate (Required for MSIX)
$Cert = New-SelfSignedCertificate -Type Custom -Subject "CN=Development" `
        -KeyUsage DigitalSignature -FriendlyName "Pokestroke Dev Cert" `
        -CertStoreLocation "Cert:\CurrentUser\My" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3")

# 5. Sign the Package
$SignTool = Get-ChildItem -Path "C:\Program Files (x86)\Windows Kits\10\bin" -Filter "signtool.exe" -Recurse | Select-Object -First 1
& $SignTool sign /fd SHA256 /a /sha1 $Cert.Thumbprint $PackageName

Write-Host "✅ MSIX Package Created and Signed: $PackageName"
