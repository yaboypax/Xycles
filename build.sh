#!/usr/bin/env bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
# Cap parallel jobs: JUCE + MSVC LTO can use ~1-2 GB per cl.exe, and the
# default of one-job-per-core OOMs/freezes machines with many cores.
cmake --build . --config Release -- -j "${JOBS:-6}"

OS=$(uname)
case "$OS" in
'Darwin')
  open Xycles_artefacts/Release/Xycles.app
  ;;
'CYGWIN'* | 'MSYS'* | 'MINGW'*)
  Xycles_artefacts/Release/Xycles.exe
  ;;
*)
  Xycles_artefacts/Release/Xycles
  ;;
esac
