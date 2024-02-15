#ifndef MiscFuncs_hhh
#define MiscFuncs_hhh

#include <math.h>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "../SMBXInternal/Layer.h"

void InfiniteFlying(int player);

template<typename Elem, typename Traits, typename Alloc>
static inline auto RemoveTail(const std::basic_string<Elem, Traits, Alloc>& filename, Elem tail, size_t pos= std::basic_string<Elem, Traits, Alloc>::npos)
{
    typedef std::basic_string<Elem, Traits, Alloc> tstring;
    size_t lastdot = filename.find_last_of(tail);
    if ((lastdot == tstring::npos) ||
        ((pos != tstring::npos) && (lastdot < pos)))
    {
        return filename;
    }
    return filename.substr(0, lastdot);
}

static inline auto RemoveExtension(const std::string& filename)
{
    size_t pos = filename.find_last_of("/\\");
    return RemoveTail(filename, '.', pos);
}
static inline auto RemoveExtension(const std::wstring& filename)
{
    size_t pos = filename.find_last_of(L"/\\");
    return RemoveTail(filename, L'.', pos);
}



BOOL FileExists(LPCTSTR szPath);
BOOL DirectoryExists(LPCTSTR szPath);

bool fileExists(const std::wstring &szPath);
bool fileExists(std::string szPath);
bool directoryExists(const std::wstring &szPath);

bool hasSuffix(const std::string &path, const std::string &suffix);
bool hasSuffix(const std::wstring &path, const std::wstring &suffix);

FIELDTYPE StrToFieldtype(std::wstring string);
void MemAssign(int address, double value, OPTYPE operation, FIELDTYPE ftype);
bool CheckMem(int address, double value, COMPARETYPE ctype, FIELDTYPE ftype);
double GetMem(int addr, FIELDTYPE ftype);

int ComputeLevelSection(int x, int y);			// Compute the current SMBX level section for the given coords, or -1 if invalid
int ToSection(int coord);

void RandomPointInRadius(double* ox, double* oy, double cx, double cy, int radius);

bool FastTestCollision(int Left1, int Up1, int Right1, int Down1, int Left2, int Up2, int Right2, int Down2);

bool SegmentIntersectRectangle(double a_rectangleMinX, double a_rectangleMinY, double a_rectangleMaxX, double a_rectangleMaxY,
                                 double a_p1x, double a_p1y, double a_p2x, double a_p2y);

void PrintSyntaxError(std::wstring errored_line);

void InitIfMissing(std::map<std::wstring, double>* map, std::wstring sought_key, double init_val);	// Init a map value if key doesn't exist

#ifndef __MINGW32__
static inline double round(double val)
{
    return floor(val + 0.5);
}
#endif

namespace std
{
    #ifdef __MINGW32__
    wstring to_wstring(long long src);
    string to_string(long long src);
    #endif
}

const wchar_t* BoolToString(bool b);

#ifndef __MINGW32__
static inline float roundf(float val)
{
    return (float)floor(val + 0.5);
}
#endif

//PGE Debug stuff:
void readAndWriteNPCSettings();

#endif
