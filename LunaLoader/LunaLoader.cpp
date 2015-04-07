// LunaLoader.cpp : Loader for SMBX with LunaDLL
//

#include "stdio.h"
#include "tchar.h"
#include "windows.h"
#include <string>

#define dbgbox(msg) MessageBoxW(NULL, msg, L"Dbg", NULL);

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(_T("smbx.exe"), // Launch smbx.exe
        lpCmdLine,        // Command line
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
        dbgbox(L"Couldn't run SMBX");
        return 1;
    }

    unsigned char LoaderPatch1[] =
    {
        0xE9, 0x82, 0x61, 0x26, 0x00,  // 0x8BEA79 JMP 00B24C00
        0x90
    };

    unsigned char LoaderPatch2[] =
    {
        0x68, 0x64, 0x6C, 0x6C, 0x00, // 0xB24C00 PUSH "dll\0"
        0x68, 0x44, 0x6C, 0x6C, 0x2E, // 0xB24C05 PUSH "Dll."
        0x68, 0x4C, 0x75, 0x6E, 0x61, // 0xB24C0A PUSH "Luna"
        0x54,                         // 0xB24C0F PUSH ESP
        0xE8, 0x00, 0x00, 0x00, 0x00, // 0xB24C10 CALL LoadLibraryA
        0x83, 0xC4, 0x0C,             // 0xB24C15 ADD ESP, 0C
        0x31, 0xDB,                   // 0xB24C18 XOR EBX, EBX (just in case)
        0xA1, 0x04, 0xF3, 0xB2, 0x00, // 0xB24C1A MOV EAX, DWORD PTR DS : [B2F304]
        0x89, 0x9D, 0x88, 0xFD, 0xFF, 0xFF, // 00B24C1F mov dword ptr ss:[ebp-278],ebx
        0x39, 0xD8,                   // 00B24C25 CMP EAX, EBX
        0xE9, 0x53, 0x9E, 0xD9, 0xFF, // 00B24C27 JMP 008BEA7F
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };

    // Set LoadLibraryA address (will be the same in all processes)
    *(DWORD*)&LoaderPatch2[0x11] = (DWORD)&LoadLibraryA - (DWORD)(0xB24C10 + 5);

    // Patch the entry point...
    if (WriteProcessMemory(pi.hProcess, (void*)0x8BEA79, LoaderPatch1, sizeof(LoaderPatch1), NULL) == 0 ||
        WriteProcessMemory(pi.hProcess, (void*)0xB24C00, LoaderPatch2, sizeof(LoaderPatch2), NULL) == 0)
    {
        dbgbox(L"Failed to patch SMBX");
    }

    // Resume the main program thread
    ResumeThread(pi.hThread);

    // Close handles
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

	return 0;
}

