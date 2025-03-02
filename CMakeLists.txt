cmake_minimum_required(VERSION 3.10)

project(Agar VERSION 0.1)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(${PROJECT_SOURCE_DIR}/Agar-libs/enet)
add_subdirectory(${PROJECT_SOURCE_DIR}/Agar-libs/glm-1.0.1)
add_subdirectory(${PROJECT_SOURCE_DIR}/Agar-libs/glfw-3.4)

include_directories(
    ${PROJECT_SOURCE_DIR}/Agar-libs/enet/include/
    ${PROJECT_SOURCE_DIR}/Agar-libs/glad/include/
    ${PROJECT_SOURCE_DIR}/Agar-libs/glfw-3.4/include/
    ${PROJECT_SOURCE_DIR}/Agar-libs/glm-1.0.1/
    ${PROJECT_SOURCE_DIR}/Agar-libs/stb/
)

link_libraries(
    ${PROJECT_SOURCE_DIR}/build/Agar-libs/enet/libenet.a
    ${PROJECT_SOURCE_DIR}/build/Agar-libs/glm-1.0.1/glm/libglm.a
    ${PROJECT_SOURCE_DIR}/build/Agar-libs/glfw-3.4/src/libglfw3.a
)

add_executable(
    Agar
    ${PROJECT_SOURCE_DIR}/src/glad.c
    ${PROJECT_SOURCE_DIR}/src/main.cpp

    ${PROJECT_SOURCE_DIR}/src/engine/graphics/framebuffer.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/graphics/texture.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/graphics/shader.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/graphics/mesh.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/util/physics.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/util/maths.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/util/time.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/io/window.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/io/logger.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/engine.cpp  
)

add_executable(
    Server
    ${PROJECT_SOURCE_DIR}/server/main.cpp
)

target_link_libraries(Server
    enet
)

target_link_libraries(Agar
    OpenGL::GL
    enet
    glfw
    glm
)