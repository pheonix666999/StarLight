#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

if ! command -v cmake >/dev/null 2>&1; then
  echo "cmake not found."
  echo "Install: python3 -m pip install --user cmake"
  exit 1
fi

GENERATOR_ARGS=()
if command -v ninja >/dev/null 2>&1; then
  GENERATOR_ARGS=(-G Ninja)
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release "${GENERATOR_ARGS[@]}"
cmake --build "${BUILD_DIR}" --config Release

echo "Built. See: ${BUILD_DIR}/StarlightDrift_artefacts/"

