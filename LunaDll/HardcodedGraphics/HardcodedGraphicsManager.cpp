
#include <fstream>
#include <iostream>
#include <sstream>

#include "HardcodedGraphicsManager.h"
#include "../Defines.h"
#include "../GlobalFuncs.h"
#include "../SdlMusic/MusicManager.h" //Need to get SMBX's application path
#include "../libs/ini-reader/INIReader.h" //Ini files reader


HardcodedGraphicsManager::HardcodedGraphicsManager()
{}


HardcodedGraphicsManager::~HardcodedGraphicsManager()
{}

int HardcodedGraphicsManager::patchGraphics(unsigned int offset_i, const char* filepath)
{
	//Check existing of address
	std::map<unsigned int, int>::iterator it = hardcoded_data_map.find(offset_i);
	if(it == hardcoded_data_map.end())
	{
		return -3;
	}

	//get internal memory size
	int sizeOfMemory = hardcoded_data_map[offset_i]-8;

	std::ifstream graphFile;
    graphFile.open(filepath, std::ios::in|std::ios::binary);
	if(!graphFile.is_open())
		return -2;

	graphFile.seekg(0, graphFile.end);
	std::streamoff length = graphFile.tellg();
	graphFile.seekg(0, graphFile.beg);

	if(length > sizeOfMemory)
	{
		graphFile.close();
		return sizeOfMemory;
	}

	char* buffer = new char[sizeOfMemory];
	for(int i=0;i<sizeOfMemory;i++)
	{
		buffer[i] = 0;
	}
	graphFile.read(buffer, length);
	// The actual patching
	void* addr = FN_OFFSET2ADDR(offset_i);
	memset(addr, 0, sizeOfMemory);
	memcpy(addr, buffer, sizeOfMemory);

	// finalize
	graphFile.close();
	delete buffer;
    return -1;
}

void HardcodedGraphicsManager::loadGraphics()
{
	if(hardcoded_data_map.empty())
		buildMap();

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

    std::string ttscrpath=root+"graphics.ini";
	if( !file_existsX(ttscrpath) ) return;

    INIReader GraphicsINI( ttscrpath );
    if (GraphicsINI.ParseError() < 0)
    {
        MessageBoxA(0, std::string(ttscrpath+"\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }
	
	std::vector<std::string > list= GraphicsINI.getAllSectionKeys("hex");

	for(unsigned int i=0; i<list.size(); i++)
	{
		bool wrong=false;
		RemoveSubStr(list[i], "0x");
		for(unsigned int j=0;j<list[i].size();j++)
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
		std::string imageFile = GraphicsINI.Get("hex", list[i], "");
	    if(imageFile.empty()) continue;
		
		imageFile = root+"graphics\\common\\"+imageFile;
		const char *str = imageFile.c_str();

		//Here we should load file  hexKey - address, imageFile:
		//is a name of image file in the <SMBX>\graphics\common
		patchGraphics(hexKey, str);
	}

}
