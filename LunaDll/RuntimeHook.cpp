#include "RuntimeHook.h"
#include "LuaMain/LunaLuaMain.h"
#include "LuaMain/LuaEvents.h"
#include <comutil.h>
#include "Input.h"
#include "GlobalFuncs.h"
#include "SdlMusic/MusicManager.h"

//std::string utf8_encode(const std::wstring &wstr)
//{
//	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
//	std::string strTo( size_needed, 0 );
//	WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
//	return strTo;
//}

#ifndef NO_SDL
bool episodeStarted=false;
#endif

void (*runAsyncDebuggerProc)(void) = 0;
void (*runAsyncLoggerProc)(void) = 0;
int (*asyncBitBltProc)(HDC, int, int, int, int, HDC, int, int, unsigned int) = 0;
void (*asyncLogProc)(const char*) = 0;
float (*__vbaR4Var)(VARIANTARG*) = 0;

HMODULE newLauncherLib = 0;
HMODULE newDebugger = 0;

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

	if(vecStrFind(args, std::string("--newlauncher")))
		gStartupSettings.newLauncher = true;
}


void TrySkipPatch()
{
	ParseArgs(splitCmdArgs(std::string(GetCommandLineA())));

	if(gStartupSettings.patch){
		memset((void*)0x8BECF2, 0x90, 0x1B5); //nop out the loader code
		*(WORD*)(0xB25046) = -1; //set run to true
		
		PATCH_FUNC(0x8BED00, &InitHook);
	}

	__vbaR4Var = (float(*)(VARIANTARG*))0x00401124;
	*(void**)0x00401124 = (void*)&vbaR4VarHook;

	*(void**)0xB2F244 = (void*)&mciSendStringHookA;
	PATCH_FUNC(0x8D6BB6, &forceTermination);
	PATCH_FUNC(0x8C11D5, &LoadWorld);
	PATCH_FUNC(0x8C16F7, &WorldLoop);
	PATCH_FUNC(0x932353, &printLunaLuaVersion);
	PATCH_FUNC(0x9090F5, &WorldRender);
}

extern void InitHook()
{
	// I still provide this code as a attempt to patch the conversion of string to float.
	// Unfortunatly I made me too much work and now I just patched the import table with my new code.
	// If someone wants to use it to get better understanding to ASM here you are:
	
	HMODULE vmVB6Lib = GetModuleHandleA("msvbvm60.dll");
	if(vmVB6Lib){
		//Get the function conversion code;
		BYTE* baseAddr = (BYTE*)GetProcAddress(vmVB6Lib, "__vbaR8Str");
		//Go to the function call asm to find out the address of the function that returns local language id 
		BYTE* funcAddrASM = baseAddr + 11;
		//Now get the relative address of this function
		DWORD relAddr = *((DWORD*)(funcAddrASM + 1));
		//Convert the relative address to an absolute address
		DWORD targetAddr = relAddr + (DWORD)funcAddrASM + 5;
		//Now get the pointer of the code
		BYTE* targetAddrBYTE = (BYTE*)targetAddr;

		// Normally this function would call another function called GetUserDefaultLCID.
		// In this case we overwrite the function and directly copy the return address to 0
		// After this code vb6 will think that this system has no language installed and
		// uses 1033 (US Code Page) as default. Now vb6 will always use "." as comma!
		DWORD oldprotect; 
		// Now remove the protection to write to the code
		if(VirtualProtect((void*)targetAddrBYTE, 10, PAGE_EXECUTE_READWRITE, &oldprotect)){
			targetAddrBYTE[0] = 0xB8; //MOV EAX, 0
			targetAddrBYTE[1] = 0x00;
			targetAddrBYTE[2] = 0x00;
			targetAddrBYTE[3] = 0x00;
			targetAddrBYTE[4] = 0x00;
			targetAddrBYTE[5] = 0x90; //NOP
			// Now get the protection back
			VirtualProtect((void*)targetAddrBYTE, 10, oldprotect, &oldprotect);
		}

		/*std::string output = "";
		output += std::to_string((long long)targetAddrBYTE[0]) + " ";
		output += std::to_string((long long)targetAddrBYTE[1]) + " ";
		output += std::to_string((long long)targetAddrBYTE[2]) + " ";
		output += std::to_string((long long)targetAddrBYTE[3]) + " ";
		output += std::to_string((long long)targetAddrBYTE[4]) + " ";
		output += std::to_string((long long)targetAddrBYTE[5]) + " ";

		MessageBoxA(NULL, output.c_str(), "Dbg", NULL);*/
	}
	
	if(gStartupSettings.newLauncher){
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
		GL_PROC_CHECK(hRunProc, run, newLauncherLib, LunadllNewLauncher.dll)
		GetPromptResultProc hPrompt = (GetPromptResultProc)GetProcAddress(newLauncherLib, "GetPromptResult");
		GL_PROC_CHECK(hPrompt, GetPromptResult, newLauncherLib, LunadllNewLauncher.dll)
		FreeVarsProc hFreeVarsProc = (FreeVarsProc)GetProcAddress(newLauncherLib, "FreeVars");
		GL_PROC_CHECK(hFreeVarsProc, FreeVars, newLauncherLib, LunadllNewLauncher.dll)
		hRunProc();
		resultStruct settings;
		hPrompt((void*)&settings);

		if(!settings.result){
			GM_ISLEVELEDITORMODE = 0; //set run to false
			_exit(0);
		}
		GM_ISGAME = -1;
		if(settings.result == 2){
			GM_ISLEVELEDITORMODE = -1;
		}
		GM_NOSOUND = COMBOOL(settings.NoSound);
		GM_FRAMESKIP = COMBOOL(settings.disableFrameskip);
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
		GL_PROC_CHECK(runAsyncDebuggerProc, runAsyncDebugger, newDebugger, LunadllNewLauncher.dll)
		GL_PROC_CHECK(asyncBitBltProc, asyncBitBlt, newDebugger, LunadllNewLauncher.dll)
		//PATCH_JMP(0x4242D0, &bitBltHook);
		
		*(void**)0xB2F1D8 = (void*)asyncBitBltProc;
		runAsyncDebuggerProc();
	}
	if(gStartupSettings.logger){
		if(!newDebugger)
			newDebugger = LoadLibraryA("LunadllNewLauncher.dll");

		runAsyncLoggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncLogger");
		asyncLogProc = (void(*)(const char*))GetProcAddress(newDebugger, "asyncLog");
		GL_PROC_CHECK(runAsyncLoggerProc, runAsyncLogger, newDebugger, LunadllNewLauncher.dll)
		GL_PROC_CHECK(asyncLogProc, asyncLog, newDebugger, LunadllNewLauncher.dll)
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
#ifndef NO_SDL
	if(!episodeStarted)
	{
		std::string wldPath = wstr2str(std::wstring((wchar_t*)GM_FULLDIR));
		MusicManager::loadCustomSounds(wldPath+"\\");
		episodeStarted=true;
	}
#endif

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



extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop)
{
#ifndef NO_SDL
	if(episodeStarted)
	{   //Reset sounds to default when main menu is loaded
		MusicManager::resetSoundsToDefault();
		episodeStarted=false;
	}
#endif
	Render::Print(std::wstring(L"LUNALUA V0.5.2 BETA"), 3, 5, 5);
	if(newDebugger)
	{
		if(asyncBitBltProc){
			return asyncBitBltProc(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
		}
	}
	return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern void* WorldRender()
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

extern float __stdcall vbaR4VarHook(VARIANTARG* variant)
{
	if(asyncLogProc)
	{
		stringstream q;
		q << variant->vt << " ";
		if(variant->vt == VT_R8)
		{
			q<<"src:"<<variant->dblVal<< " dst:"<<static_cast<float>(variant->dblVal);
		}
		string rr("vbaR4VarHook type:"+q.str()+";");
		asyncLogProc( rr.c_str() );
	}

	if(variant->vt == VT_BSTR)
	{
		wchar_t* str = variant->bstrVal;
		return (float)wcstod(str, NULL);
	}
	else
	if(variant->vt == (VT_BSTR|VT_BYREF))
	{
		if(variant->pbstrVal==NULL) return 0.0;
		wchar_t* str = *(variant->pbstrVal);
		return (float)wcstod(str, NULL);
	}
	else
	if(variant->vt == VT_I2)
	{
		short str = variant->iVal;
		return (float)str;
	}
	else
	if(variant->vt == (VT_I2|VT_BYREF))
	{
		if(variant->piVal==NULL) return 0.0;
		return (float)((int)(*(variant->piVal)));
	}
	else
	if(variant->vt == VT_I4)
	{
		long str = variant->lVal;
		return (float)str;
	}
	else
	if(variant->vt == (VT_I4|VT_BYREF))
	{
		if(variant->plVal==NULL) return 0.0;
		return (float)(*variant->plVal);
	}
	else
	if(variant->vt == VT_BOOL)
	{
		bool str = variant->boolVal;
		return (float)(int)str;
	}
	else
	if(variant->vt == (VT_BOOL|VT_BYREF))
	{
		if(variant->pboolVal==NULL) return 0.0;
		bool str = *(variant->pboolVal);
		return (float)(int)str;
	}
	else
	if(variant->vt == VT_R4)
	{
		return variant->fltVal;
	}
	else
	if(variant->vt == (VT_R4|VT_BYREF))
	{
		if(variant->pfltVal==NULL) return 0.0;
		return (*variant->pfltVal);
	}
	else
	if(variant->vt == VT_R8)
	{
		return static_cast<float>(variant->dblVal);
	}
	else
	if(variant->vt == (VT_R8|VT_BYREF))
	{
		if(variant->pdblVal==NULL) return 0.0;
		return (float)(*variant->pdblVal);
	}
	else
	if(variant->vt == VT_CY)
	{
		CY x = variant->cyVal;
		float y=(float)x.Hi;
		float z=(float)x.Lo;
		while(z>1.0)
		{
			z/=10;
		}
		y+=z;
		return y;
	}
	else
	if(variant->vt == (VT_CY|VT_BYREF))
	{
		if(variant->pcyVal==0) return 0.0;
		CY x = *(variant->pcyVal);
		float y=(float)x.Hi;
		float z=(float)x.Lo;
		while(z>1.0)
		{
			z/=10;
		}
		y+=z;
		return y;
	}
	else
	if(variant->vt == VT_UI1)
	{
		return (float)((int)((char)variant->bVal));
	}
	else
	if(variant->vt == (VT_UI1|VT_BYREF))
	{
		return (float)((int)((char)(*(variant->pbVal))));
	}
	return 0.0;//__vbaR4Var(variant);
}

