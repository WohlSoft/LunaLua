#ifndef _VB6Bool_H_
#define _VB6Bool_H_

#pragma pack(push, 2)
class VB6Bool {
private:
    int16_t raw;
public:
    VB6Bool() :
        raw(0)
    {}

    // Constructors
    VB6Bool(bool val) :
        raw(val ? -1 : 0)
    {}
    VB6Bool(const VB6Bool &other) :
        raw(other.raw)
    {}

    // Assignment operators
    void operator=(const VB6Bool &other)
    {
        raw = other.raw;
    }
    void operator=(const bool other)
    {
        raw = (other ? -1 : 0);
    }

    // Cast to c++ bool
    operator bool() const {
        return raw != 0;
    }

    // Manual raw access
    int16_t& asInt16() {
        return raw;
    }
    const int16_t& asInt16() const {
        return raw;
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
        int16_t cmpVal = other ? -1 : 0;
        return raw == cmpVal;
    }
    bool operator !=(const VB6Bool &other) const
    {
        return raw != other.raw;
    }
    bool operator !=(const bool other) const
    {
        // This is _not_ the most consistent way to compare versus bool, but this ensures "!= true"
        // behaves like it does in the VB6 code if the raw value is neither -1 nor 0
        int16_t cmpVal = other ? -1 : 0;
        return raw != cmpVal;
    }
};
#pragma pack(pop)

#endif
