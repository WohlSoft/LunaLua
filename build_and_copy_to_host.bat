call build.bat

set DeployDir=E:\vitaly\_git_repos\SMBX-Revived\data

set CurDir=%CD%

echo Copying LunaLUA binaries...
copy "%CurDir%\Release\LunaDll.dll" "%DeployDir%"
copy "%CurDir%\Release\LunaDll.pdb" "%DeployDir%"
copy "%CurDir%\Release\LunaLoader.exe" "%DeployDir%"

pause
