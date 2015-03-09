#ifndef GlobalFuncs_hhhhhhh
#define GlobalFuncs_hhhhhhh

#include <string>
#include <vector>
#include <windows.h>
#include <wchar.h>
#include <string.h>

//String manupulation things
void splitStr(std::vector<std::string>& dest, const std::string& str, const char* separator);
void replaceSubStr(std::string& str, const std::string& from, const std::string& to);
void RemoveSubStr(std::string& sInput, const std::string& sub);
std::vector<std::string> splitCmdArgs( std::string str);

std::string utf8_encode(const std::wstring &wstr);
std::wstring utf8_decode(const std::string &str);
std::string wstr2str(const std::wstring &wstr);

std::string i2str(int source);
bool is_number(const std::string& s);
bool file_existsX(const std::string& name);
bool isAbsolutePath(const std::wstring& path);
bool isAbsolutePath(const std::string& path);

/// Functions ///
void InitGlobals();
void ResetFreeGlob();
void CleanUp();

/// HELPER FUNCTIONS ///
std::vector<std::wstring> wsplit( std::wstring str, wchar_t delimiter);
std::vector<std::string> split( std::string str, char delimiter);
bool vecStrFind(const std::vector<std::string>& vecStr, const std::string& find);
HMODULE getModule(std::string moduleName);
std::wstring getModulePath();
bool readFile(std::wstring &content, std::wstring path, std::wstring errMsg = std::wstring());

#endif
