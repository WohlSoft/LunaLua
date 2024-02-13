#if !defined(SMBXINTERNAL_PORTS_H)
#define SMBXINTERNAL_PORTS_H

#include "Types.h"

namespace SMBX13 {
    namespace Ports {
        extern bool _enablePowerupPowerdownPositionFixes;

        void __stdcall PlayerEffects(int16_t& A);
    }
}

#endif // !defined(SMBXINTERNAL_PORTS_H)
