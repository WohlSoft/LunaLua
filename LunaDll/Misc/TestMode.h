#ifndef TestMode_hhhh
#define TestMode_hhhh

#include <string>
#include "VB6StrPtr.h"
#include "../libs/PGE_File_Formats/lvl_filedata.h"

struct STestModePlayerSettings
{
    Characters identity;
    short powerup;
    short mountType;
    short mountColor;
};

struct STestModeData
{
    //! Currently using level data
    std::string levelRawData;
    //! Current cache of the level data
    LevelData   levelData;
    /**
     * @brief Clear the data
     */
    void clear(void);
};

struct STestModeSettings
{
    //! Testing mode is enabled
    bool enabled;
    //! Path to actual level file to try open it when raw data wasn't sent
    std::wstring levelPath;
    //! Count of players
    int playerCount;
    //! Turn on FPS counter printing
    bool showFPS;
    //! Turn on god mode
    bool godMode;
    //! Settings of playable characters per player
    STestModePlayerSettings players[2];
    //! Warp Index
    unsigned int entranceIndex;

    STestModeSettings();
    /**
     * @brief Reset settings to initial state
     */
    void ResetToDefault(void);
};

bool testModeEnable(const STestModeSettings& settings, const std::string &newLevelData = std::string());
void testModeDisable(void);
void testModeRestartLevel(void);

void testModeSmbxChangeModeHook(void);
bool testModeLoadLevelHook(VB6StrPtr* filename);

STestModeSettings getTestModeSettings();
void setTestModeSettings(const STestModeSettings& settings);

LevelData &getCurrentLevelData();

bool TestModeCheckHideWindow(void);

// The following function decleration is not included in this header, to
// avoid applying a json.hpp dependency to this header.
//   json IPCTestLevel(const json& params)

#endif
