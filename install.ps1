# install.ps1
$ErrorActionPreference = "Stop"

Write-Host "Installing Craft..."

# Check dependencies
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "Error: git is required. Install from https://git-scm.com"
    exit 1
}
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "Error: cmake is required. Install from https://cmake.org"
    exit 1
}

# Detect build system and generator
$Generator = $null
$BuildTool = $null

if (Get-Command cl -ErrorAction SilentlyContinue) {
    # MSVC detected
    $Generator = "Visual Studio 17 2022"
    $BuildTool = "msvc"
    Write-Host "Detected MSVC compiler"
} elseif (Get-Command mingw32-make -ErrorAction SilentlyContinue) {
    $Generator = "MinGW Makefiles"
    $BuildTool = "mingw"
    Write-Host "Detected MinGW compiler"
} elseif (Get-Command make -ErrorAction SilentlyContinue) {
    $Generator = "MinGW Makefiles"
    $BuildTool = "mingw"
    Write-Host "Detected make"
} elseif (Get-Command ninja -ErrorAction SilentlyContinue) {
    $Generator = "Ninja"
    $BuildTool = "ninja"
    Write-Host "Detected Ninja build system"
} else {
    Write-Error "Error: No supported build system found. Install one of:
    - MSVC: Visual Studio with C++ workload (https://visualstudio.microsoft.com)
    - MinGW/MSYS2: https://www.msys2.org
    - Ninja: https://ninja-build.org"
    exit 1
}

# Clone and build
$TempDir = "$env:TEMP\craft-install"
if (Test-Path $TempDir) {
    Remove-Item $TempDir -Recurse -Force
}

git clone --depth 1 https://github.com/randerson112/craft.git $TempDir
Set-Location $TempDir

if ($BuildTool -eq "msvc") {
    # MSVC uses multi-config generators so specify config at build time
    cmake -S . -B build -G $Generator -A x64
    cmake --build build --config Release
    $BinaryPath = "build\Release\craft.exe"
} else {
    cmake -S . -B build -G $Generator
    cmake --build build
    $BinaryPath = "build\craft.exe"
}

# Install
$CraftBin = "$env:USERPROFILE\.craft\bin"
New-Item -ItemType Directory -Force -Path $CraftBin | Out-Null
Copy-Item $BinaryPath "$CraftBin\craft.exe"

# Add to PATH permanently for current user
$CurrentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
if ($CurrentPath -notlike "*\.craft\bin*") {
    [Environment]::SetEnvironmentVariable(
        "PATH",
        "$CraftBin;$CurrentPath",
        "User"
    )
    Write-Host "Added $CraftBin to PATH"
}

# Cleanup
Set-Location $env:USERPROFILE
Remove-Item $TempDir -Recurse -Force

Write-Host "Craft installed successfully!"
Write-Host "Restart your terminal to use craft"