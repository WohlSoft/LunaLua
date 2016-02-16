// LunaLoader.cpp : Loader for SMBX with LunaDLL
//

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <vector>
#include "LunaLoaderPatch.h"

#define ErrorBox(msg, title) MessageBox(NULL, _T(msg), _T(title), MB_ICONERROR)

std::vector<std::wstring> splitCmdArgs(std::wstring str)
{
    std::vector<std::wstring> args;
    std::wstring arg;
    arg.clear();
    bool quote_opened = false;
    for (unsigned int i = 0; i < str.size(); i++)
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

std::wstring dirnameOf(const std::wstring& fname)
{
	size_t pos = fname.find_last_of(L"\\/");
	return (std::wstring::npos == pos)
		? L""
		: fname.substr(0, pos);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    std::vector<std::wstring> cmdArgs = splitCmdArgs(lpCmdLine);
    std::wstring newCmdLine = lpCmdLine;

	std::wstring curPath;

	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	WCHAR** lppPart = { NULL };
	WCHAR fulPath_s[4096] = L"";
	GetFullPathName(szFileName, 4096, fulPath_s, lppPart);
	curPath = dirnameOf(fulPath_s);
    
    std::wstring pathToSMBX = curPath+L"\\smbx.exe"; //use either smbx.exe or the first arg
	if (pathToSMBX == fulPath_s) // If LunaLoader.exe renamed into "SMBX.exe"
		pathToSMBX = curPath + L"\\smbx.legacy";

    if (cmdArgs.size() > 0) { //if more than one arg then possible of a smbx path
        if (cmdArgs[0].find(L"--") == std::wstring::npos) { //if the first arg starting with "--" then no smbx path --> just starting with argument
            pathToSMBX = cmdArgs[0];

            newCmdLine = L""; //reset the arg path
            if (cmdArgs.size() > 1){ //strip the smbx-path from the arguments
                for (unsigned int i = 1; i < cmdArgs.size(); ++i){
                    newCmdLine += cmdArgs[i] + std::wstring(L" ");
                    if (i == cmdArgs.size() - 1){ //if the last argument of the args list then remove the last space.
                        std::wstring::iterator it = newCmdLine.end();
                        newCmdLine.erase(--it);
                    }
                }
            }
        }
    }

	FILE *test;
	_wfopen_s(&test, pathToSMBX.c_str(), L"rb");
	if (!test) {
		pathToSMBX = curPath+L"\\smbx.legacy";
	}
	else fclose(test);

    LunaLoaderResult ret = LunaLoaderRun(pathToSMBX.c_str(), newCmdLine.c_str(), curPath.c_str());

    switch(ret) {
    case LUNALOADER_OK:
        return 0;
    case LUNALOADER_CREATEPROCESS_FAIL:
        ErrorBox("Error: Couldn't run SMBX\nHowever you can try to drag and drop the SMBX exe file to LunaLoader.exe\nIf this doesn't work then ask on the forums.", "Couldn't run SMBX");
        return 1;
    case LUNALOADER_PATCH_FAIL:
        ErrorBox("Error: Failed to patch SMBX", "Failed to patch SMBX");
        return 1;
    default:
        ErrorBox("Unknown Error in LunaLoader.", "Unknown Error");
        return 1;
    }
}

