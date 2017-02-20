#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "../RenderUtils.h"
#include "../BMPBox.h"
#include "GLTextureStore.h"
#include "../LunaImage.h"

// Instance
GLTextureStore g_GLTextureStore;

// Constructor
GLTextureStore::GLTextureStore() {
    mSmbxTexMap.clear();
}

void GLTextureStore::Reset() {
	ClearSMBXSprites();
	ClearLunaTextures();
}

void GLTextureStore::ClearLunaTextures() {
	for (const auto i : mLunaTexMap) {
		glDeleteTextures(1, &i.second->name);
		GLERRORCHECK();
		delete i.second;
	}
	mLunaTexMap.clear();
}

void GLTextureStore::ClearSMBXSprites()
{
    for (const auto i : mSmbxTexMap) {
        delete i.second;
    }
    mSmbxTexMap.clear();
}

void GLTextureStore::ClearLunaTexture(const BMPBox& bmp)
{
	auto it = mLunaTexMap.find(&bmp);
	if (it != mLunaTexMap.end()) {
		glDeleteTextures(1, &it->second->name);
		GLERRORCHECK();
		delete it->second;
		mLunaTexMap.erase(it);
	}
}

const GLSprite* GLTextureStore::SpriteFromSMBXBitmap(HDC hdc) {
    // Get associated texture from cache if possible
    auto it = mSmbxTexMap.find(hdc);
    if (it != mSmbxTexMap.end()) {
        return it->second;
    }

    HBITMAP convHBMP = CopyBitmapFromHdc(hdc);
    if (convHBMP == nullptr) return nullptr;
    BITMAP bmp;
    GetObject(convHBMP, sizeof(BITMAP), &bmp);

    // Try to allocate the GLSprite
    GLSprite* sprite;
    sprite = GLSprite::Create(bmp.bmBits, GL_BGRA, bmp.bmWidth, bmp.bmHeight);

    // Deallocate temporary conversion memory
    DeleteObject(convHBMP);
    convHBMP = NULL;

    // Handle failure
    if (sprite == NULL || !sprite->IsValid())
    {
        if (sprite) delete sprite;
        return NULL;
    }

    // Cache new texture
    mSmbxTexMap[hdc] = sprite;

    return sprite;
}

const GLDraw::Texture* GLTextureStore::TextureFromLunaBitmap(const BMPBox& bmp)
{
    if (bmp.m_hbmp == NULL) return NULL;

    // Get associated texture from cache if possible
    auto it = mLunaTexMap.find(&bmp);
    if (it != mLunaTexMap.end()) {
        return it->second;
    }

    // The bitmap of BMPBox is known to be a DIB one, with pre-multiplied BGRA.
    // This makes things easy.
    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(bmp.m_hbmp, sizeof(BITMAP), &bm);
    if (bm.bmBits == NULL) return NULL; // Wrong type of bitmap?

    // Create texture object with width/height
    GLDraw::Texture tex(NULL, bm.bmWidth, bm.bmHeight);

    // Try to allocate texture
    glGenTextures(1, &tex.name);
    GLERRORCHECK();
    if (tex.name == 0) return NULL;

    g_GLDraw.BindTexture(&tex);
    GLERRORCHECK();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    GLERRORCHECK();
    float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    GLERRORCHECK();
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.pw, tex.ph, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    GLERRORCHECK("glTexImage2D", bmp.m_Filename, tex.pw, tex.ph);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex.w, tex.h, GL_BGRA, GL_UNSIGNED_BYTE, bm.bmBits);
    GLERRORCHECK("glTexSubImage2D", bmp.m_Filename, tex.pw, tex.ph);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.pw, tex.ph, 0, GL_BGRA, GL_UNSIGNED_BYTE, bm.bmBits);
    //GLERRORCHECK();

    // Cache new texture
    GLDraw::Texture* pTex = new GLDraw::Texture(tex);
    mLunaTexMap[&bmp] = pTex;

    return pTex;
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

const GLSprite* GLTextureStore::SpriteFromLunaImage(const std::shared_ptr<LunaImageData>& img)
{
    img->Lock();

    uint64_t uid = img->getUID();

    // Get associated texture from cache if possible
    auto it = mLunaImageTexMap.find(uid);
    if (it != mLunaImageTexMap.end()) {
        img->Unlock();
        return it->second;
    }

    // Try to allocate the GLSprite
    GLSprite* sprite;
    sprite = GLSprite::Create(img->getDataPtr(), GL_BGRA, img->getW(), img->getH());
    img->Unlock();

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
