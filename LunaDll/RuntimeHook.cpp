#include "RuntimeHook.h"
#include "LuaMain/LunaLuaMain.h"
#include "LuaMain/LuaEvents.h"
#include <comutil.h>

std::string utf8_encode(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo( size_needed, 0 );
	WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}


void RuntimePatch()
{
	//typedef void wordLoadFunc(BSTR* path);
	PATCH_FUNC(0x008C0DB6,&getSMBXTriggerMain);
	PATCH_FUNC(0x008C0E42,&getSMBXTriggerMain);
    PATCH_FUNC(0x008C1FD4,&getSMBXTriggerMain);
	PATCH_FUNC(0x008C205B,&getSMBXTriggerMain);
	PATCH_FUNC(0x009B233F,&getSMBXTriggerTalk);
	PATCH_FUNC(0x009E1247,&getSMBXTriggerUnknown);
	PATCH_FUNC(0x009E140C,&getSMBXTriggerUnknown);
	PATCH_FUNC(0x009E270B,&getSMBXTriggerUnknown);
	PATCH_FUNC(0x009E2806,&getSMBXTriggerUnknown);
	PATCH_FUNC(0x009E4409,&getSMBXTriggerPSwitch);
	PATCH_FUNC(0x00A07D5D,&getSMBXTriggerUnknown);
	PATCH_FUNC(0x00A07F5A,&getSMBXTriggerUnknown);
	PATCH_FUNC(0x00A082C5,&getSMBXTriggerActivateEventLayer);
	PATCH_FUNC(0x00A08862,&getSMBXTriggerActivateEventLayer);
	PATCH_FUNC(0x00A32585,&getSMBXTriggerDeathEvent);
	PATCH_FUNC(0x00A3274C,&getSMBXTriggerNoMoreObjEvent);
	PATCH_FUNC(0x00AA5FCA,&getSMBXTriggerEventTrigger);
	PATCH_FUNC(0x00AA6257,&getSMBXTriggerUnknown);
}

int getSMBXTriggerMain(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 0);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

int getSMBXTriggerTalk(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 1);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

int getSMBXTriggerNoMoreObjEvent(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 2);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

int getSMBXTriggerUnknown(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, -1);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

int getSMBXTriggerPSwitch(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 3);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

int getSMBXTriggerActivateEventLayer(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 4);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

int getSMBXTriggerDeathEvent(BSTR* trigger, int* unkVal)
{
	/*wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 5);*/
	wchar_t wbuf[250];
	ZeroMemory(wbuf, 500);
	wchar_t* wbufptr = wbuf+2;

	// Place the length of the string in bytes first
	int strlen = (*trigger)[-2];
	wbuf[0] = strlen;

	// Copy the string's bytes 4 bytes ahead of the len
	memcpy((void*)&wbuf[2], (void*)trigger, strlen);

	int daVal = *unkVal;

	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)((BSTR*)(&wbufptr), &daVal);
	return retVal;
}

int getSMBXTriggerEventTrigger(BSTR* trigger, int* unkVal)
{
	wchar_t buf[250];
	wcscpy_s(buf, *(wchar_t**)trigger);
	int cpUnkVal = *unkVal;
	record_SMBXTrigger(buf, cpUnkVal, 6);
	int retVal = ((SMBXTrigger*)GF_TRIGGER_EVENT)(trigger, unkVal);
	return retVal;
}

void record_SMBXTrigger(wchar_t* trigger, int unkVal, int type)
{
	if(LunaLua::mainState || LunaLua::mainStateGlobal){
		LuaEvents::SMBXEventQueueItem i;
		
		i.event = utf8_encode(std::wstring(trigger));
		i.unkVal = unkVal;
		i.callType = type;

	}
}



void TrySkipPatch()
{
	//PATCH_JMP(0x008BED22,0x008BECF2);
	//*(BYTE*)(0x8BECF7) = 0x90;
	memset((void*)0x8BECF2, 0x90, 0x1B5); //nop out the loader code
	*(WORD*)(0xB25046) = -1; //set run to true
	PATCH_FUNC(0x8BED00, &InitHook);
}

extern void InitHook()
{
	typedef bool (*RunProc)(void);
	typedef void (*GetPromptResultProc)(void*);
	typedef void (*FreeVarsProc)(void);
	newLauncherLib = LoadLibraryA("LunadllNewLauncher.dll");
	if(!newLauncherLib){
		std::string errMsg = "Failed to load the new Launcher D:!\nLunadllNewLauncher.dll is missing?\nError Code: ";
		errMsg += std::to_string((long long)GetLastError());
		MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
		return;
	}
	RunProc hRunProc = (RunProc)GetProcAddress(newLauncherLib, "run");
	if(!hRunProc){
		std::string errMsg = "Failed to load 'run' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll diffrent versions?\nError code:";
		errMsg += std::to_string((long long)GetLastError());
		MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
		FreeLibrary(newLauncherLib);
		newLauncherLib = NULL;
		return;
	}
	GetPromptResultProc hPrompt = (GetPromptResultProc)GetProcAddress(newLauncherLib, "GetPromptResult");
	if(!hRunProc){
		std::string errMsg = "Failed to load 'GetPromptResult' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll diffrent versions?\nError code:";
		errMsg += std::to_string((long long)GetLastError());
		MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
		FreeLibrary(newLauncherLib);
		newLauncherLib = NULL;
		return;
	}
	FreeVarsProc hFreeVarsProc = (FreeVarsProc)GetProcAddress(newLauncherLib, "FreeVars");
	if(!hRunProc){
		std::string errMsg = "Failed to load 'FreeVars' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll diffrent versions?\nError code:";
		errMsg += std::to_string((long long)GetLastError());
		MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
		FreeLibrary(newLauncherLib);
		newLauncherLib = NULL;
		return;
	}
	hRunProc();
	resultStruct settings;
	hPrompt((void*)&settings);
	hFreeVarsProc();
	FreeLibrary(newLauncherLib);
	newLauncherLib = NULL;

	if(settings.result){
		if(settings.result == 2){
			*(WORD*)(0xB25134) = -1;
		}
		*(WORD*)(0xB2D734) = (settings.NoSound ? -1 : 0);
		*(WORD*)(0xB2C684) = (settings.disableFrameskip ? 0 : -1);
	}else{
		*(WORD*)(0xB25046) = 0; //set run to true
		void (*exitCall)(void);
		exitCall = (void(*)(void))0x8D6BB0;
		exitCall();
		std::exit(0);
	}
	

}
