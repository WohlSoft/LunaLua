//*** Main.h - General function declarations ***
#ifndef Main_Hhh
#define Main_Hhh


// DLL exports
extern "C" __declspec(dllexport) int TestFunc();

extern "C" __declspec(dllexport) int OnLvlLoad();

extern "C" __declspec(dllexport) int HUDHook();

// Other funcs
void LunaDLLInit();

void TestFrameCode();

void LevelFrameCode();

void OnHUDDraw();

void InitLevel();

#endif
