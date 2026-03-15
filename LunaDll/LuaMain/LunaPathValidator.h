#ifndef LUNA_PATH_VALIDATOR_H_
#define LUNA_PATH_VALIDATOR_H_

#include <string>
#include <utility>
#include "../GlobalFuncs.h"
#include "../Misc/RAIIHandle.h"

class LunaPathValidator
{
public:
    enum class ErrorType {
        NULL_PATH,
        MODE_PARSING_ERROR,
        FILE_OPENING_ERROR,
        GET_FILE_FINAL_PATH_ERROR,
        UNAUTHORIZED_FILE_ACCESS,
        DIR_OPENING_ERROR,
        GET_DIR_FINAL_PATH_ERROR,
        UNAUTHORIZED_FILE_CREATION,
        FILE_CREATION_ERROR,
        DESCRIPTOR_CREATION_ERROR,
        FILE_OBJECT_CREATION_ERROR,
        UNAUTHORIZED_TEMP_FILE_CREATION,
        TEMP_FILE_CREATION_ERROR,
        TEMP_FILE_WRITE_ERROR,
        TEMP_FILE_INCOMPLETE_WRITE,
        FILE_REPLACE_ERROR,
        UNAUTHORIZED_FOLDER_LIST,
        FOLDER_LIST_ERROR,
        TRUNCATE_ERROR
    };

    struct Error {
        ErrorType type;
        DWORD errorCode;
        std::string pathOrMode;
    };
private:
    std::wstring mFinalEnginePath;
    std::wstring mFinalEpisodePath;

    Error mLastError;

    std::wstring mLastSuccessfulPath;

    DWORD CheckPath(std::wstring const& path, bool requestWrite); // ERROR_SUCCESS ERROR_ACCESS_DENIED ERROR_WRITE_PROTECT
    std::wstring NormalizePath(std::wstring const& path);
    HANDLE CreateTempFile(HANDLE parentFolder);
    void UpdateLastPath(std::wstring const& path);
public:
    LunaPathValidator();
    ~LunaPathValidator();

    // Initialize engine and episode paths
    void SetPaths();

    // Safely opens a file
    std::FILE* OpenFile(const char* path, const char* mode);
    std::FILE* OpenFile(const wchar_t* path, const char* mode);
    std::FILE* OpenFile(std::string const& path, const char* mode);
    std::FILE* OpenFile(std::wstring const& path, const char* mode);

    // Function to safely write data to a file, making repalcement as close to atomic as Windows seems to allow
    bool WriteFileAtomic(const char* path, const void* data, ptrdiff_t dataLen);
    bool WriteFileAtomic(const wchar_t* path, const void* data, ptrdiff_t dataLen);
    bool WriteFileAtomic(std::string const& path, const void* data, ptrdiff_t dataLen);
    bool WriteFileAtomic(std::wstring const& path, const void* data, ptrdiff_t dataLen);

    // List the contents of a directory in a safe way
    bool ListOfDir(const char* path, DWORD attributes, std::vector<std::string>& outputList);
    bool ListOfDir(const wchar_t* path, DWORD attributes, std::vector<std::string>& outputList);
    bool ListOfDir(std::string const& path, DWORD attributes, std::vector<std::string>& outputList);
    bool ListOfDir(std::wstring const& path, DWORD attributes, std::vector<std::string>& outputList);

    // If the previous call to OpenFile, WriteFileAtomic or ListOfDir succeeded, return the resolved path of the file, otherwise, return an unspecified value
    std::wstring const& LastPath();

    // If the previous call to OpenFile, WriteFileAtomic or ListOfDir failed, return the last error, otherwise, return an unspecified value
    Error const& LastError();

    // If the previous call to OpenFile, WriteFileAtomic or ListOfDir failed, return the last error message, otherwise, return an unspecified value
    std::string ErrorMessage();
public:
    static LunaPathValidator& GetForThread();
};

extern LunaPathValidator gLunaPathValidator;
extern LunaPathValidator gLunaPathValidatorLoadscreen;

#endif // LUNA_PATH_VALIDATOR_H_
