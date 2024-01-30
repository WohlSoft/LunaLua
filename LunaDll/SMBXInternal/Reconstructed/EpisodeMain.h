// EpisodeLoader.h
#ifndef EpisodeLoader_hhh
#define EpisodeLoader_hhh

#include <string>
#include <vector>

#include "../Menu.h"
#include "../../libs/PGE_File_Formats/file_formats.h"

class EpisodeMain {
    public:
        EpisodeMain();
        ~EpisodeMain();

        void LaunchEpisode(std::wstring wldPathWS, int saveSlot, int playerCount, Characters firstCharacter, Characters secondCharacter, bool suppressSound);
        int FindSaves(std::string worldPathS, int saveSlot);
        int WriteEpisodeEntry(VB6StrPtr worldNameVB6, VB6StrPtr worldPathVB6, VB6StrPtr worldFileVB6, WorldData wldData, bool isNewEpisode);
};

#endif
