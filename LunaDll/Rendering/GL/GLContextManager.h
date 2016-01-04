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

	// Context variables
	int   mOldPixelFormat;
    HDC   hDC;
    HGLRC hCTX;

	// Framebuffer variables
	// TODO: Move framebuffer into a class.
    GLuint mFB;
    GLuint mDepthRB;
    GLDraw::Texture mBufTex;

	// Init functions
    bool InitContextFromHDC(HDC hDC);
    bool InitFramebuffer();
	bool InitProjectionAndState();

	// Release functions
	void ReleaseContext();
	void ReleaseFramebuffer();
};

// Instance
extern GLContextManager g_GLContextManager;

#endif
