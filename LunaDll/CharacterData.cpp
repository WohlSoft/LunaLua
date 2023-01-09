#include <unordered_map>
#include <memory>
#include "CharacterData.h"
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../Defines.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"


// Data structures
struct ExtraCharacterDataStruct {
public:
    ExtraCharacterDataStruct(short id, short base)
    {
        mCanSlide     = (base == CHARACTER_MARIO || base == CHARACTER_LUIGI);
        mCanRideYoshi = mCanSlide;
        mCanRideBoot  = (base != CHARACTER_LINK);
    }
    ~ExtraCharacterDataStruct()
    {
    }

public:
    // extra data
    bool mCanSlide;
    bool mCanRideYoshi;
    bool mCanRideBoot;
};


static std::unordered_map<short, std::unique_ptr<ExtraCharacterDataStruct>> extraCharacteDataMap;

// basic registry functions
void extraCharacterDataRegister(short id, short base)
{
    extraCharacteDataMap[id] = std::make_unique<ExtraCharacterDataStruct>(id, base);
}
void extraCharacterDataUnregister(short id)
{
    extraCharacteDataMap.erase(id);
}
void extraCharacterDataReset()
{
    extraCharacteDataMap.clear();
    // initialize base characters
    for (int i = 1; i <= 5; i++) {
        extraCharacterDataRegister(i, i);
    }
}

// macro to define getter / setter funcs for data fields
#define _CHARACTERDATA_DEFINE(TYPE, DEFAULT, PROPERTY, GET, SET) \
    TYPE GET(short id) { \
        auto it = extraCharacteDataMap.find(id); \
        if (it != extraCharacteDataMap.end()) { return it->second->PROPERTY; } \
        else { return DEFAULT; } \
    } \
    void SET(short id, TYPE value) { \
        auto it = extraCharacteDataMap.find(id); \
        if (it != extraCharacteDataMap.end()) { it->second->PROPERTY = value; } \
    }
namespace ExtraCharacterData {
    _CHARACTERDATA_DEFINE(bool, false, mCanSlide,     canSlideGet,     canSlideSet    );
    _CHARACTERDATA_DEFINE(bool, false, mCanRideYoshi, canRideYoshiGet, canRideYoshiSet);
    _CHARACTERDATA_DEFINE(bool, false, mCanRideBoot,  canRideBootGet,  canRideBootSet );
}
#undef _CHARACTERDATA_DEFINE
