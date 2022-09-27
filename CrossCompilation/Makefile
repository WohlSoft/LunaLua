.PHONY: all clean

.EXPORT_ALL_VARIABLES:

MSVC_BASE = $(shell pwd)/msvc
WINSDK_BASE = $(shell pwd)/winsdk
WINSDK_VER = v7.1A
HOST_ARCH = x86
MSVC_VER = 1900

all: LunaDLL.dll

LunaDLL.dll: build/build.ninja
	cmake --build build

compile_commands.json: build/build.ninja
	cp build/compile_commands.json ..

build/build.ninja:
	mkdir -p build
	cd build && cmake ../.. -G Ninja \
		-DCMAKE_TOOLCHAIN_FILE=$(shell pwd)/clang-cl-msvc.cmake \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1

clean:
	rm -rf build
