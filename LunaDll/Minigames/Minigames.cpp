#include "Minigames.h"
#include "../Defines.h"

void Minigame::OnPause() {
    if(GM_PAUSE_OPEN != 0) {
        GM_PAUSE_OPEN = 0;
    }
}