#include "../LuaProxy.h"
#include "../../SMBXInternal/Layer.h"


luabind::object LuaProxy::Layer::get(lua_State *L)
{
    luabind::object layers = luabind::newtable(L);
    for (int i = 0; i < 255; ++i){
        ::LayerControl* l = LayerControl::Get(i);
        if (!l->ptLayerName)
            break;
        if (l->ptLayerName == std::wstring(L""))
            break;
        layers[i + 1] = LuaProxy::Layer(i);
    }
    return layers;
}

luabind::object LuaProxy::Layer::get(const std::string& layerName, lua_State* L)
{
    for (int i = 0; i < 255; ++i){
        ::LayerControl* l = LayerControl::Get(i);
        if (!l->ptLayerName)
            break;
        if (l->ptLayerName == std::wstring(L""))
            break;
        if (l->ptLayerName == layerName)
            return luabind::object(L, LuaProxy::Layer(i));
    }
    return luabind::object();
}

luabind::object LuaProxy::Layer::find(const std::string& layerName, lua_State* L)
{
    luabind::object foundLayers = luabind::newtable(L);
    for (int i = 0, j = 0; i < 255; ++i){
        ::LayerControl* l = LayerControl::Get(i);
        if (!l->ptLayerName)
            break;
        if (l->ptLayerName == std::wstring(L""))
            break;
        if (((std::string)l->ptLayerName).find(layerName) != std::string::npos){
            foundLayers[++j] = LuaProxy::Layer(i);
        }
    }
    return foundLayers;
}



LuaProxy::Layer::Layer(int layerIndex)
{
    m_layerIndex = layerIndex;
}

int LuaProxy::Layer::idx() const
{
    return m_layerIndex;
}

std::string LuaProxy::Layer::layerName() const
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    return thislayer->ptLayerName;
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

bool LuaProxy::Layer::isHidden()
{
    return 0 != ::Layer::Get(m_layerIndex)->isHidden;
}


void LuaProxy::Layer::stop()
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    ::Layer::Stop(thislayer);
}

void LuaProxy::Layer::show(bool noSmoke)
{
    short noSmokeNative = COMBOOL(noSmoke);
    native_showLayer(&::Layer::Get(m_layerIndex)->ptLayerName, &noSmokeNative);
}

void LuaProxy::Layer::hide(bool noSmoke)
{
    short noSmokeNative = COMBOOL(noSmoke);
    native_hideLayer(&::Layer::Get(m_layerIndex)->ptLayerName, &noSmokeNative);
}

void LuaProxy::Layer::toggle(bool noSmoke)
{
    if (::Layer::Get(m_layerIndex)->isHidden == -1){
        show(noSmoke);
    }
    else{
        hide(noSmoke);
    }
}


int LuaProxy::Layer::layerIndex() const
{
    return m_layerIndex;
}

