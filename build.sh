#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

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

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
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
