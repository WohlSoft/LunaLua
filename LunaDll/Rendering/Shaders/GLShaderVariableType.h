#pragma once

enum class GLShaderVariableType : int {
    Attribute,  // For vertex shaders
    Uniform     // For fragment shaders
};

inline const char* getGLShaderVariableTypeName(GLShaderVariableType type) {
    switch (type) {
    case GLShaderVariableType::Attribute: return "Attribute";
    case GLShaderVariableType::Uniform: return "Uniform";
    }
    return "Unknown";
}

