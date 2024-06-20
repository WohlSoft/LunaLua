#ifndef SafeFPUControl_hhhhh
#define SafeFPUControl_hhhhh

#include <float.h>

// Class which if instantiated will clear FPU exceptions upon destruction.
// Used ensure FPU exception registers are clean when a function exits.
// Also has a static method for convenience.
class SafeFPUControl
{
public:
    // Function to clear
    static inline void clear() {
        // NOTE: We used to use std::feclearexcept(E_ALL_EXCEPT) here, but that's a comparatively
        //       heavyweight routine. For our purposes, we just need to clear the flags that the
        //       compiled VB6 cares about, and a single inline FNCLEX instruction is adaquate for
        //       that.
        __asm {FNCLEX};
    }
public:
    inline SafeFPUControl() {}
    SafeFPUControl(SafeFPUControl& other) = delete;
    inline ~SafeFPUControl() { clear(); }
};


#endif

