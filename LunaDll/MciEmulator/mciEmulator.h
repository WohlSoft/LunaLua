#ifndef MciEmulator_hhhh
#define MciEmulator_hhhh
#include <windows.h>
#include <map>
#include <unordered_map>
#include <string>
#ifdef __MINGW32__
#include <mmsystem.h>
#define __in
#define __out
#define __in_opt
#define __inout
#define __in_bcount(x)
#define __in_ecount(x)
#define __out_ecount(x)
#define __out_bcount(x)
#define __out_ecount_part_opt(x,y)
#define __out_bcount_opt(x)
#define __in_bcount_opt(x)
#define __in_ecount_opt(x)
#define __out_ecount_opt(x)
#define __out_opt
#define __inout_opt
#endif

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

#endif
