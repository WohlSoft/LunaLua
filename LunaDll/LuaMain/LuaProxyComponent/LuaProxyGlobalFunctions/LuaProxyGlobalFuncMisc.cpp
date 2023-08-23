#include "../../LuaProxy.h"
#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"
#include "../../../GameConfig/GameAutostart.h"
#include "../../../SMBXInternal/PlayerMOB.h"
#include "../../../EventStateMachine.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Misc/Gui/RichTextDialog.h"
#include "../../../Misc/PerfTracker.h"

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
    Momentum position = { 0 };
    position.x = x;
    position.y = y;
    short playerIndex = 0;
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
    VB6StrPtr triggerStr = std::string(" ");
    native_updateCheatbuf(&triggerStr);
}


luabind::object listByAttributes(const std::string& path, DWORD attributes, lua_State* L) 
{
    luabind::object theList = luabind::newtable(L);
    std::string modulePath = path;
    if (!isAbsolutePath(path))
    {
        modulePath = gAppPathUTF8;
        modulePath += "\\";
        modulePath += path;
    }
    std::vector<std::string> listedFiles = listOfDir(path, attributes);
    for (unsigned int i = 0; i < listedFiles.size(); ++i) {
        theList[i + 1] = listedFiles[i];
    }
    return theList;
}

luabind::object LuaProxy::Misc::listFiles(const std::string& path, lua_State* L)
{
    return listByAttributes(path, ~FILE_ATTRIBUTE_DIRECTORY, L);
}

luabind::object LuaProxy::Misc::listDirectories(const std::string& path, lua_State* L)
{
    return listByAttributes(path, FILE_ATTRIBUTE_DIRECTORY, L);
}

luabind::object LuaProxy::Misc::listLocalFiles(const std::string& path, lua_State* L)
{
    return listFiles(WStr2Str(getCustomFolderPath()) + path, L);
}

luabind::object LuaProxy::Misc::listLocalDirectories(const std::string& path, lua_State* L)
{
    return listDirectories(WStr2Str(getCustomFolderPath()) + path, L);
}

template<const DWORD FILTER, const bool FILTER_TYPE>
luabind::object luabindResolveFile(const std::string& file, lua_State* L){
    std::vector<std::wstring> paths = {
        getCustomFolderPath(),
        GM_FULLDIR,
        gAppPathWCHAR + L"\\scripts\\",
        gAppPathWCHAR + L"\\"
    };

    std::wstring wFile = Str2WStr(file);
    for (std::wstring nextSearchPath : paths) {
        std::wstring nextEntry = nextSearchPath + wFile;
        DWORD objectAttributes = GetFileAttributesW(nextEntry.c_str());
        if(objectAttributes == INVALID_FILE_ATTRIBUTES)
            continue;
        if(((objectAttributes & FILTER) != 0) == FILTER_TYPE)
            return luabind::object(L, WStr2Str(nextEntry));
    }

    return luabind::object();
}

luabind::object LuaProxy::Misc::resolveFile(const std::string& file, lua_State* L)
{
    return luabindResolveFile<FILE_ATTRIBUTE_DIRECTORY, false>(file, L);
}

luabind::object LuaProxy::Misc::resolveDirectory(const std::string& directory, lua_State* L)
{
    return luabindResolveFile<FILE_ATTRIBUTE_DIRECTORY, true>(directory, L);
}

luabind::object LuaProxy::Misc::resolveGraphicsFile(const std::string& file, lua_State* L)
{
    std::vector<std::wstring> paths = {
        getCustomFolderPath(),
        GM_FULLDIR,
        gAppPathWCHAR + L"\\graphics\\"
    };
    std::wstring wFile = Str2WStr(file);

    // Add prefix folder as an option
    std::wstring wFilePrefix;
    if (wFile.find(L'/') == std::wstring::npos && wFile.find(L'\\') == std::wstring::npos)
    {
        size_t dashPos = wFile.find(L'-');
        if (dashPos != std::wstring::npos)
        {
            std::wstring dashPrefix = wFile.substr(0, dashPos);
            paths.push_back(gAppPathWCHAR + L"\\graphics\\" + dashPrefix + L"\\");
        }
    }

    for (std::wstring nextSearchPath : paths) {
        std::wstring nextEntry = nextSearchPath + wFile;
        DWORD objectAttributes = GetFileAttributesW(nextEntry.c_str());
        if (objectAttributes == INVALID_FILE_ATTRIBUTES)
            continue;
        if ((objectAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            return luabind::object(L, WStr2Str(nextEntry));
    }

    return luabind::object();
}

bool LuaProxy::Misc::isSamePath(const std::string first, const std::string second) 
{
    HANDLE hFileFirst = CreateFileW(Str2WStr(first).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hFileFirst == INVALID_HANDLE_VALUE) return false;
    HANDLE hFileSecond = CreateFileW(Str2WStr(second).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
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
    GM_CUR_MENUTYPE = 0;
}

void LuaProxy::Misc::exitEngine()
{
    _exit(0);
}

bool LuaProxy::Misc::loadEpisode(const std::string& episodeName)
{
    GameAutostart autoStartEpisode;
    autoStartEpisode.setSelectedEpisode(episodeName);
    autoStartEpisode.setSaveSlot(GM_CUR_SAVE_SLOT);
    bool success = autoStartEpisode.applyAutostart();
    if (success)
    {
        GM_EPISODE_MODE = 0;
        GM_LEVEL_MODE = 0xFFFF;
    }
    return success;
}

void LuaProxy::Misc::pause()
{
    g_EventHandler.requestPause(false);
}

void LuaProxy::Misc::pause(bool atFrameEnd)
{
    g_EventHandler.requestPause(atFrameEnd);
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

void LuaProxy::Misc::registerCharacterId(const luabind::object& namedArgs, lua_State* L)
{
    short id;
    short base;
    short filterBlock;
    short switchBlock;
    short deathEffect;
    std::string name;

    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, id);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, name, "");
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, base);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, filterBlock, 0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, switchBlock, 0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, deathEffect, 0);

    if (id >= 0 && id <= 5)
    {
        luaL_error(L, "Cannot register character id %d, it is reserved", id);
        return;
    }

    if (base < 1 || base > 5)
    {
        luaL_error(L, "Base character must be a vanilla character", base);
        return;
    }

    runtimeHookCharacterIdRegister(id, name, base, filterBlock, switchBlock, deathEffect);
}

std::string LuaProxy::Misc::showRichDialog(const std::string& title, const std::string& rtfText, bool isReadOnly)
{
    // Avoid pending keyboard events impacting this
    MSG msg;
    while (PeekMessageA(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE | PM_NOYIELD | PM_QS_INPUT) != 0)
    {
    }

    RichTextDialog dialog(title, rtfText, isReadOnly);
    dialog.show();
    return dialog.getRtfText();
}

// Internal use profiler functions
void LuaProxy::Misc::__enablePerfTracker()
{
    g_PerfTracker.enable();
}

void LuaProxy::Misc::__disablePerfTracker()
{
    g_PerfTracker.disable();
}

luabind::object LuaProxy::Misc::__getPerfTrackerData(lua_State* L)
{
    double snapshot[PerfTracker::PERF_MAX];
    if (!g_PerfTracker.getPerfSnapshot(snapshot))
    {
        // Return nil if no valid snapshot
        return luabind::object();
    }

    luabind::object retTable = luabind::newtable(L);
    for (int i = 0; i < PerfTracker::PERF_MAX; i++)
    {
        const char* typeName = PerfTracker::PerfTypeNames[i];
        retTable[typeName] = snapshot[i];
    }
    return retTable;
}

