#include "GLEngineCmds.h"
#include "GLEngine.h"
#include "GLCompat.h"
#include "GLDraw.h"
#include "GLTextureStore.h"
#include "GLContextManager.h"
#include "../Shaders/GLShader.h"
#include "../../LuaMain/LuaProxyFFIGraphics.h"

// Special puropose command handled by GLEngineProxy internally
void GLEngineCmd_Exit::run(GLEngine& glEngine) const {}

void GLEngineCmd_ClearTextures::run(GLEngine& glEngine) const {
    // In the future we should also consider some least-recently-used clearing, and other strategies
    g_GLTextureStore.ClearLunaImageTextures();
}
void GLEngineCmd_RenderCameraToScreen::run(GLEngine& glEngine) const {
    const GLDraw::Texture& tex = g_GLContextManager.GetCameraFBTex(mCamIdx);
    g_GLContextManager.BindPrimaryFB();
    g_GLDraw.DrawSprite(
        { mRenderX, mRenderY, mRenderX + mWidth, mRenderY + mHeight },
        &tex,
        { 0.0, 0.0, mWidth, mHeight },
        1.0f, GLDraw::RENDER_MODE_ALPHA);
}

void GLEngineCmd_EndFrame::run(GLEngine& glEngine) const {
    glEngine.EndFrame(mHdcDest, mForceSkip, mRedrawOnly, mResizeOverlay);
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

    // Now only used for blackness drawing
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

    const GLSprite* sprite = NULL;
    if (mImg) {
        sprite = g_GLTextureStore.SpriteFromLunaImage(mImg);
    }

    glBlendEquationANY(GL_FUNC_ADD);
    GLERRORCHECK();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLERRORCHECK();

    if (sprite != nullptr)
    {
        sprite->BindTexture();
    }
    else
    {
        g_GLDraw.UnbindTexture();
    }

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
    if (mImg)
    {
        sprite = g_GLTextureStore.SpriteFromLunaImage(mImg);
    }
    else if (mCapBuff)
    {
        mCapBuff->EnsureFramebufferExists();
        if (mCapBuff->mFramebuffer)
        {
            tex = &mCapBuff->mFramebuffer->AsTexture();
        }
    }

    // Render Target
    GLFramebuffer* oldFB = nullptr;
    if (mTarget)
    {
        oldFB = g_GLContextManager.GetCurrentFB();
        mTarget->EnsureFramebufferExists();
        if (mTarget->mFramebuffer != nullptr)
        {
            mTarget->mFramebuffer->Bind();
        }
    }

    glBlendEquationANY(GL_FUNC_ADD);
    GLERRORCHECK();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLERRORCHECK();

    if (mShader) {
        mShader->clearSamplers();

        mShader->bind();
        GLERRORCHECK();
        
        for (const auto& nextAttr : mAttributes)
            mShader->applyAttribute(nextAttr);
        for (const auto& nextUniform : mUniforms)
            mShader->applyUniform(nextUniform);

        GLERRORCHECK();
    }
    
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

    if (mLinearFiltered)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    for (uint8_t idx=0; idx < mNumClipPlane; idx++)
    {
        glEnable(GL_CLIP_PLANE0 + idx);
        glClipPlane(GL_CLIP_PLANE0 + idx, mClipPlane[idx]);
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

        glTranslatef(static_cast<GLfloat>(-cameraX), static_cast<GLfloat>(-cameraY), 0.0f);
    }

    // If depth testing is enabled, use it
    if (mDepthTest) {
        // Actually, there's nothing that needs to be done for this... it's all in the shader
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

    for (uint8_t idx = 0; idx < mNumClipPlane; idx++)
    {
        glDisable(GL_CLIP_PLANE0 + idx);
    }

    if (mLinearFiltered)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if (!mCapBuff)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
    }

    if (mTex == NULL) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        GLERRORCHECK();
    }
    if (mVertColor != NULL) {
        glDisableClientState(GL_COLOR_ARRAY);
        GLERRORCHECK();
    }

    if (mDepthTest) {
        // Actually, there's nothing that needs to be done for this... it's all in the shader
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
        for (const auto& nextAttr : mAttributes)
        {
            glDisableVertexAttribArray(nextAttr.getLocation());
            GLERRORCHECK();
        }

        mShader->unbind();
        GLERRORCHECK();
    }

    // Render Target Time
    if (mTarget)
    {
        // Restore the old bound FB (if null, bind screen)
        g_GLContextManager.RestoreBoundFB(oldFB);
    }
}

void GLEngineCmd_SetCamera::run(GLEngine& glEngine) const
{
    // Note, mIdx==0 is primary framebuffer. Per-camera framebuffers start index 1.
    // The primary framebuffer can still be considered the current camera for purposes of GLContextManager
    g_GLContextManager.SetActiveCamera(mIdx);
    g_GLContextManager.BindCameraFB();
    g_GLContextManager.GetCurrentFB()->Clear();
    glEngine.SetCameraPositionInScene(mX, mY);
}

void GLEngineCmd_CompileShaderObj::run(GLEngine& glEngine) const
{
    if (!g_GLContextManager.IsInitialized()) return;
    if (!mShaderObj) return;
    if (mShaderObj->mError) return;

    mShaderObj->mShader = std::make_shared<GLShader>(mShaderObj->mVertexSource, mShaderObj->mFragmentSource);

    if (!mShaderObj->mShader->isValid())
    {
        // Failed to compile...
        mShaderObj->mError = true;
        mShaderObj->mErrorString = mShaderObj->mShader->getLastErrorMsg();
        mShaderObj->mShader = nullptr;
        return;
    }

    // Get attribute/uniform metadata
    mShaderObj->mAttributeInfo = mShaderObj->mShader->getAllAttributes();
    mShaderObj->mUniformInfo = mShaderObj->mShader->getAllUniforms();
}

void GLEngineCmd_SetFramebufferSize::run(GLEngine& glEngine) const
{
    g_GLContextManager.SetMainFramebufferSize(mWidth, mHeight);
}

void GLEngineCmd_RedirectCameraFB::run(GLEngine& glEngine) const
{
    if (!g_GLContextManager.IsInitialized()) return;

    // Redirect camera to it
    g_GLContextManager.RedirectCameraFB(this);
}

GLFramebuffer* GLEngineCmd_RedirectCameraFB::getFB() const
{
    if (mBuff)
    {
        // Create framebuffer if not yet existing
        mBuff->EnsureFramebufferExists();

        return mBuff->mFramebuffer;
    }
    return nullptr;
}

void GLEngineCmd_UnRedirectCameraFB::run(GLEngine& glEngine) const
{
    if (!g_GLContextManager.IsInitialized()) return;

    // Undo redirection of camera to it
    g_GLContextManager.UnRedirectCameraFB(mStartCmd.get());
}
