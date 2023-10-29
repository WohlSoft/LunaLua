
#include <mutex>
#include "../LuaMain/LunaLuaMain.h"
#include "SaveData.h"


static GameDataStruct* gameData;
static GameDataStruct* saveData;
static std::mutex gameDataMutex;

GameDataStruct* LunaLuaSetDataRaw(static GameDataStruct* structPtr, const char* dataPtr, int dataLen)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::unique_lock<std::mutex> lck(gameDataMutex);
    if (structPtr != nullptr)
    {
        free(structPtr);
        structPtr = nullptr;
    }
    structPtr = (GameDataStruct*)malloc(dataLen + 1 + sizeof(int)); // store an extra byte so we can read the data as a c string
    if (structPtr != nullptr)
    {
        structPtr->len = dataLen;
        ::memcpy(structPtr->data, dataPtr, dataLen);
        structPtr->data[structPtr->len] = '\0'; // terminate string data
    }
    return structPtr;
}
GameDataStruct* LunaLuaGetDataRaw(static GameDataStruct* structPtr)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::unique_lock<std::mutex> lck(gameDataMutex);
    if (structPtr == nullptr)
    {
        return nullptr;
    }
    GameDataStruct* cpy = (GameDataStruct*)malloc(structPtr->len + 1 + sizeof(int)); // store an extra byte so we can read the data as a c string
    cpy->len = structPtr->len;
    ::memcpy(cpy->data, structPtr->data, structPtr->len);
    cpy->data[structPtr->len] = '\0'; // terminate string data
    return cpy;
}
void LunaLuaFreeReturnedGameDataRaw(GameDataStruct* cpy) {
    CLunaFFILock ffiLock(__FUNCTION__);
    if (cpy == nullptr)
    {
        return;
    }
    free(cpy);
}


void LunaLuaSetSaveDataRaw(const char* dataPtr, int dataLen) {
    saveData = LunaLuaSetDataRaw(saveData, dataPtr, dataLen);
}
GameDataStruct* LunaLuaGetSaveDataRaw() {
    return LunaLuaGetDataRaw(saveData);
}
void LunaLuaSetGameDataRaw(const char* dataPtr, int dataLen) {
    gameData = LunaLuaSetDataRaw(gameData, dataPtr, dataLen);
}
GameDataStruct* LunaLuaGetGameDataRaw() {
    return LunaLuaGetDataRaw(gameData);
}
