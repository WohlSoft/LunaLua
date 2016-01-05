#ifndef HardcodedGraphicsAccess_hhhh
#define HardcodedGraphicsAccess_hhhh

#include <cstdint>

// Helper function to get a HDC for a hardcoded graphic. Full details not fully understood
static inline void* getHDCForHardcodedGraphic(int32_t param1, int32_t param2)
{
    // SOME KNOWN VALUES FOR PARAMETERS:
    // param1  param2  Description
    //     35       1  HUD: Filled Heart Mask
    //     35       2  HUD: Empty Heart Mask
    //     36       1  HUD: Filled Heart
    //     36       2  HUD: Empty Heart

    void* addr = (void*)0xB2D7C4;

    void* obj1 = *((void**)addr);
    auto method1 = (void* (__stdcall *)(void*)) *(void**)(*(int32_t*)obj1 + 0x2f8 + param1 * 4);

    void* obj2 = method1(obj1);
    auto method2 = (void(__stdcall *)(void*, int32_t, void**)) *(void**)(*(int32_t*)obj2 + 0x40);

    void* obj3 = nullptr;
    method2(obj2, param2, &obj3);
    auto method3 = (void(__stdcall *)(void*, void**)) *(void**)(*(int32_t*)obj3 + 0xE0);

    void* ret = nullptr;
    method3(obj3, &ret);

    return ret;
}

#endif