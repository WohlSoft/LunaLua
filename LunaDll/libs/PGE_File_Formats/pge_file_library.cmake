# message("Path to PGE File Library is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

set(PGE_FILE_LIBRARY_SRCS)

list(APPEND PGE_FILE_LIBRARY_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/ConvertUTF_PGEFF.c
    ${CMAKE_CURRENT_LIST_DIR}/file_formats.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_lvl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_lvl_38a.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_lvlx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_meta.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_npc_txt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_sav.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_wld.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_wldx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_smbx64_cnf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rwopen.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_strlist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lvl_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/npc_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pge_x.cpp
    ${CMAKE_CURRENT_LIST_DIR}/save_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/smbx64.cpp
    ${CMAKE_CURRENT_LIST_DIR}/smbx64_cnf_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/wld_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pge_file_lib_globs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_savx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_lvl_38a_old.cpp
    ${CMAKE_CURRENT_LIST_DIR}/file_rw_wld_38a.cpp
)

