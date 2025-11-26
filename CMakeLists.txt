cmake_minimum_required(VERSION 3.10)
project(BlackHole)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(blackhole 
    src/main.cpp
    src/Shader.cpp
    src/FullscreenQuad.cpp
    src/Renderer.cpp
)

target_link_libraries(blackhole
    glad
    glfw
    OpenGL::GL
)
