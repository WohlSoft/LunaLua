cmake_minimum_required (VERSION 3.7.2)
project(LunaLoader)

if(NOT MSVC)
    message(FATAL_ERROR "NON-Microsoft Compilers are not supported right now")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/W3 /WX- /Ox /Oi /Os /Oy- /GL /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Gd /analyze- /errorReport:queue" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)

set(CMAKE_C_FLAGS_RELWITHDEBINFO "/W3 /WX- /Ox /Oi /Os /Oy- /GL /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Gd /analyze- /errorReport:queue" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)

set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/ERRORREPORT:QUEUE /INCREMENTAL:NO /MANIFEST /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\" /manifest:embed /OPT:REF /OPT:ICF /LTCG:incremental /TLBID:1 /DYNAMICBASE /NXCOMPAT /SAFESEH" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(defines
	-DNDEBUG
	-D_UNICODE
	-DUNICODE
	-DWIN32
)

set(libs
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    comdlg32.lib
    advapi32.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    odbc32.lib
    odbccp32.lib
)

set(src
	LunaLoader.cpp
	LunaLoaderPatch.cpp
)

set(winresources LunaLoader.rc)

add_executable(LunaLoader WIN32 ${src} ${winresources})

set_target_properties(LunaLoader PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib/$<$<CONFIG:Debug>:>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin/$<$<CONFIG:Debug>:>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin/$<$<CONFIG:Debug>:>"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/pdb/$<$<CONFIG:Debug>:>"
)

target_compile_definitions(LunaLoader PRIVATE ${defines})

target_link_libraries(LunaLoader PRIVATE ${libs})

add_executable(LunaLoader-exec ${src} ${winresources})

set_target_properties(LunaLoader-exec PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib/$<$<CONFIG:Debug>:>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin/$<$<CONFIG:Debug>:>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin/$<$<CONFIG:Debug>:>"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/pdb/$<$<CONFIG:Debug>:>"
)

target_compile_definitions(LunaLoader-exec PRIVATE ${defines} -DLUNALOADER_EXEC)

target_link_options(LunaLoader-exec PRIVATE /ENTRY:WinMainCRTStartup)

target_link_libraries(LunaLoader-exec PRIVATE ${libs} shlwapi.lib)
