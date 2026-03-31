# Setup Paths
$BuildDir = "bin/Release"
$LayoutDir = "msix_layout"
$PackageName = "Pokestroke.msix"

# 1. Prepare Layout Folder
if (Test-Path $LayoutDir) { Remove-Item -Recurse -Force $LayoutDir }
New-Item -ItemType Directory -Path "$LayoutDir/Assets"

# 2. Copy Binary and Resources
Copy-Item "$BuildDir/pokestroke.exe" -Destination "$LayoutDir/"
Copy-Item -Recurse "resources/*" -Destination "$LayoutDir/resources/"
Copy-Item "AppxManifest.xml" -Destination "$LayoutDir/"
Copy-Item "resources/icons/icon*.png" -Destination "$LayoutDir/Assets/"
Copy-Item "resources/icons/wide.png" -Destination "$LayoutDir/Assets/"

# 3. Create the MSIX Package
# MakeAppx is usually in the Windows SDK path
$MakeAppx = Get-ChildItem -Path "C:\Program Files (x86)\Windows Kits\10\bin" -Filter "makeappx.exe" -Recurse | Select-Object -First 1
& $MakeAppx.FullName pack /d $LayoutDir /p $PackageName /o

# 4. Create a Self-Signed Certificate (Required for MSIX)
$Cert = New-SelfSignedCertificate -Type Custom -Subject "CN=Development" `
        -KeyUsage DigitalSignature -FriendlyName "Pokestroke Dev Cert" `
        -CertStoreLocation "Cert:\CurrentUser\My" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3")

# 5. Sign the Package
$SignTool = Get-ChildItem -Path "C:\Program Files (x86)\Windows Kits\10\bin" -Filter "signtool.exe" -Recurse | Select-Object -First 1
& $SignTool.FullName sign /fd SHA256 /a /sha1 $Cert.Thumbprint $PackageName

Write-Host "✅ MSIX Package Created and Signed: $PackageName"
