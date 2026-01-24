#!/usr/bin/env bash
set -euo pipefail

if [[ "$(uname)" != "Darwin" ]]; then
  echo "This script is for macOS."
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-xcode"
ARTEFACTS_DIR="${BUILD_DIR}/StarlightDrift_artefacts/Release"

if [[ ! -d "${ARTEFACTS_DIR}" ]]; then
  echo "Build artifacts not found at: ${ARTEFACTS_DIR}"
  echo "Run: scripts/build_mac.sh"
  exit 1
fi

mkdir -p "${HOME}/Library/Audio/Plug-Ins/Components"
mkdir -p "${HOME}/Library/Audio/Plug-Ins/VST3"

if [[ -d "${ARTEFACTS_DIR}/Starlight Drift.component" ]]; then
  cp -R "${ARTEFACTS_DIR}/Starlight Drift.component" "${HOME}/Library/Audio/Plug-Ins/Components/"
  echo "Installed AU to ~/Library/Audio/Plug-Ins/Components/"
fi

if [[ -d "${ARTEFACTS_DIR}/Starlight Drift.vst3" ]]; then
  cp -R "${ARTEFACTS_DIR}/Starlight Drift.vst3" "${HOME}/Library/Audio/Plug-Ins/VST3/"
  echo "Installed VST3 to ~/Library/Audio/Plug-Ins/VST3/"
fi

echo "Done."
