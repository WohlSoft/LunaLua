# Header
cmake_minimum_required (VERSION 3.7.2)
project(LunaLua)

# Checks
if(NOT MSVC)
    message(FATAL_ERROR "NON-Microsoft Compilers are not supported right now")
endif()

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

# Compiler settings
# Force unicode for all projects
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

# Includes
include(CMake/ucm.cmake)

# Include Dependencies
include(CMake/DownloadAndConfigureDepends/FreeImage/FreeImage.cmake)
include(CMake/DownloadAndConfigureDepends/glbinding/glbinding.cmake)
include(CMake/DownloadAndConfigureDepends/luajit/luajit.cmake)
include(CMake/DownloadAndConfigureDepends/boost/boost.cmake)
include(CMake/DownloadAndConfigureDepends/luabind/luabind.cmake) # Depends on boost and luajit!
include(CMake/DownloadAndConfigureDepends/catch/catch.cmake) # For testing

# SETTINGS
set(LUNALUA_3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

# ============= LUNALUA CORE LIB START ================

# General Settings (REQUIRED)
ucm_add_dirs(LunaDll TO LUNALUA_LIB_FILES RECURSIVE FILTER_POP 1)
# Remove 'libs' because it is not part of the LunaLua source
ucm_remove_directories(libs FROM LUNALUA_LIB_FILES)

# DEPENDENCIES:
# Set stackwalker dependency as raw source
ucm_add_dirs(3rdparty/stackwalker TO LUNALUA_LIB_FILES)
include_directories(${LUNALUA_3RDPARTY_DIR}/stackwalker)

# Set luasocket dependency as raw source
ucm_add_dirs(3rdparty/luasocket TO LUNALUA_LIB_FILES)
ucm_remove_files(3rdparty/luasocket/usocket.c 3rdparty/luasocket/unix.c 3rdparty/luasocket/serial.c FROM LUNALUA_LIB_FILES)

# Set initreader dependency as raw sourc
ucm_add_dirs(3rdparty/ini-reader TO LUNALUA_LIB_FILES)

# Make LunaLuaLib object target
add_library (LunaLuaLib OBJECT ${LUNALUA_LIB_FILES})

# shared libraries need PIC
set_property(TARGET ${LunaLuaLib} PROPERTY POSITION_INDEPENDENT_CODE 1)

# Make LunaLuaLib shared target
add_library(LunaLuaLibShared SHARED $<TARGET_OBJECTS:LunaLuaLib>)
add_dependencies(LunaLuaLibShared LunaLuaLib)
# We want the name as "LunaDll.dll"
set_target_properties(LunaLuaLibShared PROPERTIES OUTPUT_NAME LunaDll)

# Make LunaLuaLib static target
add_library(LunaLuaLibStatic STATIC $<TARGET_OBJECTS:LunaLuaLib>)
add_dependencies(LunaLuaLibStatic LunaLuaLib)

# Set FreeImage dependency
include_directories(${FREEIMAGE_INCLUDE_DIR})
add_dependencies(LunaLuaLib FreeImage)
target_link_libraries(LunaLuaLibShared ${FREEIMAGE_LIBRARY_FILES})
target_link_libraries(LunaLuaLibStatic ${FREEIMAGE_LIBRARY_FILES})

# Set glbinding dependency
include_directories(${GLBINDING_INCLUDE_DIR})
add_dependencies(LunaLuaLib glbinding)
target_link_libraries(LunaLuaLibShared ${GLBINDING_LIBRARY_STATIC_FILES}) # TODO: Support Shared!
target_link_libraries(LunaLuaLibStatic ${GLBINDING_LIBRARY_STATIC_FILES})



# Set luajit dependency
include_directories(${LUAJIT_INCLUDE_DIR})
add_dependencies(LunaLuaLib luajit)
target_link_libraries(LunaLuaLibShared ${LUAJIT_LIBRARY_FILES})
target_link_libraries(LunaLuaLibStatic ${LUAJIT_LIBRARY_FILES})

# Set luabind dependency
include_directories(${LUABIND_INCLUDE_DIR})
add_dependencies(LunaLuaLib luabind)
target_link_libraries(LunaLuaLibShared ${LUABIND_LIBRARY_FILES})
target_link_libraries(LunaLuaLibStatic ${LUABIND_LIBRARY_FILES})

# Set gif-h dependency
include_directories(${LUNALUA_3RDPARTY_DIR}/gif-h/include)

# Set SDL Mixer dependency
include_directories(${LUNALUA_3RDPARTY_DIR}/sdl/include)
target_link_libraries(LunaLuaLibShared ${LUNALUA_3RDPARTY_DIR}/sdl/bin/SDL2_mixer_ext.lib ${LUNALUA_3RDPARTY_DIR}/sdl/lib/SDL2.lib ${LUNALUA_3RDPARTY_DIR}/sdl/lib/SDL2main.lib)
target_link_libraries(LunaLuaLibStatic ${LUNALUA_3RDPARTY_DIR}/sdl/bin/SDL2_mixer_ext.lib ${LUNALUA_3RDPARTY_DIR}/sdl/lib/SDL2.lib ${LUNALUA_3RDPARTY_DIR}/sdl/lib/SDL2main.lib)

# Set nlohmann json dependency
include_directories(${LUNALUA_3RDPARTY_DIR}/json)

# Set boost dependency
include_directories(${BoostSourceDir})

# Additional windows depedencies
target_link_libraries(LunaLuaLibShared comsuppw.lib opengl32.lib Gdiplus.lib Winmm.lib ws2_32.lib)
target_link_libraries(LunaLuaLibStatic comsuppw.lib opengl32.lib Gdiplus.lib Winmm.lib ws2_32.lib)

# =============  LUNALUA CORE LIB END  ================

# =============  LUNALUA TESTING START ================
# Enable Testing
enable_testing()

# Get Test files and set test executable
ucm_add_dirs(LunaLuaTest TO LUNALUA_TEST_FILES RECURSIVE FILTER_POP 1)
add_executable(LunaLuaTest ${LUNALUA_TEST_FILES})

# Setup dependency to the LunaLuaLib
target_include_directories(LunaLuaTest PUBLIC ${CMAKE_SOURCE_DIR}/LunaDll PUBLIC ${CATCH_INCLUDE_DIR})
target_link_libraries(LunaLuaTest PUBLIC LunaLuaLibStatic)
add_dependencies(LunaLuaTest catch LunaLuaLibStatic)

add_test(NAME LunaLuaTestProj COMMAND LunaLuaTest)
# =============  LUNALUA TESTING END ================




# Disable boost (non-header-only) libraries
foreach(BoostTarget ${AllBoostLibs})
    set_target_properties(${BoostTarget} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endforeach()

# Enable multi-processor build
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# ============= TIMESTAMP FORCE TARGET =================
add_custom_target(ZERO_CHECK_FORCE ALL 
    COMMAND copy /b CMakeLists.txt +,,
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_target_properties(ZERO_CHECK_FORCE PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)





