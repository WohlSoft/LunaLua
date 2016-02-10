#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "GLContextManager.h"
#include "GLCompat.h"

// Instance
GLContextManager g_GLContextManager;

// Constructor
GLContextManager::GLContextManager() :
	hDC(nullptr), hCTX(nullptr),
	mInitialized(false), mHadError(false),
    mCurrentFB(nullptr), mFramebuffer(nullptr) {
}

bool GLContextManager::Init(HDC hDC) {
	// If we're switching HDCs, deal with it...
	if (mInitialized && !mHadError && hDC != this->hDC) {
		// If we're switching HDCs, deal with it...
		g_GLDraw.UnbindTexture(); // Unbind current texture
		g_GLTextureStore.Reset(); // Delete all textures
		ReleaseFramebuffer(); // Release framebuffer
		ReleaseContext(); // Release context
		mInitialized = false;
	}

	// Don't re-run if already run
    if (mInitialized || mHadError) return true;

    if (InitContextFromHDC(hDC) &&
		InitFramebuffer() &&
		InitProjectionAndState())
	{
		mInitialized = true;
	} else {
		mHadError = true;
	}

	return mInitialized;
}

bool GLContextManager::IsInitialized() {
    return mInitialized;
}

void GLContextManager::BindScreen() {
	glBindFramebufferANY(GL_FRAMEBUFFER_EXT, 0);
    g_GLContextManager.SetCurrentFB(nullptr);
	GLERRORCHECK();
}

void GLContextManager::BindAndClearFramebuffer() {
    if (mFramebuffer == nullptr) return;

    // Bind the main screen framebuffer and clear it
    mFramebuffer->Bind();
    mFramebuffer->Clear();
}

// Set up a new context from a HDC
bool GLContextManager::InitContextFromHDC(HDC hDC) {
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

	mOldPixelFormat = GetPixelFormat(hDC);

    int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (0 == nPixelFormat)
        return false;

    BOOL bResult = SetPixelFormat(hDC, nPixelFormat, &pfd);
    if (FALSE == bResult)
        return false;

    HGLRC tempContext = wglCreateContext(hDC);
    if (NULL == tempContext)
        return false;

    bResult = wglMakeCurrent(hDC, tempContext);
    if (FALSE == bResult)
    {
        wglDeleteContext(tempContext);
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(tempContext);
        return false;
    }

    glLoadIdentity();
    glOrtho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Prefer v-sync off (but will work fine if forced on too)
    typedef BOOL(APIENTRY * PFNWGLSWAPINTERVALEXTPROC)(int);
    static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 0;
    wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT != 0) {
        wglSwapIntervalEXT(0);
    }

    this->hDC = hDC;
    this->hCTX = tempContext;
    return true;
}

bool GLContextManager::InitFramebuffer() {
    try
    {
        mFramebuffer = new GLFramebuffer(800, 600);
    }
    catch(...)
    {
        mFramebuffer = nullptr;
        return false;
    }
    return true;
}

bool GLContextManager::InitProjectionAndState() {
    if (mFramebuffer == nullptr) return false;
    GLDraw::Texture& tex = mFramebuffer->AsTexture();

    // Set projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLERRORCHECK();
    glOrtho(0.0f, (float)tex.pw, 0.0f, (float)tex.ph, -1.0f, 1.0f);
    GLERRORCHECK();
    glColor3f(1, 1, 1);
    GLERRORCHECK();
    glDisable(GL_LIGHTING);
    GLERRORCHECK();
    glDisable(GL_DEPTH_TEST);
    GLERRORCHECK();
    glDisable(GL_CULL_FACE);
    GLERRORCHECK();
    glEnable(GL_BLEND);
    GLERRORCHECK();
    glEnable(GL_TEXTURE_2D);
    GLERRORCHECK();
    glEnableClientState(GL_VERTEX_ARRAY);
    GLERRORCHECK();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERRORCHECK();

	return true;
}


void GLContextManager::ReleaseFramebuffer() {
    delete mFramebuffer;
    mFramebuffer = nullptr;
}

void GLContextManager::ReleaseContext() {
	// Delete Context
	if (hCTX) {
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(hCTX);
		hCTX = nullptr;
	}

	// Restore pixel format if necessary
	if (mOldPixelFormat != GetPixelFormat(hDC)) {
		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(hDC, mOldPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat(hDC, mOldPixelFormat, &pfd);
	}

	// Clear hDC
	hDC = nullptr;
}