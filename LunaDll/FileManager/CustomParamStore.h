#ifndef CUSTOMPARAMSTORE_H_
#define CUSTOMPARAMSTORE_H_

#include <string>

// Class definition
template<size_t SIZE>
class CustomParamStore
{
public:
    CustomParamStore() :
        stringData(), charPtrData()
    {}

    inline void clear()
    {
        for (int i = 0; i < SIZE; i++)
        {
            charPtrData[i] = nullptr;
            stringData[i].clear();
        }
    }

    inline void setData(size_t idx, const char* value)
    {
        if (value != nullptr)
        {
            stringData[idx] = value;
        }
        else
        {
            stringData[idx].clear();
        }

        if (stringData[idx].length() > 0)
        {
            charPtrData[idx] = stringData[idx].c_str();
        }
        else
        {
            charPtrData[idx] = nullptr;
        }
    }

    inline void setData(size_t idx, const std::string& value)
    {
        setData(idx, value.c_str());
    }

    inline const char** getCharPtrArray()
    {
        return charPtrData;
    }

private:
    std::string stringData[SIZE];
    const char* charPtrData[SIZE];
};

// Global Instances
extern CustomParamStore<1>     g_LevelCustomParams;
extern CustomParamStore<21>    g_SectionCustomParams;
extern CustomParamStore<5000>  g_NpcCustomParams;
extern CustomParamStore<8000>  g_BgoCustomParams;
extern CustomParamStore<20000> g_BlockCustomParams;

#endif
