#-------------------------------------------------
#
# Project created by QtCreator 2015-05-30T07:04:24
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SMBXLauncher
TEMPLATE = app


SOURCES += main.cpp\
        mainlauncherwindow.cpp \
    smbxconfig.cpp \
    autostartconfig.cpp

HEADERS  += mainlauncherwindow.h \
    smbxconfig.h \
    autostartconfig.h

FORMS    += mainlauncherwindow.ui

RESOURCES += \
    resource.qrc


RC_FILE = "SMBXLauncher.rc"
