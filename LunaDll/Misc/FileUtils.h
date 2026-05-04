#ifndef FileUtils_hhh
#define FileUtils_hhh

#include <luabind/object.hpp>
#include <lua.hpp>
#include <cstdio>

namespace FileUtils {
    struct FileOpeningMode {
        int flags; // Win32 file opening flags
        bool requestWrite; // Do we require write access?
        bool fileMustExist; // Does the file must exist?
    };

    luabind::object CFileToLua(lua_State* L, std::FILE* file, bool forIoLines);
    bool ParseFileOpeningMode(const char* mode, FileOpeningMode& out);
}

#endif