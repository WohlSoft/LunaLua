#ifndef SafeFPUControl_hhhhh
#define SafeFPUControl_hhhhh

#include <cfenv>
#pragma fenv_access (on)

class SafeFPUControl
{
public:
    inline static void clear() { std::feclearexcept(FE_ALL_EXCEPT);  }
public:
    inline SafeFPUControl() {}
    SafeFPUControl(SafeFPUControl& other) = delete;
    inline ~SafeFPUControl() { clear(); }
};


#endif

