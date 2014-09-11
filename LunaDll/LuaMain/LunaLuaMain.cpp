#include "LunaLuaMain.h"
#include "../Level.h"
#include "../MiscFuncs.h"
#include <string>


std::string utf8_encode(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo( size_needed, 0 );
	WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

void dbg(std::wstring txt){
    MessageBoxW(0, txt.c_str(), L"Debug", 0);
}

void windowDebug(const char* debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void LunaLua::init(wstring main_path)
{
    mainState = luaL_newstate();
	luabind::open(mainState);
    luaL_openlibs(mainState);

	wstring full_path = main_path.append(Level::GetName());	
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\lunadll.lua");

	wifstream code_file(full_path, ios::binary|ios::in);
    if(!code_file.is_open()){
        code_file.close();
        return;
    }

	std::wstring wluacode((std::istreambuf_iterator<wchar_t>(code_file)), std::istreambuf_iterator<wchar_t>());
    code_file.close();
    std::string luacode = utf8_encode(wluacode);
    int errcode = luaL_dostring(mainState, luacode.c_str());
    
	using namespace luabind;
	module(mainState)
	[
        def("windowDebug", &windowDebug)
	];

    if(errcode == 0){
    }else{
        dbg(L"Error");
        return;
    }
	try
	{
        luabind::call_function<void>(mainState, "onLoad");
	}
	catch (error& e)
	{
		object error_msg(from_stack(e.state(), -1));
        windowDebug(object_cast<const char*>(error_msg));
	}

    lua_close(mainState);
}

