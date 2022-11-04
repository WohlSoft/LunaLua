
#ifndef GameConfiguration_hhhhhh
#define GameConfiguration_hhhhhh

#include <IniProcessor/ini_processing.h>
class GameConfiguration
{
public:

    static void runPatchByIni(IniProcessing&& reader);
};

#endif
