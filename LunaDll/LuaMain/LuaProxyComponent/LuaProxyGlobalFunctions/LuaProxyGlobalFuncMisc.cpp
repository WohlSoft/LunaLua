#include "../../LuaProxy.h"
#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"
#include "../../../GameConfig/GameAutostart.h"
#include "../../../SMBXInternal/PlayerMOB.h"
#include "../../../EventStateMachine.h"

void LuaProxy::Misc::npcToCoins()
{
    native_npcToCoins();
}

void LuaProxy::Misc::doPOW()
{
    native_doPow();
}

void LuaProxy::Misc::doBombExplosion(double x, double y, short bombType)
{
    doBombExplosion(x, y, bombType, LuaProxy::Player(0));
}

void LuaProxy::Misc::doBombExplosion(double x, double y, short bombType, const LuaProxy::Player& playerObj) {
    Momentum position = { 0 };
    position.x = x;
    position.y = y;
    short playerIndex = playerObj.m_index;
    native_doBomb(&position, &bombType, &playerIndex);
}

void LuaProxy::Misc::doPSwitchRaw(bool activate) {
    short doPSwitchActivate = COMBOOL(activate);
    native_doPSwitch(&doPSwitchActivate);
}

void LuaProxy::Misc::doPSwitch() {
    doPSwitch(GM_PSWITCH_COUNTER == 0);
}

void LuaProxy::Misc::doPSwitch(bool activate) {
    if (activate)
        GM_PSWITCH_COUNTER = GM_PSWITCH_LENGTH - 1;
    else
        GM_PSWITCH_COUNTER = 0;

    short pswitchActive = COMBOOL(activate);
    
    if (activate) {
        native_stopMusic();
        native_playMusic(&pswitchActive);
        native_doPSwitch(&pswitchActive);
    } else {
        native_doPSwitch(&pswitchActive);
        native_stopMusic();
        native_playMusic(&::Player::Get(GM_MUSIC_RESTORE_PL)->CurrentSection);
    }
    
}

std::string LuaProxy::Misc::cheatBuffer()
{
    return (std::string)GM_INPUTSTR_BUF_PTR;
}

void LuaProxy::Misc::cheatBuffer(const luabind::object &value, lua_State* L)
{
    LuaHelper::assignVB6StrPtr(&GM_INPUTSTR_BUF_PTR, value, L);
    VB6StrPtr triggerStr = " ";
    native_updateCheatbuf(&triggerStr);
}


luabind::object LuaProxy::Misc::listFiles(const std::string& path, lua_State* L)
{
    luabind::object theList = luabind::newtable(L);
    std::string modulePath = path;
    if (!isAbsolutePath(path))
    {
        modulePath = WStr2Str(getModulePath());
        modulePath += "\\";
        modulePath += path;
    }
    std::vector<std::string> listedFiles = listFilesOfDir(modulePath);
    for (unsigned int i = 0; i < listedFiles.size(); ++i){
        theList[i + 1] = listedFiles[i];
    }
    return theList;

}


luabind::object LuaProxy::Misc::listLocalFiles(std::string path, lua_State* L)
{
    return listFiles(WStr2Str(getCustomFolderPath()) + path, L);
}

template<const DWORD FILTER>
luabind::object luabindResolveFile(std::string file, lua_State* L){
    std::vector<std::string> paths = {
        WStr2Str(getCustomFolderPath()),
        (std::string)GM_FULLDIR,
        WStr2Str(getModulePath()) + "\\LuaScriptsLib\\",
        WStr2Str(getModulePath()) + "\\"
    };



    for (std::string nextSearchPath : paths) {
        std::string nextEntry = nextSearchPath + file;
        DWORD objectAttributes = GetFileAttributesA(nextEntry.c_str());
        if(objectAttributes == INVALID_FILE_ATTRIBUTES)
            continue;
        if(objectAttributes & FILTER)
            return luabind::object(L, nextEntry);
    }

    return luabind::object();
}

luabind::object LuaProxy::Misc::resolveFile(const std::string& file, lua_State* L)
{
    return luabindResolveFile<~FILE_ATTRIBUTE_DIRECTORY>(file, L);
}

luabind::object LuaProxy::Misc::resolveDirectory(const std::string& directory, lua_State* L)
{
    return luabindResolveFile<FILE_ATTRIBUTE_DIRECTORY>(directory, L);
}

bool LuaProxy::Misc::isSamePath(const std::string first, const std::string second) 
{
    HANDLE hFileFirst = CreateFileA(first.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hFileFirst == INVALID_HANDLE_VALUE) return false;
    HANDLE hFileSecond = CreateFileA(second.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hFileSecond == INVALID_HANDLE_VALUE) {
        CloseHandle(hFileFirst);
        return false;
    }

    BY_HANDLE_FILE_INFORMATION fInfoFirst = { 0 };
    BY_HANDLE_FILE_INFORMATION fInfoSecond = { 0 };
    BOOL success = GetFileInformationByHandle(hFileFirst, &fInfoFirst);
    success = success && GetFileInformationByHandle(hFileSecond, &fInfoSecond);
    if (!success) {
        CloseHandle(hFileFirst);
        CloseHandle(hFileSecond);
        return false;
    }

    bool isSame = fInfoFirst.dwVolumeSerialNumber == fInfoSecond.dwVolumeSerialNumber &&
        fInfoFirst.nFileIndexLow == fInfoSecond.nFileIndexLow &&
        fInfoFirst.nFileIndexHigh == fInfoSecond.nFileIndexHigh;

    CloseHandle(hFileFirst);
    CloseHandle(hFileSecond);
    
    return isSame;
}

void LuaProxy::Misc::openPauseMenu()
{
    short player = 1;
    native_msgbox(&player);
}

void LuaProxy::Misc::saveGame()
{
    native_saveGame();
}

void LuaProxy::Misc::exitGame()
{
    GM_EPISODE_MODE = 0;
    GM_LEVEL_MODE = 0xFFFF;
}

bool LuaProxy::Misc::loadEpisode(const std::string& episodeName)
{
    GameAutostart autoStartEpisode;
    autoStartEpisode.setSelectedEpisode(episodeName);
    autoStartEpisode.setSaveSlot(GM_CUR_SAVE_SLOT);
    bool success = autoStartEpisode.applyAutostart();
    if (success)
        exitGame();
    return success;
}

void LuaProxy::Misc::pause()
{
    g_EventHandler.requestPause();
}

void LuaProxy::Misc::unpause()
{
    g_EventHandler.requestUnpause();
}

bool LuaProxy::Misc::isPausedByLua()
{
    return g_EventHandler.isPaused();
}

void LuaProxy::Misc::warning(const std::string& str)
{
    gLunaLua.setWarning(str);
}


void LuaProxy::Misc::updateWarp()
{
	native_updateWarp();
}