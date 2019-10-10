cmake_minimum_required (VERSION 3.7.2)
project(LunaLua C CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LunaLua_BINARY_DIR}")

if(NOT MSVC)
    message(FATAL_ERROR "NON-Microsoft Compilers are not supported right now")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "MinSizeRel")
endif()

set(CompilerFlags
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL
    CMAKE_C_FLAGS_RELWITHDEBINFO
)
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag}_OUT "${${CompilerFlag}}")
    set(${CompilerFlag} ${${CompilerFlag}_OUT})
endforeach()

# Remove "/showIncludes" flag
if(CMAKE_CXX_FLAGS MATCHES "/showIncludes")
    string(REGEX REPLACE "/showIncludes" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
if(CMAKE_C_FLAGS MATCHES "/showIncludes")
    string(REGEX REPLACE "/showIncludes" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# Add debug information into release builds
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Zi")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL} /debug /INCREMENTAL" CACHE STRING "linker flags" FORCE)

foreach(CompilerFlag ${CompilerFlags})
    message("== ${CompilerFlag} = ${${CompilerFlag}} ==")
    set(${CompilerFlag} ${${CompilerFlag}} CACHE STRING "compiler flags" FORCE)
endforeach()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(LunaDll)
