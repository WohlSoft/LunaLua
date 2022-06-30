#ifndef BaseItemArray_hhhhh
#define BaseItemArray_hhhhh

#include <cstdint>

template<
    class T, // The internal class
    int MAX_SIZE_VAL, // The maximum size of the array
    uintptr_t ARRAY_VAL_PTRINT, // The ptr of the actual array
    const int OFFSET_VAL = 0 // (Optional) Offset added to the index
>
struct SMBX_StaticBaseItemArray
{
    static inline T* Get(unsigned short index) {
        if (index >= MAX_SIZE_VAL) return NULL;
        return &((T*)*reinterpret_cast<void** const>(ARRAY_VAL_PTRINT))[index + OFFSET_VAL];
    }
};

template<
    class T, // The internal class
    const short MAX_ID_VAL, // The ptr to the max id
    uintptr_t COUNT_VAL_PTRINT, // The ptr to the count value (how many objects of type T are in that array)
    uintptr_t ARRAY_VAL_PTRINT, // The ptr of the actual array
    const int OFFSET_VAL = 0 // (Optional) Offset added to the index
>
struct SMBX_FullBaseItemArray
{
    static inline T* Get(unsigned short index) {
        if (index > Count()) return NULL;
        return &((T*)*reinterpret_cast<void**>(ARRAY_VAL_PTRINT))[index + OFFSET_VAL];
    }

    static inline T* GetRaw(unsigned short index) {
        return &((T*)*reinterpret_cast<void**>(ARRAY_VAL_PTRINT))[index + OFFSET_VAL];
    }

    static inline unsigned short Count() {
        return *reinterpret_cast<const unsigned short*>(COUNT_VAL_PTRINT);
    }

    static const short MAX_ID = MAX_ID_VAL;
};

#endif
