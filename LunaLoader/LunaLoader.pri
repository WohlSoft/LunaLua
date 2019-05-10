TEMPLATE = app
CONFIG -= qt
CONFIG += static

RC_FILE += $$PWD/LunaLoader.rc

LIBS += -static-libstdc++

DESTDIR = ../bin

DISTFILES += \
    $$PWD/LunaLoader.rc

HEADERS += \
    $$PWD/LunaLoaderPatch.h \
    $$PWD/resource.h

SOURCES += \
    $$PWD/LunaLoaderPatch.cpp \
    $$PWD/LunaLoader.cpp
