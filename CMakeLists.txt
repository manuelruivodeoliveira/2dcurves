cmake_minimum_required(VERSION 3.28)
project(2dcurves)

set(CMAKE_CXX_STANDARD 23)

add_executable(2dcurves
    src/main.cpp
    src/Shader.cpp
    deps/glad/src/gl.c
)

# glad
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include/")

# glfw
set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include/")
set(GLFW_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/build/src/Debug/glfw3.lib")

# glm
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/")

target_include_directories(2dcurves PUBLIC
    include/
    ${GLAD_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

target_link_libraries(2dcurves PUBLIC
    ${GLFW_LIB_PATH}
)
