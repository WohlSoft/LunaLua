#include "FileUtils.h"
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>

// Lua file object type, taken from LuaJIT's lib_io.c
enum class IOFileUDType : std::uint32_t {
    IOFILE_TYPE_FILE = 0, /* Regular file. */
    IOFILE_TYPE_PIPE = 1, /* Pipe. */
    IOFILE_TYPE_STDF = 2, /* Standard file handle. */
    IOFILE_TYPE_MASK = 3,
    IOFILE_FLAG_CLOSE = 4 /* Close after io.lines() iterator. */
};

inline IOFileUDType operator|(IOFileUDType a, IOFileUDType b) {
    return static_cast<IOFileUDType>(static_cast<std::underlying_type<IOFileUDType>::type>(a) | static_cast<std::underlying_type<IOFileUDType>::type>(b));
}

// Lua file object structure, taken from LuaJIT's lib_io.c
struct IOFileUD {
  std::FILE *fp;		/* File handle. */
  IOFileUDType type;	/* File type. */
};

// Create a standard file object, set the field `name` of lua stack element -1 to it, then set its metatable to lua stack element -2, taken from LuaJIT's lib_io.c
// Note: This requires modifying lua51.lib to make _io_std_new a global symbol. I know this is incredibly cursed but I couldn't think of any other way.
// Note 2: The actual return type of io_std_new is GCobj* (internal luajit garbage collected object), we don't need it so we can ignore it.
extern "C" {
    void io_std_new(lua_State *L, std::FILE* file, const char* name);
}

// Creates a lua file object from a nonnull C FILE* object
luabind::object FileUtils::CFileToLua(lua_State* L, std::FILE* file, bool forIoLines) {

    // The table which will contain the newly created file
    luabind::object fileTable = luabind::newtable(L);

    // The metatable of file objects
    luabind::object fileMetatable = luabind::object(luabind::from_stack(L, LUA_REGISTRYINDEX))["FILE*"];

    // Push metatable and table to store the new file object to stack
    fileMetatable.push(L);
    fileTable.push(L);

    // Create a new file object and store it to fileTable
    io_std_new(L, file, "fileObject");

    // Pop fileMetatable and fileTable
    lua_pop(L, 2);

    // Get newly created file object
    luabind::object fileObject = fileTable["fileObject"];

    // Push fileObject to stack
    fileObject.push(L);

    // Get address of file userdata memory
    IOFileUD* luaFile = (IOFileUD*) lua_topointer(L, -1);

    // Pop fileObject from stack
    lua_pop(L, 1);

    // Properly set the type of the file object (io_std_new initializes it to IOFileUDType::IOFILE_TYPE_STDF)
    if (forIoLines) {
        luaFile->type = IOFileUDType::IOFILE_TYPE_FILE | IOFileUDType::IOFILE_FLAG_CLOSE;
    } else {
        luaFile->type = IOFileUDType::IOFILE_TYPE_FILE;
    }

    // Return new file object
    return fileObject;
}

// Converts a file opening mode string to flags, return false if the mode string is invalid
bool FileUtils::ParseFileOpeningMode(const char* mode, FileOpeningMode& out) {
    // Get mode string length
    std::size_t len = std::strlen(mode);

    // Empty string is invalid
    if (len == 0) {
        return false;
    }

    // Get base character
    char baseMode = mode[0];

    // Refuse invalid modes
    if (baseMode != 'r' && baseMode != 'w' && baseMode != 'a') {
        return false;
    }

    // Does the mode contains a '+'?
    bool readWriteFlag = false;

    // Does the mode contains a 'b'?
    bool binaryModeFlag = false;

    // check for modifiers
    for (std::size_t i = 1; i < len; i++) {
        char modifier = mode[i];

        if (modifier == 'b') { // binary flag
            // only one occurence allowed
            if (binaryModeFlag) {
                return false;
            }

            binaryModeFlag = true;
        } else if (modifier == '+') { // read+write flag
            // only one occurence allowed
            if (readWriteFlag) {
                return false;
            }

            readWriteFlag = true;
        }
    }

    // determine if the file must exist
    out.fileMustExist = (baseMode == 'r');

    // choose between _O_RDONLY, _O_WRONLY and _O_RDWR
    if (readWriteFlag) {
        out.flags = _O_RDWR;
    } else if (baseMode == 'r') {
        out.flags = _O_RDONLY;
    } else {
        out.flags = _O_WRONLY;
    }

    // determine if we request write access
    out.requestWrite = (out.flags != _O_RDONLY);

    // choose between _O_APPEND and _O_TRUNC
    if (baseMode == 'w') {
        out.flags |= _O_TRUNC;
    } else if (baseMode == 'a') {
        out.flags |= _O_APPEND;
    }

    // Add binary or text flag
    if (binaryModeFlag) {
        out.flags |= _O_BINARY;
    } else {
        out.flags |= _O_TEXT;
    }

    return true;
}