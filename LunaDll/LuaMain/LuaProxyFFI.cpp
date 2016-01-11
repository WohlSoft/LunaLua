
#include <cstdlib>
#include <memory>

#include "../Globals.h"


extern "C" {
    __declspec(dllexport) float* __cdecl LunaLuaGlAllocCoords(size_t size) {
        return (float*)malloc(size  * sizeof(float));
    }

    __declspec(dllexport) void __cdecl LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count) {
        auto obj = std::make_shared<GLEngineCmd_Draw2DArray>();
        obj->mType = GL_TRIANGLES;
        obj->mVert = vert;
        obj->mTex = tex;
        obj->mCount = count;
        gLunaRender.GLCmd(obj);
    }
}