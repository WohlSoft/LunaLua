#include "HardcodedGraphicsManager.h"
#include "Defines.h"
#include "GlobalFuncs.h"

#include <fstream>
#include <iostream>

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
