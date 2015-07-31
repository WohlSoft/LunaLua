#ifndef CameraInfo_Hhhh
#define CameraInfo_Hhhh

#include "../Defines.h"

class CameraInfo
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

    static inline CameraInfo* Get(unsigned short index) {
        if (index >= 3) return NULL;
        return &((CameraInfo*)GM_CAMINFO)[index];
    }
};


#endif