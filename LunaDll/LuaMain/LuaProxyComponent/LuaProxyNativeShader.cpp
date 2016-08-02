#include "../LuaProxy.h"

#include "../../GlobalFuncs.h"
#include "../../Rendering/GL/GLEngineProxy.h"

LuaProxy::NativeShader::NativeShader(ShaderType type) : 
    m_shaderType(type)
{}

void LuaProxy::NativeShader::compileFromSource(const std::string& name, const std::string& source, lua_State* L)
{
    // TODO: Make allocation on OpenGL side
    if (m_shaderType == SHADER_FRAGMENT) {
        m_internalShader = g_GLEngine.CreateNewShader(name, "", source);
    } else if (m_shaderType == SHADER_VERTEX) {
        m_internalShader = g_GLEngine.CreateNewShader(name, source, "");
    }

    if (!m_internalShader->isValid()) {
        luaL_error(L, "Failed to compile shader: %s", m_internalShader->getLastErrorMsg());
        return;
    }
}

void LuaProxy::NativeShader::compileFromFile(const std::string& name, const std::string& fileName, lua_State* L)
{
    std::string source;
    bool success = readFile(source, resolveIfNotAbsolutePath(fileName));
    if (!success) {
        luaL_error(L, "Failed to open file!");
        return;
    }

    compileFromSource(name, source, L);
}

