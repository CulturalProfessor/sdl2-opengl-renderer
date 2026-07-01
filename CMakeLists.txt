cmake_minimum_required(VERSION 3.10)
project(SDLApp CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# Locate SDL2. Prefer the CMake config (Homebrew/macOS, vcpkg), and fall back to
# pkg-config (the common path on Linux distros via libsdl2-dev).
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

## Remember to include source files here
add_executable(main main.cpp src/glad.c src/Camera.cpp src/stb_image.cpp)

# The project mixes <SDL2/SDL.h> and <SDL_events.h> style includes, so expose
# both the parent dir and the SDL2 subdir on the include path.
target_include_directories(main PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${SDL2_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
)

target_link_directories(main PRIVATE ${SDL2_LIBRARY_DIRS})
target_link_libraries(main ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY})
