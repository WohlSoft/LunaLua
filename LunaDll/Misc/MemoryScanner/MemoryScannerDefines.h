#ifndef MemoryScannerDefines_hhhhh
#define MemoryScannerDefines_hhhhh

namespace MemoryScannerDefines{

    enum class MemoryType{
        MEM_BYTE,
        MEM_WORD,
        MEM_DWORD,
        MEM_FLOAT,
        MEM_DOUBLE,
        MEM_STRING
    };

    enum class ServerCommands{
        CMD_UPDATELIST
    };

    /*
    
    CMD_UPDATELIST:
    Server --> Client:
    4 Bytes                 ServerCommands
    4 Bytes                 SizeOfMap
    
    ----------------------------------------
        n * SizeOfMap
    ----------------------------------------
    4 Bytes                 Address
    4 Bytes                 MemoryType
    4 Bytes                 LengthOfString
    LengthOfString Bytes    Content
    ----------------------------------------

    Client --> Server:
    4 Bytes                 ServerCommands
    4 Bytes                 NewAddress
    4 Bytes                 MemoryType
    */

}

#endif