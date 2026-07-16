#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

LTO=ON

for arg in "$@"; do
  case "$arg" in
  --fast)
    LTO=OFF
    ;;
  -h | --help)
    echo "usage: $(basename "$0") [--fast]"
    echo "  --fast  skip link-time optimisation. much faster to build, for development."
    echo "          switching between --fast and a normal build rebuilds from scratch."
    exit 0
    ;;
  *)
    echo "error: unknown option '$arg'. try --help." >&2
    exit 1
    ;;
  esac
done

for tool in cmake cargo git; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "error: $tool not found on PATH. see the dependencies section of the README." >&2
    exit 1
  fi
done

if command -v nproc >/dev/null 2>&1; then
  JOBS=$(nproc)
elif command -v sysctl >/dev/null 2>&1; then
  JOBS=$(sysctl -n hw.ncpu)
else
  JOBS=4
fi

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DXYCLES_LTO="$LTO"
cmake --build build --config Release --parallel "$JOBS"

ARTEFACTS="build/Xycles_artefacts/Release"

case "$(uname -s)" in
'Darwin')
  open "$ARTEFACTS/Xycles.app"
  ;;
'CYGWIN'* | 'MSYS'* | 'MINGW'*)
  "$ARTEFACTS/Xycles.exe"
  ;;
*)
  "$ARTEFACTS/Xycles"
  ;;
esac
