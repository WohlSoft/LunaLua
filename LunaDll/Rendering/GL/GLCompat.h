#ifndef GLCompat_hhhh
#define GLCompat_hhhh

#include <glbinding/gl/gl.h>
#include "../../Defines.h"
#include <sstream>
#include <ios>

namespace glcompat {
    enum GLCompatContext {
        TEST_CONTEXT = 0,
        RENDER_THREAD_CONTEXT,

        CONTEXT_COUNT
    };

    bool SetupContext();

    extern thread_local void (*glGenFramebuffersANY)(gl::GLsizei n, gl::GLuint *ids);
    extern thread_local void (*glBindFramebufferANY)(gl::GLenum target, gl::GLuint framebuffer);
    extern thread_local void (*glGenRenderbuffersANY)(gl::GLsizei n, gl::GLuint *renderbuffers);
    extern thread_local void (*glBindRenderbufferANY)(gl::GLenum target, gl::GLuint renderbuffer);
    extern thread_local void (*glRenderbufferStorageANY)(gl::GLenum target, gl::GLenum internalformat, gl::GLsizei width, gl::GLsizei height);
    extern thread_local void (*glFramebufferRenderbufferANY)(gl::GLenum target, gl::GLenum attachment, gl::GLenum renderbuffertarget, gl::GLuint renderbuffer);
    extern thread_local void (*glFramebufferTexture2DANY)(gl::GLenum target, gl::GLenum attachment, gl::GLenum textarget, gl::GLuint texture, gl::GLint level);
    extern thread_local gl::GLenum (*glCheckFramebufferStatusANY)(gl::GLenum target);
    extern thread_local void (*glDeleteFramebuffersANY)(gl::GLsizei n, const gl::GLuint *framebuffers);
    extern thread_local void (*glDeleteRenderbuffersANY)(gl::GLsizei n, const gl::GLuint *renderbuffers);
    extern thread_local void (*glBlendFuncSeparateANY)(gl::GLenum srcRGB, gl::GLenum dstRGB, gl::GLenum srcAlpha, gl::GLenum dstAlpha);
    extern thread_local void (*glBlendEquationANY)(gl::GLenum mode);
}

////////////////////////
//// Error handling ////
////////////////////////

// Macro inserting the arguments
#define GLERRORCHECK(...) _GLErrorCheck(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// Don't depend on gluErrorString, because 1) It's deprecated, 2) It's unreliable, 3) adds a dependency on glu32.lib, 4) There aren't many options anyway
static const char* _GLGetErrorString(gl::GLenum err) {
#define CASESTR(ENUM) case ENUM: return #ENUM

    switch (err) {
        CASESTR(gl::GL_INVALID_ENUM);
        CASESTR(gl::GL_INVALID_VALUE);
        CASESTR(gl::GL_INVALID_OPERATION);
        CASESTR(gl::GL_STACK_OVERFLOW);
        CASESTR(gl::GL_STACK_UNDERFLOW);
        CASESTR(gl::GL_OUT_OF_MEMORY);
        CASESTR(gl::GL_INVALID_FRAMEBUFFER_OPERATION);
        CASESTR(gl::GL_CONTEXT_LOST);
        CASESTR(gl::GL_TABLE_TOO_LARGE);
    default:
        return "UNKNOWN";
    }
    
#undef CASESTR
}

static inline void _GLErrorMsgArgs(std::wostringstream& errMsg) {
}
template <typename X, typename... Ts>
static inline void _GLErrorMsgArgs(std::wostringstream& errMsg, X&& arg, Ts&&... otherArgs) {
    errMsg << arg << "\r\n";
    _GLErrorMsgArgs(errMsg, std::forward<Ts>(otherArgs)...);
}

// Error checking function
template <typename... Ts>
static inline void _GLErrorCheck(const char* fn, int line, const char* func, Ts... otherArgs) {
    gl::GLenum err = gl::glGetError();
    if (err != gl::GL_NO_ERROR) {
        std::wostringstream errMsg;

        errMsg << "OpenGL Error in " << func << " (at " << fn << ":" << line << ")\r\n";
        errMsg << "\r\n";

        errMsg << "Version: " << LUNALUA_VERSION << "\r\n";
        errMsg << "\r\n";

        errMsg << _GLGetErrorString(err) << " (0x" << std::hex << (unsigned int)err << ")";

        _GLErrorMsgArgs(errMsg, std::forward<Ts>(otherArgs)...);

        dbgbox(errMsg.str().c_str());
        _exit(1);
    }
}

#endif
