
#include <FileManager/config_manager.h>

int main(int argc, char**argv)
{
    if(argc <= 3)
        return 1;

    ConfigPackMiniManager ku;

    ku.setEpisodePath(argv[2]);
    ku.setCustomPath(argv[3]);
    ku.loadConfigPack(argv[1]);

    return 0;
}
