#ifndef GLContextManager_hhhh
#define GLContextManager_hhhh

#include <windows.h>
#include <gl/glew.h>
#include "GLTextureStore.h"

class GLContextManager {
public:
    GLContextManager();
    bool Init(HDC hDC);
    bool IsInitialized();

	void BindScreen();
	void BindFramebuffer();
	inline GLDraw::Texture& GetBufTex() { return mBufTex; }

private:
    bool  mInitialized;
    bool  mHadError;
    HDC   hDC;
    HGLRC hCTX;

    GLuint mFB;
    GLuint mColorRB;
    GLuint mDepthRB;
    GLDraw::Texture mBufTex;

    bool InitContextFromHDC(HDC hDC);
    bool InitFramebuffer();
	bool InitProjectionAndState();

	// TODO: Implement the following to support switching hDCs
	// void ReleaseContext()
};

// Instance
extern GLContextManager g_GLContextManager;

#endif
