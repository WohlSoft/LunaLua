#ifndef CharacterData_hhh
#define CharacterData_hhh

// Resets extended character data, leaving only the 5 base chars
void extraCharacterDataReset();
void extraCharacterDataRegister(short id, short base);
void extraCharacterDataUnregister(short id);

// macro to define getter / setter funcs for data fields
#define _CHARACTERDATA_DEFINE(TYPE, DEFAULT, PROPERTY, GET, SET) \
    TYPE GET(short id); \
    void SET(short id, TYPE value);
namespace ExtraCharacterData {
    _CHARACTERDATA_DEFINE(bool, false, mCanSlide,     canSlideGet,     canSlideSet    );
    _CHARACTERDATA_DEFINE(bool, false, mCanRideYoshi, canRideYoshiGet, canRideYoshiSet);
    _CHARACTERDATA_DEFINE(bool, false, mCanRideBoot,  canRideBootGet,  canRideBootSet );
}
#undef _CHARACTERDATA_DEFINE

#endif