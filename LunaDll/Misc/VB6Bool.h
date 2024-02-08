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
    VB6Bool(bool val) :
        raw(val ? -1 : 0)
    {}
    VB6Bool(const VB6Bool &other) :
        raw(other.raw)
    {}
    VB6Bool(const bool &other) :
        raw(other ? -1 : 0)
    {}
    void operator=(const VB6Bool &other)
    {
        raw = other.raw;
    }
    void operator=(const bool &other)
    {
        raw = (other ? -1 : 0);
    }
    operator bool() const {
        return raw != 0;
    }
    int16_t& asInt16() {
        return raw;
    }
};
#pragma pack(pop)

#endif
