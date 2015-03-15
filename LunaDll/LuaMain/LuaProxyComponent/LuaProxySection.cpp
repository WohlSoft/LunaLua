#include "../LuaProxy.h"
#include "../../Defines.h"


LuaProxy::Section::Section(int sectionNum)
{
	m_secNum = sectionNum;
}

LuaProxy::RECTd LuaProxy::Section::boundary() const
{
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (m_secNum * 6);

	RECTd rectd;
	rectd.left = boundtable[0];
	rectd.top = boundtable[1];
	rectd.bottom = boundtable[2];
	rectd.right = boundtable[3];

	return rectd;
}

void LuaProxy::Section::setBoundary(const LuaProxy::RECTd &boundary)
{
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (m_secNum * 6);
	boundtable[0] = boundary.left;
	boundtable[1] = boundary.top;
	boundtable[2] = boundary.bottom;
	boundtable[3] = boundary.right;
}

