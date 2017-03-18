#ifndef GlobalFuncs_hhhhhhh
#define GlobalFuncs_hhhhhhh

#include <windows.h>
#include <string>
#include <vector>
#include <wchar.h>
#include <string.h>

//String manupulation things
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

std::string ConvertWCSToMBS(const wchar_t * pstr, long wslen);
std::string ConvertBSTRToMBS(BSTR bstr);
BSTR ConvertMBSToBSTR(const std::string & str);

std::string i2str(int source);
bool is_number(const std::string& s);
bool file_existsX(const std::string& name);
bool isAbsolutePath(const std::wstring& path);
bool isAbsolutePath(const std::string& path);
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
void CleanUp();

/// HELPER FUNCTIONS ///
bool vecStrFind(const std::vector<std::wstring>& vecStr, const std::wstring& find);
std::string url_encode(const std::string &value);
// Module Helper Funcs
//HMODULE getModule(std::string moduleName);

// File/Path Helper Funcs
bool readFile(std::wstring &content, std::wstring path, std::wstring errMsg = std::wstring());
bool readFile(std::string &content, std::string path, std::string errMsg = std::string());
bool writeFile(const std::string &content, const std::string &path);
std::vector<std::string> listFilesOfDir(const std::string& path);
std::vector<std::string> listOfDir(const std::string& path, DWORD fileAttributes);
//std::wstring getModulePath();
std::wstring getCustomFolderPath();
std::wstring getLatestFile(const std::initializer_list<std::wstring>& paths);
std::wstring getLatestConfigFile(const std::wstring& configname);

// Time Helper Funcs
std::string generateTimestamp(std::string format);
std::string generateTimestampForFilename();
std::string generateTimestamp();

// Network Helper Funcs
void sendPUTRequest(const std::string& server, const std::string& data);

// Debug stuff
void RedirectIOToConsole();

#endif
