TEMPLATE = app
TARGET = LunaLoader
CONFIG -= qt
CONFIG -= console
CONFIG += windows

RC_FILE += $$PWD/LunaLoader.rc

DISTFILES += \
    LunaLoader.rc

HEADERS += \
    LunaLoaderPatch.h \
    resource.h

SOURCES += \
    LunaLoaderPatch.cpp \
    LunaLoader.cpp
