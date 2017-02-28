cmake_minimum_required (VERSION 2.8)
project (spirals)

find_package(OpenGL REQUIRED)

aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME}
    ${SRC_LIST}
    triangle/triangle.h
    triangle/triangle_fshader.glsl
    triangle/triangle_vshader.glsl
)
