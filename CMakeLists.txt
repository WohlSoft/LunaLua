# Header
cmake_minimum_required (VERSION 3.6)
project(LunaLua)

# Checks
if(NOT MSVC)
    message(FATAL_ERROR "NON-Microsoft Compilers are not supported right now")
endif()

# Includes
include(CMake/ucm.cmake)

# Include Dependencies
include(CMake/DownloadAndConfigureDepends/FreeImage/FreeImage.cmake)
include(CMake/DownloadAndConfigureDepends/glew/glew.cmake)
include(CMake/DownloadAndConfigureDepends/luajit/luajit.cmake)
include(CMake/DownloadAndConfigureDepends/boost/boost.cmake)



# ============= LUNALUA CORE LIB START ================

# General Settings (REQUIRED)
ucm_add_dirs(LunaDll TO LUNALUA_LIB_FILES RECURSIVE FILTER_POP 1)
# Remove 'libs' because it is not part of the LunaLua source
ucm_remove_directories(libs FROM LUNALUA_LIB_FILES)

add_library (LunaLuaLib MODULE ${LUNALUA_LIB_FILES})

# Set FreeImage dependency
include_directories(${FREEIMAGE_INCLUDE_DIR})
add_dependencies(LunaLuaLib FreeImage)
target_link_libraries(LunaLuaLib ${FREEIMAGE_LIBRARY_FILES})

# Set glew dependency
include_directories(${GLEW_INCLUDE_DIR})
add_dependencies(LunaLuaLib glew)
target_link_libraries(LunaLuaLib ${GLEW_LIBRARY_STATIC_FILES}) # TODO: Support Shared!

# Set luajit dependency
include_directories(${LUAJIT_INCLUDE_DIR})
add_dependencies(LunaLuaLib luajit)
target_link_libraries(LunaLuaLib ${LUAJIT_LIBRARY_FILES})


# =============  LUNALUA CORE LIB END  ================

