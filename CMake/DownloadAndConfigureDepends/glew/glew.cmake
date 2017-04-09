# This file downloads, configures and build glew.
#
# Output Variables:
# GLEW_INSTALL_DIR  The install directory
# GLEW_INCLUDE_DIR  The include directory of glew (.h)
# GLEW_LIBRARY_DIR  The library directory of glew (.lib)
# GLEW_BINARY_DIR   The binary directory of glew  (.dll) [ONLY IF BUILT SHARED!]

# Require ExternalProject and GIT!
include(ExternalProject)
find_package(Git REQUIRED)

# Posttible Input Vars:
# <None>

# SET OUTPUT VARS
set(GLEW_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/glew-install)
set(GLEW_INCLUDE_DIR ${GLEW_INSTALL_DIR}/include)
set(GLEW_LIBRARY_DIR ${GLEW_INSTALL_DIR}/lib)
set(GLEW_LIBRARY_STATIC_FILES ${GLEW_LIBRARY_DIR}/libglew32$<$<CONFIG:Debug>:d>.lib)
set(GLEW_LIBRARY_SHARED_FILES ${GLEW_LIBRARY_DIR}/glew32$<$<CONFIG:Debug>:d>.lib)
set(GLEW_BINARY_DIR ${GLEW_INSTALL_DIR}/lib)
set(GLEW_BINARY_FILES ${GLEW_BINARY_DIR}/glew32$<$<CONFIG:Debug>:d>.dll)

if(NOT GLEW_BUILD_SHARED)
    add_definitions(-DGLEW_STATIC) 
endif()


ExternalProject_Add(
    glew-download
    PREFIX ${CMAKE_BINARY_DIR}/external/glew-download
    URL https://downloads.sourceforge.net/project/glew/glew/1.13.0/glew-1.13.0.zip
	URL_HASH SHA256=bcbb058a14ef565bd3466864232206973fa801387e693177f42f660599aa6e99
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add(
    glew
	DEPENDS glew-download
    PREFIX ${CMAKE_BINARY_DIR}/external/glew
    DOWNLOAD_COMMAND ""
	SOURCE_DIR ${CMAKE_BINARY_DIR}/external/glew-download/src/glew-download/build/cmake
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${GLEW_INSTALL_DIR}"
)
