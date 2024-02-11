#include <Windows.h>
#include <cstring>
#include <cinttypes>
#include "AsmPatch.h"

/*static*/ std::vector<AsmRange> AsmRange::mAlloc;
/*static*/ std::map<std::pair<const char*, uintptr_t>, std::intptr_t> AsmRange::mCache;
/*static*/ std::intptr_t AsmRange::mFirstIdx = -1;
/*static*/ bool AsmRange::mStartChecks = false;

/*static*/ bool MemoryUnlock::UnProtect(void* addr, std::size_t size, unsigned long* oldFlags)
{
    return (VirtualProtect(addr, size, PAGE_READWRITE, oldFlags) != 0);
}

/*static*/ void MemoryUnlock::ReProtect(void* addr, std::size_t size, unsigned long flags)
{
    unsigned long oldFlags;
    VirtualProtect(addr, size, flags, &oldFlags);
}

/*static*/ bool MemoryUnlock::Memcpy(void* dest, const void* src, std::size_t count, const char* lineNum)
{
    AsmRange::getInst(lineNum, reinterpret_cast<uintptr_t>(dest), count);
    MemoryUnlock lock(dest, count);
    if (lock.IsValid())
    {
        std::memcpy(dest, src, count);
        return true;
    }
    return false;
}
