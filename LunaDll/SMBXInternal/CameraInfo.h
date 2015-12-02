#ifndef SMBX_CameraInfo_Hhhh
#define SMBX_CameraInfo_Hhhh

#include "../Defines.h"

class SMBX_CameraInfo
{
public:
    short unknown00;
    short unknown02;
    short unknown04;
    short unknown06;
    short unknown08;
    short unknown0A;
    short unknown0C;
    short unknown0E;
    double width;
    double height;
    short unknown20;
    short unknown22;
    short unknown24;
    short unknown26;
    short unknown28;
    short unknown2A;
    short unknown2C;
    short unknown2E;
    short unknown30;
    short unknown32;
    short unknown34;
    short unknown36;

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
        ((double*)GM_CAMERA_X)[index] = -value;
    }
    static inline void setCameraY(unsigned short index, double value) {
        if (index >= 201) return;
        ((double*)GM_CAMERA_Y)[index] = -value;
    }
};


#endif