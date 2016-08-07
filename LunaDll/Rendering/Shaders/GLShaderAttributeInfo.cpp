#include "GLShaderAttributeInfo.h"


GLShaderAttributeInfo::GLShaderAttributeInfo(GLint id, GLint sizeOfVariable, GLint type, const std::string& name) :
    GLShaderVariableInfo(GLShaderVariableType::Attribute, id, sizeOfVariable, type, name)
{}

GLShaderAttributeInfo::~GLShaderAttributeInfo()
{}
