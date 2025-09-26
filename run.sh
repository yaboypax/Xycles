#!/usr/bin/env bash

cmake --build cmake-build-release --clean-first -j6

OS=$(uname)
case "$OS" in
   'Darwin') 
	open  cmake-build-release/Xycles_artefacts/Release/Xycles.app  
	;;
   'CYGWIN'*|'MSYS'*|'MINGW'*)
    	cmake-build-release/Xycles_artefacts/Release/Xycles.exe
	;;
  *)
    echo "Error Detecting OS: $OS"
    	;;
esac

