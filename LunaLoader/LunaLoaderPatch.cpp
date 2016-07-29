#include <windows.h>
#include <string>
#include <cstring>
#include <stdint.h>
#include "LunaLoaderPatch.h"

static void setJmpAddr(uint8_t* patch, DWORD patchAddr, DWORD patchOffset, DWORD target) {
    DWORD* dwordAddr = (DWORD*)&patch[patchOffset+1];
    *dwordAddr = (DWORD)target - (DWORD)(patchAddr + patchOffset + 5);
}

LunaLoaderResult LunaLoaderRun(const wchar_t *pathToSMBX, const wchar_t *cmdLineArgs, const wchar_t *workingDir) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    
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
    uint32_t cmdLineMemoryLen = sizeof(wchar_t) * (strCmdLine.length() + 1); // Include null terminator
    wchar_t* cmdLine = (wchar_t*)malloc(cmdLineMemoryLen);
    std::memcpy(cmdLine, strCmdLine.c_str(), cmdLineMemoryLen);

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

    // Patch 1 (jump to Patch 2)
    uintptr_t LoaderPatchAddr1 = 0x40BDD8;
    unsigned char LoaderPatch1[] =
    {
        0xE9, 0x00, 0x00, 0x00, 0x00  // 0x40BDD8 JMP <Patch2>
    };

    // Patch 2 (loads LunaDll.dll)
    unsigned char LoaderPatch2[] =
    {
        0x68, 0x64, 0x6C, 0x6C, 0x00, // 00 PUSH "dll\0"
        0x68, 0x44, 0x6C, 0x6C, 0x2E, // 05 PUSH "Dll."
        0x68, 0x4C, 0x75, 0x6E, 0x61, // 0A PUSH "Luna"
        0x54,                         // 0F PUSH ESP
        0xE8, 0x00, 0x00, 0x00, 0x00, // 10 CALL LoadLibraryA
        0x83, 0xC4, 0x0C,             // 15 ADD ESP, 0C
        0x68, 0x6C, 0xC1, 0x40, 0x00, // 18 PUSH 40C16C (this inst used to be at 0x40BDD8)
        0xE9, 0x00, 0x00, 0x00, 0x00, // 1D JMP 40BDDD
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };

    // Allocate space for Patch 2
    DWORD LoaderPatchAddr2 = (DWORD)VirtualAllocEx(
        pi.hProcess,           // Target process
        NULL,                  // Don't request any particular address
        sizeof(LoaderPatch2),  // Length of Patch 2
        MEM_COMMIT,            // Type of memory allocation
        PAGE_READWRITE         // Memory protection type
        );
    if (LoaderPatchAddr2 == (DWORD)NULL) {
        return LUNALOADER_PATCH_FAIL;
    }

    // Set Patch1 Addresses
    setJmpAddr(LoaderPatch1, LoaderPatchAddr1, 0x00, LoaderPatchAddr2);

    // Set Patch2 Addresses
    setJmpAddr(LoaderPatch2, LoaderPatchAddr2, 0x10, (DWORD)&LoadLibraryA);
    setJmpAddr(LoaderPatch2, LoaderPatchAddr2, 0x1D, LoaderPatchAddr1 + 5);

    // Patch the entry point...
    if (WriteProcessMemory(pi.hProcess, (void*)LoaderPatchAddr1, LoaderPatch1, sizeof(LoaderPatch1), NULL) == 0 ||
        WriteProcessMemory(pi.hProcess, (void*)LoaderPatchAddr2, LoaderPatch2, sizeof(LoaderPatch2), NULL) == 0)
    {
        return LUNALOADER_PATCH_FAIL;
    }

    // Change Patch2 memory protection type
    DWORD TmpDword = 0;
    if (VirtualProtectEx(
        pi.hProcess,
        (void*)LoaderPatchAddr2,
        sizeof(LoaderPatch2),
        PAGE_EXECUTE,
        &TmpDword
    ) == 0) {
        return LUNALOADER_PATCH_FAIL;
    }

    // Resume the main program thread
    ResumeThread(pi.hThread);

    // Close handles
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return LUNALOADER_OK;
}
