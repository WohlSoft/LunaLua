@echo off
@REM Be sure to use this Batch-File with the "Visual Studio 2017 Developer Console".

mkdir build-vs2017
cd build-vs2017
cmake ../. -G "Visual Studio 15 2017" -T "v141_xp"
pause