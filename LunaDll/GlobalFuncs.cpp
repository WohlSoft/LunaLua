#include <sstream>
#include "GlobalFuncs.h"
#include <iostream>
#include <clocale>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <locale>
#include <vector>
#include "Globals.h"
#include <ctype.h>
#include <cwctype>
#include "httprequest.h"
#include <comutil.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "Misc/MiscFuncs.h"
#include "Input/Input.h"
#include "SMBXInternal/Level.h"
#include "SMBXInternal/Blocks.h"
#include "SMBXInternal/NPCs.h"
#include "Misc/RuntimeHook.h"
#include "Utils/EncodeUtils.h"


/*!
 * \brief Returns size of UTF-8 string
 * \param s Input string
 * \return length in characters (not in bytes!)
 */
size_t utf8len(const char *s)
{
    size_t len = 0;
    while(*s)
        len += (*(s++)&0xC0)!=0x80;
    return len;
}

bool file_existsX(const std::string& name)
{
    if (FILE *file = _wfopen(LunaLua::EncodeUtils::Str2WStr(name).c_str(), L"r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

void removeFilePathW(std::wstring &path)
{
    for(int i = path.size(); i > 3; i--) {
        if((path[i] == L'\\')||(path[i] == L'/'))
        {
            path.resize(i);
            break;
        }
    }
}

void removeFilePathW(wchar_t*path, int length)
{
    for(int i = length; i > 3; i--) {
        if((path[i] == L'\\')||(path[i] == L'/'))
        {
            path[i] = 0;
            break;
        }
    }
}

void removeFilePathA(std::string &path)
{
    for(int i = path.size(); i > 3; i--) {
        if((path[i] == '\\')||(path[i] == '/'))
        {
            path.resize(i);
            break;
        }
    }
}

void removeFilePathA(char*path, int length)
{
    for(int i = length; i > 3; i--) {
        if((path[i] == '\\')||(path[i] == '/'))
        {
            path[i] = 0;
            break;
        }
    }
}

void ResetLunaModule() 
{
    gLunaEnabled = true;
    gShowDemoCounter = false;
    gPrintErrorsToScreen = true;
    gLogger.m_Enabled = false;
    gIsOverworld = false;

    gSMBXHUDSettings.skip = false;
    gSMBXHUDSettings.overworldHudState = WHUD_ALL;
    gSMBXHUDSettings.skipStarCount = false;

    gFrames = 0;

    gLastDownPress = 0;
    gDownTapped = 0;
    gLastUpPress = 0;
    gUpTapped = 0;
    gLastLeftPress = 0;
    gLeftTapped = 0;
    gLastRightPress = 0;
    gRightTapped = 0;

    gLastJumpPress = 0;
    gJumpTapped = 0;
    gLastRunPress = 0;
    gRunTapped = 0;

    gLevelEnum = Invalid;

    g_EventHandler.reset();
    gLunaRender.ClearAllDebugMessages();
    gSpriteMan.ResetSpriteManager();
    gCellMan.Reset();
    gSavedVarBank.ClearBank();
    Input::ResetAll();

    runtimeHookCharacterIdReset();

    gHook_SkipTestMsgBox = false;

    // Default properties
    Blocks::InitProperties();
    NPC::InitProperties();

    // Static default hitboxes and other values
    native_initStaticVals();
    native_initDefVals();
}

static bool IsWindowsVistaOrNewer() {
    OSVERSIONINFOEX osVersionInfo;
    DWORDLONG conditionMask = 0;

    memset(&osVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osVersionInfo.dwMajorVersion = 6;
    osVersionInfo.dwMinorVersion = 0;
    osVersionInfo.wServicePackMajor = 0;
    osVersionInfo.wServicePackMinor = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL);

    return VerifyVersionInfo(
        &osVersionInfo,
        VER_MAJORVERSION | VER_MINORVERSION |
        VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
        conditionMask);
}

/*!
 * \brief Initializes game root folder variables
 */
void initAppPaths()
{
    HMODULE hModule = GetModuleHandleW(NULL);
    wchar_t fullPath[MAX_PATH];
    int count = GetModuleFileNameW(hModule, fullPath, MAX_PATH);

    //Check is path has a mixed charsets
    std::string apath= LunaLua::EncodeUtils::WStr2StrA(std::wstring_view(fullPath));
    FILE *mainexe=fopen(apath.c_str(), "r");
    removeFilePathW(fullPath, count);

    if(!mainexe)
    {
        std::wstringstream msg;
        msg << L"LunaLUA is located in a path which contains mixed charsets (for example, your locale is configured to Europan, "
            << L"but in the path are Cyrillic or Chinese characters which are not fits into your local charset). "
            << L"SMBX will crash if you will continue running of it.\n\nPlease move SMBX folder into path which has "
            << L" only characters of your current locale or ASCII-only to avoid this warning message.\n\nDetected full path:\n"
            << fullPath;
        MessageBoxW(NULL, msg.str().c_str(), L"Mixed charsets in path has been detected", MB_OK|MB_ICONWARNING);
    } else {
        fclose(mainexe);
    }

    gAppPathWCHAR = fullPath;
    gAppPathUTF8 = LunaLua::EncodeUtils::WStr2Str(std::wstring_view(fullPath));
    gAppPathANSI = LunaLua::EncodeUtils::WStr2StrA(std::wstring_view(fullPath));
}

/// INIT GLOBALS
void InitGlobals()
{
    //char* dbg = "GLOBAL INIT DBG";
	
    gIsWindowsVistaOrNewer = IsWindowsVistaOrNewer();

	//startup settings default
    memset(&gStartupSettings, 0, sizeof(gStartupSettings));

	srand((int)time(NULL));

	// Get compatible handle for screen
	ghMemDC = CreateCompatibleDC(NULL);

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 800;
	bmi.bmiHeader.biHeight = -600;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	gpScreenBits = 0;

	ghGeneralDIB = CreateDIBSection(ghMemDC, &bmi, DIB_RGB_COLORS, (void**)&gpScreenBits, 0, 0);

	// Try to activate death counter, and disable it if there's some problem
	gDeathCounter.TryLoadStats();
	if(gDeathCounter.mStatFileOK == false)
		gDeathCounter.mEnabled = false;

	/// Init autocode manager	

	gAutoMan.Clear(true);
    gAutoMan.ReadGlobals(gAppPathWCHAR);
	gAutoMan.m_GlobalEnabled = true;

    std::wstring configFolderPath = gAppPathWCHAR+L"\\config";

    // Be sure that the config folder exist
    if (GetFileAttributesW(configFolderPath.c_str()) & INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryW(configFolderPath.c_str(), NULL);
    }

}

/// CLEAN UP
void CleanUp() {
    if(ghMemDC)
		DeleteObject(ghMemDC);
	DeleteObject(ghGeneralDIB);
}

bool readFile(std::wstring &content, std::wstring path, std::wstring errMsg /*= std::wstring()*/)
{
    std::wifstream theFile(path, std::ios::binary| std::ios::in);
	if(!theFile.is_open()){
		theFile.close();
		if(!errMsg.empty())
            MessageBoxW(NULL, errMsg.c_str(), L"Error", NULL);
		return false;
	}

	content = std::wstring((std::istreambuf_iterator<wchar_t>(theFile)), std::istreambuf_iterator<wchar_t>());
	return true;
}

bool readFile(std::string &content, std::string path, std::string errMsg /*= std::string()*/)
{
    std::wstring wpath = LunaLua::EncodeUtils::Str2WStr(path);
    FILE* theFile = _wfopen(wpath.c_str(), L"rb");
    if(!theFile)
    {
        if (!errMsg.empty())
            MessageBoxA(nullptr, errMsg.c_str(), "Error", 0);
        return false;
    }
    fseek(theFile, 0, SEEK_END);
    size_t len = ftell(theFile);
    rewind(theFile);
    if(len > 0)
    {
        content.resize(len);
        fread(&content[0], 1, len, theFile);
    }
    fclose(theFile);

    return true;
}

bool isAbsolutePath(const std::wstring& path)
{
	return std::iswalpha(path[0]) && path[1] == L':' && ((path[2] == L'\\') || (path[2] == L'/'));
}

bool isAbsolutePath(const std::string& path)
{
	return std::isalpha(path[0], std::locale("C")) && path[1] == L':' && ((path[2] == '\\') || (path[2] == '/'));
}

std::wstring resolveIfNotAbsolutePath(std::wstring filename)
{
    if (!isAbsolutePath(filename)) {
        return getCustomFolderPath() + filename;
    }
    else
    {
        return filename;
    }
}

std::string resolveIfNotAbsolutePath(std::string filename) {
    if (!isAbsolutePath(filename)) {
        return LunaLua::EncodeUtils::WStr2Str(getCustomFolderPath()) + filename;
    }
    else
    {
        return filename;
    }
}


std::string generateTimestamp(std::string format)
{
    std::time_t t = std::time(NULL);
    char mbstr[300];
    std::strftime(mbstr, sizeof(mbstr), format.c_str(), std::localtime(&t));
    return std::string(mbstr);
}

std::string generateTimestamp()
{
    return generateTimestamp("%Y-%m-%d %H %M %S");
}

std::string generateTimestampForFilename()
{
    return generateTimestamp("%Y-%m-%d_%H_%M_%S");
}

bool writeFile(const std::string &content, const std::string &path)
{
    std::wstring wpath = LunaLua::EncodeUtils::Str2WStr(path);
    FILE* theFile = _wfopen(wpath.c_str(), L"wb");
    if(!theFile)
    {
        return false;
    }
    fwrite(content.c_str(), 1, content.size(), theFile);
    fclose(theFile);
    return true;
}


void sendPUTRequest(const std::string& server, const std::string& data)
{
    HRESULT hr;
    CLSID clsid;
    IWinHttpRequest *pIWinHttpRequest = NULL;

    _variant_t varFalse(false);
    _variant_t varData(data.c_str());

    hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);
    if (SUCCEEDED(hr)){
        hr = CoCreateInstance(clsid, NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWinHttpRequest,
            (void **)&pIWinHttpRequest);
    }

    if (SUCCEEDED(hr)){
        hr = pIWinHttpRequest->SetTimeouts(1000, 1000, 2000, 1000);
    }

    if (SUCCEEDED(hr)){
        _bstr_t method("PUT");
        _bstr_t url(server.c_str());
        hr = pIWinHttpRequest->Open(method, url, varFalse);
    }

    if (SUCCEEDED(hr)){
        hr = pIWinHttpRequest->Send(varData);
    }

    pIWinHttpRequest->Release();
}

std::vector<std::string> listFilesOfDir(const std::string& path)
{
    return listOfDir(path, ~FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<std::string> listOfDir(const std::string& path, DWORD fileAttributes)
{
    std::vector<std::string> out;
    HANDLE dir;
    WIN32_FIND_DATAW file_data;
    std::wstring wpath = LunaLua::EncodeUtils::Str2WStr(path);
    
    if ((dir = FindFirstFileW((wpath + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return out; /* No files found */

    do {
        const std::wstring wfile_name = file_data.cFileName;
        const bool skipFile = (file_data.dwFileAttributes & fileAttributes) == 0;
        
        if (wfile_name[0] == L'.')
            continue;

        if (skipFile)
            continue;
        std::string file_name = LunaLua::EncodeUtils::WStr2Str(wfile_name);
        out.push_back(file_name);
    } while (FindNextFileW(dir, &file_data));

    FindClose(dir);

    return out;
}


std::wstring getCustomFolderPath()
{
    std::wstring world_dir = (std::wstring)GM_FULLDIR;
    std::wstring full_path = (gIsOverworld ? world_dir : world_dir.append(Level::GetName()));
    if (!gIsOverworld){
        full_path = removeExtension(full_path);
        full_path = full_path.append(L"\\"); // < path into level folder
    }
    return full_path;
}

std::wstring getLatestFile(const std::initializer_list<std::wstring>& paths)
{
    FILETIME newest = { 0 };
    std::wstring newestFileName = L"";

    for (const std::wstring& nextPath : paths) {
        if(GetFileAttributesW(nextPath.c_str()) == INVALID_FILE_ATTRIBUTES)
            continue; // File does not exist, continue with next.

        HANDLE hNextFile = CreateFileW(nextPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
        if(!hNextFile) // Failed to open file, continue with next.
            continue;

        FILETIME nextFileTime = { 0 };
        if (!GetFileTime(hNextFile, NULL, NULL, &nextFileTime)) {
            CloseHandle(hNextFile);
            continue;
        }

        if (CompareFileTime(&newest, &nextFileTime) < 0) {
            memcpy(&newest, &nextFileTime, sizeof(FILETIME));
            newestFileName = nextPath;
        }
        CloseHandle(hNextFile);
    }

    return newestFileName;
}

std::wstring getLatestConfigFile(const std::wstring& configname)
{
    return getLatestFile({
        gAppPathWCHAR + L"//" + configname,
        gAppPathWCHAR + L"//config//" + configname
    });
}

void RedirectIOToConsole()
{
    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    // allocate a console for this app
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 500;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
}


