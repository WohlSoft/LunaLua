#ifndef GLTextureStore_hhhh
#define GLTextureStore_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "GLDraw.h"

class GLTextureStore {
public:
    std::unordered_map<HDC, const GLDraw::Texture*> mSmbxHdcMap;
    GLuint mLastTexName;

    GLTextureStore();
    void ClearSMBXTextures();
    const GLDraw::Texture* TextureFromSMBXBitmap(HDC hdc);
private:
    bool GetFromHDC(HDC hDC);
};

// Instance
extern GLTextureStore g_GLTextureStore;

#endif
