#pragma once

#include <glbinding/gl/gl.h>
#include <string>
#include "GLShaderVariableType.h"

class GLShaderVariableInfo
{
public:
    enum VariableType {
        Attribute,
        Uniform
    };
    GLShaderVariableInfo(GLShaderVariableType varType, gl::GLint id, gl::GLint arrayCount, gl::GLenum type, const std::string& name);
    ~GLShaderVariableInfo();

    GLShaderVariableType getVarType() const;
    gl::GLint getId() const;
    gl::GLint arrayCount() const;
    gl::GLenum getType() const;
    std::string getName() const; // The name with array signature, if it is an array
    const char* getNamePtr() const;
    std::string getRawName() const; // The name without any array signature
    const char* getRawNamePtr() const;
    size_t getArrayDepth() const; // If it is 0, then it is not an array

private:
    GLShaderVariableType m_varType;
    gl::GLint m_id;
    gl::GLint m_arrayCount;
    gl::GLenum m_type;
    std::string m_name;
    std::string m_rawName;
    size_t m_arrayDepth;
};

