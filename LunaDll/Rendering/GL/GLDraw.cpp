#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "GLDraw.h"
#include "GLCompat.h"
#include "../Shaders/GLShader.h"

// Instance
GLDraw g_GLDraw;

GLDraw::GLDraw() :
    mLastTexName(0),
    mLastPwScale(1.0f),
    mLastPhScale(1.0f)
{
}

void GLDraw::DrawSprite(const SRect<double>& dest, const Texture* tex, const SRect<double>& src, float opacity, RenderMode mode)
{
    if (dest.isEmpty() || src.isEmpty()) return;

    // Trim the coordinates to fit the texture
    SRect<double> trimmedSrc = src.intersection(SRect<double>::fromXYWH(0, 0, (double)tex->w, (double)tex->h));
    if (trimmedSrc.isEmpty()) return;
    SRect<double> trimmedDest = dest.shrinkProportionately(src, trimmedSrc);
    if (trimmedDest.isEmpty()) return;

    // Convert src coords to UVs
    trimmedSrc.x1 /= (double)tex->pw;
    trimmedSrc.y1 /= (double)tex->ph;
    trimmedSrc.x2 /= (double)tex->pw;
    trimmedSrc.y2 /= (double)tex->ph;

    // Set rendering mode for this draw operation
    switch (mode) {
    case RENDER_MODE_MULTIPLY:
        glBlendEquationANY(GL_FUNC_ADD);
        GLERRORCHECK();
        glBlendFuncSeparateANY(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        GLERRORCHECK();
        break;
    case RENDER_MODE_MAX:
        glBlendEquationANY(GL_MAX);
        GLERRORCHECK();
        break;
    case RENDER_MODE_AND:
        glBlendEquationANY(GL_LOGIC_OP);
        while (glGetError() != GL_NO_ERROR);
        GLERRORCHECK();
        glEnable(GL_COLOR_LOGIC_OP);
        GLERRORCHECK();
        glLogicOp(GL_AND);
        GLERRORCHECK();
        break;
    case RENDER_MODE_OR:
        glBlendEquationANY(GL_LOGIC_OP);
        while (glGetError() != GL_NO_ERROR);
        GLERRORCHECK();
        glEnable(GL_COLOR_LOGIC_OP);
        GLERRORCHECK();
        glLogicOp(GL_OR);
        GLERRORCHECK();
        break;
    case RENDER_MODE_ALPHA:
    default:
        glBlendEquationANY(GL_FUNC_ADD);
        GLERRORCHECK();
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        GLERRORCHECK();
        break;
    }

    BindTexture(tex);

    GLfloat Vertices[] = {
        (GLfloat)trimmedDest.x1, (GLfloat)trimmedDest.y1, 0,
        (GLfloat)trimmedDest.x2, (GLfloat)trimmedDest.y1, 0,
        (GLfloat)trimmedDest.x2, (GLfloat)trimmedDest.y2, 0,
        (GLfloat)trimmedDest.x1, (GLfloat)trimmedDest.y2, 0
    };
    GLfloat TexCoord[] = {
        (GLfloat)trimmedSrc.x1, (GLfloat)trimmedSrc.y1,
        (GLfloat)trimmedSrc.x2, (GLfloat)trimmedSrc.y1,
        (GLfloat)trimmedSrc.x2, (GLfloat)trimmedSrc.y2,
        (GLfloat)trimmedSrc.x1, (GLfloat)trimmedSrc.y2
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    if (opacity < 1.0f)
    {
        // Apply opacity for pre-multiplied alpha
        glColor4f(opacity, opacity, opacity, opacity);
        GLERRORCHECK();
    }
    glVertexPointer(3, GL_FLOAT, 0, Vertices);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();

    // Disable color logic op if we enabled it
    if ((mode == RENDER_MODE_AND) || (mode == RENDER_MODE_OR))
    {
        glDisable(GL_COLOR_LOGIC_OP);
        GLERRORCHECK();
    }
}

void GLDraw::DrawRectangle(int nXDest, int nYDest, int nWidth, int nHeight)
{
    UnbindTexture();
    glDisable(GL_BLEND);
    GLERRORCHECK();
    glDisable(GL_TEXTURE_2D);
    GLERRORCHECK();

    // Generate our floating point coordinates
    float x1 = (float)nXDest;
    float y1 = (float)nYDest;
    float x2 = x1 + nWidth;
    float y2 = y1 + nHeight;

    GLfloat Vertices[] = {
        x1, y1, 0,
        x2, y1, 0,
        x2, y2, 0,
        x1, y2, 0
    };
    GLfloat TexCoord[] = {
        0, 0,
        0, 0,
        0, 0,
        0, 0
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    glVertexPointer(3, GL_FLOAT, 0, Vertices);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();

    glEnable(GL_BLEND);
    GLERRORCHECK();
    glEnable(GL_TEXTURE_2D);
    GLERRORCHECK();
}


void GLDraw::DrawStretched(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, int nSrcWidth, int nSrcHeight, float opacity, GLShader* upscaleShader)
{
    // Generate our floating point coordinates
    float texw = (float)tex->pw;
    float texh = (float)tex->ph;
    float x1 = (float)nXDest;
    float y1 = (float)nYDest;
    float x2 = x1 + nWidth;
    float y2 = y1 + nHeight;
    float tx1 = (nXSrc + 0.0f) / texw;
    float ty1 = (nYSrc + 0.0f) / texh;
    float tx2 = tx1 + nSrcWidth / texw;
    float ty2 = ty1 + nSrcHeight / texh;

    // Set rendering mode for this draw operation
    glBlendEquationANY(GL_FUNC_ADD);
    GLERRORCHECK();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLERRORCHECK();
    
	// Bind Post-Processing Shader here
	if (upscaleShader)
	{
		upscaleShader->clearSamplers();
		GLERRORCHECK();

		upscaleShader->bind();
		GLERRORCHECK();

		for (auto& uniformInfo : upscaleShader->getAllUniforms())
		{
			if (uniformInfo.getName() == "inputSize")
			{
				double inputW = tex->pw;
				double inputH = tex->ph;
				if (uniformInfo.getType() == GL_DOUBLE_VEC2)
				{
					double textureSizeArr[2] = { inputW, inputH };
					GLShaderVariableEntry uniformEntry(GLShaderVariableType::Uniform, uniformInfo.getId(), GL_DOUBLE_VEC2, 1, textureSizeArr);
					upscaleShader->applyUniform(uniformEntry);
					GLERRORCHECK();
				}
				else if (uniformInfo.getType() == GL_FLOAT_VEC2)
				{
					float textureSizeArr[2] = { (float)inputW, (float)inputH };
					GLShaderVariableEntry uniformEntry(GLShaderVariableType::Uniform, uniformInfo.getId(), GL_FLOAT_VEC2, 1, textureSizeArr);
					upscaleShader->applyUniform(uniformEntry);
					GLERRORCHECK();
				}
			}
			if (uniformInfo.getName() == "crispScale")
			{
				double scaleX = double(nWidth) / nSrcWidth;
				double scaleY = double(nHeight) / nSrcHeight;
				if (scaleX < 1.0) scaleX = 1.0;
				if (scaleY < 1.0) scaleY = 1.0;
				if (uniformInfo.getType() == GL_DOUBLE_VEC2)
				{
					double textureSizeArr[2] = { scaleX, scaleY };
					GLShaderVariableEntry uniformEntry(GLShaderVariableType::Uniform, uniformInfo.getId(), GL_DOUBLE_VEC2, 1, textureSizeArr);
					upscaleShader->applyUniform(uniformEntry);
					GLERRORCHECK();
				}
				else if (uniformInfo.getType() == GL_FLOAT_VEC2)
				{
					float textureSizeArr[2] = { (float)scaleX, (float)scaleY };
					GLShaderVariableEntry uniformEntry(GLShaderVariableType::Uniform, uniformInfo.getId(), GL_FLOAT_VEC2, 1, textureSizeArr);
					upscaleShader->applyUniform(uniformEntry);
					GLERRORCHECK();
				}
			}
		}
	}

	BindTexture(tex);
	GLERRORCHECK();

	if (upscaleShader)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLERRORCHECK();
	}

    GLfloat Vertices[] = {
        x1, y1, 0,
        x2, y1, 0,
        x2, y2, 0,
        x1, y2, 0
    };
    GLfloat TexCoord[] = {
        tx1, ty1,
        tx2, ty1,
        tx2, ty2,
        tx1, ty2
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    if (opacity < 1.0f)
    {
        // Apply opacity for pre-multiplied alpha
        glColor4f(opacity, opacity, opacity, opacity);
        GLERRORCHECK();
    }
    glVertexPointer(3, GL_FLOAT, 0, Vertices);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();

	// Unbind Post-Processing Shader here
	if (upscaleShader)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLERRORCHECK();
		upscaleShader->unbind();
		GLERRORCHECK();
	}
}


