cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(vehicle-tool LANGUAGES C CXX)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "external/imgui/*.cpp" "external/glad/src/*.c")
file(GLOB_RECURSE HEADER_FILES "src/*.h")

add_compile_options("$<$<CONFIG:DEBUG>:-Wall>")

add_subdirectory(external)
include_directories("external/glad/include")
include_directories("external/stb/")
include_directories("external/imgui/")
include_directories("external/cgltf/")
include_directories("external/tiny_obj_loader/")
include_directories("src/")

add_executable(main
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

find_package(OpenGL REQUIRED)
target_link_libraries(main
    PUBLIC

    OpenGL::OpenGL
    SDL3
    glm
)