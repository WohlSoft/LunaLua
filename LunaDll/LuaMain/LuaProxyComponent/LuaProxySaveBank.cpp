#include "../LuaProxy.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"
#include "../../UserSaves/UserSaving.h"



void LuaProxy::SaveBankProxy::setValue(const char *key, double value)
{
	gSavedVarBank.SetVar(utf8_decode(std::string(key)), value);
}


luabind::object LuaProxy::SaveBankProxy::getValue(const char *key, lua_State* L)
{
	wstring wkey = utf8_decode(std::string(key));
	if(!gSavedVarBank.VarExists(wkey))
		return luabind::object();

	return luabind::object(L, gSavedVarBank.GetVar(wkey));
}


bool LuaProxy::SaveBankProxy::isValueSet(const char *key)
{
	return gSavedVarBank.VarExists(utf8_decode(std::string(key)));
}


luabind::object LuaProxy::SaveBankProxy::values(lua_State *L)
{
	luabind::object valTable = luabind::newtable(L);

	map<wstring, double> cpMap;
	gSavedVarBank.CopyBank(&cpMap);
	for(map<wstring, double>::iterator it = cpMap.begin(); it != cpMap.end(); ++it) {
		valTable[utf8_encode(it->first.c_str())] = it->second;
	}
	return valTable;
}


void LuaProxy::SaveBankProxy::save()
{
	gSavedVarBank.WriteBank();
}