@echo off
@REM Be sure to use this Batch-File with the "Visual Studio 2015 Developer Console".

mkdir build-vs2015
cd build-vs2015
cmake ../. -G "Visual Studio 14 2015"
pause