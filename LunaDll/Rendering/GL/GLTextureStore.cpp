#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "../RenderUtils.h"
#include "../BMPBox.h"
#include "GLTextureStore.h"
#include "../../Globals.h"

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

const GLDraw::Texture* GLTextureStore::TextureFromLunaBitmap(const BMPBox& bmp, bool noUpdate)
{
	if (bmp.m_hbmp == NULL) return NULL;
	// Get associated texture from cache if possible
	
	bool cacheFound = false;
	auto it = mLunaTexMap.find(&bmp);
	cacheFound = it != mLunaTexMap.end();

	//image
	if (!bmp.hasVideo && cacheFound)return it->second;

	// The bitmap of BMPBox is known to be a DIB one, with pre-multiplied BGRA.
	// This makes things easy.
	BITMAP bm;
	memset(&bm, 0, sizeof(BITMAP));
	GetObject(bmp.m_hbmp, sizeof(BITMAP), &bm);
	if (bm.bmBits == NULL) return NULL; // Wrong type of bitmap?
										// Create texture object with width/height
	GLDraw::Texture tex(NULL, bm.bmWidth, bm.bmHeight);

	//completely new bitmap
	if (!cacheFound) {
		glGenTextures(1, &tex.name);
		GLERRORCHECK();
	}//not a new bitmap but internal image has been updated
	else {
		tex.name = it->second->name;
	}




	// Try to allocate texture

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

	const_cast<BMPBox*>(&bmp)->setOnScreen(true); //sorry for const_cast'ing

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.pw, tex.ph, 0, GL_BGRA, GL_UNSIGNED_BYTE, bm.bmBits);
	//GLERRORCHECK();
	//((BMPBox*)&bmp)->m_modified.store(false, std::memory_order_relaxed);
	// Cache new texture
	if (cacheFound) {
		return it->second;
	}
	else {
		GLDraw::Texture* pTex = new GLDraw::Texture(tex);
		mLunaTexMap[&bmp] = pTex;
		return pTex;
	}
	
}

void GLTextureStore::checkRendered() {
	
	/*
	for (auto i : mLunaTexMap) {
		const_cast<BMPBox*>(i.first)->setOnScreen(true); 
	}
	*/
	/*
	for (int i = 0; i < deleteTmp.size(); i++) {
	ClearLunaTexture(*deleteTmp[i]);
	}
	*/
}