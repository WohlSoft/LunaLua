# This file downloads, configures and build glbinding.
#
# Output Variables:
# GLBINDING_INSTALL_DIR  The install directory
# GLBINDING_INCLUDE_DIR  The include directory of glbinding (.h)
# GLBINDING_LIBRARY_DIR  The library directory of glbinding (.lib)
# GLBINDING_BINARY_DIR   The binary directory of glbinding  (.dll) [ONLY IF BUILT SHARED!]

# Require ExternalProject and GIT!
include(ExternalProject)
find_package(Git REQUIRED)

# Posttible Input Vars:
# <None>

# SET OUTPUT VARS
set(GLBINDING_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/glbinding-install)
set(GLBINDING_INCLUDE_DIR ${GLBINDING_INSTALL_DIR}/include)
set(GLBINDING_LIBRARY_DIR ${GLBINDING_INSTALL_DIR}/lib)
set(GLBINDING_LIBRARY_STATIC_FILES ${GLBINDING_LIBRARY_DIR}/glbinding$<$<CONFIG:Debug>:d>.lib)
set(GLBINDING_BINARY_DIR ${GLBINDING_INSTALL_DIR}/lib)

if(NOT GLBINDING_BUILD_SHARED)
    add_definitions(-DGLBINDING_STATIC) 
endif()


ExternalProject_Add(
    glbinding
    PREFIX ${CMAKE_BINARY_DIR}/external/glbinding
    GIT_REPOSITORY https://github.com/cginternals/glbinding.git
    GIT_TAG ccb951e0bc5917f0f27ba79c76045dc21b7f70dc
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${GLBINDING_INSTALL_DIR}"
)


