#include "LunaLuaMain.h"
#include "../Level.h"
#include "../MiscFuncs.h"
#include "../PlayerMOB.h"

#include "LuaHelper.h"
#include "LuaProxy.h"
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

void windowError(const char* errorText){
    MessageBoxA(0, errorText, "Error", 0);
}

void LunaLua::init(wstring main_path)
{
    TryClose();

    mainState = luaL_newstate();
	luabind::open(mainState);
    lua_pushcfunction(mainState, luaopen_base);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_math);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_string);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_table);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_debug);
    lua_call(mainState,0,0);
    lua_pushcfunction(mainState, luaopen_os);
    lua_call(mainState,0,0);


	wstring full_path = main_path.append(Level::GetName());	
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\lunadll.lua");

	wifstream code_file(full_path, ios::binary|ios::in);
    if(!code_file.is_open()){
        code_file.close();
        return;
    }
    using namespace luabind;

	std::wstring wluacode((std::istreambuf_iterator<wchar_t>(code_file)), std::istreambuf_iterator<wchar_t>());
    code_file.close();
    std::string luacode = utf8_encode(wluacode);

    //remove nearly all os code
    object _G = globals(mainState);
    object osTable = _G["os"];
    osTable["execute"] = object();
    osTable["exit"] = object();
    osTable["getenv"] = object();
    osTable["remove"] = object();
    osTable["rename"] = object();
    osTable["setlocal"] = object();
    osTable["tmpname"] = object();

    int errcode = luaL_loadbuffer(mainState, luacode.c_str(), luacode.length(), "lunadll.lua")  || lua_pcall(mainState, 0, LUA_MULTRET, 0);
    

    //constants
    _G["PLAYER_SMALL"] = 1;
    _G["PLAYER_BIG"] = 2;
    _G["PLAYER_FIREFLOWER"] = 3;
    _G["PLAYER_LEAF"] = 4;
    _G["PLAYER_TANOOKIE"] = 5;
    _G["PLAYER_HAMMER"] = 6;
    _G["PLAYER_ICE"] = 7;

    _G["FIND_ANY"] = -1;

    _G["DIR_RIGHT"] = 1;
    _G["DIR_RANDOM"] = 0;
    _G["DIR_LEFT"] = -1;

    _G["FIELD_BYTE"] = 1;
    _G["FIELD_WORD"] = 2;
    _G["FIELD_DWORD"] = 3;
    _G["FIELD_FLOAT"] = 4;
    _G["FIELD_DFLOAT"] = 5;
    _G["FIELD_STRING"] = 6;

	module(mainState)
	[
        def("windowDebug", &LuaProxy::windowDebug),
        def("printText", (void(*)(const char*, int, int)) &LuaProxy::print),
        def("printText", (void(*)(const char*, int, int, int)) &LuaProxy::print),
        def("mem", (void(*)(int, LuaProxy::L_FIELDTYPE, luabind::object)) &LuaProxy::mem),
        def("mem", (luabind::object(*)(int, LuaProxy::L_FIELDTYPE, lua_State*)) &LuaProxy::mem),
        def("totalNPC", &LuaProxy::totalNPCs),
        def("npcs", &LuaProxy::npcs),
        def("findnpcs", &LuaProxy::findNPCs),
        class_<RECT>("RECT")
            .def_readwrite("left", &RECT::left)
            .def_readwrite("top", &RECT::top)
            .def_readwrite("right", &RECT::right)
            .def_readwrite("bottom", &RECT::bottom),

        class_<LuaProxy::RECTd>("RECTd")
            .def_readwrite("left", &LuaProxy::RECTd::left)
            .def_readwrite("top", &LuaProxy::RECTd::top)
            .def_readwrite("right", &LuaProxy::RECTd::right)
            .def_readwrite("bottom", &LuaProxy::RECTd::bottom),

        class_<LuaProxy::Section>("Section")
            .def(constructor<int>())
            .property("boundary", &LuaProxy::Section::boundary, &LuaProxy::Section::setBoundary),

        class_<LuaProxy::NPC>("NPC")
            .def(constructor<int>())
            .def("mem", static_cast<void (LuaProxy::NPC::*)(int, LuaProxy::L_FIELDTYPE, luabind::object)>(&LuaProxy::NPC::mem))
            .def("mem", static_cast<luabind::object (LuaProxy::NPC::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::NPC::mem))
            .property("id", &LuaProxy::NPC::id)
            .property("direction", &LuaProxy::NPC::direction, &LuaProxy::NPC::setDirection)
            .property("x", &LuaProxy::NPC::x, &LuaProxy::NPC::setX)
            .property("y", &LuaProxy::NPC::y, &LuaProxy::NPC::setY)
            .property("speedX", &LuaProxy::NPC::speedX, &LuaProxy::NPC::setSpeedX)
            .property("speedY", &LuaProxy::NPC::speedY, &LuaProxy::NPC::setSpeedY),

        class_<LuaProxy::Player>("Player")
            .def(constructor<>())
			.def("mem", static_cast<void (LuaProxy::Player::*)(int, LuaProxy::L_FIELDTYPE, luabind::object)>(&LuaProxy::Player::mem))
			.def("mem", static_cast<luabind::object (LuaProxy::Player::*)(int, LuaProxy::L_FIELDTYPE, lua_State*)>(&LuaProxy::Player::mem))
            .def("kill", &LuaProxy::Player::kill)
            .def("harm", &LuaProxy::Player::harm)
            .property("screen", &LuaProxy::Player::screen)
            .property("section", &LuaProxy::Player::section)
            .property("sectionObj", &LuaProxy::Player::sectionObj)
            .property("x", &LuaProxy::Player::x, &LuaProxy::Player::setX)
            .property("y", &LuaProxy::Player::y, &LuaProxy::Player::setY)
            .property("speedX", &LuaProxy::Player::speedX, &LuaProxy::Player::setSpeedX)
            .property("speedY", &LuaProxy::Player::speedY, &LuaProxy::Player::setSpeedY)
            .property("powerup", &LuaProxy::Player::powerup, &LuaProxy::Player::setPowerup)
            .property("reservePowerup", &LuaProxy::Player::reservePowerup, &LuaProxy::Player::setReservePowerup)
            .property("holdingNPC", &LuaProxy::Player::holdingNPC)

	];

	_G["player"] = new LuaProxy::Player();

    if(!(errcode == 0)){
        object error_msg(from_stack(mainState, -1));
        LuaProxy::windowDebug(object_cast<const char*>(error_msg));
        TryClose();
        return;
    }
	try
	{
        if(LuaHelper::is_function(mainState, "onLoad")){
            luabind::call_function<void>(mainState, "onLoad");
        }
	}
	catch (error& e)
	{
        object error_msg(from_stack(mainState, -1));
        LuaProxy::windowDebug(object_cast<const char*>(error_msg));
        TryClose();
	}
}



void LunaLua::TryClose()
{
    if(mainState)
        lua_close(mainState);
    mainState = 0;
    lastSection = -1;
}

void LunaLua::Do()
{
    PlayerMOB* demo = Player::Get(1);
    if(demo == 0 || mainState == 0)
        return;

    try
    {
        if(LuaHelper::is_function(mainState, "onLoop")){
            luabind::call_function<void>(mainState, "onLoop");
        }

        if(lastSection != demo->CurrentSection){
            std::string curSecLoop = "onLoadSection";

            if(LuaHelper::is_function(mainState, curSecLoop.c_str())){
                luabind::call_function<void>(mainState, curSecLoop.c_str()); //onLoadSection
            }

            curSecLoop = curSecLoop.append(std::to_string((long long)demo->CurrentSection));
            if(LuaHelper::is_function(mainState, curSecLoop.c_str())){
                luabind::call_function<void>(mainState, curSecLoop.c_str()); //onLoadSection#
            }
            lastSection = demo->CurrentSection;
        }

        std::string curSecLoop = "onLoopSection";
        curSecLoop = curSecLoop.append(std::to_string((long long)demo->CurrentSection));
        if(LuaHelper::is_function(mainState, curSecLoop.c_str())){
            luabind::call_function<void>(mainState, curSecLoop.c_str());
        }
    }
    catch (luabind::error& e)
	{
		luabind::object error_msg(luabind::from_stack(mainState, -1));
		LuaProxy::windowDebug(luabind::object_cast<const char*>(error_msg));
		TryClose();
    }
}
