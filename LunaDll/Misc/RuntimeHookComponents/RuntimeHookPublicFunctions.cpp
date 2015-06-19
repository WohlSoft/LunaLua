#include "../RuntimeHook.h"
#include "../VB6StrPtr.h"

void emulateVB6Error(int errorCode)
{
    HMODULE vmVB6Lib = GetModuleHandleA("msvbvm60.dll");
    if (vmVB6Lib){
        void(__stdcall *vbaErrorFunc)(int) = (void(__stdcall *)(int))GetProcAddress(vmVB6Lib, "__vbaError");
        vbaErrorFunc(errorCode);
    }
}


void showSMBXMessageBox(std::string message)
{
    *(VB6StrPtr*)&(GM_STR_MSGBOX) = message;

    short arg1 = 1;
    native_msgbox(&arg1);
    GM_STR_MSGBOX = GM_STR_NULL;
}


extern void patchWholeNativeFunction(void* native_func, int sizeOfNativeFunc, void* newFunc)
{
    memset(native_func, INSTR_NOP, sizeOfNativeFunc);
    PATCH_JMP((DWORD)native_func, newFunc);
}
