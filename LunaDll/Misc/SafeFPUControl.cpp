#include "SafeFPUControl.h"

/*
 FIXME: Right now disabling all FPU exceptions will crash SMBX with the SSE2 error 0xc00002b4 by _lj_set_tab function by luajit.
*/
