#!/usr/bin/env bash
set -euo pipefail

if [[ "$(uname)" != "Darwin" ]]; then
  echo "This script is for macOS."
  exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-xcode"

if ! command -v cmake >/dev/null 2>&1; then
  echo "cmake not found."
  echo "Install: brew install cmake"
  exit 1
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G Xcode
cmake --build "${BUILD_DIR}" --config Release

echo "Built. See: ${BUILD_DIR}/StarlightDrift_artefacts/Release/"

if [[ "${1:-}" == "--open" ]]; then
  open "${BUILD_DIR}/StarlightDrift.xcodeproj"
fi
