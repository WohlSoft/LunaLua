.PHONY: all clean compile-lunalua compile-launcher compile-launcher-windows compile-lunaloader compile-commands lunaloader-compile-commands

LAUNCHER_COMPILE_COMMANDS = windows
LUNALOADER_COMPILE_COMMANDS = lunaloader

CURRENT_FOLDER = $(shell pwd)
export MSVC_BASE = $(CURRENT_FOLDER)/msvc
export WINSDK_BASE = $(CURRENT_FOLDER)/winsdk
export HOST_ARCH = x86

all: compile-commands out/bin/LunaDll.dll out/lib/LunaDll.lib out/pdb/LunaDll.pdb out/bin/SMBXLauncher out/bin/SMBXLauncher.exe out/bin/LunaLoader.exe out/bin/LunaLoader-exec.exe

out/bin/LunaLoader-exec.exe: out/bin compile-lunaloader
	cp build/lunaloader/out/bin/LunaLoader-exec.exe out/bin/LunaLoader-exec.exe

out/bin/LunaLoader.exe: out/bin compile-lunaloader
	cp build/lunaloader/out/bin/LunaLoader.exe out/bin/LunaLoader.exe

out/bin/SMBXLauncher.exe: out/bin compile-launcher-windows
	cp build/launcher-windows/out/bin/SMBXLauncher.exe out/bin/SMBXLauncher.exe

out/bin/SMBXLauncher: out/bin compile-launcher
	cp build/launcher/SMBXLauncher out/bin/SMBXLauncher

out/bin/LunaDll.dll: out/bin compile-lunalua
	cp build/lunalua/out/bin/LunaDll.dll out/bin/LunaDll.dll

out/lib/LunaDll.lib: out/lib compile-lunalua
	cp build/lunalua/out/lib/LunaDll.lib out/lib/LunaDll.lib

out/pdb/LunaDll.pdb: out/pdb compile-lunalua
	cp build/lunalua/out/pdb/LunaDll.pdb out/pdb/LunaDll.pdb

lunaloader-compile-commands: build/lunaloader
ifeq ($(LUNALOADER_COMPILE_COMMANDS),exec)
	cd build/lunaloader && jq 'map(select(.command | contains("-DLUNALOADER_EXEC")))' compile_commands.json > compile_commands_noduplicate.json
else
	cd build/lunaloader && jq 'map(select(.command | contains("-DLUNALOADER_EXEC") | not))' compile_commands.json > compile_commands_noduplicate.json
endif

ifeq ($(LAUNCHER_COMPILE_COMMANDS),linux)

compile-commands: build/lunalua build/launcher lunaloader-compile-commands
	jq -s add build/lunalua/compile_commands.json build/launcher/compile_commands_nolunalua.json build/lunaloader/compile_commands_noduplicate.json > ../compile_commands.json

else

compile-commands: build/lunalua build/launcher-windows lunaloader-compile-commands
	jq -s add build/lunalua/compile_commands.json build/launcher-windows/compile_commands_nolunalua.json build/lunaloader/compile_commands_noduplicate.json > ../compile_commands.json

endif

compile-lunaloader: build/lunaloader
	cmake --build build/lunaloader

build/lunaloader: build
	mkdir -p build/lunaloader
	cd build/lunaloader && env MSVC_VER=19.00 WINSDK_VER=v7.1A cmake ../../../LunaLoader -G Ninja \
		-DCMAKE_TOOLCHAIN_FILE=$(CURRENT_FOLDER)/clang-cl-msvc.cmake \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1

compile-launcher-windows: build/launcher-windows
	cmake --build build/launcher-windows

build/launcher-windows: build
	mkdir -p build/launcher-windows
	cd build/launcher-windows && env MSVC_VER=19.16 WINSDK_VER=10.0.19041.0 cmake ../../../LunadllNewLauncher/SMBXLauncher -G Ninja \
		-DCMAKE_TOOLCHAIN_FILE=$(CURRENT_FOLDER)/clang-cl-msvc.cmake \
		-DCMAKE_PREFIX_PATH=$(CURRENT_FOLDER)/qt-win32/lib/cmake \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1
	cd build/launcher-windows && jq 'map(select(.file | startswith("../../LunaDll/") | not))' compile_commands.json > compile_commands_nolunalua.json

compile-launcher: build/launcher
	cd build/launcher && make

build/launcher: build
	mkdir -p build/launcher
	cd build/launcher && qmake -makefile CONFIG+=linux CONFIG+=release CONFIG-=debug ../../../LunadllNewLauncher/SMBXLauncher/SMBXLauncher.pro
	cd build/launcher && compiledb -n make
	cd build/launcher && jq 'map(select(.file | startswith("../../LunaDll/") | not))' compile_commands.json > compile_commands_nolunalua.json

compile-lunalua: build/lunalua
	cmake --build build/lunalua

build/lunalua: build
	mkdir -p build/lunalua
	cd build/lunalua && env MSVC_VER=19.00 WINSDK_VER=v7.1A cmake ../../.. -G Ninja \
		-DCMAKE_TOOLCHAIN_FILE=$(CURRENT_FOLDER)/clang-cl-msvc.cmake \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1

build:
	mkdir -p build

out:
	mkdir -p out

out/bin: out
	mkdir -p out/bin

out/lib: out
	mkdir -p out/lib

out/pdb: out
	mkdir -p out/pdb

clean:
	rm -rf build
	rm -rf out

	rm -f ../compile_commands.json
