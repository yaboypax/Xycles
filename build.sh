#!/usr/bin/env bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

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
