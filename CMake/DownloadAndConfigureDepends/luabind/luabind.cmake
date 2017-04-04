# This file downloads, configures and build glew.
#
# Output Variables:
# LUABIND_INSTALL_DIR  The install directory
# LUABIND_INCLUDE_DIR  The include directory of glew (.h)
# LUABIND_LIBRARY_DIR  The library directory of glew (.lib)
# LUABIND_BINARY_DIR   The binary directory of glew  (.dll) [ONLY IF BUILT SHARED!]

# Require ExternalProject and GIT!
include(ExternalProject)
find_package(Git REQUIRED)

# Posttible Input Vars:
# <None>

# SET OUTPUT VARS
set(LUABIND_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/luabind-install)
set(LUABIND_INCLUDE_DIR ${LUABIND_INSTALL_DIR}/include)
set(LUABIND_LIBRARY_DIR ${LUABIND_INSTALL_DIR}/lib)
set(LUABIND_LIBRARY_FILES ${LUABIND_LIBRARY_DIR}/luabind.lib)
set(LUABIND_BINARY_DIR ${LUABIND_INSTALL_DIR}/lib)
set(LUABIND_BINARY_FILES ${LUABIND_BINARY_DIR}/luabind.dll)

if(NOT DEFINED BoostSourceDir)
    message(FATAL_ERROR "BoostSourceDir was not found. Be sure to call add_boost first!")
endif()

if(NOT DEFINED LUAJIT_INCLUDE_DIR OR NOT DEFINED LUAJIT_LIBRARY_FILES)
    message(FATAL_ERROR "Require LUAJIT_INCLUDE_DIR and LUAJIT_LIBRARY_FILES required! Be sure to configure luajit first!")
endif()

ExternalProject_Add(
    luabind
    PREFIX ${CMAKE_BINARY_DIR}/external/luabind
    DEPENDS luajit
    # GIT_REPOSITORY https://github.com/rpavlik/luabind.git
    # GIT_TAG a0edf58e14cfb8cd4f13779707921b5e9caf686c
    GIT_REPOSITORY https://github.com/KevinW1998/luabind_cpp11.git
    GIT_TAG 11808119aaa2ccb2fe39483a1cdbae56e6efd46a
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${LUABIND_INSTALL_DIR}" "-DBOOST_ROOT=${BoostSourceDir}" "-DLUA_LIBRARIES=${LUAJIT_LIBRARY_FILES}" "-DLUA_INCLUDE_DIR=${LUAJIT_INCLUDE_DIR}" "-DBUILD_TESTING=FALSE" "-DBUILD_SHARED=FALSE"
)


