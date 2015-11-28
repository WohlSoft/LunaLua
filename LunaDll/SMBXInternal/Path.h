#ifndef Path_hhhhh
#define Path_hhhhh

#include "../Defines.h"

#pragma pack(push, 1)
struct SMBXPath
{
    Momentum momentum;
    short unkField;
    short id;

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBXPath* Get(unsigned short index) {
        if (index >= GM_PATH_COUNT) return NULL;
        return &((SMBXPath*)GM_PATH_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_PATH_COUNT;
    }

    static const short MAX_ID = 32;

};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXPath) == 0x34, "sizeof(SMBXPath) must be 0x34");
#endif

#endif

