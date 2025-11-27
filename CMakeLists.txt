cmake_minimum_required(VERSION 3.10)
project(BlackHole)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(GLEW REQUIRED glew)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

file(GLOB SRC_FILES "src/main.cpp")

add_executable(blackhole ${SRC_FILES})

target_link_libraries(blackhole
    glad
    glfw
    OpenGL::GL
    ${GLEW_LIBRARIES}
)

target_include_directories(blackhole PUBLIC ${GLEW_INCLUDE_DIRS})
