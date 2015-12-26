#include "../LuaProxy.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"
#include "../../UserSaves/UserSaving.h"



void LuaProxy::SaveBankProxy::setValue(const std::string& key, double value)
{
	gSavedVarBank.SetVar(utf8_decode(key), value);
}


luabind::object LuaProxy::SaveBankProxy::getValue(const std::string& key, lua_State* L)
{
    std::wstring wkey = utf8_decode(key);
	if(!gSavedVarBank.VarExists(wkey))
		return luabind::object();

	return luabind::object(L, gSavedVarBank.GetVar(wkey));
}


bool LuaProxy::SaveBankProxy::isValueSet(const std::string& key)
{
	return gSavedVarBank.VarExists(utf8_decode(key));
}


luabind::object LuaProxy::SaveBankProxy::values(lua_State *L)
{
	luabind::object valTable = luabind::newtable(L);

    std::map<std::wstring, double> cpMap;
	gSavedVarBank.CopyBank(&cpMap);
	for(std::map<std::wstring, double>::iterator it = cpMap.begin(); it != cpMap.end(); ++it) {
		valTable[utf8_encode(it->first.c_str())] = it->second;
	}
	return valTable;
}


void LuaProxy::SaveBankProxy::save()
{
	gSavedVarBank.WriteBank();
}