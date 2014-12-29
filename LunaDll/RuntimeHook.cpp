#include "RuntimeHook.h"
#include "LuaMain/LunaLuaMain.h"
#include "LuaMain/LuaEvents.h"
#include <comutil.h>
#include "Input.h"

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
	if(LunaLua::mainStateV2){
		LuaEvents::SMBXEventQueueItem i;
		
		i.event = utf8_encode(std::wstring(trigger));
		i.unkVal = unkVal;
		i.callType = type;

	}
}

void ParseArgs(const std::vector<std::string>& args)
{
	if(vecStrFind(args, std::string("--patch")))
		gStartupSettings.patch = true;

	if(vecStrFind(args, std::string("--game")))
		gStartupSettings.game = true;

	if(vecStrFind(args, std::string("--leveleditor")))
		gStartupSettings.lvlEditor = true;

	if(vecStrFind(args, std::string("--noframeskip")))
		gStartupSettings.frameskip = false;

	if(vecStrFind(args, std::string("--nosound")))
		gStartupSettings.noSound = true;

	if(vecStrFind(args, std::string("--debugger")))
		gStartupSettings.debugger = true;

	if(vecStrFind(args, std::string("--logger")))
		gStartupSettings.logger = true;
}


void TrySkipPatch()
{
	ParseArgs(split(std::string(GetCommandLineA()), ' '));

	if(gStartupSettings.patch){
		memset((void*)0x8BECF2, 0x90, 0x1B5); //nop out the loader code
		*(WORD*)(0xB25046) = -1; //set run to true
		
		PATCH_FUNC(0x8BED00, &InitHook);

		//PATCH_FUNC(0x933443, &prTest);
		*(void**)0xB2F244 = (void*)&mciSendStringHookA;
		//8C11D5
	}
	PATCH_FUNC(0x8D6BB6, &forceTermination);
	PATCH_FUNC(0x8C11D5, &LoadWorld);
	PATCH_FUNC(0x8C16F7, &WorldLoop);
	PATCH_FUNC(0x932353, &printLunaLuaVersion);
	PATCH_FUNC(0x9090F5, &renderTest);
}

extern void InitHook()
{
	
	if(!gStartupSettings.game&&!gStartupSettings.lvlEditor){
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
			std::string errMsg = "Failed to load 'run' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			FreeLibrary(newLauncherLib);
			newLauncherLib = NULL;
			return;
		}
		GetPromptResultProc hPrompt = (GetPromptResultProc)GetProcAddress(newLauncherLib, "GetPromptResult");
		if(!hRunProc){
			std::string errMsg = "Failed to load 'GetPromptResult' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			FreeLibrary(newLauncherLib);
			newLauncherLib = NULL;
			return;
		}
		FreeVarsProc hFreeVarsProc = (FreeVarsProc)GetProcAddress(newLauncherLib, "FreeVars");
		if(!hRunProc){
			std::string errMsg = "Failed to load 'FreeVars' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
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
			GM_ISGAME = -1;
			if(settings.result == 2){
				GM_ISLEVELEDITORMODE = -1;
			}
			GM_NOSOUND = COMBOOL(settings.NoSound);
			GM_FRAMESKIP = COMBOOL(settings.disableFrameskip);
		}

		GM_ISLEVELEDITORMODE = 0; //set run to false
		_exit(0);
	}else{
		GM_ISLEVELEDITORMODE = COMBOOL(gStartupSettings.lvlEditor);
		GM_ISGAME = COMBOOL(gStartupSettings.game);
		GM_FRAMESKIP = COMBOOL(gStartupSettings.frameskip);
		GM_NOSOUND = COMBOOL(gStartupSettings.noSound);
	}

	if(gStartupSettings.debugger){
		newDebugger = LoadLibraryA("LunadllNewLauncher.dll");
		if(!newDebugger){
			std::string errMsg = "Failed to load the new Launcher D:!\nLunadllNewLauncher.dll is missing?\nError Code: ";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			newDebugger = NULL;
			return;
		}
		runAsyncDebuggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncDebugger");
		asyncBitBltProc = (int (*)(HDC, int, int, int, int, HDC, int, int, unsigned int))GetProcAddress(newDebugger, "asyncBitBlt@36");
		if(!runAsyncDebuggerProc){
			std::string errMsg = "Failed to load 'runAsyncDebugger' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			FreeLibrary(newDebugger);
			newDebugger = NULL;
			return;
		}
		if(!asyncBitBltProc){
			std::string errMsg = "Failed to load 'asyncBitBlt' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			FreeLibrary(newDebugger);
			newDebugger = NULL;
			return;
		}
		//PATCH_JMP(0x4242D0, &bitBltHook);
		
		*(void**)0xB2F1D8 = (void*)asyncBitBltProc;
		runAsyncDebuggerProc();
	}
	if(gStartupSettings.logger){
		if(!newDebugger)
			newDebugger = LoadLibraryA("LunadllNewLauncher.dll");

		runAsyncLoggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncLogger");
		asyncLogProc = (void(*)(const char*))GetProcAddress(newDebugger, "asyncLog");
		if(!runAsyncLoggerProc){
			std::string errMsg = "Failed to load 'runAsyncLogger' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			FreeLibrary(newDebugger);
			newDebugger = NULL;
			return;
		}
		if(!asyncLogProc){
			std::string errMsg = "Failed to load 'asyncLog' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
			errMsg += std::to_string((long long)GetLastError());
			MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
			FreeLibrary(newDebugger);
			newDebugger = NULL;
			return;
		}
		runAsyncLoggerProc();
	}
	
	/*void (*exitCall)(void);
	exitCall = (void(*)(void))0x8D6BB0;
	exitCall();*/
	
	
	

}

long long dbglongTest = 0;

extern void forceTermination()
{
	_exit(0);
}

extern int LoadWorld()
{
	resetDefines();

	gSkipSMBXHUD = false;
	gIsOverworld = true;
	gLunaRender.ClearAll();
	gSpriteMan.ResetSpriteManager();
	gCellMan.Reset();
	gSavedVarBank.ClearBank();	
	Input::ResetAll();

	gLunaRender.ReloadScreenHDC();

	// Init var bank
	gSavedVarBank.TryLoadWorldVars();
	gSavedVarBank.CheckSaveDeletion();
	gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

	LunaLua::initWorld(std::wstring((wchar_t*)GM_FULLDIR));

	// Recount deaths
	gDeathCounter.Recount();

	short plValue = GM_PLAYERS_COUNT;
	__asm {
		MOV EAX, 1
		MOV CX, plValue
	}
}

extern DWORD WorldLoop()
{

	gSavedVarBank.CheckSaveDeletion();

	// Update inputs
	Input::CheckSpecialCheats();
	Input::UpdateInputTasks();	

	LunaLua::DoWorld();

	gSavedVarBank.SaveIfNeeded();

	return GetTickCount();
}

extern DWORD bitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop)
{
	if(newDebugger){
		//asyncBitBltProc(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
	}
	return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop)
{
	Render::Print(std::wstring(L"LUNALUA V0.5.1 BETA"), 3, 5, 5);
	if(newDebugger){
		if(asyncBitBltProc){
			return asyncBitBltProc(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
		}
	}
	return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern void* renderTest()
{
	if(gShowDemoCounter)
		gDeathCounter.Draw();

	gSpriteMan.RunSprites();
	gLunaRender.RenderAll();

	return (void*)0xB25010;
}

extern MCIERROR __stdcall mciSendStringHookA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback)
{
	bool doLogInput = true;
	bool doLogOutput = true;

	if(lpstrCommand == 0){
		doLogInput = false;
	}else{
		if(lpstrCommand[0] == 0){
			doLogInput = false;
		}
	}

	std::string inputStr = "";
	std::string outputStr = "";

	if(doLogInput){
		inputStr += "Input: ";
		inputStr += lpstrCommand;

		if(newDebugger){
			if(asyncLogProc){
				asyncLogProc(inputStr.c_str());
			}
		}
	}

	//Swap to restore old code or to use emulator
	//MCIERROR ret = mciSendStringA(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
	MCIERROR ret = gMciEmulator.mciEmulate(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
	
	if(lpstrReturnString == 0){
		doLogOutput = false;
	}else{
		if(lpstrReturnString[0] == 0){
			doLogOutput = false;
		}
	}
	if(doLogOutput){
		outputStr += "Output: ";
		outputStr += lpstrReturnString;

		if(newDebugger){
			if(asyncLogProc){
				asyncLogProc(outputStr.c_str());
			}
		}
	}
	return ret;
}



