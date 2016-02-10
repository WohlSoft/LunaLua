#ifndef GLContextManager_hhhh
#define GLContextManager_hhhh

#include <windows.h>
#include <gl/glew.h>
#include "GLTextureStore.h"
#include "GLFramebuffer.h"

class GLContextManager {
public:
    GLContextManager();
    bool Init(HDC hDC);
    bool IsInitialized();

	void BindScreen();
	void BindAndClearFramebuffer();
	inline GLDraw::Texture& GetBufTex()
    {
        static GLDraw::Texture nullTex(0, 0, 0);
        if (mFramebuffer == nullptr)
        {
            return nullTex;
        }
        return mFramebuffer->AsTexture();
    }
    inline GLFramebuffer* GetCurrentFB() { return mCurrentFB; }
    inline void SetCurrentFB(GLFramebuffer* fb) { mCurrentFB = fb; }

private:
    bool  mInitialized;
    bool  mHadError;

	// Context variables
	int   mOldPixelFormat;
    HDC   hDC;
    HGLRC hCTX;

	// Framebuffer variables
    GLFramebuffer* mCurrentFB;
    GLFramebuffer* mFramebuffer;

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
