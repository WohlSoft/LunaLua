#ifndef Menu_hhhhh
#define Menu_hhhhh

#include "../Defines.h"
#include <vector>

struct EpisodeListItem
{
    VB6StrPtr episodeName;
    VB6StrPtr episodePath;
    VB6StrPtr episodeWorldFile;
    DWORD unknown_C;
    DWORD unknown_10;
    VB6StrPtr unknown_14;

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline EpisodeListItem* Get(unsigned short index) {
        if (index >= GM_EP_LIST_COUNT) return NULL;
        return &((EpisodeListItem*)GM_EP_LIST_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_EP_LIST_COUNT;
    }

    static inline std::vector<EpisodeListItem*> GetAll(){
        std::vector<EpisodeListItem*> allEpisodes;
        for (int i = 0; i < GM_EP_LIST_COUNT; ++i){
            allEpisodes.push_back(Get(i));
        }
        return allEpisodes;
    }

    static const short MAX_ID = 100;

};

static_assert(sizeof(EpisodeListItem) == 0x18, "sizeof(SMBX_BGO) must be 0x24");


#endif