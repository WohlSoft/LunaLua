#ifndef Keyboard_hhhh
#define Keyboard_hhhh

#include "../Defines.h"

// Vkey table reference
// 0 - Up
// 1 - Down
// 2 - Left
// 3 - Right
// 4 - Run
// 5 - Alt run
// 6 - Jump
// 7 - Alt jump
// 8 - Drop item
// 9 - Pause

namespace SMBXInput {
    
    /* Player Input type: 
     * 0 - Keyboard
     * 1 - Joystick #1
     * 2 - Joystick #2
     * ...
     * N - Joystick #N
     */
    static inline short getPlayerInputType(unsigned short index)
    {
        if (index < 1 || index > 2) return -1;
        short* type = (short*)GM_INPUTTYPE;
        return type[index - 1];
    }

    static inline void setPlayerInputType(unsigned short index, short inputType)
    {
        if (index < 1 || index > 2) return;
        short* type = (short*)GM_INPUTTYPE;
        type[index - 1] = inputType;
    }
}

#pragma pack(push, 1)
struct __declspec(align(1)) SMBXNativeKeyboard
{
    short up;
    short down;
    short left;
    short right;
    short jump;
    short run;
    short dropitem;
    short pause;
    short altjump;
    short altrun;

    static inline SMBXNativeKeyboard* Get(unsigned short inputIndex) {
        if (inputIndex < 1 || inputIndex > 2) return nullptr;
        return &((SMBXNativeKeyboard*)GM_VKEY_TABLE_PTR)[inputIndex - 1];
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct __declspec(align(1)) SMBXNativeJoystick
{
    short jump;
    short run;
    short dropitem;
    short pause;
    short altjump;
    short altrun;

    static inline SMBXNativeJoystick* Get(unsigned short inputIndex) {
        if (inputIndex < 1 || inputIndex > 2) return nullptr;
        return &((SMBXNativeJoystick*)GM_VJOY_TABLE_PTR)[inputIndex - 1];
    }
};
#pragma pack(pop)



#endif
