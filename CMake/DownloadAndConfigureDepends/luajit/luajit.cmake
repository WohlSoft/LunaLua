# This file downloads, configures and build luajit.
#
# Output Variables:
# LUAJIT_INSTALL_DIR  The install directory
# LUAJIT_INCLUDE_DIR  The include directory of luajit (.h)
# LUAJIT_LIBRARY_DIR  The library directory of luajit (.lib)
# LUAJIT_BINARY_DIR   The binary directory of luajit  (.dll) [ONLY IF BUILT SHARED!]

# Require ExternalProject!
include(ExternalProject)

# Posttible Input Vars:
# <None>

# SET OUTPUT VARS


set(LUAJIT_COMPILE_ARG_STATIC "")

IF(LUAJIT_COMPILE_SHARED)
ELSE()
    set(LUAJIT_COMPILE_ARG_STATIC "static") # FIXME: Right now --> always static for release-mode
ENDIF()

set(LUAJIT_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/luajit-install)
set(LUAJIT_INCLUDE_DIR ${LUAJIT_INSTALL_DIR}/include)
set(LUAJIT_LIBRARY_DIR ${LUAJIT_INSTALL_DIR}/lib)
set(LUAJIT_LIBRARY_FILES ${LUAJIT_LIBRARY_DIR}/lua51.lib)
set(LUAJIT_BINARY_DIR ${LUAJIT_INSTALL_DIR}/bin)
set(LUAJIT_BINARY_FILES ${LUAJIT_BINARY_DIR}/lua51.dll)

ExternalProject_Add(
    luajit
    PREFIX ${CMAKE_BINARY_DIR}/external/luajit
    URL http://luajit.org/download/LuaJIT-2.1.0-beta2.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    # Currently dirty hack to get it right:
    BUILD_COMMAND ${CMAKE_COMMAND} -E chdir src msvcbuild.bat "$<$<CONFIG:Debug>:debug>$<$<NOT:$<CONFIG:Debug>>:static>" ${LUAJIT_COMPILE_ARG_STATIC}
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(luajit SOURCE_DIR)

ExternalProject_Add_Step(
    luajit CreateLuaJitInstallFolder
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LUAJIT_INSTALL_DIR}/include ${LUAJIT_INSTALL_DIR}/lib ${LUAJIT_INSTALL_DIR}/bin
    COMMENT "Creating install folder"
    DEPENDEES BUILD
)

ExternalProject_Add_Step(
    luajit InstallLuaJitHeaders
    COMMAND ${CMAKE_COMMAND} -E copy lua.h lua.hpp luaconf.h luajit.h lualib.h lauxlib.h ${LUAJIT_INSTALL_DIR}/include
    WORKING_DIRECTORY ${SOURCE_DIR}/src
    COMMENT "Installing luajit headers"
    DEPENDEES BUILD CreateLuaJitInstallFolder
)

ExternalProject_Add_Step(
    luajit InstallLuaJitLib
    COMMAND ${CMAKE_COMMAND} -E copy lua51.lib ${LUAJIT_INSTALL_DIR}/lib
    WORKING_DIRECTORY ${SOURCE_DIR}/src
    COMMENT "Installing luajit lib"
    DEPENDEES BUILD CreateLuaJitInstallFolder
)

IF(LUAJIT_COMPILE_SHARED)
    ExternalProject_Add_Step(
        luajit InstallLuaJitDLL
        COMMAND ${CMAKE_COMMAND} -E copy lua51.dll ${LUAJIT_INSTALL_DIR}/bin
        WORKING_DIRECTORY ${SOURCE_DIR}/src
        COMMENT "Installing luajit dll"
        DEPENDEES BUILD CreateLuaJitInstallFolder
    )
ENDIF()





















