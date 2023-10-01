@echo off

md __temp

set MIXERX_URL=https://wohlsoft.ru/docs/_laboratory/_Builds/win32/SDL-Mixer-X-MSVC15-Release-Win32.7z
set SEVENZIP="%ProgramFiles%\7-Zip\7z.exe"

echo - Downloading MixerX...
powershell -Command "(New-Object Net.WebClient).DownloadFile('%MIXERX_URL%', '__temp\mixerx.zip')"

echo - Extracting MixerX...
%SEVENZIP% x -o__temp\mixerx __temp\mixerx.zip SDL-Mixer-X/* > NUL

echo - Copying MixerX...
xcopy /E /C /Y /I __temp\mixerx\SDL-Mixer-X\* sdl > NUL

echo - Cleaning up MixerX...
rd /s /q __temp
rd /s /q sdl\bin\Release
rd /s /q sdl\lib\Release
rd /s /q sdl\lib\pkgconfig

echo - DONE!
