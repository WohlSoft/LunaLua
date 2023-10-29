#ifndef LUNA_PATH_VALIDATOR_H_
#define LUNA_PATH_VALIDATOR_H_

#include <string>
#include <utility>
#include "../GlobalFuncs.h"

class LunaPathValidator
{
public:
    struct Result
    {
        const char* path;
        unsigned int len;
        bool canWrite;
        Result() :
            path(nullptr), len(0), canWrite(false)
        {}
        Result(const char* _path, unsigned int _len, bool _canWrite) :
            path(_path), len(_len), canWrite(_canWrite)
        {}
    };
private:
    std::string mEnginePath;
    std::wstring mMatchingEnginePath;
    std::wstring mMatchingEpisodePath;
    std::wstring mMatchingSavesPath;

    std::string mNormalPath;
    Result mResult;
public:
    LunaPathValidator();
    ~LunaPathValidator();
    void SetPaths();
    Result* CheckPath(const char* path);
public:
    static LunaPathValidator& GetForThread();
};

extern LunaPathValidator gLunaPathValidator;
extern LunaPathValidator gLunaPathValidatorLoadscreen;

#endif // LUNA_PATH_VALIDATOR_H_
