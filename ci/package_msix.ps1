# Setup Paths
$BuildDir = "bin/Release"
$LayoutDir = "msix_layout"
$PackageName = "Pokestroke.msix"

# 1. Locate the Windows SDK bin folder
$sdkRoot = "${env:ProgramFiles(x86)}\Windows Kits\10\bin"
Write-Host "Searching for SDK tools in $sdkRoot..."

# 2. Find the tools by searching for the filename within an 'x64' subfolder
$MakeAppx = (Get-ChildItem -Path $sdkRoot -Filter "makeappx.exe" -Recurse | 
             Where-Object { $_.FullName -like "*\x64\*" } | 
             Select-Object -First 1).FullName

$SignTool = (Get-ChildItem -Path $sdkRoot -Filter "signtool.exe" -Recurse | 
             Where-Object { $_.FullName -like "*\x64\*" } | 
             Select-Object -First 1).FullName

# 3. Safety Check: If not found, fail early with a clear message
if (-not $MakeAppx) { Write-Error "Could not find makeappx.exe in $sdkRoot"; exit 1 }
if (-not $SignTool) { Write-Error "Could not find signtool.exe in $sdkRoot"; exit 1 }

Write-Host "✅ Found MakeAppx: $MakeAppx"
Write-Host "✅ Found SignTool: $SignTool"

# 4. Prepare Layout Folder
if (Test-Path $LayoutDir) { Remove-Item -Recurse -Force $LayoutDir }
New-Item -ItemType Directory -Path "$LayoutDir/Assets"
New-Item -ItemType Directory -Path "$LayoutDir/resources"

# 5. Copy Binary and Resources
Copy-Item "$BuildDir/pokestroke.exe" -Destination "$LayoutDir/"
Copy-Item -Recurse "resources/*" -Destination "$LayoutDir/resources/"
Copy-Item "resources/AppxManifest.xml" -Destination "$LayoutDir/"
Copy-Item "resources/icon*.png" -Destination "$LayoutDir/Assets/"
Copy-Item "resources/wide.png" -Destination "$LayoutDir/Assets/"

# 6. Create the MSIX Package
& $MakeAppx pack /d $LayoutDir /p $PackageName /o

# 7. Create a Self-Signed Certificate (Required for MSIX)
$Cert = New-SelfSignedCertificate -Type Custom -Subject "CN=Guillem96" `
        -KeyUsage DigitalSignature -FriendlyName "Pokestroke Dev Cert" `
        -CertStoreLocation "Cert:\CurrentUser\My" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3")

# 8. Sign the Package
& $SignTool sign /fd SHA256 /a /sha1 $Cert.Thumbprint $PackageName

Write-Host "✅ MSIX Package Created and Signed: $PackageName"
