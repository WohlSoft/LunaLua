#include "../LuaProxy.h"

#include "../../GlobalFuncs.h"
#include "../../Rendering/GL/GLEngineProxy.h"

LuaProxy::NativeShader::NativeShader()
{}

void LuaProxy::NativeShader::compileFromSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, lua_State* L)
{
    m_internalShader = g_GLEngine.CreateNewShader(name, vertexSource, fragmentSource);
    
    if (!m_internalShader->isValid()) {
        luaL_error(L, "Failed to compile shader: %s", m_internalShader->getLastErrorMsg());
        return;
    }
}

void LuaProxy::NativeShader::compileFromFile(const std::string& name, const luabind::object& fileNameVertex, const luabind::object& fileNameFragment, lua_State* L)
{
    std::string vertexSource("");
    std::string fragmentSource("");

    auto readFromObject = [L](std::string& readTo, const luabind::object& readFrom, const std::string shaderType) -> bool {
        if (!readFrom.is_valid())
            return true;

        boost::optional<std::string> optionalFileName = luabind::object_cast_nothrow<std::string>(readFrom);
        if (optionalFileName == boost::none) {
            luaL_error(L, "Failed to read %s shader, filename is not a string!", shaderType.c_str());
            return false;
        }

        bool success = readFile(readTo, resolveIfNotAbsolutePath(*optionalFileName));
        if (!success) {
            luaL_error(L, "Failed to open file!");
            return false;
        }
        return true;
    };

    if (!readFromObject(vertexSource, fileNameVertex, "vertex"))
        return;
    if (!readFromObject(fragmentSource, fileNameFragment, "fragment"))
        return;

    compileFromSource(name, vertexSource, fragmentSource, L);
}

bool LuaProxy::NativeShader::isCompiled() const
{
    if (!m_internalShader)
        return false;
    return m_internalShader->isValid();
}

std::shared_ptr<GLShader> LuaProxy::NativeShader::getInternalShader() const
{
    return m_internalShader;
}

