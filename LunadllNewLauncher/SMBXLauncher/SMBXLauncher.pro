#-------------------------------------------------
#
# Project created by QtCreator 2015-05-30T07:04:24
#
#-------------------------------------------------

QT       += core gui webkitwidgets

CONFIG += c++11

DEFINES += PGE_FILES_QT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SMBXLauncher
TEMPLATE = app


SOURCES += main.cpp\
        mainlauncherwindow.cpp \
    smbxconfig.cpp \
    autostartconfig.cpp \
    PGE_File_Formats/file_formats.cpp \
    PGE_File_Formats/file_rw_lvl.cpp \
    PGE_File_Formats/file_rw_lvlx.cpp \
    PGE_File_Formats/file_rw_meta.cpp \
    PGE_File_Formats/file_rw_npc_txt.cpp \
    PGE_File_Formats/file_rw_sav.cpp \
    PGE_File_Formats/file_rw_savx.cpp \
    PGE_File_Formats/file_rw_smbx64_cnf.cpp \
    PGE_File_Formats/file_rw_wld.cpp \
    PGE_File_Formats/file_rw_wldx.cpp \
    PGE_File_Formats/file_rwopen.cpp \
    PGE_File_Formats/file_strlist.cpp \
    PGE_File_Formats/lvl_filedata.cpp \
    PGE_File_Formats/npc_filedata.cpp \
    PGE_File_Formats/pge_file_lib_globs.cpp \
    PGE_File_Formats/pge_x.cpp \
    PGE_File_Formats/save_filedata.cpp \
    PGE_File_Formats/smbx64.cpp \
    PGE_File_Formats/smbx64_cnf_filedata.cpp \
    PGE_File_Formats/wld_filedata.cpp \
    controlconfig.cpp \
    launcherconfiguration.cpp

HEADERS  += mainlauncherwindow.h \
    smbxconfig.h \
    autostartconfig.h \
    PGE_File_Formats/file_formats.h \
    PGE_File_Formats/file_strlist.h \
    PGE_File_Formats/lvl_filedata.h \
    PGE_File_Formats/meta_filedata.h \
    PGE_File_Formats/npc_filedata.h \
    PGE_File_Formats/pge_file_lib_globs.h \
    PGE_File_Formats/pge_file_lib_sys.h \
    PGE_File_Formats/pge_x.h \
    PGE_File_Formats/pge_x_macro.h \
    PGE_File_Formats/save_filedata.h \
    PGE_File_Formats/smbx64.h \
    PGE_File_Formats/smbx64_cnf_filedata.h \
    PGE_File_Formats/smbx64_macro.h \
    PGE_File_Formats/wld_filedata.h \
    controlconfig.h \
    launcherconfiguration.h \
    qjsonutil.h

FORMS    += mainlauncherwindow.ui

RESOURCES += \
    resource.qrc


RC_FILE = "SMBXLauncher.rc"
