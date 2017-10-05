#pragma once

#include <gl/glew.h>
#include <string>
#include "GLShaderVariableType.h"

class GLShaderVariableInfo
{
public:
    enum VariableType {
        Attribute,
        Uniform
    };
    GLShaderVariableInfo(GLShaderVariableType varType, GLint id, GLint arrayCount, GLint type, const std::string& name);
    ~GLShaderVariableInfo();

    GLShaderVariableType getVarType() const;
    GLint getId() const;
    GLint arrayCount() const;
    GLenum getType() const;
    std::string getName() const; // The name with array signature, if it is an array
    const char* getNamePtr() const;
    std::string getRawName() const; // The name without any array signature
    const char* getRawNamePtr() const;
    size_t getArrayDepth() const; // If it is 0, then it is not an array

private:
    GLShaderVariableType m_varType;
    GLint m_id;
    GLint m_arrayCount;
    GLenum m_type;
    std::string m_name;
    std::string m_rawName;
    size_t m_arrayDepth;
};

