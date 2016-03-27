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
    STestModePlayerSettings players[2];

    STestModeSettings();
    void ResetToDefault(void);
};

bool testModeEnable(const STestModeSettings& settings);
void testModeDisable(void);

void testModeSmbxChangeModeHook(void);
bool testModeLoadLevelHook(VB6StrPtr* filename);

#endif
