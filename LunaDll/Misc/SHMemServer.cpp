#include "SHMemServer.h"

//to read data use the shmem.read(); it will return std::wstring value

SHMemServer::SHMemServer()
{
	isInit=false;
    wchar_t szName[]=SHMEM_NAME;
    hMapFile = CreateFileMappingW(
                  INVALID_HANDLE_VALUE,    // use paging file
                  NULL,                    // default security
                  PAGE_READWRITE,          // read/write access
                  0,                       // maximum object size (high-order DWORD)
                  BUF_SIZE,                // maximum object size (low-order DWORD)
                  szName);                 // name of mapping object

    if(hMapFile == NULL)
    {
        MessageBoxA(NULL, "Could not create file mapping object", "Shared memory server", MB_OK);
        return;
    }
    pBuf = (wchar_t *) MapViewOfFile(hMapFile,   // handle to map object
                         FILE_MAP_ALL_ACCESS, // read/write permission
                         0,
                         0,
                         BUF_SIZE);

    if (pBuf == NULL)
    {
        MessageBoxA(NULL, "Could not map view of file", "Shared memory server", MB_OK);
        CloseHandle(hMapFile);
        return;
    }
	isInit=true;
    //MessageBoxA(NULL, "Installed!", "Shared memory server", MB_OK);
}

SHMemServer::~SHMemServer()
{
    //MessageBoxA(NULL, "Destroyed!", "Shared memory server", MB_OK);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

std::wstring SHMemServer::read()
{
	if(!isInit) return std::wstring(L"");
    bufIn = std::wstring(pBuf);
    return bufIn;
}

