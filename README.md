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
Visual Studio 2015 (msvc140 compiler) or Clang 14

Building LunaLua on Linux
------

The `CrossCompilation` folder is a modified version of https://github.com/Nemirtingas/clang-msvc-sdk.

### Setting up headers and libs
* Install Visual C++ build tools 2015 and Windows SDK v7.1A (either on Windows or on Wine)
* Copy all files and folders from `Program Files (x86)/Microsoft Visual Studio 14.0/VC/include/` to `CrossCompilation/msvc/include/`
* Copy all files and folders from `Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt/` to `CrossCompilation/winsdk/Include/v7.1A/ucrt/`
* Copy all files and folders from `Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Include/` to `CrossCompilation/winsdk/Include/v7.1A/um/`
* Copy all files from `Program Files (x86)/Microsoft Visual Studio 14.0/VC/lib/` to `CrossCompilation/msvc/lib/x86/`
* Copy all files from `Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/x86/` to `CrossCompilation/winsdk/Lib/v7.1A/ucrt/x86/`
* Copy all files from `Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/` to `CrossCompilation/winsdk/Lib/v7.1A/um/x86/`

### Compiling the project
Just run `make` in the `CrossCompilation` folder

NB: If you're using clangd and only want to generate compile commands, run `make compile_commands.json` in the `CrossCompilation` folder.

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
