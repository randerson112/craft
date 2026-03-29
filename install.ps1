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
    # fallback: search entire build dir
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

New-Item -ItemType Directory -Force -Path $CraftBin | Out-Null

Copy-Item $exePath "$CraftBin\craft.exe" -Force

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
Write-Host "Craft installed successfully!"
Write-Host "Restart your terminal and run: craft"