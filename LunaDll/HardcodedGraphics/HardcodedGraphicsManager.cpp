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

int HardcodedGraphicsManager::patchGraphics(void* offset, const char* filepath, int sizeOfMemory)
{
	std::ifstream graphFile;
    graphFile.open(filepath, std::ios::in|std::ios::binary);
	if(!graphFile.is_open())
		return -2;
	
	graphFile.seekg(0, graphFile.end);
	std::streamoff length = graphFile.tellg();
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

void toLog(std::string str)
{
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

	FILE *x = fopen(std::string(root+"\\lunalog.txt").c_str(), "a") ;
	fprintf(x, "%s\n", str.c_str());
	fclose(x);
}

void HardcodedGraphicsManager::loadGraphics()
{
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

	toLog("get hex key");

	std::vector<std::string > list= GraphicsINI.getAllSectionKeys("hex");
	if(list.empty())
		toLog("...no keys :(");
	else
		toLog("...keys found :)");

	for(int i=0; i<list.size(); i++)
	{
		toLog("key"+list[i]);

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
		toLog("is wrong?");
		if(wrong) continue;

		toLog("get hex key");
		unsigned int hexKey;   
		std::stringstream ss;
		ss << std::hex << list[i];
		ss >> hexKey;
		std::string imageFile = GraphicsINI.Get("hex", list[i], "");
	    if(imageFile.empty()) continue;
		
		toLog("check file");
		imageFile = root+"graphics\\common\\"+imageFile;
		const char *str = imageFile.c_str();

		toLog(imageFile+" "+list[i]);

		toLog("patch");
		//Here we should load file  hexKey - address, imageFile:
		//is a name of image file in the <SMBX>\graphics\common
		patchGraphics((void*)hexKey, str, 34816);
		toLog("done");
	}

}
