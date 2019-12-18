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
    ExposedObjects/controlconfig.cpp \
    launcherconfiguration.cpp \
    Utils/Network/networkutils.cpp \
    Utils/Network/qreplytimeout.cpp \
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
    ExposedObjects/controlconfig.h \
    launcherconfiguration.h \
    Utils/Network/networkutils.h \
    Utils/Network/qreplytimeout.h \
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

# LunaLoader
win32: SOURCES += ../../LunaLoader/LunaLoaderPatch.cpp

# PGE File Library
INCLUDEPATH += $$PWD/../../LunaDll/libs
SOURCES += $$files($$PWD/../../LunaDll/libs/PGE_File_Formats/*.cpp)
SOURCES += $$files($$PWD/../../LunaDll/libs/PGE_File_Formats/*.c)
HEADERS += $$files($$PWD/../../LunaDll/libs/PGE_File_Formats/*.h)

FORMS    += mainlauncherwindow.ui \
    devtoolsdialog.ui

RESOURCES += \
    resource.qrc


RC_FILE = "SMBXLauncher.rc"

DISTFILES +=
