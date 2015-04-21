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
    std::vector<const GLDraw::Texture> mTexList;
    std::unordered_map<HDC, unsigned int> mHDCMap;
    GLuint mLastTexName;

    GLTextureStore();
    void ClearTextures();
    const GLDraw::Texture* TextureFromBitmapHDC(HDC hdc);
private:
    bool GetFromHDC(HDC hDC);
};

// Instance
extern GLTextureStore g_GLTextureStore;

#endif
