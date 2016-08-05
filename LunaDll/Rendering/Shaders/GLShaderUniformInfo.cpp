#include "GLShaderUniformInfo.h"


GLShaderUniformInfo::GLShaderUniformInfo(GLint id, GLint sizeOfVariable, GLint type, const std::string& name) : 
    GLShaderVariableInfo(GLShaderVariableInfo::Uniform, id, sizeOfVariable, type, name)
{}

GLShaderUniformInfo::~GLShaderUniformInfo()
{}
