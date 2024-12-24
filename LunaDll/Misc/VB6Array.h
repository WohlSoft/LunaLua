#ifndef _VB6Array_H_
#define _VB6Array_H_

// Wrapper to raise VB "Subscript Out of Range" exception
[[noreturn]] static inline __declspec(noinline) void VBSubscriptOutOfRangeException()
{
    (*reinterpret_cast<void(__stdcall **)()>(0x4010F4))();
    // Throwing a C++ exception in this function seemed necessary to make stack traces work as
    // expected. Something with the frame pointer perhaps, or avoiding it being seen as a tail
    // call. We never actually reach throwing the C++ exception though.
    throw std::exception();
}

#pragma pack(push, 1)

// Class representing an array as VB6  uses arrays
template <typename T, ptrdiff_t MIN_IDX, ptrdiff_t MAX_IDX>
class VB6Array {
private:
    T data[1+MAX_IDX-MIN_IDX];
public:
    T& operator[](ptrdiff_t idx)
    {
        idx -= MIN_IDX;
        if (idx > (MAX_IDX - MIN_IDX))
        {
            // VB6 Subscript out of range
            VBSubscriptOutOfRangeException();
        }
        return data[idx];
    }
    const T& operator[](ptrdiff_t idx) const
    {
        idx -= MIN_IDX;
        if (idx > (MAX_IDX - MIN_IDX))
        {
            // VB6 Subscript out of range
            VBSubscriptOutOfRangeException();
        }
        return data[idx];
    }
    operator void*() { return data; }
    operator void*() const { return data; }
};

// Type alias to simplify 2D array
template <typename T, ptrdiff_t MIN_IDX, ptrdiff_t MAX_IDX, ptrdiff_t MIN_IDX_2, ptrdiff_t MAX_IDX_2>
using VB6Array2D = VB6Array<VB6Array<T, MIN_IDX_2, MAX_IDX_2>, MIN_IDX, MAX_IDX>;

// Class for storing a reference to an array, as VB seems to use for module level variables
// declaring an array.
template <typename T, ptrdiff_t MIN_IDX, ptrdiff_t MAX_IDX>
class VB6ArrayRef
{
private:
    uint32_t _reserved_before_[4];
    VB6Array<T, MIN_IDX, MAX_IDX>& data;
    uint32_t _reserved_after_[2];
public:
    T& operator[](ptrdiff_t idx)
    {
        return data[idx];
    }
    const T& operator[](ptrdiff_t idx) const
    {
        return data[idx];
    }
    operator void*() { return data; }
    operator void*() const { return data; }
};

// Forward declare for the following
namespace SMBX13 {
    namespace Types {
        struct StdPicture_t;
    }
}

// Specialization for StdPicture since it seems to need more reserved space
template <ptrdiff_t MIN_IDX, ptrdiff_t MAX_IDX>
class VB6ArrayRef<SMBX13::Types::StdPicture_t, MIN_IDX, MAX_IDX>
{
private:
    uint32_t _reserved_before_[7];
    VB6Array<SMBX13::Types::StdPicture_t, MIN_IDX, MAX_IDX>& data;
    uint32_t _reserved_after_[2];
public:
    SMBX13::Types::StdPicture_t& operator[](ptrdiff_t idx)
    {
        return data[idx];
    }
    const SMBX13::Types::StdPicture_t& operator[](ptrdiff_t idx) const
    {
        return data[idx];
    }
};

#pragma pack(pop)

#endif
