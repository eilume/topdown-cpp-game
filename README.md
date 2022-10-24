# Topdown C++ Game

A fun little game where you defend yourself from an ever growing horde of enemies.

<p align="center">
    <img src="docs/demo.gif">
</p>

The intention of this project was to try writing a small game engine for a topdown game as a learning exercise.

> **Note:** The physics system is pretty much a direct port from [Falconerd's awesome C Game + Engine Series](https://youtube.com/playlist?list=PLYokS5qr7lSsvgemrTwMSrQsdk4BRqJU6), as it wasn't the main focus of this project!

## Controls

- **WASD** - Player Movement
- **Arrow Keys** - Weapon Fire Direction

## Compiling

### Requirements

- C++ Compiler (Tested with LLVM's [Clang](https://clang.llvm.org/) v13.1.6 using `-std=c++17`)
- [CMake](https://cmake.org/) - Cross-platform build environment setup
- [SDL](https://github.com/libsdl-org/SDL) - Cross-platform library for windowing, graphics, input, audio

### Building

*Note*: Only a [Makefile](/Makefile) is provided, so Windows users will have to convert some commands from the makefile to Win32 alternatives!

1. `git clone https://github.com/eilume/topdown-cpp-game && cd topdown-cpp-game` - Clone project and enter root directory
2. `make [build|debug]` - Compile project
3. `make run`- Run project