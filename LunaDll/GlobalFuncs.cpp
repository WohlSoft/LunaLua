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

/// INIT GLOBALS
void InitGlobals()
{
	//char* dbg = "GLOBAL INIT DBG";
	gLunaEnabled = true;
	gShowDemoCounter = false;
	gSkipSMBXHUD = false;
	gPrintErrorsToScreen = true;
	gLogger.m_Enabled = false;
	gIsOverworld = false;
	gCellMan.Reset();

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

	gCurrentMainPlayer = 1;

	gNumpad4 = 0;
	gNumpad8 = 0;
	gNumpad2 = 0;
	gNumpad6 = 0;

	gLevelEnum = Invalid;

	//startup settings default
	gStartupSettings.patch = false;
	gStartupSettings.noSound = false;
	gStartupSettings.frameskip = true;
	gStartupSettings.game = false;
	gStartupSettings.lvlEditor = false;
	gStartupSettings.debugger = false;
	gStartupSettings.logger = false;
	gStartupSettings.newLauncher = false;

	gHook_SkipTestMsgBox = false;

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

	// Allocate 40k bytes of free mem
	gpFreeGlob = NULL;
	gpFreeGlob = new int[10000];
	ResetFreeGlob();

	// Try to activate death counter, and disable it if there's some problem
	gDeathCounter.TryLoadStats();
	if(gDeathCounter.mStatFileOK == false)
		gDeathCounter.mEnabled = false;

	/// Init autocode manager	

	gAutoMan.Clear(true);
	gAutoMan.ReadGlobals(getModulePath());
	gAutoMan.m_GlobalEnabled = true;
}

/// RESET FREE GLOB
void ResetFreeGlob() {
	if(gpFreeGlob)
		ZeroMemory(gpFreeGlob, 10000 * sizeof(int));
}

/// CLEAN UP
void CleanUp() {
	if(gpFreeGlob)
		delete[] gpFreeGlob;
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

bool vecStrFind(const std::vector<std::string>& vecStr, const std::string& find)
{
	for(int i = 0; i < (int)vecStr.size(); ++i){
		if(vecStr[i] == find)
			return true;
	}
	return false;
}


using namespace std;
#include <sstream>
void resetDefines(){
	VASM_END_ANIM = 11;
	VASM_END_COINSOUND = 14;
	VASM_END_COINVAL = 1;

	GM_GRAVITY = 12;
	GM_JUMPHIGHT = 20;
	GM_JUMPHIGHT_BOUNCE = 20;

	wstring resetDefinies = getModulePath();
	resetDefinies = resetDefinies.append(L"\\resetdefines.txt");
    wifstream rdef(wstr2str(resetDefinies).c_str(), ios::binary|ios::in);
	if(!rdef.is_open()){
		return;
	}

	std::wstring wrdefCode((std::istreambuf_iterator<wchar_t>(rdef)), std::istreambuf_iterator<wchar_t>());
	rdef.close();

	vector<wstring> lines = wsplit(wrdefCode, L'\n');
	for(int i = 0; i < (int)lines.size(); ++i){
		wstring rdefLine = lines[i];
		vector<wstring> reDef = wsplit(rdefLine, L'\t');
		vector<wstring> clReDef;
		for(int j = 0; j < (int)reDef.size(); ++j){
			if(reDef[j].length()){
				clReDef.push_back(reDef[j]);
			}
		}
		if(clReDef.size() < 3)
			continue;


		DWORD addr;
		wstring addrType;
		double val;

		addr = wcstoul(clReDef[0].c_str(), NULL, 16);
		addrType = clReDef[1];
		val = wcstod(clReDef[2].c_str(), NULL);

		if(addrType == L"b"){
			*(BYTE*)addr = (BYTE)val;
		}else if(addrType == L"w"){
			*(WORD*)addr = (WORD)val;
		}else if(addrType == L"dw"){
			*(DWORD*)addr = (DWORD)val;
		}else if(addrType == L"f"){
			*(float*)addr = (float)val;
		}else if(addrType == L"df"){
			*(double*)addr = val;
		}
	}

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
