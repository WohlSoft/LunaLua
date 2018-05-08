#-------------------------------------------------
#
# Project created by QtCreator 2015-05-30T07:04:24
#
#-------------------------------------------------

QT       += core gui widgets webenginewidgets

CONFIG += c++11

DEFINES += PGE_FILES_QT
DEFINES -= QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT QT_NO_INFO_OUTPUT

TARGET = SMBXLauncher
TEMPLATE = app

LIBS += User32.lib

SOURCES += main.cpp\
        mainlauncherwindow.cpp \
    ExposedObjects/smbxconfig.cpp \
    ExposedObjects/autostartconfig.cpp \
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
    ExposedObjects/controlconfig.cpp \
    launcherconfiguration.cpp \
    ../../LunaLoader/LunaLoaderPatch.cpp \
    Utils/Network/networkutils.cpp \
    Utils/Network/qreplytimeout.cpp \
    PGE_File_Formats/file_rw_lvl_38a.cpp \
    PGE_File_Formats/ConvertUTF.c \
    Utils/filesysutils.cpp \
    Utils/Json/extendedqjsonreader.cpp \
    Utils/Json/qjsonparseexception.cpp \
    Utils/Json/qjsonfileopenexception.cpp \
    Utils/Json/qjsonvalidationexception.cpp \
    Utils/Network/qnetworkreplytimeoutexception.cpp \
    Utils/Network/qnetworkreplyexception.cpp \
    Utils/Common/qurlinvalidexception.cpp \
    Utils/networkjsonutils.cpp \
    Utils/Json/qjsonurlvalidationexception.cpp \
    launchercustomwebpage.cpp \
    hybridlogger.cpp \
    devtoolsdialog.cpp

HEADERS  += mainlauncherwindow.h \
    ExposedObjects/smbxconfig.h \
    ExposedObjects/autostartconfig.h \
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
    ExposedObjects/controlconfig.h \
    launcherconfiguration.h \
    Utils/Network/networkutils.h \
    Utils/Network/qreplytimeout.h \
    PGE_File_Formats/charsetconvert.h \
    PGE_File_Formats/ConvertUTF.h \
    PGE_File_Formats/CSVReader.h \
    PGE_File_Formats/CSVReaderPGE.h \
    PGE_File_Formats/CSVUtils.h \
    Utils/filesysutils.h \
    Utils/Json/extendedqjsonreader.h \
    Utils/Json/qjsonparseexception.h \
    Utils/Json/qjsonfileopenexception.h \
    Utils/Json/qjsonvalidationexception.h \
    Utils/Json/qjsonextractor.h \
    Utils/Network/qnetworkreplytimeoutexception.h \
    Utils/Network/qnetworkreplyexception.h \
    Utils/Common/qurlinvalidexception.h \
    Utils/networkjsonutils.h \
    Utils/Json/qjsonurlvalidationexception.h \
    launchercustomwebpage.h \
    hybridlogger.h \
    devtoolsdialog.h

FORMS    += mainlauncherwindow.ui \
    devtoolsdialog.ui

RESOURCES += \
    resource.qrc


RC_FILE = "SMBXLauncher.rc"

DISTFILES +=
