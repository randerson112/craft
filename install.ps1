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
if (-not (Get-Command mingw32-make -ErrorAction SilentlyContinue) -and
    -not (Get-Command make -ErrorAction SilentlyContinue)) {
    Write-Error "Error: make is required. Install MinGW or MSYS2 from https://www.msys2.org"
    exit 1
}

# Clone and build
$TempDir = "$env:TEMP\craft-install"
if (Test-Path $TempDir) {
    Remove-Item $TempDir -Recurse -Force
}

git clone --depth 1 https://github.com/randerson112/craft.git $TempDir
Set-Location $TempDir
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build

# Install
$CraftBin = "$env:USERPROFILE\.craft\bin"
New-Item -ItemType Directory -Force -Path $CraftBin | Out-Null
Copy-Item "build\craft.exe" "$CraftBin\craft.exe"

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