#!/usr/bin/env bash

cmake --build cmake-build-release --clean-first -j6
open /Users/raf-dev/workspace/repos/Xycles/cmake-build-release/Xycles_artefacts/Release/Xycles.app
