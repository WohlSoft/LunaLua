#include "GLShaderUniformInfo.h"


GLShaderUniformInfo::GLShaderUniformInfo(GLint id, GLint sizeOfVariable, GLint type, const std::string& name) : 
    GLShaderVariableInfo(GLShaderVariableType::Uniform, id, sizeOfVariable, type, name)
{}

GLShaderUniformInfo::~GLShaderUniformInfo()
{}
