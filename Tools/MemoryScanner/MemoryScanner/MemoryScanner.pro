#-------------------------------------------------
#
# Project created by QtCreator 2015-05-09T08:19:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MemoryScanner
TEMPLATE = app

CONFIG += C++11

SOURCES += main.cpp\
        scannerwindow.cpp \
    addnewentrywidget.cpp \
    addnewsimplestructwidget.cpp \
    parsetools.cpp

HEADERS  += scannerwindow.h \
    win32Helper.h \
    addnewentrywidget.h \
    addnewsimplestructwidget.h \
    parsetools.h \
    qtreewidgetitemsmbxaddress.h

FORMS    += scannerwindow.ui \
    addnewentrywidget.ui \
    addnewsimplestructwidget.ui
