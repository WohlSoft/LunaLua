#include "../RuntimeHook.h"
#include <windows.h>


void fixup_TypeMismatch13()
{
    // I still provide this code as a attempt to patch the conversion of string to float.
    // Unfortunatly I made me too much work and now I just patched the import table with my new code.
    // If someone wants to use it to get better understanding to ASM here you are:

    HMODULE vmVB6Lib = GetModuleHandleA("msvbvm60.dll");
    if (vmVB6Lib){
        //Get the function conversion code;
        BYTE* baseAddr = (BYTE*)GetProcAddress(vmVB6Lib, "__vbaR8Str");
        //Go to the function call asm to find out the address of the function that returns local language id 
        BYTE* funcAddrASM = baseAddr + 11;
        //Now get the relative address of this function
        DWORD relAddr = *((DWORD*)(funcAddrASM + 1));
        //Convert the relative address to an absolute address
        DWORD targetAddr = relAddr + (DWORD)funcAddrASM + 5;
        //Now get the pointer of the code
        BYTE* targetAddrBYTE = (BYTE*)targetAddr;

        // Normally this function would call another function called GetUserDefaultLCID.
        // In this case we overwrite the function and directly copy the return address to 0
        // After this code vb6 will think that this system has no language installed and
        // uses 1033 (US Code Page) as default. Now vb6 will always use "." as comma!
        DWORD oldprotect;
        // Now remove the protection to write to the code
        if (VirtualProtect((void*)targetAddrBYTE, 10, PAGE_EXECUTE_READWRITE, &oldprotect)){
            targetAddrBYTE[0] = 0xB8; //MOV EAX, 0
            targetAddrBYTE[1] = 0x00;
            targetAddrBYTE[2] = 0x00;
            targetAddrBYTE[3] = 0x00;
            targetAddrBYTE[4] = 0x00;
            targetAddrBYTE[5] = 0x90; //NOP
            // Now get the protection back
            VirtualProtect((void*)targetAddrBYTE, 10, oldprotect, &oldprotect);
        }

        /*std::string output = "";
        output += std::to_string((long long)targetAddrBYTE[0]) + " ";
        output += std::to_string((long long)targetAddrBYTE[1]) + " ";
        output += std::to_string((long long)targetAddrBYTE[2]) + " ";
        output += std::to_string((long long)targetAddrBYTE[3]) + " ";
        output += std::to_string((long long)targetAddrBYTE[4]) + " ";
        output += std::to_string((long long)targetAddrBYTE[5]) + " ";

        MessageBoxA(NULL, output.c_str(), "Dbg", NULL);*/
    }
}

