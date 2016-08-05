#include "GLShaderAttributeInfo.h"


GLShaderAttributeInfo::GLShaderAttributeInfo(GLint id, GLint sizeOfVariable, GLint type, const std::string& name) :
    GLShaderVariableInfo(GLShaderVariableInfo::Attribute, id, sizeOfVariable, type, name)
{}

GLShaderAttributeInfo::~GLShaderAttributeInfo()
{}
