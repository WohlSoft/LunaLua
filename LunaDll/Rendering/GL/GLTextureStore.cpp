#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "../RenderUtils.h"
#include "../BMPBox.h"
#include "GLTextureStore.h"

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
    uint32_t w;
    uint32_t h;

    // Get associated texture from cache if possible
    auto it = mSmbxTexMap.find(hdc);
    if (it != mSmbxTexMap.end()) {
        return it->second;
    }

    {
        BITMAP bmp;
        HBITMAP hbmp;

        // Get handle to bitmap
        hbmp = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
        if (hbmp == NULL) return 0;

        // Get bitmap structure to check the height/width
        GetObject(hbmp, sizeof(BITMAP), &bmp);
        w = bmp.bmWidth;
        h = bmp.bmHeight;
    }

    // Convert to 24bpp BGR in memory that's accessible
    void* pData = NULL;
    HBITMAP convHBMP = FreeImageHelper::CreateEmptyBitmap(w, h, 32, &pData);
    HDC screenHDC = GetDC(NULL);
    if (screenHDC == NULL) {
        return 0;
    }
    HDC convHDC = CreateCompatibleDC(screenHDC);
    SelectObject(convHDC, convHBMP);
    BitBlt(convHDC, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
    DeleteDC(convHDC);
    convHDC = NULL;
    ReleaseDC(NULL, screenHDC);
    screenHDC = NULL;

    // Set alpha channel to 0xFF, because it's always supposed to be and won't
    // be copied as such by BitBlt
    uint32_t pixelCount = w * h;
    for (uint32_t idx = 0; idx < pixelCount; idx++) {
        ((uint8_t*)pData)[idx * 4 + 3] = 0xFF;
    }

    // Try to allocate the GLSprite
    GLSprite* sprite;
    sprite = GLSprite::Create(pData, GL_BGRA, w, h);

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
