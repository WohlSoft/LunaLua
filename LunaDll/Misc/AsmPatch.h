#ifndef AsmPatch_hhhh
#define AsmPatch_hhhh

#include <cstdint>
#include <exception>
#include <type_traits>
#include <tuple>
#include <map>
#include <vector>

template<void* TARGETADDR>
_declspec(naked) static void __stdcall RETADDR_TRACE_HOOK_IMPL(void)
{
    static const void* thisPtr = TARGETADDR;
    __asm {
        PUSH DWORD PTR DS : [esp]
        JMP thisPtr
    }
}

template<void* TARGETADDR>
static inline constexpr void* GET_RETADDR_TRACE_HOOK(void) {
    return static_cast<void(__stdcall *)(void)>(&RETADDR_TRACE_HOOK_IMPL<TARGETADDR>);
}

class Patchable {
public:
    virtual void Apply() = 0;
    virtual void Unapply() = 0;
    virtual bool IsPatched() const = 0;
};

namespace AsmConsts {
    enum R32  {
        R32_EAX = 0x0,
        R32_ECX = 0x1,
        R32_EDX = 0x2,
        R32_EBX = 0x3,
        R32_ESP = 0x4,
        R32_EBP = 0x5,
        R32_ESI = 0x6,
        R32_EDI = 0x7
    };
};

struct AsmPatchNoCondtionalJump : std::exception {
    const char* what() const { return "No conditional jump at detected"; }
};

// Class to temporarily unlock memory
class MemoryUnlock {
    private:
        void* const mAddr;
        const std::size_t mSize;
        unsigned long mOldProtect;
        const bool mSuccess;

        static bool UnProtect(void* addr, std::size_t size, unsigned long* oldFlags);
        static void ReProtect(void* addr, std::size_t size, unsigned long flags);

    public:
        MemoryUnlock(std::uintptr_t addr, std::size_t size) :
            MemoryUnlock(reinterpret_cast<void*>(addr), size)
        {}

        MemoryUnlock::MemoryUnlock(void* addr, std::size_t size) :
            mAddr(addr),
            mSize(size),
            mOldProtect(0),
            mSuccess(UnProtect(mAddr, mSize, &mOldProtect))
        {}

        MemoryUnlock::~MemoryUnlock()
        {
            if (mSuccess)
            {
                ReProtect(mAddr, mSize, mOldProtect);
            }
        }

        bool IsValid() const { return mSuccess; }

        MemoryUnlock(const MemoryUnlock&) = delete;
        MemoryUnlock& operator= (const MemoryUnlock&) = delete;

        static bool Memcpy(void* dest, const void* src, std::size_t count, const char* lineNum);
};

// Alias for MemoryUnlock::Memcpy with tracking
#define MemoryUnlock_Memcpy(dest, src, size) MemoryUnlock::Memcpy(dest, src, size, __FILE__ ":" PATCH__STRINGIZE(__LINE__))

// Structure for detecting conflicts
struct AsmRange;
struct AsmRange
{
    static std::vector<AsmRange> mAlloc;
    static std::map<std::pair<const char*, uintptr_t>, std::intptr_t> mCache;
    static std::intptr_t mFirstIdx;
    static bool mStartChecks;
    std::intptr_t mIdx;
    const char* mLineNum;
    std::uintptr_t mAddr;
    std::uintptr_t mSize;
    std::intptr_t mNextIdx;
    bool mCollided;

    static std::intptr_t getInst(const char* lineNum, std::uintptr_t addr, std::uintptr_t size = 0)
    {
        auto key = std::pair<const char*, uintptr_t>(lineNum, addr);
        auto cursor = mCache.find(key);
        if (cursor == mCache.end())
        {
            // New copy needed
            std::intptr_t idx = mAlloc.size();
            mAlloc.emplace_back(idx, lineNum, addr, size);
            mCache[key] = idx;
            return idx;
        }
        else
        {
            // Reuse existing copy
            std::intptr_t idx = cursor->second;
            mAlloc[idx].SetSize(size);
            return idx;
        }
    }

    void SetSize(std::uintptr_t size)
    {
        if (size > mSize)
        {
            mSize = size;
            CheckCollision();
        }
    }

    static void UpdateSize(std::intptr_t idx, std::uintptr_t size)
    {
        if (idx >= 0)
        {
            mAlloc[idx].SetSize(size);
        }
    }

    //AsmRange(const AsmRange& other) :
    //    mIdx(other.mIdx), mLineNum(other.mLineNum), mAddr(other.mAddr), mSize(other.mSize), mNextIdx(other.mNextIdx), mCollided(other.mCollided)
    //{
    //}

    AsmRange(std::intptr_t idx, const char* lineNum, std::uintptr_t addr, std::uintptr_t size) :
        mIdx(idx), mLineNum(lineNum), mAddr(addr), mSize(0), mNextIdx(-1), mCollided(false)
    {
        if (mFirstIdx < 0)
        {
            // First entry
            mFirstIdx = mIdx;
            SetSize(size);
        }
        else
        {
            // Subsequent entry
            std::intptr_t mCursor = mFirstIdx;
            std::intptr_t mPrev = -1;
            while ((mCursor >= 0) && (mAlloc[mCursor].mAddr < mAddr))
            {
                mPrev = mCursor;
                mCursor = mAlloc[mCursor].mNextIdx;
            }

            if (mPrev < 0)
            {
                // Replacing first element
                mFirstIdx = mIdx;
            }
            else
            {
                // Replacing later element
                mAlloc[mPrev].mNextIdx = mIdx;
            }
            // Set next chain entry
            mNextIdx = mCursor;
            SetSize(size);

            if (mPrev >= 0)
            {
                mAlloc[mPrev].CheckCollision();
            }
        }
    }

    // Don't actually start checking ranges until this.
    // This is needed since we might not have allocated a console we we started keeping track of
    // patched ranges.
    static void StartChecking()
    {
        mStartChecks = true;
        
        // Check any data we've already recorded
        for (std::intptr_t cursor = mFirstIdx; cursor <= 0; cursor = mAlloc[cursor].mNextIdx)
        {
            mAlloc[cursor].CheckCollision();
        }
    }

private:
    void CheckCollision()
    {
        if (!mStartChecks) return;
        if (mCollided) return;

        if ((mNextIdx >= 0) && ((mAddr + mSize) > mAlloc[mNextIdx].mAddr))
        {
            mCollided = true;
            printf("WARNING: Conflict between 0x%X and 0x%X\n\t%s\n\t%s\n", mAddr, mAlloc[mNextIdx].mAddr, mLineNum, mAlloc[mNextIdx].mLineNum);
        }
    }
};

template <std::uintptr_t Size>
struct AsmPatch : public Patchable {
    /********************
     * Member variables *
     ********************/
public:
    const std::uintptr_t mAddr;
    std::uint8_t mPatchBytes[Size ? Size : 1];
    std::uint8_t mOrigBytes[Size ? Size : 1];
    bool mIsPatched;
    intptr_t const mRangeTrackIdx;

    /***********************************
     * Constructor and utility methods *
     ***********************************/
public:
    AsmPatch(std::uintptr_t addr, const char* lineNum) :
        mAddr(addr),
        mIsPatched(false),
        mRangeTrackIdx(AsmRange::getInst(lineNum, addr, Size))
    {}

    template <std::uintptr_t OldSize>
    AsmPatch(std::uintptr_t addr, const AsmPatch<OldSize>& old) :
        mAddr(addr),
        mIsPatched(false),
        mRangeTrackIdx(old.mRangeTrackIdx)
    {
        if (mRangeTrackIdx >= 0)
        {
            AsmRange::UpdateSize(mRangeTrackIdx, Size);
        }
    }

    std::uintptr_t size() const {
        return Size;
    }

    std::uintptr_t cursor() const {
        return mAddr + Size;
    }

    void Apply() {
        if (mIsPatched) return;
        if (Size == 0) return;
        {
            MemoryUnlock lock(mAddr, Size);
            if (!lock.IsValid()) return;
            for (std::uintptr_t i = 0; i < Size; i++) {
                ((uint8_t*)mAddr)[i] = mPatchBytes[i];
            }
        }
        mIsPatched = true;
    }

    void Unapply() {
        if (!mIsPatched) return;
        if (Size == 0) return;
        {
            MemoryUnlock lock(mAddr, Size);
            if (!lock.IsValid()) return;
            for (std::uintptr_t i = 0; i < Size; i++) {
                ((uint8_t*)mAddr)[i] = mOrigBytes[i];
            }
        }
        mIsPatched = false;
    }

    bool IsPatched() const {
        return mIsPatched;
    }

    /*******************************
     * Appending data to the patch *
     *******************************/
public:
    inline AsmPatch<Size> bytes() const {
        return *this;
    }

    template <typename... Ts>
    inline AsmPatch<Size + 1 + sizeof...(Ts)> bytes(std::uint8_t newByte, Ts... params) const {
        AsmPatch<Size + 1> ret(mAddr, *this);
        for (std::uintptr_t i = 0; i < Size; i++) {
            ret.mPatchBytes[i] = mPatchBytes[i];
        }
        for (std::uintptr_t i = 0; i < Size; i++) {
            ret.mOrigBytes[i] = mOrigBytes[i];
        }
        ret.mPatchBytes[Size] = newByte;
        ret.mOrigBytes[Size] = ((std::uint8_t*)mAddr)[Size];
        return ret.bytes(params...);
    }

    inline AsmPatch<Size + 1> byte(std::uint8_t newByte) const {
        return bytes(newByte);
    }

    inline AsmPatch<Size + 2> word(std::uint16_t newWord) const {
        const std::uint8_t* data = (const std::uint8_t*)&newWord;
        return bytes(data[0], data[1]);
    }

    inline AsmPatch<Size + 4> dword(std::uint32_t newDWord) const {
        const std::uint8_t* data = (const std::uint8_t*)&newDWord;
        return bytes(data[0], data[1], data[2], data[3]);
    }

    /*****************************
     * Insertion of instructions *
     *****************************/
public:
    inline AsmPatch<Size + 1> NOP() const {
        return byte(0x90);
    }
    inline AsmPatch<Size + 1> RET() const {
        return byte(0xC3);
    }
    inline AsmPatch<Size + 3> RET_NEAR() const {
        return bytes(0xC2, 0x04, 0x00);
    }
    inline AsmPatch<Size + 1> PUSH_R32(AsmConsts::R32 arg) const {
        return byte(0x50 | arg);
    }
    inline AsmPatch<Size + 1> POP_R32(AsmConsts::R32 arg) const {
        return byte(0x58 | arg);
    }
    inline AsmPatch<Size + 5> PUSH_IMM32(std::uint32_t newDWord) const {
        return byte(0x68).dword(newDWord);
    }
    inline AsmPatch<Size + 1> PUSHFD() const {
        return byte(0x9C);
    }
    inline AsmPatch<Size + 1> POPFD() const {
        return byte(0x9D);
    }

    // Convenience shorthand
    inline AsmPatch<Size + 1> PUSH_EAX() const { return PUSH_R32(AsmConsts::R32_EAX); }
    inline AsmPatch<Size + 1> PUSH_ECX() const { return PUSH_R32(AsmConsts::R32_ECX); }
    inline AsmPatch<Size + 1> PUSH_EDX() const { return PUSH_R32(AsmConsts::R32_EDX); }
    inline AsmPatch<Size + 1> PUSH_EBX() const { return PUSH_R32(AsmConsts::R32_EBX); }
    inline AsmPatch<Size + 1> PUSH_ESP() const { return PUSH_R32(AsmConsts::R32_ESP); }
    inline AsmPatch<Size + 1> PUSH_EBP() const { return PUSH_R32(AsmConsts::R32_EBP); }
    inline AsmPatch<Size + 1> PUSH_ESI() const { return PUSH_R32(AsmConsts::R32_ESI); }
    inline AsmPatch<Size + 1> PUSH_EDI() const { return PUSH_R32(AsmConsts::R32_EDI); }
    inline AsmPatch<Size + 1> POP_EAX() const { return POP_R32(AsmConsts::R32_EAX); }
    inline AsmPatch<Size + 1> POP_ECX() const { return POP_R32(AsmConsts::R32_ECX); }
    inline AsmPatch<Size + 1> POP_EDX() const { return POP_R32(AsmConsts::R32_EDX); }
    inline AsmPatch<Size + 1> POP_EBX() const { return POP_R32(AsmConsts::R32_EBX); }
    inline AsmPatch<Size + 1> POP_ESP() const { return POP_R32(AsmConsts::R32_ESP); }
    inline AsmPatch<Size + 1> POP_EBP() const { return POP_R32(AsmConsts::R32_EBP); }
    inline AsmPatch<Size + 1> POP_ESI() const { return POP_R32(AsmConsts::R32_ESI); }
    inline AsmPatch<Size + 1> POP_EDI() const { return POP_R32(AsmConsts::R32_EDI); }

    inline AsmPatch<Size + 2> MOV_RESTORE_STACKPTR() {
        return bytes(0x8B, 0xE5);
    }

    inline AsmPatch<Size + 9> RET_STDCALL_FULL() {
        return (
            POP_EDI().
            POP_ESI().
            POP_EBX().
            MOV_RESTORE_STACKPTR().
            POP_EBP().
            RET_NEAR()
            );
    }

    inline AsmPatch<Size + 5> CALL(void* func) const { return CALL((std::uintptr_t)func); }
    inline AsmPatch<Size + 5> CALL(std::uintptr_t func) const {
        return byte(0xE8).dword(func - cursor() - 5);
    }
    template <void* func>
    inline AsmPatch<Size + 5> TRACE_CALL(void) const {
        return CALL(static_cast<void(__stdcall *)(void)>(&RETADDR_TRACE_HOOK_IMPL<func>));
    }

    inline AsmPatch<Size + 5> JMP(void* addr) const { return JMP((std::uintptr_t)addr); }
    inline AsmPatch<Size + 5> JMP(std::uintptr_t addr) const {
        return byte(0xE9).dword(addr - cursor() - 5);
    }

    inline AsmPatch<Size + 6> JE(void* addr) const { return JE((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JE(std::uintptr_t addr) const {
        return bytes(0x0F, 0x84).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JZ(T addr) const { return JE(addr); }

    inline AsmPatch<Size + 6> JNE(void* addr) const { return JNE((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JNE(std::uintptr_t addr) const {
        return bytes(0x0F, 0x85).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNZ(T addr) const { return JNE(addr); }

    inline AsmPatch<Size + 6> JB(void* addr) const { return JB((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JB(std::uintptr_t addr) const {
        return bytes(0x0F, 0x82).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNAE(T addr) const { return JB(addr); }

    template<typename T>
    inline AsmPatch<Size + 6> JC(T addr) const { return JB(addr); }

    inline AsmPatch<Size + 6> JNB(void* addr) const { return JNB((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JNB(std::uintptr_t addr) const {
        return bytes(0x0F, 0x83).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JAE(T addr) const { return JNB(addr); }

    template<typename T>
    inline AsmPatch<Size + 6> JNC(T addr) const { return JNB(addr); }

    inline AsmPatch<Size + 6> JBE(void* addr) const { return JBE((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JBE(std::uintptr_t addr) const {
        return bytes(0x0F, 0x86).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNA(T addr) const { return JBE(addr); }

    inline AsmPatch<Size + 6> JA(void* addr) const { return JA((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JA(std::uintptr_t addr) const {
        return bytes(0x0F, 0x87).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNBE(T addr) const { return JA(addr); }

    inline AsmPatch<Size + 6> JL(void* addr) const { return JL((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JL(std::uintptr_t addr) const {
        return bytes(0x0F, 0x8C).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNGE(T addr) const { return JL(addr); }

    inline AsmPatch<Size + 6> JGE(void* addr) const { return JGE((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JGE(std::uintptr_t addr) const {
        return bytes(0x0F, 0x8D).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNL(T addr) const { return JGE(addr); }

    inline AsmPatch<Size + 6> JLE(void* addr) const { return JLE((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JLE(std::uintptr_t addr) const {
        return bytes(0x0F, 0x8E).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNG(T addr) const { return JLE(addr); }

    inline AsmPatch<Size + 6> JG(void* addr) const { return JG((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JG(std::uintptr_t addr) const {
        return bytes(0x0F, 0x8F).dword(addr - cursor() - 6);
    }

    template<typename T>
    inline AsmPatch<Size + 6> JNLE(T addr) const { return JG(addr); }

    inline AsmPatch<Size + 6> JO(void* addr) const { return JO((std::uintptr_t)addr); }
    inline AsmPatch<Size + 6> JO(std::uintptr_t addr) const {
        return bytes(0x0F, 0x80).dword(addr - cursor() - 6);
    }

    inline AsmPatch<Size + 13> SAFE_CALL(void* func) const { return SAFE_CALL((std::uintptr_t)func); }
    inline AsmPatch<Size + 13> SAFE_CALL(std::uintptr_t func) const {
        return (
            PUSHFD().
            PUSH_EAX().
            PUSH_ECX().
            PUSH_EDX().
            CALL(func).
            POP_EDX().
            POP_ECX().
            POP_EAX().
            POPFD()
        );
    }

    template <std::uintptr_t PadSize>
    inline AsmPatch<PadSize> NOP_PAD_TO_SIZE() const {
        static_assert(PadSize >= Size, "Cannot pad smaller than old size");

        AsmPatch<PadSize> ret(mAddr, *this);
        for (std::uintptr_t i = 0; i < Size; i++) {
            ret.mPatchBytes[i] = mPatchBytes[i];
        }
        for (std::uintptr_t i = 0; i < Size; i++) {
            ret.mOrigBytes[i] = mOrigBytes[i];
        }

        for (std::uintptr_t i = Size; i < PadSize; i++) {
            ret.mPatchBytes[i] = 0x90;
        }
        for (std::uintptr_t i = Size; i < PadSize; i++) {
            ret.mOrigBytes[i] = ((std::uint8_t*)mAddr)[i];
        }
        return ret;
    }

    template <std::uintptr_t NopCount>
    inline AsmPatch<Size + NopCount> NOPS() const {
        return NOP_PAD_TO_SIZE<Size + NopCount>();
    }

    inline AsmPatch<Size + 2> CONDJMP_TO_NOPJMP() const {
        const uint8_t* ptr = (const uint8_t*)cursor();
        if ((ptr[0] != 0x0F) || ((ptr[1] & 0xF0) != 0x80)) {
            throw AsmPatchNoCondtionalJump();
        }
        return NOP().byte(0xE9);
    }
};

static inline AsmPatch<0> PATCH_impl(std::uintptr_t addr, const char* lineNum) {
    return AsmPatch<0>(addr, lineNum);
}
static inline AsmPatch<0> PATCH_impl(void* addr, const char* lineNum) {
    return PATCH_impl((std::uintptr_t)addr, lineNum);
}

#define PATCH__STRINGIZE2(x) #x
#define PATCH__STRINGIZE(x) PATCH__STRINGIZE2(x)
#define PATCH(x) PATCH_impl(x, __FILE__ ":" PATCH__STRINGIZE(__LINE__))

/********************/
/* Patch colleciton */
/********************/

template <typename... Ts>
class PatchCollectionImpl : public Patchable {
private:
    std::tuple<Ts...> items;

private:
    template<std::size_t I>
    inline typename std::enable_if < I == sizeof...(Ts), void>::type ApplyImpl() {}
    template<std::size_t I>
    inline typename std::enable_if < I != sizeof...(Ts), void>::type ApplyImpl() {
        std::get<I>(items).Apply();
        ApplyImpl<I + 1>();
    }

    template<std::size_t I>
    inline typename std::enable_if < I == sizeof...(Ts), void>::type UnapplyImpl() {}
    template<std::size_t I>
    inline typename std::enable_if < I != sizeof...(Ts), void>::type UnapplyImpl() {
        std::get<I>(items).Unapply();
        UnapplyImpl<I + 1>();
    }

public:
    PatchCollectionImpl(const Ts&... args) :
        items(args...)
    {}

    void Apply() {
        ApplyImpl<0>();
    }
    void Unapply() {
        UnapplyImpl<0>();
    }
    bool IsPatched() const {
        return std::get<0>(items).IsPatched();
    }
};

template <typename... Ts>
static inline PatchCollectionImpl<Ts...> PatchCollection(const Ts&... args) {
    return PatchCollectionImpl<Ts...>(args...);
}

#endif
