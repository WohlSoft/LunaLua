#include <Windows.h>
#include <cstring>
#include <cinttypes>
#include "AsmPatch.h"

/*static*/ bool MemoryUnlock::UnProtect(void* addr, std::size_t size, unsigned long* oldFlags)
{
    return (VirtualProtect(addr, size, PAGE_READWRITE, oldFlags) != 0);
}

/*static*/ void MemoryUnlock::ReProtect(void* addr, std::size_t size, unsigned long flags)
{
    unsigned long oldFlags;
    VirtualProtect(addr, size, flags, &oldFlags);
}

/*static*/ bool MemoryUnlock::Memcpy(void* dest, const void* src, std::size_t count)
{
    MemoryUnlock lock(dest, count);
    if (lock.IsValid())
    {
        std::memcpy(dest, src, count);
        return true;
    }
    return false;
}
