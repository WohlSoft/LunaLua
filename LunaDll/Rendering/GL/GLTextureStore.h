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
class LunaImage;

class GLTextureStore {
public:
    std::unordered_map<uint64_t, const GLSprite*> mLunaImageTexMap;
    
	void Reset();

    GLTextureStore();

    void ClearLunaImageTextures();
    void ClearLunaImageTexture(uint64_t uid);
    const GLSprite* SpriteFromLunaImage(const std::shared_ptr<LunaImage>& img);
};

// Instance
extern GLTextureStore g_GLTextureStore;

#endif
