#ifndef Minigames_H
#define Minigames_H

#include <queue>
#include <list>
#include "../Defines.h"
#include "CGUI/CGUI.h"
#include "../Rendering/Rendering.h"

class Minigame {
    virtual bool Initialize() =0;
    virtual void Process() =0;
    virtual void Draw(Renderer g) =0;
    virtual void End() =0;

    virtual void OnPause();				// Should be called if user tries to pause SMBX, to disable pausing

    Minigame* m_ChildGame;
    std::list<CGUIElement*> m_GUIElements;	
};

//class MinigameController {
//	queue<Minigame*> m_Games;
//};

#endif
