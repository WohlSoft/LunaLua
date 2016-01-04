#ifndef BaseItemArray_hhhhh
#define BaseItemArray_hhhhh

template<
    class T, // The internal class
        const short MAX_ID_VAL, // The ptr to the max id
        const unsigned short* COUNT_VAL_PTR, // The ptr to the count value (how many objects of type T are in that array)
        void** const ARRAY_VAL_PTR, // The ptr of the actual array
        const int OFFSET_VAL = 0 // (Optional) Offset added to the index
>
struct SMBX_Base_Item_Array
{
    static inline T* Get(unsigned short index) {
        if (index > Count()) return NULL;
        return &((T*)*ARRAY_VAL_PTR)[index + OFFSET_VAL];
    }

    static inline T* GetRaw(unsigned short index) {
        return &((T*)*ARRAY_VAL_PTR)[index + OFFSET_VAL];
    }

    static inline unsigned short Count() {
        return *COUNT_VAL_PTR;
    }

    static const short MAX_ID = MAX_ID_VAL;
};

#endif
