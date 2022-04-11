
if "%1"=="" SET CONFIG=Release
if NOT "%1"=="" SET CONFIG=%1

"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe" LunaDll.sln /m /nologo /p:Configuration="%CONFIG%" /p:Platfprm="Win32"
