#include <unordered_map>
#include <memory>
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../Defines.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"
#include "CharacterData.h"

// Data structures
struct ExtraCharacterDataStruct {
public:
    ExtraCharacterDataStruct(short id, short base)
    {
        mCanSlide     = (base == CHARACTER_MARIO || base == CHARACTER_LUIGI);
        mCanRideYoshi = mCanSlide;
        mCanRideBoot  = (base != CHARACTER_LINK);
        mCanSpinJump  = (base != CHARACTER_PEACH && base != CHARACTER_LINK);
        switch (base) {
        case CHARACTER_MARIO:
            mDeathEffect = 3;
            break;
        case CHARACTER_LUIGI:
            mDeathEffect = 5;
            break;
        case CHARACTER_PEACH:
            mDeathEffect = 129;
            break;
        case CHARACTER_TOAD:
            mDeathEffect = 130;
            break;
        case CHARACTER_LINK:
            mDeathEffect = 134;
            break;
        }

        memset(mHitbox, 0, PowerupState::MAX_ID * sizeof(CharacterHitBoxData));
    }
    ~ExtraCharacterDataStruct()
    {
    }

    // hitboxes
    CharacterHitBoxData mHitbox[PowerupState::MAX_ID];

    // extra data
    bool mCanSlide;
    bool mCanRideYoshi;
    bool mCanRideBoot;
    bool mCanSpinJump;
    short mDeathEffect;

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
CharacterHitBoxData* characterDataGetHitboxes(short characterId, short powerupId)
{
    if (powerupId < 1 || powerupId > PowerupState::MAX_ID) {
        return nullptr;
    }
    auto it = extraCharacteDataMap.find(characterId);
    if (it != extraCharacteDataMap.end())
    {
        return &(it->second->mHitbox[powerupId-1]);
    }
    return nullptr;
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
    _CHARACTERDATA_DEFINE(bool , false, mCanSlide,     canSlideGet,     canSlideSet    );
    _CHARACTERDATA_DEFINE(bool , false, mCanRideYoshi, canRideYoshiGet, canRideYoshiSet);
    _CHARACTERDATA_DEFINE(bool , false, mCanRideBoot,  canRideBootGet,  canRideBootSet );
    _CHARACTERDATA_DEFINE(bool , false, mCanSpinJump,  canSpinJumpGet,  canSpinJumpSet );
    _CHARACTERDATA_DEFINE(short, 3    , mDeathEffect,  deathEffectGet,  deathEffectSet );
}
#undef _CHARACTERDATA_DEFINE
