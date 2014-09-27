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

void LunaLua::initCodeFile(lua_State *&L, wstring main_path, const char* chunckName)
{
    TryCloseState(L);

    L = luaL_newstate();
    luabind::open(L);
    lua_pushcfunction(L, luaopen_base);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_os);
    lua_call(L,0,0);


    wifstream code_file(main_path, ios::binary|ios::in);
    if(!code_file.is_open()){
        code_file.close();
        return;
    }

    using namespace luabind;

    std::wstring wluacode((std::istreambuf_iterator<wchar_t>(code_file)), std::istreambuf_iterator<wchar_t>());
    code_file.close();
    std::string luacode = utf8_encode(wluacode);

    //remove nearly all os code
    object _G = globals(L);
    object osTable = _G["os"];
    osTable["execute"] = object();
    osTable["exit"] = object();
    osTable["getenv"] = object();
    osTable["remove"] = object();
    osTable["rename"] = object();
    osTable["setlocal"] = object();
    osTable["tmpname"] = object();

    //reset data
    LuaProxy::evPlayer1.playerUPressing = 0;
    LuaProxy::evPlayer1.playerDPressing = 0;
    LuaProxy::evPlayer1.playerLPressing = 0;
    LuaProxy::evPlayer1.playerRPressing = 0;
    LuaProxy::evPlayer1.playerJPressing = 0;
    LuaProxy::evPlayer1.playerSJPressing = 0;
    LuaProxy::evPlayer1.playerXPressing = 0;
    LuaProxy::evPlayer1.playerRNPressing = 0;
    LuaProxy::evPlayer1.playerSELPressing = 0;
    LuaProxy::evPlayer1.playerSTRPressing = 0;
    LuaProxy::evPlayer1.playerJumping = 0;

    LuaProxy::evPlayer2.playerUPressing = 0;
    LuaProxy::evPlayer2.playerDPressing = 0;
    LuaProxy::evPlayer2.playerLPressing = 0;
    LuaProxy::evPlayer2.playerRPressing = 0;
    LuaProxy::evPlayer2.playerJPressing = 0;
    LuaProxy::evPlayer2.playerSJPressing = 0;
    LuaProxy::evPlayer2.playerXPressing = 0;
    LuaProxy::evPlayer2.playerRNPressing = 0;
    LuaProxy::evPlayer2.playerSELPressing = 0;
    LuaProxy::evPlayer2.playerSTRPressing = 0;
    LuaProxy::evPlayer2.playerJumping = 0;


    int errcode = luaL_loadbuffer(L, luacode.c_str(), luacode.length(), chunckName)  || lua_pcall(L, 0, LUA_MULTRET, 0);


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

    _G["KEY_UP"] = 0;
    _G["KEY_DOWN"] = 1;
    _G["KEY_LEFT"] = 2;
    _G["KEY_RIGHT"] = 3;
    _G["KEY_JUMP"] = 4;
    _G["KEY_SPINJUMP"] = 5;
    _G["KEY_X"] = 6;
    _G["KEY_RUN"] = 7;
    _G["KEY_SEL"] = 8;
    _G["KEY_STR"] = 9;




    module(L)
    [
        def("windowDebug", &LuaProxy::windowDebug),
        def("printText", (void(*)(const char*, int, int)) &LuaProxy::print),
        def("printText", (void(*)(const char*, int, int, int)) &LuaProxy::print),
        def("mem", (void(*)(int, LuaProxy::L_FIELDTYPE, luabind::object)) &LuaProxy::mem),
        def("mem", (luabind::object(*)(int, LuaProxy::L_FIELDTYPE, lua_State*)) &LuaProxy::mem),
        def("totalNPC", &LuaProxy::totalNPCs),
        def("npcs", &LuaProxy::npcs),
        def("findnpcs", &LuaProxy::findNPCs),
        def("triggerEvent", &LuaProxy::triggerEvent),
        def("playSFX", (void(*)(int))&LuaProxy::playSFX),
        def("playSFX", (void(*)(const char*))&LuaProxy::playSFX),

        namespace_("UserData")[
            def("setValue", &LuaProxy::SaveBankProxy::setValue),
            def("getValue", &LuaProxy::SaveBankProxy::getValue),
            def("isValueSet", &LuaProxy::SaveBankProxy::isValueSet),
            def("values", &LuaProxy::SaveBankProxy::values),
            def("save", &LuaProxy::SaveBankProxy::save)
        ],

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
            .def(constructor<int>())
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
            .property("isValid", &LuaProxy::Player::isValid)
    ];

    _G["player"] = new LuaProxy::Player();
    LuaProxy::Player* pl = new LuaProxy::Player(2);
    if(pl->isValid())
        _G["player2"] = pl;
    else
        delete pl;


    if(!(errcode == 0)){
        object error_msg(from_stack(L, -1));
        LuaProxy::windowDebug(object_cast<const char*>(error_msg));
        TryClose();
        return;
    }
    try
    {
        if(LuaHelper::is_function(L, "onLoad")){
            luabind::call_function<void>(L, "onLoad");
        }
    }
    catch (error& e)
    {
        object error_msg(from_stack(L, -1));
        LuaProxy::windowDebug(object_cast<const char*>(error_msg));
        TryClose();
    }
}

void LunaLua::init(wstring main_path)
{
	wstring globalPath = main_path;
	globalPath = globalPath.append(L"lunaworld.lua");
    initCodeFile(mainStateGlobal, globalPath, "lunaworld.lua");
	if(!mainStateGlobal)
		return;

    wstring full_path = main_path.append(Level::GetName());
    full_path = removeExtension(full_path);
    full_path = full_path.append(L"\\lunadll.lua");
    
    initCodeFile(mainState, full_path, "lunadll.lua");
    
}


void LunaLua::DoCodeFile(lua_State *L)
{
    PlayerMOB* demo = Player::Get(1);
    if(demo == 0 || L == 0)
        return;

    try
    {
        if(LuaHelper::is_function(L, "onLoop")){
            luabind::call_function<void>(L, "onLoop");
        }
        if(lastSection != demo->CurrentSection){
            std::string curSecLoop = "onLoadSection";

            if(LuaHelper::is_function(L, curSecLoop.c_str())){
                luabind::call_function<void>(L, curSecLoop.c_str()); //onLoadSection
            }
            curSecLoop = curSecLoop.append(std::to_string((long long)demo->CurrentSection));
            if(LuaHelper::is_function(L, curSecLoop.c_str())){
                luabind::call_function<void>(L, curSecLoop.c_str()); //onLoadSection#
            }

        }
        std::string curSecLoop = "onLoopSection";
        curSecLoop = curSecLoop.append(std::to_string((long long)demo->CurrentSection));
        if(LuaHelper::is_function(L, curSecLoop.c_str())){
            luabind::call_function<void>(L, curSecLoop.c_str());
        }
        LuaProxy::processKeyboardEvents(L);
        LuaProxy::processJumpEvent(L);
    }
    catch (luabind::error& e)
    {
        luabind::object error_msg(luabind::from_stack(L, -1));
        LuaProxy::windowDebug(luabind::object_cast<const char*>(error_msg));
        TryClose();
    }
}


void LunaLua::Do()
{
    PlayerMOB* demo = Player::Get(1);
    if(demo == 0)
        return;

	DoCodeFile(mainStateGlobal);
    DoCodeFile(mainState);
    
    LuaProxy::finishEventHandling();

    lastSection = demo->CurrentSection;
}


void LunaLua::TryClose()
{
    TryCloseState(mainState);
    TryCloseState(mainStateGlobal);
    lastSection = -1;
}


void LunaLua::TryCloseState(lua_State* &L)
{
    if(L)
        lua_close(L);

    L = 0;
}



