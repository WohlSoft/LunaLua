#pragma once
#include "Defines.h"
#include "Globals.h"
#pragma comment(lib, "comsuppw.lib")

#define PATCH_FUNC(ptr, func) *(BYTE*)ptr = 0xE8;\
	*((DWORD*)(ptr+1)) = ((DWORD)(((DWORD)func) - ptr - 5))

#define PATCH_JMP(dest, source) *(BYTE*)source = 0xE9;\
	*((DWORD*)(source+1)) = ((DWORD)(((DWORD)dest) - source - 5))

#define COMBOOL(b) (b ? -1 : 0)

typedef int SMBXTrigger(BSTR*, int*);

void ParseArgs(const std::vector<std::string>& args);

void TrySkipPatch();

//Hooks
extern void InitHook();
extern void forceTermination();
extern int DbgTestHook();
extern DWORD DbgWorldTestHook();

//DBG
extern long long dbglongTest;

//Libs
extern HMODULE newLauncherLib = 0;

struct resultStruct{
	int result;
	bool disableFrameskip;
	bool NoSound;
};




//scraped for now..... D:
void RuntimePatch();

//Event Hooks
void record_SMBXTrigger(wchar_t* trigger, int unkVal, int type);

int getSMBXTriggerMain(BSTR* trigger, int* unkVal);
int getSMBXTriggerTalk(BSTR* trigger, int* unkVal);
int getSMBXTriggerUnknown(BSTR* trigger, int* unkVal);
int getSMBXTriggerPSwitch(BSTR* trigger, int* unkVal);
int getSMBXTriggerActivateEventLayer(BSTR* trigger, int* unkVal);
int getSMBXTriggerDeathEvent(BSTR* trigger, int* unkVal);
int getSMBXTriggerNoMoreObjEvent(BSTR* trigger, int* unkVal);
int getSMBXTriggerEventTrigger(BSTR* trigger, int* unkVal);