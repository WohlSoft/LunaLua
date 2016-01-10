@echo off

SET SEVENZIP=C:\Program Files\7-Zip
IF NOT EXIST "%SEVENZIP%\7z.exe" SET SEVENZIP=C:\Program Files (x86)\7-Zip
IF NOT EXIST "%SEVENZIP%\7z.exe" SET SEVENZIP=C:\Programs\7-Zip

set CurDir=%CD%

set DeployDir=%CurDir%\LunaLUA-bin
set DeployDirL=%CurDir%\LunaLUA-loader-bin

if exist "%DeployDir%\*" del /Q /F /S "%DeployDir%\*" > NUL
if not exist "%DeployDir%\*" md "%DeployDir%"  > NUL
if exist "%DeployDirL%\*" del /Q /F /S "%DeployDirL%\*"  > NUL
if not exist "%DeployDirL%\*" md "%DeployDirL%"  > NUL

echo Copying Lua scripts library...
xcopy /Y /E /I %CurDir%\LuaScriptsLib\*.* "%DeployDir%\LuaScriptsLib" > NUL

echo Copying LunaLUA binaries...
copy "%CurDir%\Release\LunaDll.dll" "%DeployDir%" > NUL
copy "%CurDir%\Release\LunaDll.pdb" "%DeployDir%" > NUL
copy "%CurDir%\Release\LunaLoader.exe" "%DeployDirL%"  > NUL

echo Installing SDL2 and SDL2_mixer_ext...
copy "%CurDir%\LunaDll\libs\sdl\bin\*.dll" "%DeployDir%"  > NUL

rem Free image no needed if static FreeImageLite was built
rem echo Installing FreeImage...
rem copy "%CurDir%\LunaDll\libs\freeimage\*.dll" "%DeployDir%"  > NUL

echo Installing Glew32...
copy "%CurDir%\LunaDll\libs\glew\bin\Release\Win32\*.dll" "%DeployDir%"  > NUL

copy "%CurDir%\LunaDll\Changelog.txt" "%DeployDir%" > NUL

echo Packing LunaLUA...
"%SEVENZIP%\7z" a -tzip "LunaLUA-bin.zip" %DeployDir%\*.* "%DeployDir%\LuaScriptsLib"  > NUL
echo Packing LunaLoader...
"%SEVENZIP%\7z" a -tzip "LunaLoader-bin.zip" %DeployDirL%\*.* > NUL

echo.
echo "Everything is completed!"
echo.

if not "%1"=="nopause" pause
