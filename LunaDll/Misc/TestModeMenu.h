#ifndef TestModeMenu_hhhh
#define TestModeMenu_hhhh

void testModeClosePauseMenu(bool restart, bool exit);
void testModePauseMenu(bool allowContinue, bool skipEnded);
void testModeMenuSkipTick();
bool testModeMenuIsSkipTickPending();
void testModeMenuCheckOpenAfterSkipTick();

#endif
