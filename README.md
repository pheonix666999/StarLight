# Starlight Drift (v0.1)

Granular delay → shimmer reverb spatial effect plugin (VST3/AU/Standalone) built with JUCE + CMake.

## Build

Prereqs:
- CMake 3.22+
- A C++17 compiler toolchain (MSVC / clang / gcc)
- A build tool (Ninja or Make)

```bash
cmake -S . -B build
cmake --build build --config Release
```

Artifacts land under `build/StarlightDrift_artefacts/`.

Notes:
- If your environment doesn't have CMake, you can install it with `python -m pip install --user cmake`.
- If CMake can't find a generator, install Ninja and configure with `-G Ninja`.
- Convenience script: `scripts/build.sh`

## One‑Command Build (Recommended)

### macOS (Xcode)
```bash
./scripts/build_mac.sh
```
Open the generated Xcode project:
```bash
./scripts/build_mac.sh --open
```

### Windows (PowerShell)
```powershell
.\scripts\build.ps1
```

## CMake Presets (macOS + Windows)
This repo includes `CMakePresets.json` so you can build with one command:

```bash
cmake --preset mac-xcode
cmake --build --preset mac-xcode
```

```powershell
cmake --preset win-vs
cmake --build --preset win-vs
```

## macOS (Xcode)

There is no `.jucer` file because this project is **CMake-based** (modern JUCE workflow).
You can generate an Xcode project directly from CMake:

```bash
cmake -S . -B build-xcode -G Xcode
cmake --build build-xcode --config Release
```

Or open the generated Xcode project:

```bash
open build-xcode/StarlightDrift.xcodeproj
```

Artifacts land under `build-xcode/StarlightDrift_artefacts/Release/`.

### Install the plugin (user)
```bash
./scripts/install_mac.sh
```

### macOS Gatekeeper / Quarantine
If you download a prebuilt `.component` or `.vst3`, macOS may block it. Remove quarantine:

```bash
sudo xattr -dr com.apple.quarantine "Starlight Drift.component"
sudo xattr -dr com.apple.quarantine "Starlight Drift.vst3"
```

### AU Validation (optional)

```bash
auval -v aufx SDrf Stlt
```
