// EpisodeLoader.h
#ifndef EpisodeLoader_hhh
#define EpisodeLoader_hhh

#include <string>
#include <vector>

#include "../Menu.h"
#include "../../libs/PGE_File_Formats/file_formats.h"

#include "../Functions.h"
#include "../Types.h"
#include "../Variables.h"

class EpisodeMain {
    public:
        EpisodeMain();
        ~EpisodeMain();

        void LaunchEpisode(std::wstring wldPathWS, int saveSlot, int playerCount, Characters firstCharacter, Characters secondCharacter);
        int FindSaves(std::string worldPathS, int saveSlot);
        int WriteEpisodeEntry(VB6StrPtr worldNameVB6, VB6StrPtr worldPathVB6, VB6StrPtr worldFileVB6, WorldData wldData, bool isNewEpisode);
        bool CheckCollision(SMBX13::Types::Location_t momentumA, SMBX13::Types::Location_t momentumB);
};

extern EpisodeMain gEpisodeMain;

#endif
