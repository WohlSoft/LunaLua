"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\dumpbin.exe" /nologo /exports SDL2_mixer_ext.dll > SDL2_mixer_ext_exports.txt
pause

"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\lib" /MACHINE:X86 /def:SDL2_mixer_ext.def /OUT:SDL2_mixer_ext.lib

pause
