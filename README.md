LunaLua
=======
[![Build status](https://ci.appveyor.com/api/projects/status/72ttgr33ggar3x21?svg=true)](https://ci.appveyor.com/project/Wohlstand/lunadll)

LunaLua - LunaDLL with Lua is a free extension for SMBX game engine
This version is based on LunaDLL ver. 8 with Lua Support.

Quick Overview
-------

**Features:**
* Increasing of total performance
* Fixes of some bugs
 * npc-*.txt with decimal numbers will no more crash SMBX with "runtime error 13"
 * fixes a random crash caused by contacting of the mushrooms with a lava blocks
* OpenGL render support
* SMBX's audio engine has been replaced with the better and more flexible SDL2_mixer library which also gives to SMBX the internal support of a [lots of additional music formats](http://wohlsoft.ru/pgewiki/SDL2_mixer#Music_Formats)
* Added PGE's [sounds.ini](http://wohlsoft.ru/pgewiki/Game_Configuration_Pack_References#sounds.ini) and [music.ini](http://wohlsoft.ru/pgewiki/Game_Configuration_Pack_References#music.ini) support which gives ability to have custom musics (include world map musics!) and custom sound effects per episode.

**Download links**
* [Download LunaLua](http://wohlsoft.ru/LunaLua/)
* [LunaLua Documenation](http://wohlsoft.ru/pgewiki/Category:LunaLua_API)

Requirements to build LunaDLL yourself
------
Visual Studio 2015 (msvc140 compiler)


Note for Qt Creator IDE
------
If you wish you edit code via Qt Creator, you still must have Visual Studio because MSVC compiler is required.
**How to setup building from Qt creator**
* Open project and configure it
* Go to the "Projects" tab
* Disable shadow build for both Debug and Release
* remove ALL items from "Building" and "Cenaring" sections
* Add into "Building" a special item with:
 ** Command "build.bat"
 ** Argument "Debug" for debug build and "Release" argument for release build
* Add into "Clearing" a special item with "clear.bat" command
* Try to build project. If you have "MSBuild" in another path, please modify build.bat to take working build!
