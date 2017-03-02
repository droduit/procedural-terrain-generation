# Build configuration file for "Introduction to Computer Graphics"
# Copyright (C) 2016 - LGG EPFL
#
# To understand its content:
#   http://www.cmake.org/cmake/help/syntax.html
#   http://www.cmake.org/Wiki/CMake_Useful_Variables
#

# This is how you show a status message in the build system
message(STATUS "Intro to Graphics - Loading Common Configuration")

# Tell CMake he'll be able to include ".cmake" configurations
# files in the folder where the current file is located
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# CMake extension to load GLFW3
find_package(GLFW3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIRS})
add_definitions(${GLFW3_DEFINITIONS})
if(NOT GLFW3_FOUND)
    message(ERROR " GLFW3 not found!")
endif()

# GLEW (STATICALLY LINKED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(-DGLEW_STATIC)

# Deploy the shaders to build folder on every build
macro(deploy_shaders_to_build_dir)
    set(SHADERS "${ARGN}")
    get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
    set(TARGET_NAME copy_shaders_${EXERCISENAME})
    add_custom_target(${TARGET_NAME} ALL)
    foreach(SHADER ${SHADERS})
        add_custom_command(
            TARGET ${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Copying ${SHADER} to build folder")
    endforeach()
endmacro()

# Deploy files once to the executable dir
macro(copy_files_once)
    set(FILES_LIST "${ARGN}")
    foreach(SOME_FILE ${FILES_LIST})
        message(STATUS "Copying ${SOME_FILE} to build folder")
        file(COPY ${SOME_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()
endmacro()

# OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND) 
    message(ERROR " OPENGL not found!")
endif()

# Make headers in common directory visible in the IDE
file(GLOB_RECURSE COMMON_DIR_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
add_custom_target(common_headers SOURCES ${COMMON_DIR_HEADERS})

# GLM
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
if(NOT GLM_FOUND)
    message(ERROR " GLM not found!")
endif() 
# force glm to use radians (older versions were using degrees and this can lead
# to problems if an older glm version is already installed).
add_definitions(-DGLM_FORCE_RADIANS)

# Common headers/libraries for all the exercises
include_directories(${CMAKE_CURRENT_LIST_DIR})
SET(COMMON_LIBS ${OPENGL_LIBRARIES} ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES})

