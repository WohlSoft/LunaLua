#include "../LuaProxy.h"
#include "../../Layer/Layer.h"



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
