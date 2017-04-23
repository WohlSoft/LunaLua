#include "GLShaderAttributeInfo.h"

using namespace gl;

GLShaderAttributeInfo::GLShaderAttributeInfo(GLint id, GLint sizeOfVariable, GLenum type, const std::string& name) :
    GLShaderVariableInfo(GLShaderVariableType::Attribute, id, sizeOfVariable, type, name)
{}

GLShaderAttributeInfo::~GLShaderAttributeInfo()
{}
