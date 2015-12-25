#ifndef SafeFPUControl_hhhhh
#define SafeFPUControl_hhhhh

#include <cfenv>

class SafeFPUControl
{
public:
    SafeFPUControl() = default;
    SafeFPUControl(SafeFPUControl& other) = delete;
    ~SafeFPUControl();
};


#endif

