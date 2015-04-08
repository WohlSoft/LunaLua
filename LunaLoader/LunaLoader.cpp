// LunaLoader.cpp : Loader for SMBX with LunaDLL
//

#include "stdio.h"
#include "tchar.h"
#include "windows.h"
#include <string>
#include <vector>

#define ErrorBox(title, msg) MessageBox(NULL, _T(title), _T(msg), MB_ICONERROR)

std::vector<std::string> splitCmdArgs(std::string str)
{
    std::vector<std::string> args;
    std::string arg;
    arg.clear();
    bool quote_opened = false;
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (quote_opened)
            goto qstr;
        if (str[i] == ' ')
        {
            if (!arg.empty())
                args.push_back(arg);
            arg.clear();
            continue;
        }
        if (str[i] == '\"')
        {
            quote_opened = true;
            continue;
        }
        arg.push_back(str[i]);
        continue;

    qstr:
        if (str[i] == '\"')
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

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    std::vector<std::string> cmdArgs = splitCmdArgs(lpCmdLine);
    std::string newCmdLine = lpCmdLine;

    std::string pathToSMBX = "smbx.exe"; //use either smbx.exe or the first arg
    if (cmdArgs.size() > 0){ //if more than one arg then possible of a smbx path
        if (cmdArgs[0].find("--") == std::string::npos){ //if the first arg starting with "--" then no smbx path --> just starting with argument
            pathToSMBX = cmdArgs[0];

            newCmdLine = ""; //reset the arg path
            if (cmdArgs.size() > 1){ //strip the smbx-path from the arguments
                for (unsigned int i = 1; i < cmdArgs.size(); ++i){
                    newCmdLine += cmdArgs[i] + std::string(" ");
                }
            }
        }
    }

    char* cmdLineNonConst = new char[newCmdLine.length() + 1];
    memcpy((void*)cmdLineNonConst, (const void*)newCmdLine.c_str(), newCmdLine.length());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(pathToSMBX.c_str(), // Launch smbx.exe
        cmdLineNonConst,        // Command line
        NULL,             // Process handle not inheritable
        NULL,             // Thread handle not inheritable
        FALSE,            // Set handle inheritance to FALSE
        CREATE_SUSPENDED, // Create in suspended state
        NULL,             // Use parent's environment block
        NULL,             // Use parent's starting directory 
        &si,              // Pointer to STARTUPINFO structure
        &pi)              // Pointer to PROCESS_INFORMATION structure
        )
    {
        ErrorBox("Error: Couldn't run SMBX\nHowever you can try to drag and drop the SMBX exe file to LunaLoader.exe\nIf this doesn't work then ask on the forums.", "Couldn't run SMBX");
        return 1;
    }

    unsigned char LoaderPatch1[] =
    {
        0xE9, 0x23, 0x8E, 0x71, 0x00  // 0x40BDD8 JMP 00B24C00
    };

    unsigned char LoaderPatch2[] =
    {
        0x68, 0x64, 0x6C, 0x6C, 0x00, // 0xB24C00 PUSH "dll\0"
        0x68, 0x44, 0x6C, 0x6C, 0x2E, // 0xB24C05 PUSH "Dll."
        0x68, 0x4C, 0x75, 0x6E, 0x61, // 0xB24C0A PUSH "Luna"
        0x54,                         // 0xB24C0F PUSH ESP
        0xE8, 0x00, 0x00, 0x00, 0x00, // 0xB24C10 CALL LoadLibraryA
        0x83, 0xC4, 0x0C,             // 0xB24C15 ADD ESP, 0C
        0x68, 0x6C, 0xC1, 0x40, 0x00, // 0xB24C18 PUSH 40C16C
        0xE9, 0xBB, 0x71, 0x8E, 0xFF, // 0xB24C1D JMP 40BDDD
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };

    // Set LoadLibraryA address (will be the same in all processes)
    *(DWORD*)&LoaderPatch2[0x11] = (DWORD)&LoadLibraryA - (DWORD)(0xB24C10 + 5);

    // Patch the entry point...
    if (WriteProcessMemory(pi.hProcess, (void*)0x40BDD8, LoaderPatch1, sizeof(LoaderPatch1), NULL) == 0 ||
        WriteProcessMemory(pi.hProcess, (void*)0xB24C00, LoaderPatch2, sizeof(LoaderPatch2), NULL) == 0)
    {
        ErrorBox("Error: Failed to patch SMBX", "Failed to patch SMBX");
    }

    // Resume the main program thread
    ResumeThread(pi.hThread);

    // Close handles
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

	return 0;
}

