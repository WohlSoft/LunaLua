#include "../RuntimeHook.h"
#include "../../SMBXInternal/Reconstructed/ReconstructedDefines.h"
#include <windows.h>

#include "../RuntimeHookUtils/APIHook.h"

BYTE* tracedownAddress(BYTE* addr){
    //Now get the relative address of this function
    DWORD relAddr = *((DWORD*)(addr + 1));
    //Convert the relative address to an absolute address
    DWORD targetAddr = relAddr + (DWORD)addr + 5;
    //Now get the pointer of the code
    return (BYTE*)targetAddr;
}

// For reading
DWORD __stdcall fixup_TypeMismatch13_GetUserDefaultLCID() 
{
    return 1033;
}

// For writing
HRESULT __stdcall fixup_TypeMismatch13_VariantChangeTypeEx(VARIANTARG *pvargDest, VARIANTARG *pvarSrc, LCID lcid, USHORT wFlags, VARTYPE vt)
{
    return VariantChangeTypeEx(pvargDest, pvarSrc, 1033, wFlags, vt);
}


void fixup_TypeMismatch13()
{
    new CAPIHook("Kernel32.dll", "GetUserDefaultLCID", (PROC)&fixup_TypeMismatch13_GetUserDefaultLCID, true);
    new CAPIHook("OleAut32.dll", "VariantChangeTypeEx", (PROC)&fixup_TypeMismatch13_VariantChangeTypeEx, true);
}

typedef struct _SEH_CHAIN_RECORD {
    struct _SEH_CHAIN_RECORD* next;
    SEH_HANDLER* handler;
} SEH_CHAIN_RECORD;

void fixup_ErrorReporting()
{
    HMODULE vmVB6Lib = GetModuleHandleA("msvbvm60.dll");
    if (vmVB6Lib){
        // This is the old version

        BYTE* overflowFuncAddr = (BYTE*)GetProcAddress(vmVB6Lib, "__vbaErrorOverflow");
        BYTE* internalRaiseErrorFunc = tracedownAddress(overflowFuncAddr + 2);

        BYTE* toPatch = internalRaiseErrorFunc + 20;
        DWORD oldprotect;
        if (VirtualProtect((void*)toPatch, 10, PAGE_EXECUTE_READWRITE, &oldprotect)){

            // Apply patch to call recordVBErrCode with ESI as an argument
            PATCH(toPatch).PUSH_ESI().CALL(&recordVBErrCode).Apply();
                //NOP

            // Now get the protection back
            VirtualProtect((void*)toPatch, 10, oldprotect, &oldprotect);
        }
    }

    // Find the first link of the SEH chain which is *not* smbx.__vbaExceptHandler
    SEH_CHAIN_RECORD* seh = (SEH_CHAIN_RECORD *)__readfsdword(0x00);
    while ((seh->next != (void*)0xFFFFFFFF) && (seh->handler == (void*)0x0040BA66)) {
        seh = seh->next;
    }

    // Substitute that exception handler with our own.
    LunaDLLOriginalExceptionHandler = seh->handler;
    seh->handler = LunaDLLCustomExceptionHandler;
}


void fixup_WarpLimit()
{
    unsigned int newWarpLimit = 2000;

    // Allocate new warp array memory, each element is 0x90
    // Note: We don't try to de-allocate the old warp array because we don't
    //       know how the VB runtime allocated that in the first place
    *(void**)(0xB258F4) = calloc(newWarpLimit, 0x90);

    static const unsigned int offset1Addresses[] = {
        // push C8 | 68 C8 00 00 00
        0xAE95BA, 0xAE9670, 0xAE971B,
        // cmp eax, C8 | 3D C8 00 00 00
        0x9CAC97, 0x9CAD91, 0xB11837, 0xB1184A, 0xB1185D, 0xB11870, 0xB11883, 0xB11896, 0xB118A9,
        0xB118BC, 0xB118CF, 0xB118E2, 0xB118F5, 0xB11908, 0xB1191B, 0xB1192E, 0xB11DC4, 0xB11DDE,
        0xB11DF8, 0xB11E12, 0xB11E2C, 0xB11E46, 0xB11E60,
        // end of list
        0x000000
    };

    static const unsigned int offset2Addresses[] = {
        // cmp esi, C8 | 81 FE C8 00 00 00
        0xA0217A, 0xAA7E52, 0xAA7E9B, 0x8DC13C, 0x8DC169, 0x8DC3E7, 0x8DC40F, 0x8DD3E2, 0x8F7DDC,
        0x96A38B, 0x96A406, 0x96A4A4, 0x96A4FC, 0x96A545, 0x96A593, 0x96A5E8, 0x96A646, 0x96A6C1,
        0x96A719, 0x96A766, 0x96A7DA, 0x96A85B, 0x96A8D5, 0x96A973, 0x96A9CC, 0x96AA14, 0x96AA64,
        0x96AAB7, 0x96AB16, 0x96AB90, 0x96ABE9, 0x96AC35, 0x96ACA7, 0x9931E6, 0x9AB423, 0x9AB6E6,
        0x9AB78D, 0x9AB7F4, 0x9AB8AF, 0x9C985B, 0x9C9894, 0x9C98D2, 0x9C990F, 0x9C994F, 0x9C998F,
        0x9C99DA, 0x9C9A07, 0x9C9A32, 0x9C9A6D, 0x9C9AA6, 0x9C9B4F, 0x9C9C81, 0x9C9E5C, 0x9C9F9B,
        0x9CA125, 0x9CA15E, 0x9CA1B0, 0x9CA257, 0x9CA285, 0x9CA3DD, 0x9CA407, 0x9CA64C, 0x9CA891,
        0x9CA91C, 0x9CAD7E, 0x9D80FC, 0x9EFC7C, 0x9EFCA6, 0x9EFCF6, 0x9EFD26, 0x9EFD4C, 0x9EFD75,
        0x9EFD9E, 0x9EFDC5, 0x9EFDEB, 0x9EFE11, 0x9EFE37, 0x9EFE5D, 0x9EFE83, 0x9EFF2B, 0x9EFF4E,
        0x9EFFCB, 0x9F0073, 0x9F01CF, 0x9F01F9, 0x9F02A2, 0x9F0375, 0x9F03AA, 0x9F045A, 0x9F052D,
        0x9F055B, 0x9F1261, 0x9F12B6, 0x9F710A, 0x9F7142, 0x9F7169, 0x9F71C6, 0x9F71EC, 0x9F72BF,
        0x9F7381, 0x9F74D6, 0x9F7668, 0x9F7725, 0x9F77E2, 0x9F789C, 0x9F7920, 0x9F79A4, 0x9F7A5D,
        0x9F7C0D, 0x9F7CBC, 0x9F7F35, 0x9F7FC4, 0x9F80AC, 0x9F80D2, 0x9F81A5, 0x9F8267, 0x9F83BC,
        0x9F854E, 0x9F860B, 0x9F86C8, 0x9F8782, 0x9F8806, 0x9F888A, 0x9F890F, 0x9F8ABF, 0x9F8B6E,
        0x9F8DE7, 0x9F8E70, 0xA021B0, 0xA27295, 0xA9F615, 0xAA1BA4, 0xAA1C31, 0xAA3578, 0xAA35F9,
        0xAA410F, 0xAA4194, 0xAA7D79, 0xAA7DB8, 0xAA7DC9, 0xAA7E05, 0xAA7E16, 0xAA7E63, 0xAA7EAC,
        // cmp edi, C8 | 81 FF C8 00 00 00
        0x8C1A67, 0x8C1AF2, 0x8C1B68, 0x8C1BF3, 0x8C1C7F, 0x8C1CC0, 0x8C1D64, 0x8C1DA9, 0x8C1E7A,
        0x8C1F05, 0x8D7CDB, 0x8DA38F, 0x8DC1C7, 0x8DC20A, 0x8DC29C, 0x8DC2F8, 0x8DC46A, 0x8DC4AF,
        0x8DC4ED, 0x96A3D4, 0x96A472, 0x96A68F, 0x96A8A3, 0x96A941, 0x96AB5E, 0x9C9D0B, 0x9C9EE6,
        0x9CA027, 0x9CA2AE, 0x9CA87B, 0x9CA932, 0x9CA9C7, 0x9D325D, 0x9D32BC, 0x9D3347, 0x9D3930,
        0x9D398E, 0x9D3A19, 0x9D3C46, 0x9D3CA2, 0x9D3D1B, 0x9D401C, 0x9D4079, 0x9D40F6, 0x9D4426,
        0x9D44F1, 0x9D4538, 0x9D45C3, 0x9D4723, 0x9D4769, 0x9D47F4, 0x9D4A11, 0x9D4A64, 0x9D4AA6,
        0x9D4D14, 0x9D4D7D, 0x9D4DC2, 0x9D50F8, 0x9D5120, 0x9D5524, 0x9D5555, 0x9D55AC, 0x9D55D3,
        0x9D56C6, 0x9D5713, 0x9D5920, 0x9D5982, 0x9D5B2A, 0x9D5B7F, 0x9D60B0, 0x9D6106, 0x9D6614,
        0x9D6641, 0x9D6671, 0x9D66A9, 0x9D67F1, 0x9D6DFB, 0x9D6E86, 0x9D6F3E, 0x9D6F78, 0x9D6FD0,
        0x9D6FF9, 0x9D7976, 0x9D7D9E, 0x9D802D, 0x9D806C, 0x9D8095, 0x9D80C2, 0x9F02C8, 0x9F0480,
        0xA02169, 0xA2731F,
        // cmp ebx, C8 | 81 FB C8 00 00 00
        0x8C19FF, 0x8C1A28, 0x8C1A55, 0x8C1B22, 0x8C1B52, 0x8C1C09, 0x8C1C3C, 0x8C1CD6, 0x8C1D0A,
        0x8C1D4E, 0x8C1DBF, 0x8C1E3F, 0x8C1E68, 0x8C1F1B, 0x8DC307, 0x969C90, 0x969CF7, 0x969D42,
        0x969D6E, 0x969D9E, 0x969DD8, 0x969E1B, 0x969E5A, 0x969E9E, 0x969ECB, 0x969F26, 0x969F75,
        0x969FB7, 0x96A01E, 0x96A063, 0x96A090, 0x96A0BF, 0x96A0F9, 0x96A13D, 0x96A17B, 0x96A1BF,
        0x96A1EC, 0x96A244, 0x9AB702, 0x9AB7A3, 0x9CA9A6, 0x9CACAF, 0x9D32A6, 0x9D335D, 0x9D3978,
        0x9D3CB4, 0x9D408F, 0x9D410C, 0x9D4522, 0x9D4753, 0x9D480A, 0x9D4ABC, 0x9D4D67, 0x9D4DD8,
        0x9D51BF, 0x9D596C, 0x9D5B95, 0x9D6D1A, 0x9D6DE5, 0x9D6E9C, 0x9F02B5, 0x9F046D, 0xAE951B,
        0xAE954B,
        // cmp ebp, C8 | 81 FD C8 00 00 00
        0x969C50, 0x969C7E, 0x969CE1, 0x969E44, 0x969FA1, 0x96A008, 0x96A165,
        // end of list
        0x000000
    };

    static const unsigned int offset6Addresses[] = {
        // cmp dword ptr ss : [ebp - 2E8], C8 | 81 BD 18 FD FF FF C8 00 00 00
        0xB05C8F,
        // cmp dword ptr ss : [ebp - 2E4], C8 | 81 BD 1C FD FF FF C8 00 00 00
        0xB02765, 0xB029C9, 0xB02C2D, 0xB02E91, 0xB030F5, 0xB03363, 0xB035D1, 0xB0383C, 0xB03AE6,
        0xB03D38, 0xB03FE1, 0xB04270, 0xB04500, 0xB04752, 0xB049BE, 0xB04C29, 0xB04E94, 0xB05102,
        0xB05350, 0xB055D8, 0xB05867, 0xB05971, 0xB059CB, 0xB05A25, 0xB05A7F, 0xB05BE2, 0xB05C3B,
        0xB05CC8,
        // cmp dword ptr ss : [ebp - 1D0], C8 | 81 BD 30 FE FF FF C8 00 00 00
        0x92D51D,
        // cmp dword ptr ss : [ebp - 1CC], C8 | 81 BD 34 FE FF FF C8 00 00 00
        0x92D474, 0x92D4E6, 0x93FC19,
        // end of list
        0x000000
    };

    // Patch offset 1 addresses
    for (int i = 0; offset1Addresses[i] != 0x000000; i++)
    {
        *(unsigned int*)(offset1Addresses[i] + 1) = newWarpLimit;
    }

    // Patch offset 2 addresses
    for (int i = 0; offset2Addresses[i] != 0x000000; i++)
    {
        *(unsigned int*)(offset2Addresses[i] + 2) = newWarpLimit;
    }

    // Patch offset 6 addresses
    for (int i = 0; offset6Addresses[i] != 0x000000; i++)
    {
        *(unsigned int*)(offset6Addresses[i] + 6) = newWarpLimit;
    }
}



void fixup_Credits()
{
    const unsigned char nullStrMove[] = { 0xBA, 0x00, 0x3D, 0x42, 0x00 };
    const unsigned char line1_createdBy[] = { 0xBA, 0x70, 0xA6, 0x42, 0x00 };
    const unsigned char line2_andrewSpinks[] = { 0xBA, 0x8C, 0xA6, 0x42, 0x00 };
    const unsigned char line3_redigit[] = { 0xBA, 0xAC, 0xA6, 0x42, 0x00 };

    memcpy((void*)0x008F72D0, nullStrMove, sizeof(nullStrMove));  //Sorry Redigit, but I need that space
    memcpy((void*)0x008F7300, nullStrMove, sizeof(nullStrMove));
    memcpy((void*)0x008F7318, nullStrMove, sizeof(nullStrMove));


    memcpy((void*)0x008F7288, line1_createdBy, sizeof(line1_createdBy)); //Still give you the "king" position
    memcpy((void*)0x008F72A0, line2_andrewSpinks, sizeof(line2_andrewSpinks));
    memcpy((void*)0x008F72B8, line3_redigit, sizeof(line3_redigit));


    VB6StrPtr* text_HackedBy = new VB6StrPtr(std::string("Hacked By [LunaDll]: "));
    VB6StrPtr* text_Kevsoft = new VB6StrPtr(std::string("Kevsoft"));
    VB6StrPtr* text_Rednaxela = new VB6StrPtr(std::string("Rednaxela"));
    VB6StrPtr* text_Kil = new VB6StrPtr(std::string("Kil"));
    VB6StrPtr* text_Wohlstand = new VB6StrPtr(std::string("Wohlstand"));

    memcpy((void*)0x008F7301, text_HackedBy, 4);
    memcpy((void*)0x008F7319, text_Kevsoft, 4);
    memcpy((void*)0x008F7331, text_Rednaxela, 4);
    memcpy((void*)0x008F7349, text_Kil, 4);
    memcpy((void*)0x008F7361, text_Wohlstand, 4);
}

void fixup_Mushbug()
{
    const uint8_t mushbugPatch[22] = {
        0x0f, 0x84, 0x2e, 0x01, 0x00, 0x00,      // 0x00a2d08b je 0x00a2d1bf
        0x66, 0x3d, 0x05, 0x00,                  // 0x00a2d091 cmp ax, 5
        0x0f, 0x95, 0xc2,                        // 0x00a2d095 setne dl
        0x33, 0xc9,                              // 0x00a2d098 xor ecx, ecx
        0x0f, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00 // 0x00a2d09a nop (7-byte version)
    };
    memcpy((void*)0x00a2d08b, mushbugPatch, sizeof(mushbugPatch));
}



void fixup_NativeFuncs()
{
    // Patch the whole native function
    PATCH(0xA3C580).JMP(&Reconstructed::Util::npcToCoins).NOP_PAD_TO_SIZE<0x40B>().Apply();
    Reconstructed::Util::npcToCoins_setup();
}

__declspec(naked) static void fixup_BGODepletionASM()
{
    // Replaces the following code:
    // 008D9010 | 66 89 1D E4 B9 B2 00 | mov word ptr ds:[B2B9E4],bx
    __asm {
        MOV WORD PTR DS : [0xB250D6], BX
        MOV WORD PTR DS : [0xB2B9E4], BX
        PUSH 0x8D9017
        RET
    }
}

void fixup_BGODepletion()
{
    PATCH(0x8D9010)
        .NOP()
        .NOP()
        .JMP(fixup_BGODepletionASM)
        .Apply();
}
