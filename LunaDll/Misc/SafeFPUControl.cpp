#include "SafeFPUControl.h"

#include <float.h>
#include <iostream>
#include <cfenv>


#pragma fenv_access (on)
#pragma STDC FENV_ACCESS ON

/*
 FIXME: Right now disabling all FPU exceptions will crash SMBX with the SSE2 error 0xc00002b4 by _lj_set_tab function by luajit.
        But clearing the exception flag is not a problem!
*/

SafeFPUControl::~SafeFPUControl()
{
    std::feclearexcept(FE_ALL_EXCEPT);
}
