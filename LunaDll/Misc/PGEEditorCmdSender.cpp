#include "PGEEditorCmdSender.h"
#include "../GlobalFuncs.h"

WinSemaphore::WinSemaphore(const std::wstring &key, int initialValue) :
    m_semaphore(nullptr), m_key(key)
{
    setKey(key, initialValue);
}

WinSemaphore::~WinSemaphore()
{
    close();
}

void WinSemaphore::setKey(const std::wstring &key, int initialValue)
{
    if(m_semaphore)
        CloseHandle(m_semaphore);
    m_key = key;
    m_semaphore = (HANDLE*)CreateSemaphoreW(NULL, initialValue, max(initialValue, 1), m_key.c_str());
    if (m_semaphore == NULL)
    {
        m_semaphore = (HANDLE*)OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, 0, m_key.c_str());
    }
}

std::wstring WinSemaphore::key() const
{
    return m_key;
}



bool WinSemaphore::acquire()
{
    DWORD r  =  WaitForSingleObject(m_semaphore, 3000L);
    return r == WAIT_OBJECT_0;
}

bool WinSemaphore::release(int n)
{
    BOOL r = ReleaseSemaphore(m_semaphore, n, NULL);
    return r != FALSE;
}

void WinSemaphore::close()
{
    CloseHandle(m_semaphore);
}




WinSHMemAttacher::WinSHMemAttacher(const std::wstring &key, int size):
    m_shmem(nullptr),
    m_key(key),
    m_data(nullptr),
    m_size(size)
{}

bool WinSHMemAttacher::attach()
{
    m_shmem = OpenFileMappingW( FILE_MAP_ALL_ACCESS,   // read/write access
                                FALSE,                 // do not inherit the name
                                m_key.c_str() );       // name of mapping object
    if( m_shmem )
    {
        m_data = MapViewOfFile( m_shmem, // handle to map object
                                FILE_MAP_ALL_ACCESS, // read/write permission
                                0,
                                0,
                                m_size );
        return (m_data != NULL);
    }
    return false;
}

void WinSHMemAttacher::detach()
{
    UnmapViewOfFile(m_data);
    CloseHandle(m_shmem);
    m_data = 0;
    m_shmem = 0;
}

WinSHMemAttacher::~WinSHMemAttacher()
{
    detach();
}

void *WinSHMemAttacher::data()
{
    return m_data;
}







PGE_EditorCommandSender::PGE_EditorCommandSender() :
    m_sema(PGE_EDITOR_SEMAPHORE, 1),
    m_shmem(PGE_EDITOR_SHARED_MEMORY, 4096)
{}

void PGE_EditorCommandSender::sendCommandW(const std::wstring &command)
{
    sendCommandUTF8(WStr2Str(command));
}

void PGE_EditorCommandSender::sendCommandUTF8(const std::string &command)
{
    bool needAgain = false;
    do
    {
        if(m_sema.acquire())
        {
            if(m_shmem.attach())
            {
                char* data = (char*) m_shmem.data();
                if( data[0] == 0 )
                {
                    data[0] = 1;
                    int *size = (int*)(data+1);
                    *size = command.size();
                    memcpy(data+1+sizeof(int),
                           command.c_str(),
                           min(command.size(), int(4095-sizeof(int))));
                }
                else
                {
                    needAgain = true;
                }
                m_shmem.detach();
            }
        }
        m_sema.release();
        if(needAgain)
            Sleep(50);
    } while(needAgain);
}
