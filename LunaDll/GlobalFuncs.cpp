#include <sstream>
#include "GlobalFuncs.h"
#include <iostream>
#include <clocale>
#include <stdlib.h>
#include <ctype.h>
#include <cstdio>
#include <cstdarg>
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
#include <mutex>

#include "Misc/MiscFuncs.h"
#include "Input/Input.h"
#include "SMBXInternal/Level.h"
#include "SMBXInternal/Blocks.h"
#include "SMBXInternal/NPCs.h"
#include "Misc/RuntimeHook.h"

void splitStr(std::vector<std::string>& dest, const std::string& str, const char* separator)
{
    dest.clear();
    std::string st=str;
    while ( true )
    {
        size_t pos = str.find_first_of( separator );
        std::string cur = st.substr( 0, pos );
        dest.push_back( cur );
        if ( pos == std::string::npos )
            break;
        st = st.substr( pos + 1 );
    }
}

void replaceSubStr(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
    return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void replaceSubStrW(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void RemoveSubStr(std::string& sInput, const std::string& sub)
{
    std::string::size_type foundpos = sInput.find(sub);
    if ( foundpos != std::string::npos )
    sInput.erase(sInput.begin() + foundpos, sInput.begin() + foundpos + sub.length());
}

std::vector<std::string> splitCmdArgs( std::string str)
{
    std::vector<std::string> args;
    std::string arg;
    arg.clear();
    bool quote_opened=false;
    for(unsigned int i=0; i<str.size();i++)
    {
        if(quote_opened)
            goto qstr;
        if(str[i] == ' ')
        {
            if(!arg.empty())
                args.push_back(arg);
            arg.clear();
            continue;
        }
        if(str[i] == '\"')
        {
            quote_opened=true;
            continue;
        }
        arg.push_back(str[i]);
    continue;

        qstr:
            if(str[i] == '\"')
            {
                if(!arg.empty())
                    args.push_back(arg);
                arg.clear();
                quote_opened=false;
                continue;
            }
            arg.push_back(str[i]);
    }

    if(!arg.empty())
                args.push_back(arg);

    return args;
}


std::vector<std::wstring> splitCmdArgsW(std::wstring str)
{
    std::vector<std::wstring> args;
    std::wstring arg;
    arg.clear();
    bool quote_opened = false;
    for (unsigned int i = 0; i<str.size(); i++)
    {
        if (quote_opened)
            goto qstr;
        if (str[i] == L' ')
        {
            if (!arg.empty())
                args.push_back(arg);
            arg.clear();
            continue;
        }
        if (str[i] == L'\"')
        {
            quote_opened = true;
            continue;
        }
        arg.push_back(str[i]);
        continue;

    qstr:
        if (str[i] == L'\"')
        {
            if (!arg.empty())
                args.push_back(arg);
            arg.clear();
            quote_opened = false;
            continue;
        }
        arg.push_back(str[i]);
    }

    if (!arg.empty())
        args.push_back(arg);

    return args;
}

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

std::wstring Str2WStr(const std::string &str)
{
    std::wstring dest;
    dest.resize(str.size());
    int newlen=MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &dest[0], str.length());
    dest.resize(newlen);
    return dest;
}

std::string WStr2Str(const std::wstring &wstr)
{
    std::string dest;
    dest.resize((wstr.size()*2));
    int newlen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &dest[0], dest.size(), NULL, NULL);
    dest.resize(newlen);
    return dest;
}

std::string WStr2StrA(const std::wstring &wstr)
{
    std::string dest;
    dest.resize((wstr.size()*2));
    int newlen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), &dest[0], dest.size(), NULL, NULL);
    dest.resize(newlen);
    return dest;
}

std::wstring StrA2WStr(const std::string &str)
{
    std::wstring dest;
    dest.resize(str.size());
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &dest[0], str.length());
    return dest;
}

std::wstring GetNonANSICharsFromWStr(const std::wstring& wstr)
{
    std::wstring ansi = StrA2WStr(WStr2StrA(wstr));
    std::wstring mismatches;

    for (size_t i = 0, j = 0; (i < wstr.size()) && (j < ansi.size()); i++)
    {
        if (wstr[i] != ansi[j])
        {
            mismatches += wstr[i];
            if (ansi[j] == L'\xFFFD')
            {
                j++;
            }
        }
        else
        {
            j++;
        }
    }

    return mismatches;
}

std::string ConvertWCSToMBS(const wchar_t * pstr, long wslen)
{
    int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

    std::string dblstr(len, '\0');
    len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
        pstr, wslen /* not necessary NULL-terminated */,
        &dblstr[0], len,
        NULL, NULL /* no default char */);

    return dblstr;
}

std::string ConvertBSTRToMBS(BSTR bstr)
{
    int wslen = ::SysStringLen(bstr);
    return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

BSTR ConvertMBSToBSTR(const std::string & str)
{
    int wslen = ::MultiByteToWideChar(CP_ACP, 0 /* no flags */,
        str.data(), str.length(),
        NULL, 0);

    BSTR wsdata = ::SysAllocStringLen(NULL, wslen);
    ::MultiByteToWideChar(CP_ACP, 0 /* no flags */,
        str.data(), str.length(),
        wsdata, wslen);
    return wsdata;
}

std::string i2str(int source)
{
    std::stringstream s;
    s<<source;
    return s.str();
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (isdigit(*it))) ++it;
    return !s.empty() && it == s.end();
}


bool file_existsX(const std::string& name)
{
    if (FILE *file = _wfopen(Str2WStr(name).c_str(), L"r")) {
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
    gPrintErrorsToScreen = true;
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
    Renderer::Get().ClearAllDebugMessages();
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

bool IsWindowsVersionOrNewer(DWORD major, DWORD minor)
{
    OSVERSIONINFOEXA osVersionInfo;
    DWORDLONG conditionMask = 0;

    memset(&osVersionInfo, 0, sizeof(osVersionInfo));
    osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
    osVersionInfo.dwMajorVersion = major;
    osVersionInfo.dwMinorVersion = minor;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    return VerifyVersionInfoA(
        &osVersionInfo,
        VER_MAJORVERSION | VER_MINORVERSION |
        VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
        conditionMask);
}

FARPROC Luna_GetProc_impl(const char* libFilename, const char* procName)
{
    HMODULE mod = LoadLibraryA(libFilename);
    if (mod == nullptr) return nullptr;

    return GetProcAddress(mod, procName);
}

/*!
 * \brief Initializes game root folder variables
 */
void initAppPaths()
{
    HMODULE hModule = GetModuleHandleW(NULL);
    wchar_t fullPath[MAX_PATH];
    int count = GetModuleFileNameW(hModule, fullPath, MAX_PATH);
    removeFilePathW(fullPath, count);

    // Check for path having characters which are not compatible with the system default Windows ANSI code page
    std::wstring nonAnsiChars = GetNonANSICharsFromWStr(fullPath);
    if (!nonAnsiChars.empty())
    {
        std::wstring path = L"SMBX2 has been installed in a path with characters which are not compatible with the system default Windows ANSI code page. This is not currently supported. Please install SMBX2 in a location without unsupported characters.\n\nUnsupported characters: " + nonAnsiChars + L"\n\nPath:\n" + fullPath;
        MessageBoxW(0, path.c_str(), L"Invalid SMBX Installation Path", MB_ICONERROR);
        _exit(1);
    }

    // Check for UNC path
    if ((fullPath[0] == L'\\') && (fullPath[1] == L'\\'))
    {
        std::wstring path = L"SMBX2 cannot be run from a UNC path (starting with \\\\). Please install SMBX2 elsewhere or map the network drive to a drive letter.\n\nPath:\n" + std::wstring(fullPath);
        MessageBoxW(0, path.c_str(), L"Invalid SMBX Installation Path", MB_ICONERROR);
        _exit(1);
    }

    // Check for path that might otherwise cause weird problems
    if (!(
        (
            ((fullPath[0] >= L'A') && (fullPath[0] <= L'Z')) ||
            ((fullPath[0] >= L'a') && (fullPath[0] <= L'z'))
        ) &&
        (fullPath[1] == L':') &&
        (fullPath[2] == L'\\')
        ))
    {
        std::wstring path = L"The SMBX2 installation path is not recognized as having a normal drive letter.\n\nPath:\n" + std::wstring(fullPath);
        MessageBoxW(0, path.c_str(), L"Invalid SMBX Installation Path", MB_ICONERROR);
        _exit(1);
    }

    gAppPathWCHAR = fullPath;
    gAppPathUTF8 = WStr2Str(gAppPathWCHAR);
    gAppPathANSI = WStr2StrA(gAppPathWCHAR);

    // Store intial cwd
    DWORD cwdLen = GetCurrentDirectoryW(0, NULL);
    wchar_t* cwdPath = new wchar_t[cwdLen + 1];
    GetCurrentDirectoryW(cwdLen + 1, cwdPath);
    gCwdPathWCHAR = cwdPath;
    gCwdPathUTF8 = WStr2Str(cwdPath);
    gCwdPathANSI = WStr2StrA(cwdPath);
    delete[] cwdPath;

    // Set cwd to app path
    SetCurrentDirectoryW(gAppPathWCHAR.c_str());
}

/// INIT GLOBALS
void InitGlobals()
{
    //char* dbg = "GLOBAL INIT DBG";

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





std::vector<std::wstring> wsplit( std::wstring str, wchar_t delimiter )
{
    std::vector<std::wstring> ret;
    while ( true )
    {
        size_t pos = str.find_first_of( delimiter );
        std::wstring cur = str.substr( 0, pos );
        ret.push_back( cur );
        if ( pos == std::wstring::npos )
            break;
        str = str.substr( pos + 1 );
    }
    return ret;
}

std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> ret;
    while ( true )
    {
        size_t pos = str.find_first_of( delimiter );
        std::string cur = str.substr( 0, pos );
        ret.push_back( cur );
        if ( pos == std::string::npos )
            break;
        str = str.substr( pos + 1 );
    }
    return ret;
}


std::string url_encode(const std::string &value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

bool vecStrFind(const std::vector<std::wstring>& vecStr, const std::wstring& find)
{
    for(int i = 0; i < (int)vecStr.size(); ++i){
        if(vecStr[i] == find)
            return true;
    }
    return false;
}

// Function to normalize a path, in such a way that all slashes become forward
// slashes, duplicate consecutive slashes are removed, and trailing slashes are
// removed.
std::wstring normalizePathSlashes(const std::wstring& input)
{
    std::wstring out;
    out.reserve(input.length());

    bool haveSlash = false;
    for (const wchar_t & c : input)
    {
        if ((c == L'/') || (c == L'\\'))
        {
            haveSlash = true;
        }
        else
        {
            if (haveSlash)
            {
                out += L'/';
                haveSlash = false;
            }
            out += c;
        }
    }

    return out;
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
    std::wstring wpath = Str2WStr(path);
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

std::wstring resolveCwdOrWorldsPath(const std::wstring& path)
{
    std::wstring fullPath;
    if (isAbsolutePath(path)) {
        fullPath = normalizePathSlashes(path);
        replaceSubStrW(fullPath, L"/", L"\\");
    }
    else
    {
        fullPath = normalizePathSlashes(gCwdPathWCHAR + L"\\" + path);
        replaceSubStrW(fullPath, L"/", L"\\");
        if (!fileExists(fullPath))
        {
            std::wstring worldsPath = normalizePathSlashes(gAppPathWCHAR + L"\\worlds\\" + path);
            replaceSubStrW(worldsPath, L"/", L"\\");
            if (fileExists(worldsPath))
            {
                fullPath = worldsPath;
            }
        }
    }
    return fullPath;
}

std::wstring resolveIfNotAbsolutePath(std::wstring filename)
{
    if (isAbsolutePath(filename)) {
        return filename;
    }

    std::vector<std::wstring> paths = {
        getCustomFolderPath(),
        GM_FULLDIR,
        gAppPathWCHAR + L"\\scripts\\",
        gAppPathWCHAR
    };

    for (std::wstring nextSearchPath : paths) {
        std::wstring nextEntry = nextSearchPath + filename;
        DWORD objectAttributes = GetFileAttributesW(nextEntry.c_str());
        if (objectAttributes == INVALID_FILE_ATTRIBUTES) continue;
        if (objectAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        return nextEntry;
    }

    return filename;
}

std::string resolveIfNotAbsolutePath(std::string filename) {
    if (isAbsolutePath(filename)) {
        return filename;
    }

    std::vector<std::wstring> paths = {
        getCustomFolderPath(),
        GM_FULLDIR,
        gAppPathWCHAR + L"\\scripts\\",
        gAppPathWCHAR
    };

    std::wstring wFilename = Str2WStr(filename);
    for (std::wstring nextSearchPath : paths) {
        std::wstring nextEntry = nextSearchPath + wFilename;
        DWORD objectAttributes = GetFileAttributesW(nextEntry.c_str());
        if (objectAttributes == INVALID_FILE_ATTRIBUTES) continue;
        if (objectAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        return WStr2Str(nextEntry);
    }

    return filename;
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
    std::wstring wpath = Str2WStr(path);
    FILE* theFile = _wfopen(wpath.c_str(), L"wb");
    if(!theFile)
    {
        return false;
    }
    fwrite(content.c_str(), 1, content.size(), theFile);
    fclose(theFile);
    return true;
//    std::ofstream theFile(path, std::ios::binary | std::ios::out);
//    if (!theFile.is_open()){
//        theFile.close();
//        return false;
//    }
//    theFile << content;
//    theFile.close();
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
    std::wstring wpath = Str2WStr(path);

    if ((dir = FindFirstFileW((wpath + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return out; /* No files found */

    do {
        const std::wstring wfile_name = file_data.cFileName;
        const bool skipFile = (file_data.dwFileAttributes & fileAttributes) == 0;

        if (wfile_name[0] == L'.')
            continue;

        if (skipFile)
            continue;
        std::string file_name = WStr2Str(wfile_name);
        out.push_back(file_name);
    } while (FindNextFileW(dir, &file_data));

    FindClose(dir);

    return out;
}

std::wstring getEpisodeFolderPath()
{
    std::wstring full_path = (std::wstring)GM_FULLDIR;
    full_path = full_path.append(L"\\"); // < path into episode folder
    return full_path;
}

std::wstring getCustomFolderPath()
{
    std::wstring world_dir = (std::wstring)GM_FULLDIR;
    std::wstring full_path = (gIsOverworld ? world_dir : world_dir.append(Level::GetName()));
    if (!gIsOverworld){
        full_path = RemoveExtension(full_path);
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
    std::wstring ret = getLatestFile({
        gAppPathWCHAR + L"/" + configname,
        gAppPathWCHAR + L"/config/" + configname
    });
    
    if (ret.empty())
    {
        ret = gAppPathWCHAR + L"/config/" + configname;
    }

    return ret;
}

void InitDebugConsole()
{

    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    // allocate a console for this app
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    conout = stdout;

    // set the screen buffer to be big enough to let us scroll text
    SetConsoleTitleA("LunaLua Debug Console");
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 500;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
}

int DebugPrint(const char * format, ...)
{
    if (conout)
    {
        va_list args;
        va_start(args, format);
        int ret = vfprintf_s(conout, format, args);
        va_end(args);
        fflush(conout);
        return ret;
    }
    return 0;
}

void DebugClear(HANDLE hConsole)
{
    if (conout)
    {
        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        SMALL_RECT scrollRect;
        COORD scrollTarget;
        CHAR_INFO fill;

        // Get the number of character cells in the current buffer.
        if (!GetConsoleScreenBufferInfo(hConsole, &coninfo))
        {
            return;
        }

        // Scroll the rectangle of the entire buffer.
        scrollRect.Left = 0;
        scrollRect.Top = 0;
        scrollRect.Right = coninfo.dwSize.X;
        scrollRect.Bottom = coninfo.dwSize.Y;

        // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
        scrollTarget.X = 0;
        scrollTarget.Y = (SHORT)(0 - coninfo.dwSize.Y);

        // Fill with empty spaces with the buffer's default text attribute.
        fill.Char.UnicodeChar = TEXT(' ');
        fill.Attributes = coninfo.wAttributes;

        // Do the scroll
        ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

        // Move the cursor to the top left corner too.
        coninfo.dwCursorPosition.X = 0;
        coninfo.dwCursorPosition.Y = 0;

        SetConsoleCursorPosition(hConsole, coninfo.dwCursorPosition);
    }
}

#ifdef BUILD_WITH_ATL_STUFF
// WIP
#include <atlbase.h>
#include "Misc/TypeLib.h"

#define COMUTILS_RETURN_IF_FAILED_START HRESULT ___errCodeUse_
#define COMUTILS_RETURN_IF_FAILED(code, outputter, text) \
    ___errCodeUse_ = code; \
    if(FAILED(___errCodeUse_)){ \
        outputter << L"ERROR: " << text << L" HRESULT = " << std::hex << (int)___errCodeUse_ << std::dec << std::endl; \
        return; \
    }

void dumpTYPEATTR(TYPEATTR* attr, std::wostream& toOutput) {
    //toOutput << L"GUID: " << std::hex << attr->guid.Data1 << L"-" << attr->guid.Data2 << "-" << attr->guid.Data3 << "-"
//        << *(long long*)(&attr->guid.Data4) << std::dec << std::endl;
    toOutput << L"Num of functions: " << attr->cFuncs << std::endl;
    toOutput << L"Num of vars: " << attr->cVars << std::endl;
    toOutput << L"Num of implemented interfaces: " << attr->cImplTypes << std::endl;
    toOutput << L"Version number: " << attr->wMajorVerNum << L"." << attr->wMinorVerNum << std::endl;

}

void dumpTypeLibrary(IDispatch* dispatchToDump, std::wostream& toOutput)
{
    toOutput << L"Start dumping!" << std::endl;
    COMUTILS_RETURN_IF_FAILED_START;

    UINT typeLibCount = 0;
    COMUTILS_RETURN_IF_FAILED(dispatchToDump->GetTypeInfoCount(&typeLibCount), toOutput, L"Failed to get type count");

    toOutput << "Type Library count: " << typeLibCount << std::endl;
    if (typeLibCount == 0) return;

    ATL::CComPtr<ITypeInfo> typeInfoOfObj = NULL;
    COMUTILS_RETURN_IF_FAILED(dispatchToDump->GetTypeInfo(NULL, LOCALE_SYSTEM_DEFAULT, &typeInfoOfObj), toOutput, L"Failed to get type info");
    toOutput << L"DEBUG: Got type info!" << std::endl;
    toOutput << L"DEBUG: Ptr to ITypeLib: " << std::hex << (int)typeInfoOfObj.p << std::dec << std::endl;

    ATL::CComPtr<ITypeLib> typeLibOfObj = NULL;
    UINT index = 0;
    COMUTILS_RETURN_IF_FAILED(typeInfoOfObj->GetContainingTypeLib(&typeLibOfObj, &index), toOutput, L"Failed to get type lib");
    toOutput << "DEBUG: Got type lib with index " << index << std::endl;

    /*

    TYPEATTR* descriptor = NULL;
    COMUTILS_RETURN_IF_FAILED(typeInfoOfObj->GetTypeAttr(&descriptor), toOutput, L"Failed to get descriptor!");
    toOutput << L"DEBUG: Got descriptor!" << std::endl;
    toOutput << std::hex << (UINT)descriptor << std::dec << std::endl;

    dumpTYPEATTR(descriptor, toOutput);


    typeInfoOfObj->ReleaseTypeAttr(descriptor);

    toOutput << L"DEBUG: Released descriptor!" << std::endl;

    */

}


#endif

void ShowAndFocusWindow(HWND hWindow)
{
    if (!hWindow) return;

    DWORD otherProcessId = 0;
    DWORD otherThreadId = GetWindowThreadProcessId(GetForegroundWindow(), &otherProcessId);
    DWORD thisProcessId = 0;
    DWORD thisThreadId = GetWindowThreadProcessId(hWindow, &thisProcessId);

    if (thisThreadId != otherThreadId)
    {
        AttachThreadInput(otherThreadId, thisThreadId, TRUE);
        SetForegroundWindow(hWindow);
        AttachThreadInput(otherThreadId, thisThreadId, FALSE);
    }
    else
    {
        SetForegroundWindow(hWindow);
    }


    if (IsIconic(hWindow))
    {
        ShowWindow(hWindow, SW_RESTORE);
    }
    else
    {
        ShowWindow(hWindow, SW_SHOW);
    }

    BringWindowToTop(hWindow);
}

void HandleEventsWhileLoading()
{
    static DWORD lastTime = 0;
    DWORD thisTime = GetTickCount();
    DWORD elapsedTime = thisTime - lastTime;
    if (elapsedTime > 100)
    {
        // Run if >100ms has elapsed since last event handling
        native_rtcDoEvents();
        lastTime = thisTime;
    }
}

std::string GetEditorPlacedItem()
{
    std::lock_guard<std::mutex> editorEntityIPCLock(g_editorIPCMutex);
    return (std::string)gEditorPlacedItem;
}
