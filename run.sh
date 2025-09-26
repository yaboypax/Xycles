#!/usr/bin/env bash

cmake --build cmake-build-release --clean-first -j6 --target Xycles_Standalone
open /Users/raf-dev/workspace/repos/Xycles/cmake-build-release/Xycles_artefacts/Release/Standalone/Xycles.app
