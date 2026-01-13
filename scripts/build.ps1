# Starlight Drift Build Script (Windows)
# Run from project root: .\scripts\build.ps1

$ErrorActionPreference = "Stop"

$RootDir = Split-Path -Parent (Split-Path -Parent $PSCommandPath)
$BuildDir = Join-Path $RootDir "build"

# Check for CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "CMake not found. Install via: winget install Kitware.CMake"
    exit 1
}

# Create build directory if needed
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

Write-Host "Configuring CMake..." -ForegroundColor Cyan
cmake -S $RootDir -B $BuildDir -DCMAKE_BUILD_TYPE=Release

Write-Host "Building..." -ForegroundColor Cyan
cmake --build $BuildDir --config Release --parallel

$ArtifactDir = Join-Path $BuildDir "StarlightDrift_artefacts"
Write-Host ""
Write-Host "Build complete!" -ForegroundColor Green
Write-Host "Artifacts: $ArtifactDir" -ForegroundColor Yellow

# List output files
if (Test-Path $ArtifactDir) {
    Get-ChildItem -Path $ArtifactDir -Recurse -File | ForEach-Object {
        Write-Host "  $($_.FullName.Replace($BuildDir, ''))" -ForegroundColor White
    }
}
