#ifndef HardcodedGraphicsAccess_hhhh
#define HardcodedGraphicsAccess_hhhh

#include <cstdint>

// Helper function to get a HDC for a hardcoded graphic. Full details not fully understood
static inline void* getHDCForHardcodedGraphic(int32_t param1, int32_t param2)
{
    // KNOWN VALUES FOR PARAMETERS:
    // param1 param2 Description
    // 0x384  0x1    HUD: HEART MASK
    // 0x388  0x1    HUD: FILLED HEART
    // 0x388  0x2    HUD: EMPTY HEART

    void* addr = (void*)0xB2D7C4;

    void* obj1 = *((void**)addr);
    auto method1 = (void* (__stdcall *)(void*)) *(void**)(*(int32_t*)obj1 + param1);

    void* obj2 = method1(obj1);
    auto method2 = (void(__stdcall *)(void*, int32_t, void**)) *(void**)(*(int32_t*)obj2 + 0x40);

    void* obj3 = nullptr;
    method2(obj2, param2, &obj3);
    auto method3 = (void(__stdcall *)(void*, void**)) *(void**)(*(int32_t*)obj3 + 0xE0);

    void* ret;
    method3(obj3, &ret);

    return ret;
}

#endif