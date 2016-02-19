
#include <cstdlib>
#include <memory>

#include "../Globals.h"

#define FFI_EXPORT(sig) __declspec(dllexport) sig __cdecl

extern "C" {
    FFI_EXPORT(void*) LunaLuaAlloc(size_t size) {
        return malloc(size);
    }

    FFI_EXPORT(void) LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count) {
        auto obj = std::make_shared<GLEngineCmd_Draw2DArray>();
        obj->mType = GL_TRIANGLES;
        obj->mVert = vert;
        obj->mTex = tex;
        obj->mCount = count;
        gLunaRender.GLCmd(obj);
    }

    FFI_EXPORT(uint32_t*) LunaLuaGetImageResourceBits(uintptr_t bmpBoxIntPtr) {
        return reinterpret_cast<uint32_t*>(reinterpret_cast<BMPBox*>(bmpBoxIntPtr)->getBits());
    }

	FFI_EXPORT(void) LunaLuaSetMovieHitCallback(uintptr_t bmpBoxIntPtr,void(*fn)(int)) {
		reinterpret_cast<BMPBox*>(bmpBoxIntPtr)->setCallback(fn);
	}
}