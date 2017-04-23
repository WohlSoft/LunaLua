#include "GLShaderUniformInfo.h"

using namespace gl;

GLShaderUniformInfo::GLShaderUniformInfo(GLint id, GLint sizeOfVariable, GLenum type, const std::string& name) :
    GLShaderVariableInfo(GLShaderVariableType::Uniform, id, sizeOfVariable, type, name)
{}

GLShaderUniformInfo::~GLShaderUniformInfo()
{}
