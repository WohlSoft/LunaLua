@echo off

set CurDir=%CD%
set QT_DIR=C:\Qt\5.12.2\mingw73_32
set MINGW_DIR=C:\Qt\Tools\mingw730_32\

set BUILD_DIR=%CurDir%\build-LunaExec
set SOURCE_DIR=%CurDir%\LunaLoader

PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%PATH%

if not exist "%BUILD_DIR%\*" md "%BUILD_DIR%"

cd "%BUILD_DIR%"
"%QT_DIR%\bin\qmake.exe" CONFIG-=debug CONFIG+=release "%SOURCE_DIR%\LunaLoader.pro"
"%MINGW_DIR%\bin\mingw32-make.exe" -j 2

echo === Built! ===
cd "%CurDir%"
