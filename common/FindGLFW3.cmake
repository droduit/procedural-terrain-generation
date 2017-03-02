# Locate the GLFW library (version 3.0)
# This module defines the following variables:
# GLFW_LIBRARIES, the name of the library;
# GLFW_INCLUDE_DIRS, where to find GLFW include files.
# GLFW_FOUND, true if library path was resolved
#
# Usage example to compile an "executable" target to the glfw library:
#
# FIND_PACKAGE (GLFW REQUIRED)
# INCLUDE_DIRECTORIES (${GLFW_INCLUDE_DIRS})
# ADD_EXECUTABLE (executable ${YOUR_EXECUTABLE_SRCS})
# TARGET_LINK_LIBRARIES (executable ${GLFW_LIBRARIES})
#
# TODO:
# Lookup for windows
# Allow the user to select to link to a shared library or to a static library.
#
# SEE:
# - https://raw.github.com/progschj/OpenGL-Examples/master/cmake_modules/FindGLFW.cmake
#

# We only officially support visual studio 2015's compiler but if you already
# have an older version of visual studio you can change the following LIBPATH
# to match your version of visual studio (2015, 2013, 2012, 2010)
IF (WIN32)
    IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        SET( BITS "64")
                SET(LIBPATH ${CMAKE_SOURCE_DIR}/external/glfw3/x64/lib-vc2015/)
                SET(INCLPATH ${CMAKE_SOURCE_DIR}/external/glfw3/x64/include)
    ELSE()
                SET(LIBPATH ${CMAKE_SOURCE_DIR}/external/glfw3/x32/lib-vc2015/)
                SET(INCLPATH ${CMAKE_SOURCE_DIR}/external/glfw3/x32/include)
    ENDIF()
ENDIF()

FIND_PATH( GLFW3_INCLUDE_DIRS GLFW/glfw3.h
    $ENV{GLFWDIR}/include
    /usr/local/include
    /usr/local/X11R6/include
    /usr/X11R6/include
    /usr/X11/include
    /usr/include/X11
    /usr/include
    /opt/X11/include
    /opt/include
    ${INCLPATH})

FIND_LIBRARY( GLFW3_LIBRARIES NAMES glfw3 glfw PATHS
    $ENV{GLFWDIR}/lib
    $ENV{GLFWDIR}/support/msvc80/Debug
    $ENV{GLFWDIR}/support/msvc80/Release
    /usr/local/lib
    /usr/local/X11R6/lib
    /usr/X11R6/lib
    /usr/X11/lib
    /usr/lib/X11
    /usr/lib
    /opt/X11/lib
    /opt/lib
    ${LIBPATH})

SET(GLFW3_FOUND "NO")
IF(GLFW3_LIBRARIES AND GLFW3_INCLUDE_DIRS)
    SET(GLFW3_FOUND "YES")
    message(STATUS "Found GLFW3: ${GLFW3_LIBRARIES}")
ENDIF(GLFW3_LIBRARIES AND GLFW3_INCLUDE_DIRS)
