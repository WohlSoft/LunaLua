#include "CustomParamStore.h"
#include "../Globals.h"

// Global Instances
CustomParamStore<1>     g_LevelCustomParams;
CustomParamStore<21>    g_SectionCustomParams;
CustomParamStore<5000>  g_NpcCustomParams;
CustomParamStore<8000>  g_BgoCustomParams;
CustomParamStore<20000> g_BlockCustomParams;

ConfigPackMiniManager   g_configManager;

void loadConfigPack(const std::string &episodePath, const std::string &customDirectoryName)
{
    g_configManager.setEpisodePath(episodePath);
    g_configManager.setCustomPath(episodePath + "/" + customDirectoryName);
    g_configManager.loadConfigPack(gAppPathUTF8 + "/PGE/configs/SMBX2-Integration");
}
