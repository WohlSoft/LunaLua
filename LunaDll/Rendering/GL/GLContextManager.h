#ifndef GLContextManager_hhhh
#define GLContextManager_hhhh

#include <windows.h>
#include <gl/glew.h>
#include "GLTextureStore.h"
#include "GLFramebuffer.h"

class GLContextManager {
public:
    struct GLConstants
    {
        const char* pcVENDOR;
        const char* pcRENDERER;
        const char* pcVERSION;
        const char* pcSHADING_LANGUAGE_VERSION;
        int iMAJOR_VERSION;
        int iMINOR_VERSION;
        int iMAX_COMPUTE_SHADER_STORAGE_BLOCKS;
        int iMAX_COMBINED_SHADER_STORAGE_BLOCKS;
        int iMAX_COMPUTE_UNIFORM_BLOCKS;
        int iMAX_COMPUTE_TEXTURE_IMAGE_UNITS;
        int iMAX_COMPUTE_UNIFORM_COMPONENTS;
        int iMAX_COMPUTE_ATOMIC_COUNTERS;
        int iMAX_COMPUTE_ATOMIC_COUNTER_BUFFERS;
        int iMAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS;
        int iMAX_COMPUTE_WORK_GROUP_INVOCATIONS;
        int iMAX_COMPUTE_WORK_GROUP_COUNT;
        int iMAX_COMPUTE_WORK_GROUP_SIZE;
        int iMAX_DEBUG_GROUP_STACK_DEPTH;
        int iMAX_3D_TEXTURE_SIZE;
        int iMAX_ARRAY_TEXTURE_LAYERS;
        int iMAX_CLIP_DISTANCES;
        int iMAX_COLOR_TEXTURE_SAMPLES;
        int iMAX_COMBINED_ATOMIC_COUNTERS;
        int iMAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS;
        int iMAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS;
        int iMAX_COMBINED_TEXTURE_IMAGE_UNITS;
        int iMAX_COMBINED_UNIFORM_BLOCKS;
        int iMAX_COMBINED_VERTEX_UNIFORM_COMPONENTS;
        int iMAX_CUBE_MAP_TEXTURE_SIZE;
        int iMAX_DEPTH_TEXTURE_SAMPLES;
        int iMAX_DRAW_BUFFERS;
        int iMAX_DUAL_SOURCE_DRAW_BUFFERS;
        int iMAX_ELEMENTS_INDICES;
        int iMAX_ELEMENTS_VERTICES;
        int iMAX_FRAGMENT_ATOMIC_COUNTERS;
        int iMAX_FRAGMENT_SHADER_STORAGE_BLOCKS;
        int iMAX_FRAGMENT_INPUT_COMPONENTS;
        int iMAX_FRAGMENT_UNIFORM_COMPONENTS;
        int iMAX_FRAGMENT_UNIFORM_VECTORS;
        int iMAX_FRAGMENT_UNIFORM_BLOCKS;
        int iMAX_FRAMEBUFFER_WIDTH;
        int iMAX_FRAMEBUFFER_HEIGHT;
        int iMAX_FRAMEBUFFER_LAYERS;
        int iMAX_FRAMEBUFFER_SAMPLES;
        int iMAX_GEOMETRY_ATOMIC_COUNTERS;
        int iMAX_GEOMETRY_SHADER_STORAGE_BLOCKS;
        int iMAX_GEOMETRY_INPUT_COMPONENTS;
        int iMAX_GEOMETRY_OUTPUT_COMPONENTS;
        int iMAX_GEOMETRY_TEXTURE_IMAGE_UNITS;
        int iMAX_GEOMETRY_UNIFORM_BLOCKS;
        int iMAX_GEOMETRY_UNIFORM_COMPONENTS;
        int iMAX_INTEGER_SAMPLES;
        int iMAX_LABEL_LENGTH;
        int iMAX_PROGRAM_TEXEL_OFFSET;
        int iMAX_RECTANGLE_TEXTURE_SIZE;
        int iMAX_RENDERBUFFER_SIZE;
        int iMAX_SAMPLE_MASK_WORDS;
        int iMAX_SERVER_WAIT_TIMEOUT;
        int iMAX_SHADER_STORAGE_BUFFER_BINDINGS;
        int iMAX_TESS_CONTROL_ATOMIC_COUNTERS;
        int iMAX_TESS_EVALUATION_ATOMIC_COUNTERS;
        int iMAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS;
        int iMAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS;
        int iMAX_TEXTURE_BUFFER_SIZE;
        int iMAX_TEXTURE_IMAGE_UNITS;
        int iMAX_TEXTURE_LOD_BIAS;
        int iMAX_TEXTURE_SIZE;
        int iMAX_UNIFORM_BUFFER_BINDINGS;
        int iMAX_UNIFORM_BLOCK_SIZE;
        int iMAX_UNIFORM_LOCATIONS;
        int iMAX_VARYING_COMPONENTS;
        int iMAX_VARYING_VECTORS;
        int iMAX_VARYING_FLOATS;
        int iMAX_VERTEX_ATOMIC_COUNTERS;
        int iMAX_VERTEX_ATTRIBS;
        int iMAX_VERTEX_SHADER_STORAGE_BLOCKS;
        int iMAX_VERTEX_TEXTURE_IMAGE_UNITS;
        int iMAX_VERTEX_UNIFORM_COMPONENTS;
        int iMAX_VERTEX_UNIFORM_VECTORS;
        int iMAX_VERTEX_OUTPUT_COMPONENTS;
        int iMAX_VERTEX_UNIFORM_BLOCKS;
        int iMAX_VIEWPORT_DIMS;
        int iMAX_VIEWPORTS;
        int iMAX_VERTEX_ATTRIB_RELATIVE_OFFSET;
        int iMAX_VERTEX_ATTRIB_BINDINGS;
        int iMAX_ELEMENT_INDEX;
    };
public:
    GLContextManager();
    bool Init(HDC hDC);
    bool IsInitialized();

    void BindScreen();
    void BindAndClearFramebuffer();
    void BindFramebuffer();
    inline const GLDraw::Texture& GetBufTex()
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
    void EnsureMainThreadCTXApplied();
    inline const GLConstants& Constants() { return mConstants; }

private:
    bool  mInitialized;
    bool  mHadError;
    bool  mMainThreadCTXApplied;  // Whether wglMakeCurrent has been called for the main thread or not
    int   mMainFBWidth;
    int   mMainFBHeight;

    // Context variables
    int   mOldPixelFormat;
    HDC   hDC;
    HGLRC hQueueThreadCTX;
    HGLRC hMainThreadCTX;

    // Framebuffer variables
    GLFramebuffer* mCurrentFB;
    GLFramebuffer* mFramebuffer;

    // Constants list
    GLConstants mConstants;

    // Init functions
    bool InitContextFromHDC(HDC hDC);
    bool InitFramebuffer();
    bool InitProjectionAndState();

    // Release functions
    void ReleaseContext();
    void ReleaseFramebuffer();

public:
    // Framebuffer config
    void SetMainFramebufferSize(int width, int height);

    inline int GetMainFBWidth() { return mMainFBWidth; }
    inline int GetMainFBHeight() { return mMainFBHeight; }
};

// Instance
extern GLContextManager g_GLContextManager;

#endif
