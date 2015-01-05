#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T08:43:55
#
#-------------------------------------------------

QT       += core gui webkit webkitwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LunadllNewLauncher
TEMPLATE = lib

CONFIG += c++11
CONFIG += sharedlib

QMAKE_CXXFLAGS = -std=c++11 
LIBS += -pthread


DEFINES += LunadllNewLauncher


SOURCES += main.cpp\
        mainwindow.cpp \
    asyncdebugger.cpp \
    logger.cpp \
    launcherwindowhtml.cpp \
    settingsobject.cpp

HEADERS  += mainwindow.h \
    setting.h \
    asyncdebugger.h \
    logger.h \
    launcherwindowhtml.h \
    settingsobject.h

FORMS    += mainwindow.ui \
    asyncdebugger.ui \
    logger.ui \
    launcherwindowhtml.ui

RESOURCES += \
    res.qrc
