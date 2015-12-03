#include <windows.h>
#include <string>
#include <cstring>
#include "LunaLoaderPatch.h"

LunaLoaderResult LunaLoaderRun(const wchar_t *pathToSMBX, const wchar_t *cmdLineArgs, const wchar_t *workingDir) {
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    
    // Prepare command line
    size_t pos = 0;
    std::wstring quotedPathToSMBX(pathToSMBX);
    while ((pos = quotedPathToSMBX.find(L"\"", pos)) != std::string::npos) {
        quotedPathToSMBX.replace(pos, 1, L"\\\"");
        pos += 2;
    }
    std::wstring strCmdLine = (
        std::wstring(L"\"") + quotedPathToSMBX + std::wstring(L"\" ") +
        std::wstring(cmdLineArgs)
        );
    wchar_t* cmdLine = (wchar_t*)malloc(sizeof(wchar_t) * strCmdLine.length());
    std::memcpy(cmdLine, strCmdLine.c_str(), sizeof(wchar_t) * strCmdLine.length());

    // Create process
    if (!CreateProcessW(pathToSMBX, // Launch smbx.exe
        cmdLine,          // Command line
        NULL,             // Process handle not inheritable
        NULL,             // Thread handle not inheritable
        FALSE,            // Set handle inheritance to FALSE
        CREATE_SUSPENDED, // Create in suspended state
        NULL,             // Use parent's environment block
        workingDir,       // Use parent's starting directory 
        &si,              // Pointer to STARTUPINFO structure
        &pi)              // Pointer to PROCESS_INFORMATION structure
        )
    {
        free(cmdLine); cmdLine = NULL;
        return LUNALOADER_CREATEPROCESS_FAIL;
    }
    free(cmdLine); cmdLine = NULL;

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
        return LUNALOADER_PATCH_FAIL;
    }

    // Resume the main program thread
    ResumeThread(pi.hThread);

    // Close handles
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return LUNALOADER_OK;
}