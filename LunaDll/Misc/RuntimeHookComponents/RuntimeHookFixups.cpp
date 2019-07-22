#include <windows.h>

#include "../RuntimeHook.h"
#include "../../SMBXInternal/Reconstructed/ReconstructedDefines.h"

#include "../RuntimeHookUtils/APIHook.h"
#include "../AsmPatch.h"

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

void fixup_EventLimit()
{
    unsigned int newEventLimit = 255;

    static const unsigned int limitAddresses[] = {
        0x8c0de8, 0x8c0e17, 0x8c2001, 0x8c2030, 0x8d8249, 0x8d827d, 0x8d82a9, 0x8d82d6, 0x8d8304, 0x8d8347, 0x8d8388, 0x8d83cb,
        0x8d8440, 0x8d8485, 0x8d84cc, 0x8d851e, 0x8d8571, 0x8d85c4, 0x8d862f, 0x8d865e, 0x8d8695, 0x8d86c1, 0x8d86f2, 0x8d8723,
        0x8d8753, 0x8d8784, 0x8d87b5, 0x8d87e5, 0x8d8816, 0x8d8847, 0x8d8877, 0x8d88a8, 0x8d88d9, 0x8d8908, 0x8d8938, 0x8d8968,
        0x8d8997, 0x8d89c7, 0x8dab36, 0x8dab65, 0x8dab98, 0x8dabcc, 0x8dac0f, 0x8dac4e, 0x8dac93, 0x8dad10, 0x8dad55, 0x8dad96,
        0x8daddd, 0x8dae24, 0x8dae6a, 0x8daed7, 0x8daf06, 0x8daf3d, 0x8daf74, 0x8dafa3, 0x8dafd3, 0x8db003, 0x8db032, 0x8db062,
        0x8db092, 0x8db0c1, 0x8db0f1, 0x8db121, 0x8db15b, 0x8db18b, 0x8db1bb, 0x8db1ea, 0x8db225, 0x8db255, 0x8db284, 0x8dc897,
        0x8dc8d9, 0x8dc902, 0x8dc936, 0xa9f7d2, 0xa9f819, 0xa9f93c, 0xa9f988, 0xa9fa0d, 0xaa1e84, 0xaa1f24, 0xaa1f5b, 0xaa1ff7,
        0xaa43ae, 0xaa443c, 0xaa44dc, 0xaa450c, 0xaa4589, 0xaa45f3, 0xaa4624, 0xaa4675, 0xaa4700, 0xaa4777, 0xaa47de, 0xaa480e,
        0xaa4c70, 0xaa5260, 0xaa53b9, 0xaa53ef, 0xaa542e, 0xaa5438, 0xaa5478, 0xaa547f, 0xaa54c3, 0xaa5524, 0xaa557b,
        0xaa5593, 0xaa55cb, 0xaa562a, 0xaa5688, 0xaa56d8, 0xaa5703, 0xaa5a4c, 0xaa5a6b, 0xaa5a72, 0xaa5aac, 0xaa5ae2, 0xaa5ae9,
        0xaa5b25, 0xaa5b6b, 0xaa5bb6, 0xaa5bd5, 0xaa5bff, 0xaa5c27, 0xaa5ca7, 0xaa5ccf, 0xaa5cf4, 0xaa5d19, 0xaa5d3e, 0xaa5d63,
        0xaa5d88, 0xaa5da9, 0xaa5dca, 0xaa5deb, 0xaa5e1c, 0xaa5e48, 0xaa5e88, 0xaa5ee1, 0xaa5eec, 0xaa5f42, 0xaa5f4d, 0xaa5f98,
        0xaa5fe6, 0xaa602a, 0xaa8025, 0xaa818d, 0xaa8496, 0xaa8605, 0xaa862c, 0xaa8826, 0xaa8995, 0xaa89bc, 0xaa8bb6, 0xaa8d25,
        0xaa8d4c, 0xaa8f46, 0xaa90b5, 0xaa90dc, 0xaa9345, 0xaa94cc, 0xaa94fc, 0xaa9795, 0xaa991c, 0xaa9950, 0xaa9b76, 0xaa9ce5,
        0xaa9d0c, 0xaa9f06, 0xaaa075, 0xaaa09c, 0xaaa304, 0xaaa462, 0xaaa655, 0xaaa7ba, 0xaaab8e, 0xaaace9, 0xaab227, 0xaab27f,
        0xaab84f, 0xaaba3f, 0xaabadc, 0xaabb93, 0xaac11b, 0xaac132, 0xaad295, 0xaad5e4, 0xaad7a5, 0xaad800, 0xaad82b, 0xaad892,
        0xaad903, 0xaadda8, 0xaadef7, 0xaadf3c, 0xaae115, 0xaae3bf, 0xaae543, 0xaae56f, 0xaae73f, 0xaae92f, 0xaae9cc, 0xaaea83,
        0xaaed9f, 0xaaef8f, 0xaaf03b, 0xaaf101, 0xaaf1d0, 0xaaf2a0, 0xaaf36f, 0xaaf638, 0xaaf787, 0xaaf7cc, 0xaaf9a5, 0xaafc4f,
        0xaafdd3, 0xaafdff, 0xaaff68, 0xab00b7, 0xab00ff, 0xab02db, 0xab058f, 0xab0713, 0xab073f, 0xab08c6, 0xab0a35, 0xab0a5c,
        0xab0c56, 0xab0dc5, 0xab0dec, 0xab0fe6, 0xab1155, 0xab117c, 0xab1376, 0xab14e5, 0xab150c, 0xab1722, 0xab1860, 0xab1a11,
        0xab1a1c, 0xab1aba, 0xab1b26, 0xab25e7, 0xab2734, 0xab29d5, 0xab2b3a, 0xab2df5, 0xab2f5a, 0xab31a5, 0xab32f8, 0xab3bd5,
        0xab3d01, 0xab3d3d, 0xab4c87, 0xab4cca, 0xab516e, 0xab6c38, 0xab6f85, 0xab82a3, 0xab8337, 0xab83cc, 0xab8460, 0xab84f5,
        0xab8589, 0xab861e, 0xab8694, 0xab873c, 0xab87b0, 0xab8825, 0xab88ac, 0xab8937, 0xab89c2, 0xab8a4d, 0xab8ad8, 0xab8b63,
        0xab8bee, 0xab8c79, 0xab8d04, 0xab8d8f, 0xab93a5, 0xab950a, 0xab97c5, 0xab992a, 0xb127a9, 0xb127b9, 0xb127c9, 0xb127d9,
        0xb127e9, 0xb127f9, 0xb12809, 0xb12819, 0xb12829, 0xb12839, 0xb12849, 0xb12859, 0xb12869, 0xb12879, 0xb12d54, 0xb12d6b,
        0xb12d82, 0xb12d99, 0xb12db0, 0xb12dc7, 0xb12dde, 0xb12df5, 0xb12e0c, 0xb12e23, 0xb13183, 0xb131a9, 0xb134db, 0xb13500,
        0xb13522, 0xb168a7, 0xb18a62, 0xb18b02, 0xb18b9f, 0xb18c3c, 0xb18cd8, 0xb18d73, 0xb18e10, 0xb18ea9, 0xb18f45, 0xb18fdf,
        0xb1907a, 0xb19116, 0xb191af, 0xb1924b, 0xb192e7, 0xb19384, 0xb19421, 0xb194be, 0xb1955b, 0xb195f8, 0xb19695, 0xb19732,
        0xb197cf, 0xb1986c, 0xb1991f, 0xb199d2, 0xb19a85, 0xb19b3c, 0xb19bf2, 0xb19ca8, 0xb19d88, 0xb19e38, 0xb19ee1, 0xb19f4d,
        0x000000 };


    // Allocate new event array memory, each element is 0x588
    // Note: We don't try to de-allocate the old array because we don't
    //       know how the VB runtime allocated that in the first place
    *(void**)(0xB2C6CC) = calloc(newEventLimit, 0x588);

    // Update all limits
    for (int i = 0; limitAddresses[i] != 0x000000; i++)
    {
        *((unsigned char*)limitAddresses[i]) = newEventLimit;
    }

    static const unsigned int triggerEventLoopCountAddr = 0xAA437D;
    *((unsigned int*)triggerEventLoopCountAddr) = newEventLimit-1; // -1 because this is the max index
}

void fixup_LayerLimit()
{
    unsigned int newLayerLimit = 255;

    static const unsigned int limitAddresses[] = {
        0x8d8056, 0x8d8119, 0x8d8135, 0x8d8186, 0x8d81b2, 0x8d81db, 0x8da663, 0x8da688, 0x8da6b2, 0x8da6d6, 0x8da720, 0x8da75c,
        0x8da7ac, 0x8da7fb, 0x8da937, 0x8da9c1, 0x8dcaed, 0x8dcb0c, 0x8dcb29, 0x8dcb3d, 0x9d0381, 0x9d03a5, 0x9d10ad, 0x9d10db,
        0x9d1116, 0x9d1126, 0x9d113c, 0x9d118c, 0x9d11c0, 0x9d11e5, 0x9d1279, 0x9d13b8, 0x9d13c8, 0x9d13fa, 0x9e22e6, 0x9e230e,
        0xa22bb2, 0xa22be0, 0xa22c72, 0xa22ca7, 0xa22ce3, 0xa22d04, 0xa22d27, 0xa22da6, 0xa22de5, 0xa22e30, 0xa22efd, 0xa2309a,
        0xa230b7, 0xa230f1, 0xa3a2fc, 0xa3a322, 0xa3a336, 0xaa2344, 0xaa248c, 0xaa24b2, 0xaa2512, 0xaa2548, 0xaa267a, 0xaa2807,
        0xaa2881, 0xaa289b, 0xaa28c5, 0xaa28e4, 0xaa290c, 0xaa37d1, 0xaa384e, 0xaa5519, 0xaa559a, 0xaa55d2, 0xaa567d, 0xaa56c8, 0xaa56e9,
        0xaa571e, 0xaa574e, 0xaa577b, 0xaa57b1, 0xaa57f8, 0xaa5830, 0xaa586a, 0xaa58fb, 0xaa680a, 0xaa6811, 0xaa683a, 0xaa688c,
        0xaa68d6, 0xaa69d3, 0xaa69da, 0xaa6a03, 0xaa6a7b, 0xaa6b1e, 0xaa6b25, 0xaa6b4f, 0xaa6bab, 0xaa6bfe, 0xaa6c31, 0xaa6cf4,
        0xaa6d41, 0xaa6d84, 0xaa6da2, 0xaa6e32, 0xaa6e73, 0xaa6ec6, 0xaa6f4e, 0xaa6f8b, 0xaa6fd6, 0xaa7060, 0xaa70b2, 0xaa7117,
        0xaa75d4, 0xaa760a, 0xaa765d, 0xaa7689, 0xaa7a14, 0xaa7a7e, 0xaa7b7c, 0xaa7bb2, 0xaa7c96, 0xaa7cd2, 0xaa7cdd, 0xaa7cf4,
        0xaa7d07, 0xaa7d85, 0xaa7dc4, 0xaa7e11, 0xaa7e5e, 0xaa7ea7, 0xb0619f, 0xb06482, 0xb064d4, 0xb0652a, 0xb0657d, 0xb067b1,
        0xb06a18, 0xb06d4c, 0xb06e00, 0xb06ead, 0xb12494, 0xb124a1, 0xb124ae, 0xb124bb, 0xb167d3,
        0x000000 };

    static const unsigned int loop4Addresses[] = {
        0x8d8170, 0x8d8228, 0x8dcacb, 0x9d034e, 0xa22b96, 0xa3a2de, 0xaa228e, 0xaa2605, 0xaa27ef, 0xaa37bc, 0xaa5507, 0xaa5662,
        0xb16750,
        0x000000
    };

    static const unsigned int loop2Addresses[] = {
        0x9e2335, 0xaa6974, 0xaa7f02,
        0x000000
    };

    static const unsigned int loop1Addresses[] = {
        0xaa7d31,
        0x000000
    };

    // Allocate new layer array memory, each element is 0x14
    // Note: We don't try to de-allocate the old array because we don't
    //       know how the VB runtime allocated that in the first place
    *(void**)(0xB2C6B0) = calloc(newLayerLimit, 0x14);

    // Update all limits
    for (int i = 0; limitAddresses[i] != 0x000000; i++)
    {
        *((unsigned char*)limitAddresses[i]) = newLayerLimit;
    }

    for (int i = 0; loop4Addresses[i] != 0x000000; i++)
    {
        *((unsigned int*)loop4Addresses[i]) = newLayerLimit - 1;
    }

    for (int i = 0; loop2Addresses[i] != 0x000000; i++)
    {
        *((unsigned short*)loop2Addresses[i]) = newLayerLimit - 1;
    }

    for (int i = 0; loop1Addresses[i] != 0x000000; i++)
    {
        *((unsigned char*)loop1Addresses[i]) = newLayerLimit - 1;
    }
}

void fixup_WebBox()
{
    const wchar_t* aboutBlank = L"about:blank";
    const wchar_t* webBoxTitle = L"LunaLua-SMBX";

    memcpy((void*)0x00431A34, aboutBlank, sizeof(wchar_t) * lstrlenW(aboutBlank) + 2);
    memcpy((void*)0x00427614, webBoxTitle, sizeof(wchar_t) * lstrlenW(webBoxTitle) + 2);

    //const unsigned char nullStrMove[] = { 0xBA, 0x00, 0x3D, 0x42, 0x00 };
    //memcpy((void*)0x00B201AA, nullStrMove, sizeof(nullStrMove));//Heck off, WebBox!
    //memcpy((void*)0x0096AF25, nullStrMove, sizeof(nullStrMove));//Heck off, WebBox!

    //VB6StrPtr* aboutBlank = new VB6StrPtr(std::string("about:blank"));
    //memcpy((void*)0x00B201AB, aboutBlank, 4);
    //VB6StrPtr* webBoxTitle = new VB6StrPtr(std::string("What the heck?"));
    //memcpy((void*)0x0096AF26, webBoxTitle, 4);
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

void fixup_Veggibug()
{
    PATCH(0xA2B17D)
        .CALL(&IsNPCCollidesWithVeggiHook_Wrapper).NOP_PAD_TO_SIZE<10>().Apply();
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

void fixup_RenderPlayerJiterX()
{
    PATCH(0x9895A8).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x9897A1).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x9898E7).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x989A4E).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x989B9F).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x989D07).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98A585).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98A818).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98AB8E).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98AEE8).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98B0F2).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98B334).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98B614).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98B8CD).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98BB83).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98BE19).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98C193).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98C4E9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98C6F3).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98C934).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98CC15).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98CECA).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98D761).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98DA9F).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98DCA9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98DEEA).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98E7E5).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98EB46).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98EE8F).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98F1E5).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98F3EF).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x98F630).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x990B7D).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x990D67).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x990F3F).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x991127).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x991325).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x9914DA).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x99167E).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x991833).NOP_PAD_TO_SIZE<6>().Apply();
}
