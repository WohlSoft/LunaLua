#ifndef GLTextureStore_hhhh
#define GLTextureStore_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "GLDraw.h"
#include "BMPBox.h"

class GLTextureStore {
public:
    std::unordered_map<HDC, const GLDraw::Texture*> mSmbxTexMap;
    std::unordered_map<const BMPBox*, const GLDraw::Texture*> mLunaTexMap;
    
    GLTextureStore();
    void ClearSMBXTextures();
    const GLDraw::Texture* TextureFromSMBXBitmap(HDC hdc);

    void ClearLunaTexture(const BMPBox& bmp);
    const GLDraw::Texture* TextureFromLunaBitmap(const BMPBox& bmp);
};

// Instance
extern GLTextureStore g_GLTextureStore;

#endif
