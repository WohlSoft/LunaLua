#ifndef SHMEMSERVER_H
#define SHMEMSERVER_H

#include <windows.h>
#include <string>

#define BUF_SIZE 15360
#define SHMEM_NAME L"LunaDLL_LevelFileName_834727238"

class SHMemServer
{
public:
    SHMemServer();
    ~SHMemServer();
    std::wstring read();
private:
    wchar_t *pBuf; //<--------- Shared memory!----------------
    std::wstring bufIn; //<----------------------------- Input WString buffer --------------------------------
    HANDLE hMapFile;
	bool isInit;
};

#endif // SHMEMSERVER_H
