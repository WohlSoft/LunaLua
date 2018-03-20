
#include <cstdlib>
#include <memory>

#include "../Globals.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/FrameCapture.h"

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

    FFI_EXPORT(void) LunaLuaKillPlayer(short playerIndex)
    {
        native_killPlayer(&playerIndex);
    }

    FFI_EXPORT(void) LunaLuaHarmPlayer(short playerIndex)
    {
        native_harmPlayer(&playerIndex);
    }
}
