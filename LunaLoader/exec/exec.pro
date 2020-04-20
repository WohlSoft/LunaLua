CONFIG += console
CONFIG -= windows
TARGET = LunaLoader-exec

DEFINES += LUNALOADER_EXEC
LIBS += -lshlwapi
include($$PWD/../LunaLoader.pri)
