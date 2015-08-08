"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\dumpbin.exe" /exports SDL2_mixer_ext.dll > SDL2_mixer_ext_exports.txt
pause
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\lib" /def:SDL2_mixer_ext.def /OUT:SDL2_mixer_ext.lib
