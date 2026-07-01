# sdl2-opengl-renderer

A small real-time renderer built with SDL2 and modern OpenGL (4.1 core profile),
written in C++17. A learning/sandbox project for experimenting with graphics
pipeline concepts: shaders, textures, depth testing, free-look camera etc.

## Dependencies

- A C++17 compiler
- CMake >= 3.10
- SDL2 (`libsdl2-dev` on Debian/Ubuntu, `sdl2` via Homebrew/vcpkg)
- OpenGL

GLM, glad, KHR, and stb_image are vendored under `include/`, so no extra setup
is needed for those.

## Build & run

```sh
make run      # configure, build, and run
make build    # configure and build only
make clean    # remove the build directory
```

These wrap CMake; you can also drive it directly:

```sh
cmake -S . -B build
cmake --build build -j
cd build && ./main
```

Note: the binary loads shaders relative to its own location (`../shaders/...`),
so run it from inside `build/` as the targets above do.

## Controls

- Arrow keys — move forward/back and strafe left/right
- Space / Left Shift — move up / down
- Mouse — look around
- Close the window to quit

## Layout

```
main.cpp        entry point, window/context setup, render loop
src/            Camera, glad, and stb_image implementations
include/        headers and vendored libraries (glm, glad, KHR, stb_image)
shaders/        GLSL vertex and fragment shaders
assets/         textures
CMakeLists.txt  build configuration
Makefile        thin wrapper over CMake
```
