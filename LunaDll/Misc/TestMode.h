#ifndef TestMode_hhhh
#define TestMode_hhhh

#include <string>

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
    int playerCount;
    STestModePlayerSettings players[2];

    STestModeSettings();
    void ResetToDefault(void);
};

bool testModeEnable(const STestModeSettings& settings);
void testModeDisable(void);

void testModeSmbxChangeModeHook(void);

#endif
