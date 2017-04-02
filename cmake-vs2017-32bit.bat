@echo off
@REM Be sure to use this Batch-File with the "Visual Studio 2017 Developer Console".

@REM FIND GIT:
for /f "delims=" %%i in ('where git') do set git_path=%%i
IF "%git_path%" == "" (
   set /p git_path="Enter Git Path: "
) ELSE (
   echo found git executable: %git_path%
)

mkdir build-vs2017
cd build-vs2017
cmake ../. -G "Visual Studio 15 2017" -T "v141_xp" "-DGIT_EXECUTABLE=%git_path%"
pause