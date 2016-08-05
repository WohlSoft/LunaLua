#include "GLShader.h"

#include "../../GlobalFuncs.h"

#include <iostream>

bool GLShader::compileShaderSource(GLuint shaderID, const std::string& source)
{
    const char* sources[] = { source.c_str() };
    glShaderSource(shaderID, 1, sources, NULL);
    glCompileShader(shaderID);

    GLint result;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    return result != GL_FALSE;
}

std::string GLShader::getLastShaderError(GLuint shaderID)
{
    GLint length;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
    std::string errorStr(length, ' ');
    glGetShaderInfoLog(shaderID, length, &length, &errorStr[0]);
    return errorStr;
}

GLShader::GLShader(const std::string & name, const std::string & vertexSource, const std::string & fragementSource) :
    m_name(name),
    m_vertexSource(vertexSource),
    m_fragmentSource(fragementSource),
    m_isValid(false)
{
    load();
}

GLShader::~GLShader()
{
    glDeleteProgram(m_shaderID);
}


std::vector<GLShaderAttributeInfo> GLShader::getAllAttributes() const
{
    return getAllVariables<GLShaderAttributeInfo>(GL_ACTIVE_ATTRIBUTES, glGetActiveAttrib, glGetAttribLocation);
}


std::vector<GLShaderUniformInfo> GLShader::getAllUniforms() const
{
    return getAllVariables<GLShaderUniformInfo>(GL_ACTIVE_UNIFORMS, glGetActiveUniform, glGetUniformLocation);
}

void GLShader::bind()
{
    glUseProgram(m_shaderID);
}

void GLShader::unbind()
{
    glUseProgram(0);
}



void GLShader::applyAttribute(GLint location, GLenum type, void* data)
{
    UniversalArray unsiversalArray;
    unsiversalArray.data = data;

    switch (type) {
    case GL_FLOAT:              glVertexAttrib1f(location, unsiversalArray.farr[0]); break;
    case GL_FLOAT_VEC2:         glVertexAttrib2f(location, unsiversalArray.farr[0], unsiversalArray.farr[1]); break;
    case GL_FLOAT_VEC3:         glVertexAttrib3f(location, unsiversalArray.farr[0], unsiversalArray.farr[1], unsiversalArray.farr[2]); break;
    case GL_FLOAT_VEC4:         glVertexAttrib4f(location, unsiversalArray.farr[0], unsiversalArray.farr[1], unsiversalArray.farr[2], unsiversalArray.farr[3]); break;
    case GL_INT:                glVertexAttribI1i(location, unsiversalArray.iarr[0]); break;
    case GL_INT_VEC2:           glVertexAttribI2i(location, unsiversalArray.iarr[0], unsiversalArray.iarr[1]); break;
    case GL_INT_VEC3:           glVertexAttribI3i(location, unsiversalArray.iarr[0], unsiversalArray.iarr[1], unsiversalArray.iarr[2]); break;
    case GL_INT_VEC4:           glVertexAttribI4i(location, unsiversalArray.iarr[0], unsiversalArray.iarr[1], unsiversalArray.iarr[2], unsiversalArray.iarr[3]); break;
    case GL_UNSIGNED_INT:       glVertexAttribI1ui(location, unsiversalArray.uiarr[0]); break;
    case GL_UNSIGNED_INT_VEC2:  glVertexAttribI2ui(location, unsiversalArray.uiarr[0], unsiversalArray.uiarr[1]); break;
    case GL_UNSIGNED_INT_VEC3:  glVertexAttribI3ui(location, unsiversalArray.uiarr[0], unsiversalArray.uiarr[1], unsiversalArray.uiarr[2]); break;
    case GL_UNSIGNED_INT_VEC4:  glVertexAttribI4ui(location, unsiversalArray.uiarr[0], unsiversalArray.uiarr[1], unsiversalArray.uiarr[2], unsiversalArray.uiarr[3]); break;
    case GL_DOUBLE:             glVertexAttrib1d(location, unsiversalArray.darr[0]); break;
    case GL_DOUBLE_VEC2:         glVertexAttrib2d(location, unsiversalArray.darr[0], unsiversalArray.darr[1]); break;
    case GL_DOUBLE_VEC3:         glVertexAttrib3d(location, unsiversalArray.darr[0], unsiversalArray.darr[1], unsiversalArray.darr[2]); break;
    case GL_DOUBLE_VEC4:         glVertexAttrib4d(location, unsiversalArray.darr[0], unsiversalArray.darr[1], unsiversalArray.darr[2], unsiversalArray.darr[3]); break;

    default:
        break;
    }

    GLERRORCHECK();
}

void GLShader::applyUniform(GLint location, GLenum type, void* data)
{
    UniversalArray unsiversalArray;
    unsiversalArray.data = data;

    switch (type) {
    case GL_FLOAT:              glUniform1f(location, unsiversalArray.farr[0]); break;
    case GL_FLOAT_VEC2:         glUniform2f(location, unsiversalArray.farr[0], unsiversalArray.farr[1]); break;
    case GL_FLOAT_VEC3:         glUniform3f(location, unsiversalArray.farr[0], unsiversalArray.farr[1], unsiversalArray.farr[2]); break;
    case GL_FLOAT_VEC4:         glUniform4f(location, unsiversalArray.farr[0], unsiversalArray.farr[1], unsiversalArray.farr[2], unsiversalArray.farr[3]); break;
    case GL_FLOAT_MAT2:         glUniformMatrix2fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT3:         glUniformMatrix3fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT4:         glUniformMatrix4fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT2x3:       glUniformMatrix2x3fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT2x4:       glUniformMatrix2x4fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT3x2:       glUniformMatrix3x2fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT3x4:       glUniformMatrix3x4fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT4x2:       glUniformMatrix4x2fv(location, 1, false, unsiversalArray.farr); break;
    case GL_FLOAT_MAT4x3:       glUniformMatrix4x3fv(location, 1, false, unsiversalArray.farr); break;
    case GL_INT:                glUniform1i(location, unsiversalArray.iarr[0]); break;
    case GL_INT_VEC2:           glUniform2i(location, unsiversalArray.iarr[0], unsiversalArray.iarr[1]); break;
    case GL_INT_VEC3:           glUniform3i(location, unsiversalArray.iarr[0], unsiversalArray.iarr[1], unsiversalArray.iarr[2]); break;
    case GL_INT_VEC4:           glUniform4i(location, unsiversalArray.iarr[0], unsiversalArray.iarr[1], unsiversalArray.iarr[2], unsiversalArray.iarr[3]); break;
    case GL_UNSIGNED_INT:       glUniform1ui(location, unsiversalArray.uiarr[0]); break;
    case GL_UNSIGNED_INT_VEC2:  glUniform2ui(location, unsiversalArray.uiarr[0], unsiversalArray.uiarr[1]); break;
    case GL_UNSIGNED_INT_VEC3:  glUniform3ui(location, unsiversalArray.uiarr[0], unsiversalArray.uiarr[1], unsiversalArray.uiarr[2]); break;
    case GL_UNSIGNED_INT_VEC4:  glUniform4ui(location, unsiversalArray.uiarr[0], unsiversalArray.uiarr[1], unsiversalArray.uiarr[2], unsiversalArray.uiarr[3]); break;
    case GL_DOUBLE:             glUniform1d(location, unsiversalArray.darr[0]); break;
    case GL_DOUBLE_VEC2:         glUniform2d(location, unsiversalArray.darr[0], unsiversalArray.darr[1]); break;
    case GL_DOUBLE_VEC3:         glUniform3d(location, unsiversalArray.darr[0], unsiversalArray.darr[1], unsiversalArray.darr[2]); break;
    case GL_DOUBLE_VEC4:         glUniform4d(location, unsiversalArray.darr[0], unsiversalArray.darr[1], unsiversalArray.darr[2], unsiversalArray.darr[3]); break;
    case GL_DOUBLE_MAT2:         glUniformMatrix2dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT3:         glUniformMatrix3dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT4:         glUniformMatrix4dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT2x3:       glUniformMatrix2x3dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT2x4:       glUniformMatrix2x4dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT3x2:       glUniformMatrix3x2dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT3x4:       glUniformMatrix3x4dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT4x2:       glUniformMatrix4x2dv(location, 1, false, unsiversalArray.darr); break;
    case GL_DOUBLE_MAT4x3:       glUniformMatrix4x3dv(location, 1, false, unsiversalArray.darr); break;

    default:
        break;
    }

    GLERRORCHECK();
}



void GLShader::load()
{
    bool isVertexSourceValid = m_vertexSource != "";
    bool isFragmentSourceValid = m_vertexSource != "";
    if (!isVertexSourceValid && !isFragmentSourceValid)
        return;
    
    GLuint program = glCreateProgram();
    GLERRORCHECK();
    GLuint vertex; 
    GLuint fragment;
    if(isVertexSourceValid) 
        vertex = glCreateShader(GL_VERTEX_SHADER);
    GLERRORCHECK();
    if (isFragmentSourceValid)
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
    GLERRORCHECK();
    
    if (isVertexSourceValid) {
        if (!compileShaderSource(vertex, m_vertexSource))
        {
            m_lastErrorMsg = getLastShaderError(vertex);
            glDeleteShader(vertex);
            GLERRORCHECK();
            return;
        }
    }
    if (isFragmentSourceValid) {
        if (!compileShaderSource(fragment, m_fragmentSource))
        {
            m_lastErrorMsg = getLastShaderError(fragment);
            glDeleteShader(fragment);
            GLERRORCHECK();
            return;
        }
    }
    
    if(isVertexSourceValid)
        glAttachShader(program, vertex);
    GLERRORCHECK();
    if(isFragmentSourceValid)
        glAttachShader(program, fragment);
    GLERRORCHECK();

    glLinkProgram(program);
    GLERRORCHECK();
    glValidateProgram(program);
    GLERRORCHECK();

    if(isVertexSourceValid)
        glDeleteShader(vertex);
    GLERRORCHECK();
    if(isFragmentSourceValid)
        glDeleteShader(fragment);
    GLERRORCHECK();

    m_isValid = true; // Success!
    m_shaderID = program;
}
