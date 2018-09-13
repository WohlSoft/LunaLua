#ifndef GLDraw_hhhh
#define GLDraw_hhhh

#include <windows.h>
#include <glbinding/gl/gl.h>
#include <cstdint>
#include "GLCompat.h"
#include "../../Misc/SRect.h"

class GLDraw {
private:
    gl::GLuint mLastTexName;
public:
    float mLastPwScale;
    float mLastPhScale;
public:
    struct Texture {
        gl::GLuint name;
        int32_t w;
        int32_t h;
        int32_t pw; // Padded width
        int32_t ph; // Padded height
        float   pwScale; // Padding width scaling
        float   phScale; // Padding height scaling

        inline Texture(gl::GLuint name, int32_t w, int32_t h) :
            Texture(name, w, h, RoundDimension(w), RoundDimension(h))
        {
        }
        inline Texture(gl::GLuint name, int32_t w, int32_t h, int32_t pw, int32_t ph) :
            name(name),
            w(w), h(h),
            pw(pw), ph(ph),
            pwScale(((float)w) / pw), phScale(((float)h) / ph)
        {}

        static inline int32_t RoundDimension(int32_t v)
        {
            return v;
            /*
            // Perform rounding to the nearest power of two
            if (v < 1) v = 1;
            v -= 1;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v += 1;
            return v;
            */
        }
    };
    enum RenderMode {
        RENDER_MODE_ALPHA,
        RENDER_MODE_MULTIPLY,
        RENDER_MODE_MAX,
        RENDER_MODE_AND,
        RENDER_MODE_OR
    };

public:
    GLDraw();

    inline void BindTexture(const Texture* tex) {
        gl::GLuint textName = tex ? tex->name : 0;
        if (textName == 0)
        {
            UnbindTexture();
            gl::glBindTexture(gl::GL_TEXTURE_2D, textName);
        }
        else
        {
            gl::GLuint textName = tex ? tex->name : 0;
            if (mLastTexName != textName)
            {
                mLastPwScale = tex->pwScale;
                mLastPhScale = tex->phScale;
                if (mLastTexName == 0)
                {
                    gl::glEnable(gl::GL_TEXTURE_2D);
                }
                gl::glBindTexture(gl::GL_TEXTURE_2D, textName);
                GLERRORCHECK();
                mLastTexName = textName;
            }
            gl::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            GLERRORCHECK();
        }
    }
    inline void UnbindTexture() {
        if (mLastTexName == 0) return;
        mLastTexName = 0;
        mLastPwScale = 1.0f;
        mLastPhScale = 1.0f;
        gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
        gl::glDisable(gl::GL_TEXTURE_2D);
        GLERRORCHECK();
    }
    inline gl::GLuint GetCurrentTexName()
    {
        return mLastTexName;
    }

    void DrawSprite(const SRect<double>& dest, const Texture* tex, const SRect<double>& src, float opacity, RenderMode mode);
    void DrawRectangle(int nXDest, int nYDest, int nWidth, int nHeight);
    void DrawStretched(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, int nSrcWidth, int nSrcHeight, float opacity);
};

// Instance
extern GLDraw g_GLDraw;

#endif
