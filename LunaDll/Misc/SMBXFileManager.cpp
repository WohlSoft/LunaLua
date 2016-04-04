#include "SMBXFileManager.h"
#include "../Defines.h"
#include "MiscFuncs.h"

SMBXLevelFileBase::SMBXLevelFileBase() : 
    m_isValid(false)
{

}

#include <iostream>

// TODO: Return Error?
void SMBXLevelFileBase::ReadFile(const std::wstring& fullPath)
{
    m_isValid = false; // Ensure that we are not valid right now
    
    *(DWORD*)0xB2B9E4 = 0; // Unknown
    native_cleanupLevel();
    native_setupSFX();
    GM_FREEZWITCH_ACTIV = 0;

    // Reset counters (values where reset by native_cleanupLevel)
    GM_BLOCK_COUNT = 0;
    GM_BGO_COUNT = 0;
    GM_NPCS_COUNT = 0;
    GM_WARP_COUNT = 0;
    GM_WATER_AREA_COUNT = 0;
 
    // Check if Attributes is valid
    if (GetFileAttributesW(fullPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        return;
    
    size_t findLastSlash = fullPath.find_last_of(L"/\\");
    
    // Check if path has slash, if not then invalid
    if (findLastSlash == std::wstring::npos)
        return;
    
    std::wstring dir = fullPath.substr(0U, findLastSlash + 1);
    std::wstring filename = fullPath.substr(findLastSlash + 1);
    std::wstring levelname = RemoveExtension(filename);
    std::wstring customFolder = dir + levelname;

    
    std::wcout << L"Calc --> fullPath: \"" << fullPath << "\"" << std::endl
        << L"Calc --> path: \"" << dir << "\"" << std::endl
        << L"Calc --> filename: \"" << filename << "\"" << std::endl
        << L"Calc --> levelname: \"" << levelname << "\"" << std::endl
        << L"Calc --> custom folder: \"" << customFolder << "\"" << std::endl;

    GM_LVLFILENAME_PTR = filename;
    GM_LVLNAME_PTR = levelname;
    GM_FULLPATH = fullPath;
    GM_FULLDIR = dir;

    


    // Init Config-Txt
    VB6StrPtr customFolderVB6 = customFolder;
    native_loadNPCConfig(&customFolderVB6);

    // Load Episode GFX
    native_loadLocalGfx();

    // Load GFX from custom folder
    native_loadGraphicsFromFolder(&customFolderVB6);


}
