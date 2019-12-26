@echo off

SET SEVENZIP=C:\Program Files\7-Zip
IF NOT EXIST "%SEVENZIP%\7z.exe" SET SEVENZIP=C:\Program Files (x86)\7-Zip
IF NOT EXIST "%SEVENZIP%\7z.exe" SET SEVENZIP=C:\Programs\7-Zip

set CurDir=%CD%

set DeployDir=%CurDir%\build-smbx-launcher\Deploy

echo Packing SMBX Launcher...
"%SEVENZIP%\7z" a -tzip "SMBX-Launcher" %DeployDir%\*.* %DeployDir%\imageformats %DeployDir%\launcher %DeployDir%\platforms %DeployDir%\resources %DeployDir%\styles %DeployDir%\translations > NUL

echo.
echo "Everything is completed!"
echo.

if not "%1"=="nopause" pause
