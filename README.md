LunaLua
=======
[![Build status](https://ci.appveyor.com/api/projects/status/72ttgr33ggar3x21?svg=true)](https://ci.appveyor.com/project/Wohlstand/lunadll)

LunaLua (a.k.a. LunaDLL with Lua) -  is a free extension for SMBX game engine.
This version is based on LunaDLL ver. 8 with Lua Support. After the release of 0.7.3.1, LunaLua is being developed as a core part of the [SMBX2 project](https://codehaus.moe).

Quick Overview
-------

**Features:**
* Increasing of total performance
* Fixes of some bugs
 * npc-*.txt with decimal numbers will no more crash SMBX with "runtime error 13"
 * fixes a random crash caused by contacting of the mushrooms with a lava blocks
* OpenGL render support
* SMBX's audio engine has been replaced with the better and more flexible `SDL2_mixer` (Later its fork called `SDL2 Mixer X` or just `MixerX`) library which also gives to SMBX the internal support of a [lots of additional music formats](https://wohlsoft.ru/pgewiki/SDL_Mixer_X#Music_Formats)
* Added Moondust's [sounds.ini](https://wohlsoft.ru/pgewiki/Sounds.ini_(Episode)) and [music.ini](https://wohlsoft.ru/pgewiki/Music.ini_(Episode)) support which gives ability to have custom musics (include world map musics!) and custom sound effects per episode.
* And more other stuff...

**Download links**
* [Download SMBX2 game](https://codehaus.moe/) - The mainstream game based on the LunaLua.
* [SMBX2 Documentation](https://docs.codehaus.moe/) - The most current documentation for the LunaLua API.

**Old downloads**
* [Download old releases of LunaLua](https://wohlsoft.ru/projects/LunaLua/) - Legacy standalone LunaLua releases.
* [Legacy LunaLua Documenation](https://wohlsoft.ru/pgewiki/Category:LunaLua_API) - The documentation mostly for old versions of LunaLua.

Requirements to build LunaLua yourself
------
Visual Studio 2015 (msvc140 compiler) or Clang

Building LunaLua on Linux
------

Disclaimer: The `CrossCompilation` folder is a modified version of [clang-msvc-sdk](https://github.com/Nemirtingas/clang-msvc-sdk/tree/75fa0a0ef5c90e7667053926de4fdf7b97436f80) by Nemirtingas, originally licensed under [MIT](https://github.com/Nemirtingas/clang-msvc-sdk/blob/75fa0a0ef5c90e7667053926de4fdf7b97436f80/LICENSE).

### Prerequisites

#### Installing dependencies
Compiling LunaLua on Linux requires installing the following dependencies:
- [GNU Make](https://www.gnu.org/software/make/)
- [CMake](https://cmake.org/)
- [jq](https://jqlang.org/)
- [Clang](https://clang.llvm.org/) >= 21.1.0
- [Qt](https://www.qt.io/) 5 (+ QtWebEngine)
- [Another Qt installer](https://github.com/miurahr/aqtinstall) (only needed for the prerequisites)
- [Wine](https://www.winehq.org/)
- [Winetricks](https://github.com/Winetricks/winetricks) (only needed for the prerequisites)
- [compiledb](https://github.com/nickdiego/compiledb)

##### Instructions for Arch-based distros
```bash
$ sudo pacman -S make cmake jq clang qt5 wine winetricks
$ yay -S compiledb python-aqtinstall qt5-webengine
```

#### Setting up Windows headers and libs
* Create a clean Wine prefix
  ```bash
  $ export WINEPREFIX="/absolute/path/to/prefix"
  $ wineboot
  ```
* If Wine Mono was installed as a system package:
  * Run the Wine uninstaller tool
	```bash
	$ wine uninstaller
	```
  * Select Wine Mono Windows Support and uninstall it
* Install .NET Framework 4.8
  ```bash
  $ winetricks -q dotnet48
  ```
* Download Visual Studio Build Tools 2017 from [here](https://aka.ms/vs/15/release/vs_buildtools.exe) and run it
  ```bash
  $ wine vs_BuildTools.exe
  ```
* In the installer window, keep already checked options and check Visual C++ 2015 and the Windows XP toolsets, then click the install button
* Setup MSVC 2015 files
  * Copy `include` from `$WINEPREFIX/drive_c/Program Files (x86)/Microsoft Visual Studio 14.0/VC/include/` to `CrossCompilation/msvc/19.00/`
  * Copy all files (not folders)  from `$WINEPREFIX/drive_c/Program Files (x86)/Microsoft Visual Studio 14.0/VC/lib/` to `CrossCompilation/msvc/19.00/lib/x86/`
* Setup MSVC 2017 files
  * Copy `include` and `lib` from `$WINEPREFIX/drive_c/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/VC/Tools/MSVC/14.16.27023/` to `CrossCompilation/msvc/19.16/`
* Setup Windows SDK v7.1A files
  * Copy `ucrt` from `$WINEPREFIX/drive_c/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/` to `CrossCompilation/winsdk/v7.1A/Include/`
  * Copy all files and folders from `$WINEPREFIX/drive_c/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Include` to `CrossCompilation/winsdk/v7.1A/Include/um/` 
  * Copy `x86` from `$WINEPREFIX/drive_c/Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/` to `CrossCompilation/winsdk/v7.1A/Lib/ucrt/`
  * Copy all files (not folders) from `$WINEPREFIX/drive_c/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/` to `CrossCompilation/winsdk/v7.1A/Lib/um/x86/`
* Setup Windows SDK 10.0.19041.0 files
  * Copy all folders from `$WINEPREFIX/drive_c/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/` to `CrossCompilation/winsdk/10.0.19041.0/Include/`
  * Copy `x86` from `$WINEPREFIX/drive_c/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/` to `CrossCompilation/winsdk/10.0.19041.0/Lib/ucrt/`
  * Copy `x86` from `$WINEPREFIX/drive_c/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/` to `CrossCompilation/winsdk/10.0.19041.0/Lib/um/`

#### Setting up Qt libs and headers

* Download the Windows version of Qt 5.12.12
  ```bash
  $ aqt install-qt windows desktop 5.12.12 win32_msvc2017 -m all -O /tmp
  ```
* Copy all folders from `/tmp/5.12.12/msvc2017/` to `CrossCompilation/qt-win32`
* Make Qt5 tools executable
  ```bash
  $ cd /path/to/repo/CrossCompilation/qt5-win32/bin/
  $ chmod +x *.exe
  ```
### Compiling the project
Just run `make` in the `CrossCompilation` folder. Compilation results will be found in `CrossCompilation/out`.

Running this command will also generate compile commands for Clangd. They can also be generated without compiling the project by running `make compile-commands`.

NB: The generated compile commands assume that the Windows version of the launcher and `LunaLoader.exe`  are being built. Add `LAUNCHER_COMPILE_COMMANDS=linux` as an extra argument to `make` if you want the compile commands to assume that the Linux version of the launcher is being build and `LUNALOADER_COMPILE_COMMANDS=exec` if you want them to assume that `LunaLoader-exec.exe` is being built.


Note for Qt Creator IDE
------
If you wish you edit code via Qt Creator, you still must have Visual Studio because MSVC compiler is required.
**How to setup building from Qt creator**
* Open project and configure it
* Go to the "Projects" tab
* Disable shadow build for both Debug and Release
* remove ALL items from "Building" and "Cleaning" sections
* Add into "Building" a special item with:
 ** Command "build.bat"
 ** Argument "Debug" for debug build and "Release" argument for release build
* Add into "Clearing" a special item with "clear.bat" command
* Try to build project. If you have "MSBuild" in another path, please modify build.bat to take working build!
