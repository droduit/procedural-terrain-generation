FIND_PATH( GLEW_INCLUDE_DIRS GL/glew.h
    $ENV{GLEWDIR}/include
    /usr/local/include
    /usr/local/X11R6/include
    /usr/X11R6/include
    /usr/X11/include
    /usr/include/X11
    /usr/include
    /opt/X11/include
    /opt/include 
    # FOR WINDOWS
    ${CMAKE_SOURCE_DIR}/external/glew/include)

if(WIN32)
    IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        SET(GLEWLIBNAME glew64)
		SET(WINLIBPATH ${CMAKE_SOURCE_DIR}/external/glew/lib/x64)
    ELSE()
        SET(GLEWLIBNAME glew32s)
		SET(WINLIBPATH ${CMAKE_SOURCE_DIR}/external/glew/lib/win32)
    ENDIF()
else()
    # IMPORTANT: uppercase otherwise problem on linux
    SET(GLEWLIBNAME GLEW)
endif() 

FIND_LIBRARY( GLEW_LIBRARIES 
    NAMES ${GLEWLIBNAME} PATHS 
    $ENV{GLEWDIR}/lib
    /usr/local/lib
    /usr/local/X11R6/lib
    /usr/X11R6/lib
    /usr/X11/lib
    /usr/lib/X11
    /usr/lib
    /opt/X11/lib
    /opt/lib
    # FOR UBUNTU 12.04 LTS
    /usr/lib/x86_64-linux-gnu    
    # FOR WINDOWS 
    ${WINLIBPATH})

SET(GLEW_FOUND "NO")
IF(GLEW_LIBRARIES AND GLEW_INCLUDE_DIRS)
    SET(GLEW_FOUND "YES")
	message(STATUS "Found GLEW: ${GLEW_LIBRARIES}")
ENDIF()
