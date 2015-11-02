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

void GLShader::bind()
{
    glUseProgram(m_shaderID);
}

void GLShader::unbind()
{
    glUseProgram(0);
}

GLuint GLShader::getAttribute(const std::string& name)
{
    std::cout << "Get Attribute: " << name << std::endl;
    GLuint ret = glGetAttribLocation(m_shaderID, name.c_str());
    GLERRORCHECK();
    return ret;
}

GLuint GLShader::getUniform(const std::string& name)
{
    std::cout << "Get Uniform: " << name << std::endl;
    GLuint ret = glGetUniformLocation(m_shaderID, name.c_str());
    GLERRORCHECK();
    return ret;
}

void GLShader::load()
{
    bool isVertexSourceValid = m_vertexSource != "";
    bool isFragmentSourceValid = m_vertexSource != "";
    if (!isVertexSourceValid && !isFragmentSourceValid)
        return;
    
    GLuint program = glCreateProgram();
    GLuint vertex; 
    GLuint fragment;
    if(isVertexSourceValid) 
        vertex = glCreateShader(GL_VERTEX_SHADER);
    if (isVertexSourceValid) 
        fragment = glCreateShader(GL_FRAGMENT_SHADER);

    
    if (isVertexSourceValid) {
        if (!compileShaderSource(vertex, m_vertexSource))
        {
            std::cout << "Failed to compile vertex shader: " << std::endl
                << getLastShaderError(vertex) << std::endl;
            glDeleteShader(vertex);
            return;
        }
    }
    if (isFragmentSourceValid) {
        if (!compileShaderSource(fragment, m_fragmentSource))
        {
            std::cout << "Failed to compile fragment shader: " << std::endl
                << getLastShaderError(fragment) << std::endl;
            glDeleteShader(fragment);
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

GLShader* GLShader::fromData(const std::string& name, const std::string& vertexSource, const std::string& fragementSource)
{
    return new GLShader(name, vertexSource, fragementSource);
}

GLShader* GLShader::fromFile(const std::string& name, const std::string& vertexFile, const std::string& fragmentFile)
{
    std::string vertexSource("");
    std::string fragementSource("");
    readFile(vertexSource, vertexFile);
    readFile(fragementSource, fragmentFile);
    return new GLShader(name, vertexSource, fragementSource);
}
