#ifndef _VB6Bool_H_
#define _VB6Bool_H_

#pragma pack(push, 2)

// Version for use in by-value calling conventions... passes as a raw value correctly
enum class VB6BoolVal : int16_t
{
    VB_FALSE = 0,
    VB_TRUE = -1
};

inline bool FromVB6BoolVal(VB6BoolVal a)
{
    return (static_cast<int16_t>(a) != 0);
}
inline bool FromVB6BoolVal(bool a)
{
    return a;
}
inline VB6BoolVal ToVB6BoolVal(bool a)
{
    return a ? VB6BoolVal::VB_TRUE : VB6BoolVal::VB_FALSE;
}
inline VB6BoolVal ToVB6BoolVal(VB6BoolVal a)
{
    return a;
}

// Version for use in structures... has more casting capability and such
// WARNING: Do not pass this by value as an argument or return value... it will do the wrong thing.
class VB6Bool {
private:
    VB6BoolVal raw;
public:
    VB6Bool() :
        raw(VB6BoolVal::VB_FALSE)
    {}

    // Constructors
    VB6Bool(bool val) :
        raw(val ? VB6BoolVal::VB_TRUE : VB6BoolVal::VB_FALSE)
    {}
    VB6Bool(const VB6Bool &other) :
        raw(other.raw)
    {}
    VB6Bool(const VB6BoolVal &other) :
        raw(other)
    {}

    // Assignment operators
    void operator=(const VB6Bool &other)
    {
        raw = other.raw;
    }
    void operator=(const bool& other)
    {
        raw = (other ? VB6BoolVal::VB_TRUE : VB6BoolVal::VB_FALSE);
    }
    void operator=(const VB6BoolVal other)
    {
        raw = other;
    }

    // Casts
    operator bool() const {
        return static_cast<int16_t>(raw) != 0;
    }
    operator VB6BoolVal() const {
        return raw;
    }

    // Manual raw access
    int16_t& asInt16() {
        return reinterpret_cast<int16_t&>(raw);
    }
    const int16_t& asInt16() const {
        return reinterpret_cast<const int16_t&>(raw);
    }

    // Comparison operators
    bool operator ==(const VB6Bool &other) const
    {
        return raw == other.raw;
    }
    bool operator ==(const bool other) const
    {
        // This is _not_ the most consistent way to compare versus bool, but this ensures "== true"
        // behaves like it does in the VB6 code if the raw value is neither -1 nor 0
        return raw == (other ? VB6BoolVal::VB_TRUE : VB6BoolVal::VB_FALSE);
    }
    bool operator !=(const VB6Bool &other) const
    {
        return raw != other.raw;
    }
    bool operator !=(const bool other) const
    {
        // This is _not_ the most consistent way to compare versus bool, but this ensures "!= true"
        // behaves like it does in the VB6 code if the raw value is neither -1 nor 0
        return raw != (other ? VB6BoolVal::VB_TRUE : VB6BoolVal::VB_FALSE);
    }
};

#pragma pack(pop)

#endif
