#ifndef MciEmulator_hhhh
#define MciEmulator_hhhh
#include <windows.h>
#include <map>
#include <unordered_map>
#include <string>

class MciEmulator
{
public:
    MciEmulator(void);
    ~MciEmulator(void);
    MCIERROR mciEmulate(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);

    struct regSoundFile{
        std::string fileName;
        int volume;
    };

private:
    std::map<std::string, regSoundFile> registeredFiles;
};

#endif
