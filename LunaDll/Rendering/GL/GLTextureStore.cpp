#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "../RenderUtils.h"
#include "GLTextureStore.h"
#include "../LunaImage.h"

// Instance
GLTextureStore g_GLTextureStore;

// Constructor
GLTextureStore::GLTextureStore() {
}

void GLTextureStore::Reset() {
    ClearLunaImageTextures();
}

void GLTextureStore::ClearLunaImageTextures()
{
    for (const auto i : mLunaImageTexMap) {
        delete i.second;
    }
    mLunaImageTexMap.clear();
}

void GLTextureStore::ClearLunaImageTexture(uint64_t uid)
{
    auto it = mLunaImageTexMap.find(uid);
    if (it != mLunaImageTexMap.end()) {
        delete it->second;
        mLunaImageTexMap.erase(it);
    }
}

const GLSprite* GLTextureStore::SpriteFromLunaImage(const std::shared_ptr<LunaImage>& img)
{
    img->lock();

    uint64_t uid = img->getUID();

    // Get associated texture from cache if possible
    auto it = mLunaImageTexMap.find(uid);
    if (it != mLunaImageTexMap.end()) {
        img->unlock();
        return it->second;
    }

    // Try to allocate the GLSprite
    GLSprite* sprite;
    sprite = GLSprite::Create(img->getDataPtr(), GL_BGRA, img->getW(), img->getH());
    img->unlock();

    // Handle failure
    if (sprite == NULL || !sprite->IsValid())
    {
        if (sprite) delete sprite;
        return NULL;
    }

    // Cache new texture
    mLunaImageTexMap[uid] = sprite;

    return sprite;
}
