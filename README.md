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
