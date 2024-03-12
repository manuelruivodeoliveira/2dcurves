cmake_minimum_required(VERSION 3.28)
project(2dcurves)

set(CMAKE_CXX_STANDARD 23)

# Fetch libraries
include(FetchContent)

    # Fetch GLFW
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/
    )

    # Fetch GLM
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/
    )

FetchContent_MakeAvailable(glfw glm)


add_executable(2dcurves
    src/main.cpp
    src/Shader.cpp
    src/glad/gl.c
)

# glad
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include/")
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/")

target_include_directories(2dcurves PUBLIC
    include/
    ${GLAD_INCLUDE_DIR}
    ${glfw_SOURCE_DIR}/include/
    ${glm_SOURCE_DIR}/
)

target_link_libraries(2dcurves PUBLIC
    glfw
)
