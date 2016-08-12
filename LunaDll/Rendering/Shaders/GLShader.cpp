#include "GLShader.h"

#include "../../GlobalFuncs.h"

#include <iostream>

static constexpr char* DefaultBaseVertexShaderSRC =
    "void main() {"
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
    "}";

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

GLShader::GLShader(const std::string& name, const std::string& fragmentSource) :
    GLShader(name, DefaultBaseVertexShaderSRC, fragmentSource)
{}

GLShader::~GLShader()
{
    if(m_isValid)
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


void GLShader::applyAttribute(const GLShaderVariableEntry& entry)
{
    
    GLuint bufID;
    glGenBuffers(1, &bufID);
    GLERRORCHECK();
    glBindBuffer(GL_ARRAY_BUFFER, bufID);
    GLERRORCHECK();
    glBufferData(GL_ARRAY_BUFFER, entry.getNumberOfElements() * sizeof(float), entry.getDataPtr(), GL_STATIC_DRAW);
    GLERRORCHECK();

    /*
    glGenVertexArrays(1, &bufID);
    GLERRORCHECK();
    glBindVertexArray(bufID);
    GLERRORCHECK();
    */

    glEnableVertexAttribArray(entry.getLocation());
    
    
    switch (entry.getGLType()) {
    case GL_FLOAT:              glVertexAttribPointer(entry.getLocation(), 1, GL_FLOAT, GL_FALSE, 0, 0); break;
    case GL_FLOAT_VEC2:         glVertexAttribPointer(entry.getLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0); break;
    case GL_FLOAT_VEC3:         glVertexAttribPointer(entry.getLocation(), 3, GL_FLOAT, GL_FALSE, 0, 0); break;
    case GL_FLOAT_VEC4:         glVertexAttribPointer(entry.getLocation(), 4, GL_FLOAT, GL_FALSE, 0, 0); break;
    case GL_INT:                glVertexAttribIPointer(entry.getLocation(), 1, GL_INT, 0, 0); break;
    case GL_INT_VEC2:           glVertexAttribIPointer(entry.getLocation(), 2, GL_INT, 0, 0); break;
    case GL_INT_VEC3:           glVertexAttribIPointer(entry.getLocation(), 3, GL_INT, 0, 0); break;
    case GL_INT_VEC4:           glVertexAttribIPointer(entry.getLocation(), 4, GL_INT, 0, 0); break;
    case GL_UNSIGNED_INT:       glVertexAttribIPointer(entry.getLocation(), 1, GL_UNSIGNED_INT, 0, 0); break;
    case GL_UNSIGNED_INT_VEC2:  glVertexAttribIPointer(entry.getLocation(), 2, GL_UNSIGNED_INT, 0, 0); break;
    case GL_UNSIGNED_INT_VEC3:  glVertexAttribIPointer(entry.getLocation(), 3, GL_UNSIGNED_INT, 0, 0); break;
    case GL_UNSIGNED_INT_VEC4:  glVertexAttribIPointer(entry.getLocation(), 4, GL_UNSIGNED_INT, 0, 0); break;
    case GL_DOUBLE:             glVertexAttribLPointer(entry.getLocation(), 1, GL_DOUBLE, 0, 0); break;
    case GL_DOUBLE_VEC2:        glVertexAttribLPointer(entry.getLocation(), 2, GL_DOUBLE, 0, 0); break;
    case GL_DOUBLE_VEC3:        glVertexAttribLPointer(entry.getLocation(), 3, GL_DOUBLE, 0, 0); break;
    case GL_DOUBLE_VEC4:        glVertexAttribLPointer(entry.getLocation(), 4, GL_DOUBLE, 0, 0); break;

    default:
        break;
    }

    GLERRORCHECK();


    /*
    switch (entry.getGLType()) {
    case GL_FLOAT:              glVertexAttrib1fv(entry.getLocation(), entry.getFloatPtr()); break;
    case GL_FLOAT_VEC2:         glVertexAttrib2fv(entry.getLocation(), entry.getFloatPtr()); break;
    case GL_FLOAT_VEC3:         glVertexAttrib3fv(entry.getLocation(), entry.getFloatPtr()); break;
    case GL_FLOAT_VEC4:         glVertexAttrib4fv(entry.getLocation(), entry.getFloatPtr()); break;
    case GL_INT:                glVertexAttribI1iv(entry.getLocation(), entry.getIntPtr()); break;
    case GL_INT_VEC2:           glVertexAttribI2iv(entry.getLocation(), entry.getIntPtr()); break;
    case GL_INT_VEC3:           glVertexAttribI3iv(entry.getLocation(), entry.getIntPtr()); break;
    case GL_INT_VEC4:           glVertexAttribI4iv(entry.getLocation(), entry.getIntPtr()); break;
    case GL_UNSIGNED_INT:       glVertexAttribI1uiv(entry.getLocation(), entry.getUIntPtr()); break;
    case GL_UNSIGNED_INT_VEC2:  glVertexAttribI2uiv(entry.getLocation(), entry.getUIntPtr()); break;
    case GL_UNSIGNED_INT_VEC3:  glVertexAttribI3uiv(entry.getLocation(), entry.getUIntPtr()); break;
    case GL_UNSIGNED_INT_VEC4:  glVertexAttribI4uiv(entry.getLocation(), entry.getUIntPtr()); break;
    case GL_DOUBLE:             glVertexAttrib1dv(entry.getLocation(), entry.getDoublePtr()); break;
    case GL_DOUBLE_VEC2:        glVertexAttrib2dv(entry.getLocation(), entry.getDoublePtr()); break;
    case GL_DOUBLE_VEC3:        glVertexAttrib3dv(entry.getLocation(), entry.getDoublePtr()); break;
    case GL_DOUBLE_VEC4:        glVertexAttrib4dv(entry.getLocation(), entry.getDoublePtr()); break;

    default:
    break;
    }
    */
}


void GLShader::applyUniform(const GLShaderVariableEntry& entry)
{
    
    switch (entry.getGLType()) {
    case GL_FLOAT:               glUniform1fv(entry.getLocation(), entry.getNumberOfElements(), entry.getFloatPtr()); break;
    case GL_FLOAT_VEC2:          glUniform2fv(entry.getLocation(), entry.getNumberOfElements(), entry.getFloatPtr()); break;
    case GL_FLOAT_VEC3:          glUniform3fv(entry.getLocation(), entry.getNumberOfElements(), entry.getFloatPtr()); break;
    case GL_FLOAT_VEC4:          glUniform4fv(entry.getLocation(), entry.getNumberOfElements(), entry.getFloatPtr()); break;
    case GL_FLOAT_MAT2:          glUniformMatrix2fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT3:          glUniformMatrix3fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT4:          glUniformMatrix4fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT2x3:        glUniformMatrix2x3fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT2x4:        glUniformMatrix2x4fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT3x2:        glUniformMatrix3x2fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT3x4:        glUniformMatrix3x4fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT4x2:        glUniformMatrix4x2fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_FLOAT_MAT4x3:        glUniformMatrix4x3fv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getFloatPtr()); break;
    case GL_INT:                 glUniform1iv(entry.getLocation(), entry.getNumberOfElements(), entry.getIntPtr()); break;
    case GL_INT_VEC2:            glUniform2iv(entry.getLocation(), entry.getNumberOfElements(), entry.getIntPtr()); break;
    case GL_INT_VEC3:            glUniform3iv(entry.getLocation(), entry.getNumberOfElements(), entry.getIntPtr()); break;
    case GL_INT_VEC4:            glUniform4iv(entry.getLocation(), entry.getNumberOfElements(), entry.getIntPtr()); break;
    case GL_UNSIGNED_INT:        glUniform1uiv(entry.getLocation(), entry.getNumberOfElements(), entry.getUIntPtr()); break;
    case GL_UNSIGNED_INT_VEC2:   glUniform2uiv(entry.getLocation(), entry.getNumberOfElements(), entry.getUIntPtr()); break;
    case GL_UNSIGNED_INT_VEC3:   glUniform3uiv(entry.getLocation(), entry.getNumberOfElements(), entry.getUIntPtr()); break;
    case GL_UNSIGNED_INT_VEC4:   glUniform4uiv(entry.getLocation(), entry.getNumberOfElements(), entry.getUIntPtr()); break;
    case GL_DOUBLE:              glUniform1dv(entry.getLocation(), entry.getNumberOfElements(), entry.getDoublePtr()); break;
    case GL_DOUBLE_VEC2:         glUniform2dv(entry.getLocation(), entry.getNumberOfElements(), entry.getDoublePtr()); break;
    case GL_DOUBLE_VEC3:         glUniform3dv(entry.getLocation(), entry.getNumberOfElements(), entry.getDoublePtr()); break;
    case GL_DOUBLE_VEC4:         glUniform4dv(entry.getLocation(), entry.getNumberOfElements(), entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT2:         glUniformMatrix2dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT3:         glUniformMatrix3dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT4:         glUniformMatrix4dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT2x3:       glUniformMatrix2x3dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT2x4:       glUniformMatrix2x4dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT3x2:       glUniformMatrix3x2dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT3x4:       glUniformMatrix3x4dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT4x2:       glUniformMatrix4x2dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;
    case GL_DOUBLE_MAT4x3:       glUniformMatrix4x3dv(entry.getLocation(), entry.getNumberOfElements(), false, entry.getDoublePtr()); break;

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
            glDeleteShader(vertex);
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
