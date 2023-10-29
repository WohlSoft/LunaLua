
#ifndef SaveData_hhhhhhhhhhhhhh
#define SaveData_hhhhhhhhhhhhhh


typedef struct
{
    int len;
    char data[1];
} GameDataStruct;
void LunaLuaSetSaveDataRaw(const char* dataPtr, int dataLen);
GameDataStruct* LunaLuaGetSaveDataRaw();
void LunaLuaSetGameDataRaw(const char* dataPtr, int dataLen);
GameDataStruct* LunaLuaGetGameDataRaw();
void LunaLuaFreeReturnedGameDataRaw(GameDataStruct* cpy);


#endif
