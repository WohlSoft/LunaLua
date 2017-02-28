#include "GLEngineCmds.h"
#include "GLEngine.h"
#include "GLCompat.h"
#include "GLDraw.h"
#include "GLTextureStore.h"
#include "GLContextManager.h"
#include "../Shaders/GLShader.h"

// Special puropose command handled by GLEngineProxy internally
void GLEngineCmd_Exit::run(GLEngine& glEngine) const {}

void GLEngineCmd_ClearSMBXSprites::run(GLEngine& glEngine) const {
    glEngine.ClearSMBXSprites();

    // TODO: Remove the following at a later point in time once some least-recently-used texture clearing mechanism is implemented
    g_GLTextureStore.ClearLunaImageTextures();
}

void GLEngineCmd_ClearLunaTexture::run(GLEngine& glEngine) const {
    glEngine.ClearLunaTexture(*mBmp);
}

void GLEngineCmd_RenderCameraToScreen::run(GLEngine& glEngine) const {
    glEngine.RenderCameraToScreen(
        mHdcDest,
        mXDest, mYDest,
        mWidthDest, mHeightDest,
        mHdcSrc,
        mXSrc, mYSrc,
        mWidthSrc, mHeightSrc,
        mRop);
}

void GLEngineCmd_EndFrame::run(GLEngine& glEngine) const {
    glEngine.EndFrame(mHdcDest);
}

void GLEngineCmd_InitForHDC::run(GLEngine& glEngine) const {
    glEngine.InitForHDC(mHdcDest);
}

void GLEngineCmd_EmulateBitBlt::run(GLEngine& glEngine) const {
    if (!g_GLContextManager.IsInitialized()) return;

    if (mRop == BLACKNESS || mRop == 0x10)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        g_GLDraw.DrawRectangle(mXDest, mYDest, mWidthDest, mHeightDest);
        glColor3f(1, 1, 1);
        return;
    }

    GLDraw::RenderMode mode;
    switch (mRop) {
    case SRCAND:
        mode = glEngine.IsBitwiseCompatEnabled() ? GLDraw::RENDER_MODE_AND : GLDraw::RENDER_MODE_MULTIPLY;
        break;
    case SRCPAINT:
        mode = glEngine.IsBitwiseCompatEnabled() ? GLDraw::RENDER_MODE_OR : GLDraw::RENDER_MODE_MAX;
        break;
    default:
        mode = GLDraw::RENDER_MODE_ALPHA;
        break;
    }

    const GLSprite* sprite = g_GLTextureStore.SpriteFromSMBXBitmap(mHdcSrc);
    if (sprite == NULL) {
        return;
    }

    sprite->Draw(mXDest, mYDest, mWidthDest, mHeightDest, mXSrc, mYSrc, mode);
}

void GLEngineCmd_LunaDrawSprite::run(GLEngine& glEngine) const {
    if (!g_GLContextManager.IsInitialized()) return;

    if (mBmp != nullptr)
    {
        const GLDraw::Texture* tex = g_GLTextureStore.TextureFromLunaBitmap(*mBmp);
        if (tex == nullptr) return;
        g_GLDraw.DrawStretched(mXDest, mYDest, mWidthDest, mHeightDest, tex, mXSrc, mYSrc, mWidthSrc, mHeightSrc, mOpacity);
    }
}

void GLEngineCmd_DrawSprite::run(GLEngine& glEngine) const {
    if (!g_GLContextManager.IsInitialized()) return;

    if (!mImg) return;
    const GLSprite* sprite = g_GLTextureStore.SpriteFromLunaImage(mImg);
    if (sprite == nullptr) return;
    sprite->Draw(
        SRect<double>::fromXYWH(mXDest, mYDest, mWidthDest, mHeightDest),
        SRect<double>::fromXYWH(mXSrc, mYSrc, mWidthSrc, mHeightSrc),
        mOpacity,
        mMode);
}

void GLEngineCmd_SetTexture::run(GLEngine& glEngine) const {
    if (!g_GLContextManager.IsInitialized()) return;

    const GLDraw::Texture* tex = NULL;
    if (mBmp) {
        tex = g_GLTextureStore.TextureFromLunaBitmap(*mBmp);
    }

    glBlendEquationANY(GL_FUNC_ADD);
    GLERRORCHECK();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLERRORCHECK();

    g_GLDraw.BindTexture(tex);

    float r = ((0xFF000000 & mColor) >> 24) / 255.0f;
    float g = ((0x00FF0000 & mColor) >> 16) / 255.0f;
    float b = ((0x0000FF00 & mColor) >> 8) / 255.0f;
    float a = ((0x000000FF & mColor) >> 0) / 255.0f;
    glColor4f(r*a, g*a, b*a, a);
    GLERRORCHECK();
}

void GLEngineCmd_Draw2DArray::run(GLEngine& glEngine) const {
    if (!g_GLContextManager.IsInitialized()) return;

    // Convert texel coordinates to what we need for our power-of-two padded textures
    bool texIsPadded = (g_GLDraw.mLastPwScale != 1.0f) || (g_GLDraw.mLastPhScale != 1.0f);
    if (texIsPadded) {
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glScalef(g_GLDraw.mLastPwScale, g_GLDraw.mLastPhScale, 1.0f);
        GLERRORCHECK();
    }

    glVertexPointer(2, GL_FLOAT, 0, mVert);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, mTex);
    GLERRORCHECK();
    glDrawArrays(mType, 0, mCount);
    GLERRORCHECK();

    if (texIsPadded) {
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        GLERRORCHECK();
    }
}


void GLEngineCmd_LuaDraw::run(GLEngine& glEngine) const {
    if (!g_GLContextManager.IsInitialized()) return;

    const GLDraw::Texture* tex = NULL;
    const GLSprite* sprite = NULL;
    if (mImg) {
        sprite = g_GLTextureStore.SpriteFromLunaImage(mImg);
    }
    else if (mBmp) {
        tex = g_GLTextureStore.TextureFromLunaBitmap(*mBmp);
    }
    else if (mCapBuff) {
        tex = &mCapBuff->mFramebuffer->AsTexture();
    }

    glBlendEquationANY(GL_FUNC_ADD);
    GLERRORCHECK();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLERRORCHECK();

    if (sprite != nullptr)
    {
        sprite->BindTexture();
    }
    else if (tex != nullptr)
    {
        g_GLDraw.BindTexture(tex);
    }
    else
    {
        g_GLDraw.UnbindTexture();
    }

    if (mShader) {
        mShader->bind();
        GLERRORCHECK();
        
        for (const auto& nextAttr : mAttributes)
            mShader->applyAttribute(nextAttr);
        for (const auto& nextUniform : mUniforms)
            mShader->applyUniform(nextUniform);

        GLERRORCHECK();
    }
    
    glColor4f(mColor[0] * mColor[3], mColor[1] * mColor[3], mColor[2] * mColor[3], mColor[3]);
    GLERRORCHECK();

    // Convert texel coordinates to what we need for our power-of-two padded textures
    bool texIsPadded = (g_GLDraw.mLastPwScale != 1.0f) || (g_GLDraw.mLastPhScale != 1.0f);
    if ((mTex != NULL) && texIsPadded) {
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glScalef(g_GLDraw.mLastPwScale, g_GLDraw.mLastPhScale, 1.0f);
        GLERRORCHECK();
    }

    // For scene coordinates, translate appropriately
    if (mSceneCoords) {
        double cameraX, cameraY;
        glEngine.GetCamera(cameraX, cameraY);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(static_cast<GLfloat>(-cameraX - 0.5), static_cast<GLfloat>(-cameraY - 0.5), 0.0f);
    }

    glVertexPointer(2, GL_FLOAT, 0, mVert);
    GLERRORCHECK();

    if (mTex != NULL) {
        glTexCoordPointer(2, GL_FLOAT, 0, mTex);
        GLERRORCHECK();
    } else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        GLERRORCHECK();
    }

    if (mVertColor != NULL) {
        glEnableClientState(GL_COLOR_ARRAY);
        GLERRORCHECK();
        glColorPointer(4, GL_FLOAT, 0, mVertColor);
        GLERRORCHECK();
    }

    glDrawArrays(mType, 0, mCount);
    GLERRORCHECK();

    if (mTex == NULL) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLERRORCHECK();
    }
    if (mVertColor != NULL) {
        glDisableClientState(GL_COLOR_ARRAY);
        GLERRORCHECK();
    }

    if (mSceneCoords) {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    if ((mTex != NULL) && texIsPadded) {
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        GLERRORCHECK();
    }

    if (mShader) {
        mShader->unbind();
        GLERRORCHECK();
    }
}

void GLEngineCmd_SetCamera::run(GLEngine& glEngine) const
{
    glEngine.SetCamera(mX, mY);
}