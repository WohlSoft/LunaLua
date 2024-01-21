#ifndef GlobalFuncs_hhhhhhh
#define GlobalFuncs_hhhhhhh

#include <windows.h>
#include <string>
#include <vector>
#include <wchar.h>
#include <string.h>
#include <mutex>

//String manupulation things
void splitStr(std::vector<std::string>& dest, const std::string& str, const char* separator);
void replaceSubStr(std::string& str, const std::string& from, const std::string& to);
void replaceSubStrW(std::wstring& str, const std::wstring& from, const std::wstring& to);
void RemoveSubStr(std::string& sInput, const std::string& sub);
std::vector<std::string> splitCmdArgs( std::string str);
std::vector<std::wstring> splitCmdArgsW(std::wstring str);

//Compatibility macroses left just in case
#define utf8_encode(wstr) WStr2Str(wstr)
#define utf8_decode(wstr) Str2WStr(wstr)
/*!
 * \brief Converts std::wstring into UTF8-encoded std::string
 * \param wstr input std::wstring
 * \return output std::string
 */
std::string WStr2Str(const std::wstring &wstr);

/*!
 * \brief Converts UTF8-encoded std::string into std::wstring
 * \param str input std::string
 * \return output std::wstring
 */
std::wstring Str2WStr(const std::string &str);

/*!
 * \brief Converts std::wstring into Local ANSI-encoded std::string
 * \param wstr input std::wstring
 * \return output std::string
 */
std::string WStr2StrA(const std::wstring &wstr);

/*!
 * \brief Converts Local ANSI-encoded std::string into std::wstring
 * \param str input std::string
 * \return output std::wstring
 */
std::wstring StrA2WStr(const std::string &str);

std::wstring GetNonANSICharsFromWStr(const std::wstring& wstr);

std::string ConvertWCSToMBS(const wchar_t * pstr, long wslen);
std::string ConvertBSTRToMBS(BSTR bstr);
BSTR ConvertMBSToBSTR(const std::string & str);

std::string i2str(int source);
bool is_number(const std::string& s);
bool file_existsX(const std::string& name);
bool isAbsolutePath(const std::wstring& path);
bool isAbsolutePath(const std::string& path);
std::wstring resolveCwdOrWorldsPath(const std::wstring& path);
std::wstring resolveIfNotAbsolutePath(std::wstring filename);
std::string resolveIfNotAbsolutePath(std::string filename);

extern void removeFilePathW(std::wstring &path);
extern void removeFilePathW(wchar_t*path, int length);
void removeFilePathA(std::string &path);
void removeFilePathA(char*path, int length);

/// Functions ///
void initAppPaths();
void ResetLunaModule();
void InitGlobals();
void ResetFreeGlob();
void CleanUp();

// Windows version checks
bool IsWindowsVersionOrNewer(DWORD major, DWORD minor);

inline bool Luna_IsWindowsVistaOrNewer()
{
    static bool ret = IsWindowsVersionOrNewer(6, 0);
    return ret;
}
inline bool Luna_IsWindowsWin8OrNewer()
{
    static bool ret = IsWindowsVersionOrNewer(6, 2);
    return ret;
}

// Helper function to load functions from DLLs
// NOTE: Calling this increments the module reference counter each time and isn't so fast
//       so the result should be cached.
//       Static variables are potentially a good enough way to cache this result.
// NOTE: The template argument allows you to avoid reinterpret_cast boilerplate and storing
//       this in an 'auto' variable.
FARPROC Luna_GetProc_impl(const char* libFilename, const char* procName);
template <typename T>
T Luna_GetProc(const char* libFilename, const char* procName)
{
    return reinterpret_cast<T>(Luna_GetProc_impl(libFilename, procName));
}


/// HELPER FUNCTIONS ///
std::vector<std::wstring> wsplit( std::wstring str, wchar_t delimiter);
std::vector<std::string> split( std::string str, char delimiter);
bool vecStrFind(const std::vector<std::wstring>& vecStr, const std::wstring& find);
std::string url_encode(const std::string &value);
// Module Helper Funcs
//HMODULE getModule(std::string moduleName);

// File/Path Helper Funcs
std::wstring normalizePathSlashes(const std::wstring& input);
bool readFile(std::wstring &content, std::wstring path, std::wstring errMsg = std::wstring());
bool readFile(std::string &content, std::string path, std::string errMsg = std::string());
bool writeFile(const std::string &content, const std::string &path);
std::vector<std::string> listFilesOfDir(const std::string& path);
std::vector<std::string> listOfDir(const std::string& path, DWORD fileAttributes);
//std::wstring getModulePath();
std::wstring getEpisodeFolderPath();
std::wstring getCustomFolderPath();
std::wstring getLatestFile(const std::initializer_list<std::wstring>& paths);
std::wstring getLatestConfigFile(const std::wstring& configname);

template <typename T>
T normalizeToBackslashAndResolvePath(const T& path)
{
    size_t len = path.size();
    T ret;
    ret.resize(len);
    size_t outIdx = 0;
    size_t idx = 0;
    while (true)
    {
        // Detect if we're now ending a .. segment
        if ((outIdx >= 3) && ((idx >= len) || ((idx < len) && ((path[idx] == '\\') || (path[idx] == '/')))) &&
            (ret[outIdx - 3] == '\\') &&
            (ret[outIdx - 2] == '.') &&
            (ret[outIdx - 1] == '.')
            )
        {
            // Remove the prior segment from output
            outIdx -= 3;
            for (; outIdx > 0; outIdx--)
            {
                if (ret[outIdx - 1] == '\\') break;
            }
            idx++;
        }

        // End of string
        if (idx >= len) break;

        T::value_type c = path[idx];
        if (c == '/')
        {
            c = '\\';
        }
        if ((c == '\\') &&
            (outIdx > 0) && (ret[outIdx - 1] == '\\')
            )
        {
            idx++;
            continue;
        }
        ret[outIdx++] = c;
        idx++;
    }
    ret.resize(outIdx);
    return ret;
}

template <typename T>
class NormalizedPath
{
private:
    T mData;
public:
    // Default constructor
    NormalizedPath() :
        mData()
    {
    }

    // Copy consturctor
    NormalizedPath(const NormalizedPath<T>& input) :
        mData(input.mData)
    {
    }

    // Construct from string
    NormalizedPath(const T& input) :
        mData(normalizeToBackslashAndResolvePath(input))
    {
    }

    // Construct from C string
    NormalizedPath(const typename T::value_type* input) :
        mData(normalizeToBackslashAndResolvePath(T(input)))
    {
    }

    // Assign from string
    NormalizedPath<T>& operator= (const T& input)
    {
        mData = normalizeToBackslashAndResolvePath(input);
        return *this;
    }

    // Assign from C string
    NormalizedPath<T>& operator= (const typename T::value_type* input)
    {
        mData = normalizeToBackslashAndResolvePath(T(input));
        return *this;
    }

    // Append from string
    NormalizedPath<T>& operator+= (const T& input)
    {
        mData = normalizeToBackslashAndResolvePath(mData + input);
        return *this;
    }

    // Append from C string
    NormalizedPath<T>& operator+= (const typename T::value_type* input)
    {
        mData = normalizeToBackslashAndResolvePath(mData + input);
        return *this;
    }

    // Destructor
    ~NormalizedPath()
    {
    }

    // Type cast operator
    operator T() const { return mData; }

    // Other string-like operators
    const typename T::value_type* c_str() const { return mData.c_str(); }
    const T& str() const { return mData; }
    const T& str_lower() const { return mDataLower; }
};

// Time Helper Funcs
std::string generateTimestamp(std::string format);
std::string generateTimestampForFilename();
std::string generateTimestamp();

// Network Helper Funcs
void sendPUTRequest(const std::string& server, const std::string& data);

// Debug stuff
void InitDebugConsole();
int DebugPrint(const char * format, ...);
void DebugClear(HANDLE hConsole);
void dumpTypeLibrary(IDispatch* dispatchToDump, std::wostream& toOutput);

// Window Helper Funcs
void ShowAndFocusWindow(HWND hWindow);

// Handle WIN32 events when loading
void HandleEventsWhileLoading();

// Float/double manipulation
#ifdef __clang__
constexpr std::uint32_t FloatToDWord(float f) {
    return __builtin_bit_cast(std::uint32_t, f);
}

constexpr std::uint32_t DoubleLeastSignificantDWord(double d) {
    return static_cast<std::uint32_t>(__builtin_bit_cast(std::uint64_t, d) & 0xFFFFFFFFL);
}

constexpr std::uint32_t DoubleMostSignificantDWord(double d) {
    return static_cast<std::uint32_t>(__builtin_bit_cast(std::uint64_t, d) >> 32);
}
#endif

#endif

std::string GetEditorPlacedItem();
