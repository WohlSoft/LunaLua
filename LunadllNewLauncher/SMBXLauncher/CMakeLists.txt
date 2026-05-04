cmake_minimum_required (VERSION 3.7.2)
project(SMBXLauncher)

if(NOT MSVC)
    message(FATAL_ERROR "NON-Microsoft Compilers are not supported right now")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Zc:wchar_t -FS -Zc:rvalueCast -Zc:inline -Zc:strictStrings -Zc:throwingNew -Zc:referenceBinding -Zc:__cplusplus -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)

set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Zc:wchar_t -FS -Zc:rvalueCast -Zc:inline -Zc:strictStrings -Zc:throwingNew -Zc:referenceBinding -Zc:__cplusplus -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_RELWITHDEBINFO}" CACHE
    STRING "compiler flags" FORCE)

set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO \"/MANIFESTDEPENDENCY:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' publicKeyToken='6595b64144ccf1df' language='*' processorArchitecture='*'\" /MANIFEST:embed" CACHE
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

find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5WebEngineWidgets)

include(../../LunaDll/libs/PGE_File_Formats/pge_file_library.cmake)

set(launcher_src main.cpp
	../../LunaDll/Input/LunaGameController.cpp
	mainlauncherwindow.cpp
	ExposedObjects/smbxconfig.cpp
	ExposedObjects/autostartconfig.cpp
	ExposedObjects/controlconfig.cpp
	launcherconfiguration.cpp
	Utils/Network/networkutils.cpp
	Utils/Network/qreplytimeout.cpp
	Utils/filesysutils.cpp
	Utils/Json/extendedqjsonreader.cpp
	Utils/Json/qjsonparseexception.cpp
	Utils/Json/qjsonfileopenexception.cpp
	Utils/Json/qjsonvalidationexception.cpp
	Utils/Network/qnetworkreplytimeoutexception.cpp
	Utils/Network/qnetworkreplyexception.cpp
	Utils/Common/qurlinvalidexception.cpp
	Utils/networkjsonutils.cpp
	Utils/Json/qjsonurlvalidationexception.cpp
	launchercustomwebpage.cpp
	hybridlogger.cpp
	devtoolsdialog.cpp
	launcherurlrequestinterceptor.cpp
)

set(resources resource.qrc)
set(forms mainlauncherwindow.ui devtoolsdialog.ui)
set(winresources SMBXLauncher.rc)

add_executable(SMBXLauncher WIN32 ${launcher_src} ${PGE_FILE_LIBRARY_SRCS} ${resources} ${forms} ${winresources})

set_target_properties(SMBXLauncher PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/lib/$<$<CONFIG:Debug>:>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin/$<$<CONFIG:Debug>:>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/bin/$<$<CONFIG:Debug>:>"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/pdb/$<$<CONFIG:Debug>:>"
)

target_include_directories(SMBXLauncher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../LunaDll/libs ${CMAKE_CURRENT_SOURCE_DIR}/../../LunaDll/libs/sdl/include/)

target_compile_definitions(SMBXLauncher PRIVATE
	-DUNICODE
	-D_UNICODE
	-DPGE_FILES_QT
	-DBUILDING_SMBXLAUNCHER
	-DWIN32
    -DNDEBUG
    -DWINDOWS
    -D_ENABLE_EXTENDED_ALIGNED_STORAGE
    -D__PRFCHWINTRIN_H # Fix compilation with Clang >= 21.1.0
    -DSDL_DISABLE_MM3DNOW_H # 3DNow is deprecated
)

target_link_directories(SMBXLauncher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../LunaDll/libs/sdl/lib/)
target_link_libraries(SMBXLauncher PRIVATE Qt5::Gui Qt5::Widgets Qt5::Core Qt5::WebEngineWidgets shell32.lib user32.lib SDL2.lib SDL2main.lib)
