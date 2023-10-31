#ifndef SMBXFileManager_hhhhh
#define SMBXFileManager_hhhhh

#include <string>

#include "../libs/PGE_File_Formats/lvl_filedata.h"
#include "../libs/PGE_File_Formats/wld_filedata.h"

// This interface is the bridge which manages our data
struct SMBXDataInterface {

};


class SMBXLevelFileBase {
public:
    SMBXLevelFileBase();

    // Common Level File Function
    /**
     * @brief Load level file from the disk and fill SMBX's internals with gotten data
     * @param path Path to the level file to load
     * @param outData Level output data structure to store gotten data for usage
     */
    void ReadFile(const std::wstring& path, LevelData &outData);

    /**
     * @brief Read level data from the memory block and fill SMBX's internals with gotten data
     * @param rawData Raw level data to load
     * @param outData Level output data structure to store gotten data for usage
     * @param fakePath Fake level file path to tell where is need to look for custom resources
     */
    void ReadFileMem(std::string &rawData, LevelData &outData, const std::wstring& fakePath = L"");

    // Status
    inline bool isValid() const { return m_isValid; }

protected:
    bool m_isValid;
};

class SMBXWorldFileBase {
public:
    static void PopulateEpisodeList();
    void ReadFile(const std::wstring& path, WorldData& outData);
    // Status
    inline bool isValid() const { return m_isValid; }

protected:
    bool m_isValid;
};

#endif
