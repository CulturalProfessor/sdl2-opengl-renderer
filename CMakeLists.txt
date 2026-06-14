cmake_minimum_required(VERSION 3.10)
project(SDLApp)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

## Remember to include source files here
add_executable(main main.cpp src/glad.c src/Camera.cpp)

target_link_libraries(main ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY})

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)