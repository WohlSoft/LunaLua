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

# ============= LUNALUA CORE LIB START ================

# General Settings (REQUIRED)
ucm_add_dirs(LunaDll TO LUNALUA_LIB_FILES RECURSIVE FILTER_POP 1)
# Remove 'libs' because it is not part of the LunaLua source
ucm_remove_directories(libs FROM LUNALUA_LIB_FILES)

add_library (LunaLuaLib MODULE ${LUNALUA_LIB_FILES})




# =============  LUNALUA CORE LIB END  ================

