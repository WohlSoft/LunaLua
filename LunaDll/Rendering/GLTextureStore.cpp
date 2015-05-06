#include <windows.h>
#include <gl/glew.h>
#include "../Defines.h"
#include "GLTextureStore.h"
#include "RenderUtils.h"

// Instance
GLTextureStore g_GLTextureStore;

// Constructor
GLTextureStore::GLTextureStore() :
    mLastTexName(0) {
    mSmbxHdcMap.clear();
}

void GLTextureStore::ClearSMBXTextures()
{
    mLastTexName = 0;
    for (const auto i : mSmbxHdcMap) {
        glDeleteTextures(1, &i.second->name);
        delete i.second;
    }
    mSmbxHdcMap.clear();
}

const GLDraw::Texture* GLTextureStore::TextureFromSMBXBitmap(HDC hdc) {
    GLDraw::Texture tex = { 0, 0, 0 };

    // Get associated texture from cache if possible
    auto it = mSmbxHdcMap.find(hdc);
    if (it != mSmbxHdcMap.end()) {
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
        tex.w = bmp.bmWidth;
        tex.h = bmp.bmHeight;
    }

    // Convert to 24bpp BGR in memory that's accessible
    void* pData = NULL;
    HBITMAP convHBMP = CreateEmptyBitmap(tex.w, tex.h, 24, &pData);
    HDC screenHDC = GetDC(NULL);
    if (screenHDC == NULL) {
        return 0;
    }
    HDC convHDC = CreateCompatibleDC(screenHDC);
    SelectObject(convHDC, convHBMP);
    BitBlt(convHDC, 0, 0, tex.w, tex.h, hdc, 0, 0, SRCCOPY);
    DeleteDC(convHDC);
    convHDC = NULL;
    ReleaseDC(NULL, screenHDC);
    screenHDC = NULL;

    // Move into texture
    glGenTextures(1, &tex.name);
    if (tex.name == 0)
    {
        DeleteObject(convHBMP);
        convHBMP = NULL;
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, tex.name);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.w, tex.h, 0, GL_BGR, GL_UNSIGNED_BYTE, pData);

    // Delete conversion DIB section
    DeleteObject(convHBMP);
    convHBMP = NULL;

    // Cache new texture
    GLDraw::Texture* pTex = new GLDraw::Texture(tex);
    mSmbxHdcMap[hdc] = pTex;

    return pTex;
}
