cmake_minimum_required (VERSION 3.7.2)
project(LunaLuaExtraSettingsTest C CXX)


set(LL_ROOT ${LunaLuaExtraSettingsTest_SOURCE_DIR}/../../LunaDll)

include(${LL_ROOT}/libs/DirManager/dirman.cmake)
include(${LL_ROOT}/libs/IniProcessor/IniProcessor.cmake)
include(${LL_ROOT}/libs/Utils/Utils.cmake)
include(${LL_ROOT}/libs/fmt/fmt.cmake)

add_executable(lles_test
    lles-test.cpp
    ${LL_ROOT}/FileManager/config_manager.cpp
    ${FMT_SRCS}
    ${UTILS_SRCS}
    ${DIRMANAGER_SRCS}
    ${INIPROCESSOR_SRCS}
)

target_compile_definitions(lles_test PRIVATE
    -DUNIT_TEST
)

target_include_directories(lles_test PRIVATE
    ${LL_ROOT}
    ${LL_ROOT}/libs
)

target_link_libraries(lles_test
    PRIVATE
    ${UTILS_LIBS}
    SDL2
)
