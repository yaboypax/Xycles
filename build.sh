#!/usr/bin/env bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -- -j 6

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
