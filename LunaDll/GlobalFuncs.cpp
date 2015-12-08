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


std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string wstr2str(const std::wstring &wstr)
{
	std::wstring ws = wstr;
	std::string s;
    const std::locale locale("");
    typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
    const converter_type& converter = std::use_facet<converter_type>(locale);
    std::vector<char> to(ws.length() * converter.max_length());
    std::mbstate_t state;
    const wchar_t* from_next;
    char* to_next;
    const converter_type::result result = converter.out(state,
		wstr.data(), wstr.data() + wstr.length(),
		from_next, &to[0], &to[0] + to.size(), to_next);
    if (result == converter_type::ok || result == converter_type::noconv)
	{
      s = std::string(&to[0], to_next);
    }
	return s;
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
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
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

    gLunaRender.ClearAll();
    gSpriteMan.ResetSpriteManager();
    gCellMan.Reset();
    gSavedVarBank.ClearBank();
    Input::ResetAll();

    gHook_SkipTestMsgBox = false;

    // Static default hitboxes and other values
    native_initStaticVals();
    native_initDefVals();

    gLunaRender.ReloadScreenHDC();
}

/// INIT GLOBALS
void InitGlobals()
{
    //char* dbg = "GLOBAL INIT DBG";
	
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
	gAutoMan.ReadGlobals(getModulePath());
	gAutoMan.m_GlobalEnabled = true;

    // Be sure that the config folder exist
    if (GetFileAttributesW(L"config") & INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryW(L"config", NULL);
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
	vector<std::wstring> ret;
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
	vector<std::string> ret;
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
    escaped << hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

bool vecStrFind(const std::vector<std::string>& vecStr, const std::string& find)
{
	for(int i = 0; i < (int)vecStr.size(); ++i){
		if(vecStr[i] == find)
			return true;
	}
	return false;
}


HMODULE getModule(std::string moduleName)
{
	HMODULE ret = 0;
	if( !(ret = GetModuleHandleA(moduleName.c_str())) ){
		ret = LoadLibraryA(moduleName.c_str());
	}
	return ret;
}

std::wstring getModulePath()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int count = GetModuleFileNameW(hModule, path, MAX_PATH);
	for(int i = count; i > 3; i--) {
		if(path[i] == L'\\') {
			path[i] = 0;
			break;
		}
	}
	return std::wstring(path);
}

bool readFile(std::wstring &content, std::wstring path, std::wstring errMsg /*= std::wstring()*/)
{
	wifstream theFile(path, ios::binary|ios::in);
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
    ifstream theFile(path, ios::binary | ios::in);
    if (!theFile.is_open()) {
        theFile.close();
        if (!errMsg.empty())
            MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
        return false;
    }

    content = std::string((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
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
    ofstream theFile(path, ios::binary | ios::out);
    if (!theFile.is_open()){
        theFile.close();
        return false;
    }
    theFile << content;
    theFile.close();
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
    WIN32_FIND_DATAA file_data;
    
    if ((dir = FindFirstFileA((path + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return out; /* No files found */

    do {
        const string file_name = file_data.cFileName;
        const bool skipFile = (file_data.dwFileAttributes & fileAttributes) == 0;
        
        if (file_name[0] == '.')
            continue;

        if (skipFile)
            continue;

        out.push_back(file_name);
    } while (FindNextFileA(dir, &file_data));

    FindClose(dir);

    return out;
}


std::wstring getCustomFolderPath()
{
    wstring world_dir = (wstring)GM_FULLDIR;
    wstring full_path = (gIsOverworld ? world_dir : world_dir.append(GM_LVLFILENAME_PTR));
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
        getModulePath() + L"//" + configname,
        getModulePath() + L"//config//" + configname
    });
}

void RedirectIOToConsole()
{
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;

    // allocate a console for this app
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 500;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen(hConHandle, "w");

    *stdout = *fp;

    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console

    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen(hConHandle, "w");

    *stderr = *fp;

    setvbuf(stderr, NULL, _IONBF, 0);

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    ios::sync_with_stdio();
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