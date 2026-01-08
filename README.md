# Xycles

Xycles is a standalone application asynchronous loop station and texture generation suite. It has a custom audio engine built in Rust and uses JUCE/C++ for its GUI. Inspiration and build templates for this came from Stephan Eckes' [rust-audio-plugin](https://github.com/steckes/rust-audio-plugin) repository.

## Experiments in FSM and Fat Enums

Xycles began as an exploration into Rust, particularly in building a Finite State Machine using Rust's fat enums. The FSM has been extremely useful in preventing OOP-related state management errors which can come up building something similar in JUCE.

## Build
>[!WARNING]
>This project is still in the works and is not recommended to be built from source

Clone this repository and run the `./build.sh bash` script. This will build using `cmake --build` and run the release build for you.

To generate platform specific project files without building run:

```
mkdir build

cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
```

## Dependencies
- [cmake](https://cmake.org/)
- [Rust + cargo](https://github.com/rust-lang/cargo)  
- [JUCE](https://juce.com/) (fetched by cmake unless configured to point to specific JUCE version on system in `CMakeLists.txt`)
