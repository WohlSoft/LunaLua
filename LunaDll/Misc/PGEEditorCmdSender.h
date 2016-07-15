#ifndef PGE_EDITORFEEDBACK_H
#define PGE_EDITORFEEDBACK_H

#include <windows.h>
#include <string>
#include <algorithm>

#define PGE_EDITOR_SEMAPHORE     L"PGE_EDITOR_SEMAF_wejQEThQetjqetJQEtjQeTJQTYJ"
#define PGE_EDITOR_SHARED_MEMORY L"PGE_EDITOR_SHMEM_wejQEThQetjqetJQEtjQeTJQTYJ"

class WinSemaphore
{
    HANDLE*         m_semaphore;
    std::wstring    m_key;
public:
    WinSemaphore(const std::wstring &key, int initialValue = 0);
    ~WinSemaphore();

    void setKey(const std::wstring &key, int initialValue = 0);
    std::wstring key() const;

    bool acquire();
    bool release(int n=1);
    void close();
};

class WinSemaphoreLocker
{
    WinSemaphore* m_sema;
public:
    WinSemaphoreLocker(WinSemaphore* sema);
    ~WinSemaphoreLocker();
};

class WinSHMemAttacher
{
    HANDLE          m_shmem;
    std::wstring    m_key;
    void*           m_data;
    int             m_size;

public:
    WinSHMemAttacher(const std::wstring &key, int size);
    ~WinSHMemAttacher();
    bool attach();
    void detach();
    void* data();
};


class PGE_EditorCommandSender
{
    WinSemaphore     m_sema;
    WinSHMemAttacher m_shmem;
public:
    PGE_EditorCommandSender();
    void sendCommandW(const std::wstring &command);
    void sendCommandUTF8(const std::string &command);
};

#endif // PGE_EDITORFEEDBACK_H
