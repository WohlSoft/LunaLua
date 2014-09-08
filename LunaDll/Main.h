//*** Main.h - General function declarations ***
#pragma once


// DLL exports
extern "C" __declspec(dllexport) int TestFunc();

extern "C" __declspec(dllexport) int OnLvlLoad();

extern "C" __declspec(dllexport) int HUDHook();

// Other funcs
void TestFrameCode();

void LevelFrameCode();

void OnHUDDraw();

void InitLevel();