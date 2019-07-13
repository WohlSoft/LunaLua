#include "../../Defines.h"
#include "../../Globals.h"
//#include "../RuntimeHook.h"

static int __stdcall playerGetHitboxH(PlayerMOB* player)
{
    // Standing height
    return 32 + player->FacingDirection * 16;
}

static int __stdcall playerGetHitboxHD(PlayerMOB* player)
{
    // Ducking height
    return 32 + player->FacingDirection * 16;
}

static int __stdcall playerGetHitboxW(PlayerMOB* player)
{
    // Width
    return 64;
}

// out edx
// in ebx
// len 44
_declspec(naked) void __stdcall playerGetHitboxHD_99986D()
{
    __asm {
        PUSHFD
        PUSH EAX
        PUSH ECX
        //PUSH EDX

        PUSH EBX // Player
        CALL playerGetHitboxHD

        MOV EDX, EAX

        //POP EDX
        POP ECX
        POP EAX
        POPFD

        RET
    }
}

// out ecx
// in ebx
// len 51
// ALSO USED AT 9A5054 with len 44
_declspec(naked) void __stdcall playerGetHitboxHD_9A04B6()
{
    __asm {
        PUSHFD
        PUSH EAX
        //PUSH ECX
        PUSH EDX

        PUSH EBX // Player
        CALL playerGetHitboxHD

        MOV ECX, EAX

        POP EDX
        //POP ECX
        POP EAX
        POPFD

        RET
    }
}

// out ecx
// in eax
// len 10
_declspec(naked) void __stdcall playerGetHitboxHD_9AEA8A()
{
    __asm {
        PUSHFD
        PUSH EAX
        //PUSH ECX
        PUSH EDX

        PUSH EAX // Player
        CALL playerGetHitboxHD

        MOV ECX, EAX

        POP EDX
        //POP ECX
        POP EAX
        POPFD

        RET
    }
}

// out edx
// in 
// len 
_declspec(naked) void __stdcall playerGetHitboxHD_9BE963()
{
    __asm {
        PUSHFD
        PUSH EAX
        PUSH ECX
        //PUSH EDX

        LEA EAX, [ECX + EAX * 4] // Player address
        PUSH EAX // Player
        CALL playerGetHitboxHD

        MOV EDX, EAX

        //POP EDX
        POP ECX
        POP EAX
        POPFD

        RET
    }
}

// out ecx
// in ecx
// len 59
_declspec(naked) void __stdcall playerGetHitboxHD_9C1030()
{
    __asm {
        PUSHFD
        PUSH EAX
        //PUSH ECX
        PUSH EDX

        PUSH ECX // Player
        CALL playerGetHitboxHD

        MOV ECX, EAX

        POP EDX
        //POP ECX
        POP EAX
        POPFD

        RET
    }
}



// out edx
// in ecx
// len 56
_declspec(naked) void __stdcall playerGetHitboxHD_9C0F86()
{
    __asm {
        PUSHFD
        PUSH EAX

        PUSH ECX // Player
        CALL playerGetHitboxHD

        MOV EDX, EAX

        POP EAX
        POPFD

        RET
    }
}

// out edx
// in ecx
// len 56
_declspec(naked) void __stdcall playerGetHitboxHD_9C10E6()
{
    __asm {
        PUSHFD
        PUSH EAX
        PUSH ECX
        //PUSH EDX

        PUSH ECX // Player
        CALL playerGetHitboxHD

        MOV EDX, EAX

        //POP EDX
        POP ECX
        POP EAX
        POPFD

        RET
    }
}

// out edx
// in eax
// len 96
_declspec(naked) void __stdcall playerGetHitboxHD_9C126E()
{
    __asm {
        PUSHFD
        PUSH EAX
        PUSH ECX
        //PUSH EDX

        PUSH EAX // Player
        CALL playerGetHitboxHD

        MOV EDX, EAX

        //POP EDX
        POP ECX
        POP EAX
        POPFD

        RET
    }
}

// out edx
// in ecx
// len 99
_declspec(naked) void __stdcall playerGetHitboxHD_9C11A5()
{
    __asm {
        PUSHFD
        PUSH EAX
        PUSH ECX
        //PUSH EDX

        PUSH ECX // Player
        CALL playerGetHitboxHD

        MOV EDX, EAX

        //POP EDX
        POP ECX
        POP EAX
        POPFD

        RET
    }
}

