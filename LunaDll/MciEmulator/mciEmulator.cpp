#include "mciEmulator.h"
#include "../SdlMusic/MusicManager.h"
#include <vector>
//#include "Globals.h"
#include "../GlobalFuncs.h"
#include "../LuaMain/LuaProxyComponent/LuaProxyAudio.h"

MciEmulator::MciEmulator(void)
{}

MciEmulator::~MciEmulator(void)
{}


#pragma warning(suppress: 6054)
MCIERROR MciEmulator::mciEmulate(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback)
{
#ifndef NO_SDL

	std::string cmd = lpstrCommand;
	std::vector<std::string> spCmd = splitCmdArgs(cmd);
	
	if(spCmd.size() == 2){
		if(spCmd[0] == "pause" && spCmd[1] == "all"){
			//Add pause code
			MusicManager::pause();
		}else if(spCmd[0] == "close"){
			std::map<std::string, regSoundFile>::iterator it = registeredFiles.find(spCmd[1]);
			if(it != registeredFiles.end()){
				
				//remove registration
				registeredFiles.erase(it);
				MusicManager::close();
			}
		}else if(spCmd[0] == "stop"){
			std::map<std::string, regSoundFile>::iterator it = registeredFiles.find(spCmd[1]);
			if(it != registeredFiles.end()){
				//do stop code
                MusicManager::stop(spCmd[1]);
			}
		}
	}else if(spCmd.size() == 3){
		if(spCmd[0] == "Status"){
			uReturnLength = 11;
			std::map<std::string, regSoundFile>::iterator it = registeredFiles.find(spCmd[1]);
			if(it != registeredFiles.end())
			{
				if(spCmd[2] == "Position"){
					strcpy(lpstrReturnString, MusicManager::position().c_str());
				}else if(spCmd[2] == "Length"){
					strcpy(lpstrReturnString, MusicManager::lenght().c_str());
				}
			}
			else
			{
				if(spCmd[2] == "Position"){
					strcpy(lpstrReturnString, "00:04:12:45");
				}else if(spCmd[2] == "Length"){
#pragma warning(suppress: 6387)
#pragma warning(suppress: 6386)
					strcpy(lpstrReturnString, "52:12:11:12");
				}
			}
		}
    }else if(spCmd.size() == 4){
		if(spCmd[0] == "open" && spCmd[2] == "alias"){
			//register music/sound file
			regSoundFile snFile;
			snFile.fileName = std::string(spCmd[1]);
			snFile.volume = 400;
			registeredFiles[spCmd[3]] = snFile;
			/******/MusicManager::addSound(spCmd[3], snFile.fileName);/******/
		}else if(spCmd[0] == "play" && spCmd[2] == "from"){
			std::map<std::string, regSoundFile>::iterator it = registeredFiles.find(spCmd[1]);
			if(it != registeredFiles.end()){
				//play code
                MusicManager::play(spCmd[1]);                
			}
		}
	}else if(spCmd.size() == 5){
		if(spCmd[0] == "setaudio" && spCmd[2] == "volume" && spCmd[3] == "to"){
			if(registeredFiles.find(spCmd[1])!=registeredFiles.end()){
				if(is_number(spCmd[4])){
					//set audio volume
                    /******/MusicManager::setVolume(atoi(spCmd[4].c_str()));/******/
                    registeredFiles[spCmd[1]].volume = atoi(spCmd[4].c_str());
				}
			}
		}
	}

//errorFinalize:;
	if(uReturnLength < 2)
	{
		return MCIERR_UNSUPPORTED_FUNCTION;
	}
	if(uReturnLength==2)
	{
#pragma warning(suppress: 6011)
		lpstrReturnString[0] = '0';
		lpstrReturnString[1] = '\0';
	}
	return 0;
#else
	return mciSendStringA(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
#endif
}
