#ifndef TestMode_hhhh
#define TestMode_hhhh

#include <string>
#include "VB6StrPtr.h"

struct STestModePlayerSettings
{
    Characters identity;
    short powerup;
    short mountType;
    short mountColor;
};

struct STestModeSettings
{
    bool enabled;
    std::wstring levelPath;
    std::string levelData;
    int playerCount;
	bool showFPS;
	bool godMode;
	STestModePlayerSettings players[2];

    STestModeSettings();
    void ResetToDefault(void);
};

bool testModeEnable(const STestModeSettings& settings);
void testModeDisable(void);
void testModeRestartLevel(void);

void testModeSmbxChangeModeHook(void);
bool testModeLoadLevelHook(VB6StrPtr* filename);

STestModeSettings getTestModeSettings();
void setTestModeSettings(const STestModeSettings& settings);

bool TestModeCheckHideWindow(void);

// The following function decleration is not included in this header, to
// avoid applying a json.hpp dependency to this header.
//   json IPCTestLevel(const json& params)

#endif
