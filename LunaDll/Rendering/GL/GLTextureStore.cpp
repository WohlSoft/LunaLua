#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "../RenderUtils.h"
#include "GLTextureStore.h"
#include "../LunaImage.h"

// Instance
GLTextureStore g_GLTextureStore;

std::atomic<uint64_t> GLTextureStore::totalMem = 0;

// Constructor
GLTextureStore::GLTextureStore() {
}

void GLTextureStore::Reset() {
    ClearLunaImageTextures();
}

void GLTextureStore::ClearLunaImageTextures()
{
    for (const auto i : mLunaImageTexMap) {
		totalMem -= i.second->GetSizeBytes();
        delete i.second;
    }
    mLunaImageTexMap.clear();
}

void GLTextureStore::ClearLunaImageTexture(uint64_t uid)
{
    auto it = mLunaImageTexMap.find(uid);
    if (it != mLunaImageTexMap.end()) {
		totalMem -= it->second->GetSizeBytes();
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

	uint32_t w = img->getW();
	uint32_t h = img->getH();

    // Try to allocate the GLSprite
    GLSprite* sprite = nullptr;
	void* data = img->getDataPtr();
	if (data != nullptr)
	{
		sprite = GLSprite::Create(data, GL_BGRA, w, h);
		data = nullptr;
		img->notifyTextureified();
	}
    img->unlock();

    // Handle failure
    if (sprite == NULL || !sprite->IsValid())
    {
        if (sprite) delete sprite;
        return NULL;
    }

	totalMem += sprite->GetSizeBytes();

    // Cache new texture
    mLunaImageTexMap[uid] = sprite;

    return sprite;
}

uint64_t GLTextureStore::GetTextureMemory()
{
	return totalMem;
}