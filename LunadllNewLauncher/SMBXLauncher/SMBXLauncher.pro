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
    controlconfig.cpp \
    launcherconfiguration.cpp \
    ../../LunaLoader/LunaLoaderPatch.cpp \
    NetworkUtils/networkutils.cpp \
    NetworkUtils/qreplytimeout.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_formats.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_lvl.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_lvl_38a.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_lvlx.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_meta.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_npc_txt.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_sav.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_savx.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_smbx64_cnf.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_wld.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rw_wldx.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_rwopen.cpp \
    ../../LunaDll/libs/PGE_File_Formats/file_strlist.cpp \
    ../../LunaDll/libs/PGE_File_Formats/lvl_filedata.cpp \
    ../../LunaDll/libs/PGE_File_Formats/npc_filedata.cpp \
    ../../LunaDll/libs/PGE_File_Formats/pge_file_lib_globs.cpp \
    ../../LunaDll/libs/PGE_File_Formats/pge_x.cpp \
    ../../LunaDll/libs/PGE_File_Formats/save_filedata.cpp \
    ../../LunaDll/libs/PGE_File_Formats/smbx64.cpp \
    ../../LunaDll/libs/PGE_File_Formats/smbx64_cnf_filedata.cpp \
    ../../LunaDll/libs/PGE_File_Formats/wld_filedata.cpp \
    ../../LunaDll/libs/PGE_File_Formats/ConvertUTF.c

HEADERS  += mainlauncherwindow.h \
    smbxconfig.h \
    autostartconfig.h \
    controlconfig.h \
    launcherconfiguration.h \
    qjsonutil.h \
    NetworkUtils/networkutils.h \
    NetworkUtils/qreplytimeout.h \
    ../../LunaDll/libs/PGE_File_Formats/charsetconvert.h \
    ../../LunaDll/libs/PGE_File_Formats/ConvertUTF.h \
    ../../LunaDll/libs/PGE_File_Formats/CSVReader.h \
    ../../LunaDll/libs/PGE_File_Formats/CSVReaderPGE.h \
    ../../LunaDll/libs/PGE_File_Formats/CSVUtils.h \
    ../../LunaDll/libs/PGE_File_Formats/file_formats.h \
    ../../LunaDll/libs/PGE_File_Formats/file_strlist.h \
    ../../LunaDll/libs/PGE_File_Formats/lvl_filedata.h \
    ../../LunaDll/libs/PGE_File_Formats/meta_filedata.h \
    ../../LunaDll/libs/PGE_File_Formats/npc_filedata.h \
    ../../LunaDll/libs/PGE_File_Formats/pge_file_lib_globs.h \
    ../../LunaDll/libs/PGE_File_Formats/pge_file_lib_sys.h \
    ../../LunaDll/libs/PGE_File_Formats/pge_x.h \
    ../../LunaDll/libs/PGE_File_Formats/pge_x_macro.h \
    ../../LunaDll/libs/PGE_File_Formats/save_filedata.h \
    ../../LunaDll/libs/PGE_File_Formats/smbx64.h \
    ../../LunaDll/libs/PGE_File_Formats/smbx64_cnf_filedata.h \
    ../../LunaDll/libs/PGE_File_Formats/smbx64_macro.h \
    ../../LunaDll/libs/PGE_File_Formats/wld_filedata.h

INCLUDEPATH += ../../LunaDll/libs/
DEFINES += PGE_FILES_QT

FORMS    += mainlauncherwindow.ui

RESOURCES += \
    resource.qrc


RC_FILE = "SMBXLauncher.rc"
