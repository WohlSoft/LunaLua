#pragma once
#include <map>
#include <Windows.h>
class MciEmulator
{
public:
	MciEmulator(void);
	~MciEmulator(void);
	MCIERROR mciEmulate(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback);

	struct regSoundFile{
		std::string fileName;
		int volume;
	};

private:
	std::map<std::string, regSoundFile> registeredFiles;
};

