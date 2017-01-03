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
set(GLEW_LIBRARY_STATIC_FILES ${GLEW_LIBRARY_DIR}/glew.lib)
set(GLEW_LIBRARY_SHARED_FILES ${GLEW_LIBRARY_DIR}/libglew_shared.lib)
set(GLEW_BINARY_DIR ${GLEW_INSTALL_DIR}/lib)
set(GLEW_BINARY_FILES ${GLEW_BINARY_DIR}/glew.dll)

ExternalProject_Add(
    glew
    PREFIX ${CMAKE_BINARY_DIR}/external/glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG 7be092340b60838bc860bf8bbde01f42b8d51125
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${GLEW_INSTALL_DIR}"
)


