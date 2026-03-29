$ErrorActionPreference = "Stop"
Write-Host "Installing Craft..."

# Check dependencies
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "git is required: https://git-scm.com"
    exit 1
}
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "cmake is required: https://cmake.org"
    exit 1
}

# Temp directory
$TempDir = "$env:USERPROFILE\craft-install"
if (Test-Path $TempDir) {
    Remove-Item $TempDir -Recurse -Force
}

# Clone repo
git clone --depth 1 https://github.com/randerson112/craft.git $TempDir
Set-Location $TempDir

# Build (let CMake choose toolchain)
cmake -S . -B build
cmake --build build

# Locate executable (primary + fallback)
$exePath = "build\craft.exe"
if (-not (Test-Path $exePath)) {
    $exe = Get-ChildItem -Path build -Recurse -Filter craft.exe | Select-Object -First 1
    if (-not $exe) {
        Write-Error "Build failed: craft.exe not found"
        exit 1
    }
    $exePath = $exe.FullName
}

# Install location
$CraftHome = "$env:USERPROFILE\.craft"
$CraftBin = "$CraftHome\bin"
$CraftExe = "$CraftBin\craft.exe"
New-Item -ItemType Directory -Force -Path $CraftBin | Out-Null

# Copy binary — use delayed replacement if upgrading, direct copy if fresh install
if (Test-Path $CraftExe) {
    # Upgrade — craft.exe is currently running so we can't overwrite it directly
    $TempExe = "$env:TEMP\craft_new.exe"
    Copy-Item $exePath $TempExe -Force

    $BatchScript = "$env:TEMP\craft_update.bat"
@"
@echo off
timeout /t 2 /nobreak > NUL
copy /y "$TempExe" "$CraftExe"
del "$TempExe"
del "%~f0"
"@ | Out-File -FilePath $BatchScript -Encoding ASCII

    Start-Process -FilePath $BatchScript -WindowStyle Hidden
} else {
    # Fresh install — no running process, copy directly
    Copy-Item $exePath $CraftExe -Force
}

# Add to PATH (user)
$CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
if ($CurrentPath -notmatch [regex]::Escape($CraftBin)) {
    [Environment]::SetEnvironmentVariable(
        "PATH",
        "$CraftBin;$CurrentPath",
        "User"
    )
    Write-Host "Added Craft to PATH"
}

# Cleanup
Set-Location $env:USERPROFILE
Remove-Item $TempDir -Recurse -Force

Write-Host ""
if (Test-Path "$env:TEMP\craft_update.bat") {
    Write-Host "Craft updated successfully!"
    Write-Host "The update will complete in a few seconds."
} else {
    Write-Host "Craft installed successfully!"
}
Write-Host "Restart your terminal and run: craft"