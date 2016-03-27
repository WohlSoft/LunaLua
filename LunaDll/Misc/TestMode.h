#ifndef TestMode_hhhh
#define TestMode_hhhh

#include <string>

bool testModeEnable(const std::wstring& path);
void testModeDisable(void);

void testModeSmbxChangeModeHook(void);

#endif
