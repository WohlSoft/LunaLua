#pragma once
class HardcodedGraphicsManager
{
public:
	HardcodedGraphicsManager(void);
	~HardcodedGraphicsManager(void);

	
	//************************************
	// Method:    patchGraphics
	// FullName:  HardcodedGraphicsManager::patchGraphics
	// Access:    public 
	// Returns:   int -1 if Successful, -2 If file not found or the size of the file if too big to be patched.
	// Qualifier:
	// Parameter: void * offset The memory offset from the beginning of the EXE file.
	// Parameter: char * filepath The image file to patch
	// Parameter: int sizeOfFile The size of the original image memory segment
	//************************************
	int patchGraphics(void* offset, char* filepath, int sizeOfMemory);

};

