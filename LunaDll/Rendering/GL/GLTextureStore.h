#ifndef GLTextureStore_hhhh
#define GLTextureStore_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include "GLDraw.h"
#include "GLSprite.h"
class BMPBox;
class LunaImageData;

class GLTextureStore {
public:
    std::unordered_map<HDC, const GLSprite*> mSmbxTexMap;
    std::unordered_map<const BMPBox*, const GLDraw::Texture*> mLunaTexMap;
    std::unordered_map<uint64_t, const GLSprite*> mLunaImageTexMap;
    
	void Reset();

    GLTextureStore();
    void ClearSMBXSprites();
    const GLSprite* SpriteFromSMBXBitmap(HDC hdc);

	void ClearLunaTextures();
    void ClearLunaTexture(const BMPBox& bmp);
    const GLDraw::Texture* TextureFromLunaBitmap(const BMPBox& bmp);

    void ClearLunaImageTextures();
    void ClearLunaImageTexture(uint64_t uid);
    const GLSprite* SpriteFromLunaImage(const std::shared_ptr<LunaImageData>& img);
};

// Instance
extern GLTextureStore g_GLTextureStore;

#endif
