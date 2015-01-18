#include "HardcodedGraphicsManager.h"
#include "../Defines.h"
#include "../GlobalFuncs.h"
#include "../SdlMusic/MusicManager.h" //Need to get SMBX's application path
#include "../libs/ini-reader/INIReader.h" //Ini files reader
#include <fstream>
#include <iostream>
#include <sstream>

HardcodedGraphicsManager::HardcodedGraphicsManager(void)
{}


HardcodedGraphicsManager::~HardcodedGraphicsManager(void)
{}

int HardcodedGraphicsManager::patchGraphics(void* offset, char* filepath, int sizeOfMemory)
{
	std::ifstream graphFile;
    graphFile.open(filepath, std::ios::in|std::ios::binary);
	if(!graphFile.is_open())
		return -2;
	
	graphFile.seekg(0, graphFile.end);
	int length = graphFile.tellg();
	graphFile.seekg(0, graphFile.beg);
	if(length > sizeOfMemory){
		graphFile.close();
		return length;
	}

	char* buffer = new char[sizeOfMemory];
	graphFile.read(buffer, length);
	// The actual patching
	void* addr = FN_OFFSET2ADDR(offset);
	memset(addr, 0, sizeOfMemory);
	memcpy(addr, buffer, sizeOfMemory);

	// finalize
	graphFile.close();
	delete buffer;
    return -1;
}

void HardcodedGraphicsManager::loadGraphics(std::string imgRootPath)
{
    #ifndef NO_SDL
    MusicManager::initAudioEngine();//Init audio engine if it not initalized
    std::string root=PGE_SDL_Manager::appPath;
    #else
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    int count = GetModuleFileNameW(hModule, path, MAX_PATH);
    for(int i = count; i > 3; i--)
    {
        if(path[i] == L'\\')
        {
            path[i] = 0;
            break;
        }
    }
    std::wstring smbxPath = path;
    smbxPath = smbxPath.append(L"\\");
    std::string root = wstr2str(smbxPath);
    #endif

    if( !file_existsX(imgRootPath) ) return;
    std::string ttscrpath=root+"graphics.ini";

    INIReader GraphicsINI( ttscrpath );
    if (GraphicsINI.ParseError() < 0)
    {
        MessageBoxA(0, std::string(ttscrpath+"\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }

	std::vector<std::string > list= GraphicsINI.getAllSectionKeys("keys");
	for(int i=0; i<list.size(); i++)
	{
		bool wrong=false;
		RemoveSubStr(list[i], "0x");
		for(int j=0;j<list[i].size();j++)
		{
			if(!isdigit(list[i][j]))
			{
				if((list[i][j]!='a')&&(list[i][j]!='A')&&
					(list[i][j]!='b')&&(list[i][j]!='B')&&
					(list[i][j]!='c')&&(list[i][j]!='C')&&
					(list[i][j]!='d')&&(list[i][j]!='D')&&
					(list[i][j]!='e')&&(list[i][j]!='E')&&
					(list[i][j]!='f')&&(list[i][j]!='F'))
				{
					wrong=true; break;
				}
			}
		}
		if(wrong) continue;

		unsigned int hexKey;   
		std::stringstream ss;
		ss << std::hex << list[i];
		ss >> hexKey;
		std::string imageFile = GraphicsINI.Get("HEX", list[i], "");
	    if(imageFile.empty()) continue;
		
		//Here we should load file  hexKey - address, imageFile:
		//is a name of image file in the <SMBX>\graphics\common

	}

}
