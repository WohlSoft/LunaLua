#ifndef GLContextManager_hhhh
#define GLContextManager_hhhh

#include <windows.h>
#include <gl/glew.h>

class GLContextManager {
public:
    GLContextManager();
    bool Init(HDC hDC);
    bool IsInitialized();

private:
    HDC   hDC;
    HGLRC hCTX;

    bool InitFromHDC(HDC hDC);
};

// Instance
extern GLContextManager g_GLContextManager;

#endif
