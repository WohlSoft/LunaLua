#ifndef SafeFPUControl_hhhhh
#define SafeFPUControl_hhhhh

#include <cfenv>

class SafeFPUControl
{
private:
    unsigned int fpu_control;
    unsigned int sse2_control;
public:
    SafeFPUControl();
    ~SafeFPUControl();
};


#endif

