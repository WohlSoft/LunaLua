@echo off

mkdir build-vs2015
cd build-vs2015
cmake ../. -G "Visual Studio 14 2015"
pause