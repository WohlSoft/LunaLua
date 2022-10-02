.PHONY: all clean

CURRENT_FOLDER = $(shell pwd)
export MSVC_BASE = $(CURRENT_FOLDER)/msvc
export WINSDK_BASE = $(CURRENT_FOLDER)/winsdk
export WINSDK_VER = v7.1A
export HOST_ARCH = x86
export MSVC_VER = 1900

all: compile_commands.json LunaDLL.dll

LunaDLL.dll: build/build.ninja
	cmake --build build

compile_commands.json: build/build.ninja
	cp build/compile_commands.json ..

build/build.ninja:
	mkdir -p build
	cd build && cmake ../.. -G Ninja \
		-DCMAKE_TOOLCHAIN_FILE=$(CURRENT_FOLDER)/clang-cl-msvc.cmake \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1

clean:
	rm -rf build
	rm ../compile_commands.json
