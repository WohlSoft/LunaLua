# This file downloads, configures and build FreeImage.
# This file works in conjunction with FreeImageBuild.cmake, 
# which will be copied as "CMakeLists.txt" in the FreeImage root folder after download
#
# Output Variables:
# FREEIMAGE_INSTALL_DIR  The install directory
# FREEIMAGE_INCLUDE_DIR  The include directory of FreeImage (.h)
# FREEIMAGE_LIBRARY_DIR  The library directory of FreeImage (.lib)
# FREEIMAGE_BINARY_DIR   The binary directory of FreeImage  (.dll) [ONLY IF BUILT SHARED!]

# Require ExternalProject!
include(ExternalProject)

# Depend Config-Engine on FreeImageBuild.cmake
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/FreeImageBuild.cmake)

# Posttible Input Vars:
# <None>

# SET OUTPUT VARS
set(FREEIMAGE_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/freeimage-install)
set(FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_INSTALL_DIR}/include)
set(FREEIMAGE_LIBRARY_DIR ${FREEIMAGE_INSTALL_DIR}/lib)
set(FREEIMAGE_LIBRARY_FILES ${FREEIMAGE_LIBRARY_DIR}/FreeImage.lib ${FREEIMAGE_LIBRARY_DIR}/FreeImagePlus.lib)
set(FREEIMAGE_BINARY_DIR ${FREEIMAGE_INSTALL_DIR}/bin)
set(FREEIMAGE_BINARY_FILES ${FREEIMAGE_BINARY_DIR}/FreeImage.dll ${FREEIMAGE_BINARY_DIR}/FreeImagePlus.dll)

ExternalProject_Add(
    FreeImage
    PREFIX ${CMAKE_BINARY_DIR}/external/freeimage
    URL http://downloads.sourceforge.net/project/freeimage/Source%20Distribution/3.17.0/FreeImage3170.zip
    BUILD_IN_SOURCE 1
    TIMEOUT 10
    UPDATE_COMMAND ""
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${FREEIMAGE_INSTALL_DIR}"
)

ExternalProject_Get_Property(FreeImage SOURCE_DIR)

# Inject our CMake file to build FreeImage with cmake
ExternalProject_Add_Step(
    FreeImage InjectCMakeFile
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/FreeImageBuild.cmake ${SOURCE_DIR}/CMakeLists.txt
    COMMENT "Injecting FreeImage CMake File"
    DEPENDEES DOWNLOAD
    LOG 1
)

# Apply tif_config patch
ExternalProject_Add_Step(
    FreeImage ApplyMSVC14TifConfigPatch
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/tif_config.h ${SOURCE_DIR}/Source/LibTIFF4/tif_config.h
    COMMENT "Apply tif_config.h patch"
    DEPENDEES DOWNLOAD
    LOG 1
)
