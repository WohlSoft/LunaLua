#ifndef SMBX_CameraInfo_Hhhh
#define SMBX_CameraInfo_Hhhh

#include "../Defines.h"

#pragma pack(push, 1)
class SMBX_CameraInfo
{
public:
    double x;                       //+0x00
    double y;                       //+0x08
    double width;                   //+0x10
    double height;                  //+0x18
    short unkIsSplitScreen;         //+0x20
    short unknown22;                //+0x22
    short unknown24;                //+0x24
    short unknown26;                //+0x26
    short unknown28;                //+0x28
    short unknown2A;                //+0x2A
    double diffYToOtherPlayer;      //+0x2C
    unsigned int unknownCounter;    //+0x34

    static const int MAX_CAMERAINFO = 3;

    static inline SMBX_CameraInfo* Get(unsigned short index) {
        if (index >= MAX_CAMERAINFO) return NULL;
        return &((SMBX_CameraInfo*)GM_CAMINFO)[index];
    }

    static inline double getCameraX(unsigned short index) {
        if (index >= 201) return 0;
        return -((double*)GM_CAMERA_X)[index];
    }

    static inline double getCameraY(unsigned short index) {
        if (index >= 201) return 0;
        return -((double*)GM_CAMERA_Y)[index];
    }

    static inline void setCameraX(unsigned short index, double value) {
        if (index >= 201) return;
        ((double*)GM_CAMERA_X)[index] = -std::round(value);
    }
    static inline void setCameraY(unsigned short index, double value) {
        if (index >= 201) return;
        ((double*)GM_CAMERA_Y)[index] = -std::round(value);
    }

    template<typename T>
    static inline void transformSceneToScreen(unsigned short index, T& x, T& y) {
        static_assert(std::is_arithmetic<T>::value, "T must be arithmetic!");
        if (index >= 201) return;
        x -= (T)getCameraX(index);
        y -= (T)getCameraY(index);
    }
};
#pragma pack(pop)

#ifndef __INTELLISENSE__
static_assert(sizeof(SMBX_CameraInfo) == 0x38, "sizeof(SMBX_CameraInfo) must be 0x38");
#endif

#endif