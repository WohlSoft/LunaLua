#include "../LuaProxy.h"
#include "../../SMBXInternal/Layer.h"


luabind::object LuaProxy::Layer::get(lua_State *L)
{
    luabind::object layers = luabind::newtable(L);
    for (int i = 0; i < 100; ++i){
        ::LayerControl* l = LayerControl::Get(i);
        if (!l->ptLayerName)
            break;
        if (l->ptLayerName == std::wstring(L""))
            break;
        layers[i + 1] = new LuaProxy::Layer(i);
    }
    return layers;
}

luabind::object LuaProxy::Layer::get(const std::string& layerName, lua_State* L)
{
    for (int i = 0; i < 100; ++i){
        ::LayerControl* l = LayerControl::Get(i);
        if (!l->ptLayerName)
            break;
        if (l->ptLayerName == std::wstring(L""))
            break;
        if (l->ptLayerName == layerName)
            return luabind::object(L, new LuaProxy::Layer(i));
    }
    return luabind::object();
}

luabind::object LuaProxy::Layer::find(const std::string& layerName, lua_State* L)
{
    luabind::object foundLayers = luabind::newtable(L);
    for (int i = 0, j = 0; i < 100; ++i){
        ::LayerControl* l = LayerControl::Get(i);
        if (!l->ptLayerName)
            break;
        if (l->ptLayerName == std::wstring(L""))
            break;
        if (((std::string)l->ptLayerName).find(layerName) != std::string::npos){
            foundLayers[++j] = new LuaProxy::Layer(i);
        }
    }
    return foundLayers;
}



LuaProxy::Layer::Layer(int layerIndex)
{
	m_layerIndex = layerIndex;
}

LuaProxy::VBStr LuaProxy::Layer::layerName() const
{
	LayerControl* thislayer = ::Layer::Get(m_layerIndex);
	return VBStr(thislayer->ptLayerName.ptr);
}

float LuaProxy::Layer::speedX() const
{
	LayerControl* thislayer = ::Layer::Get(m_layerIndex);
	return (thislayer->xSpeed == 0.0001f ? 0 : thislayer->xSpeed);
}

void LuaProxy::Layer::setSpeedX(float speedX)
{
	LayerControl* thislayer = ::Layer::Get(m_layerIndex);
	::Layer::SetXSpeed(thislayer, speedX);
}

float LuaProxy::Layer::speedY() const
{
	LayerControl* thislayer = ::Layer::Get(m_layerIndex);
	return (thislayer->ySpeed == 0.0001f ? 0 : thislayer->ySpeed);
}

void LuaProxy::Layer::setSpeedY(float speedY)
{
	LayerControl* thislayer = ::Layer::Get(m_layerIndex);
	::Layer::SetYSpeed(thislayer, speedY);
}

void LuaProxy::Layer::stop()
{
	LayerControl* thislayer = ::Layer::Get(m_layerIndex);
	::Layer::Stop(thislayer);
}

int LuaProxy::Layer::layerIndex() const
{
	return m_layerIndex;
}
