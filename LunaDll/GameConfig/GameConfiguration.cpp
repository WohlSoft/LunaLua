#include "GameConfiguration.h"
#include "../Misc/VB6StrPtr.h"
#include "../Defines.h"

namespace gameCharacters{
    VB6StrPtr& marioGame = *(VB6StrPtr*)0x9339C2;
    VB6StrPtr& luigiGame = *(VB6StrPtr*)0x933B00;
    VB6StrPtr& peachGame = *(VB6StrPtr*)0x933C3D;
    VB6StrPtr& toadGame = *(VB6StrPtr*)0x933D79;
    VB6StrPtr& linkGame = *(VB6StrPtr*)0x933EB7;
}

namespace mainMenu{
    VB6StrPtr& player1Game = *(VB6StrPtr*)0x933417;
    VB6StrPtr& player2Game = *(VB6StrPtr*)0x933479;
    VB6StrPtr& battleGame = *(VB6StrPtr*)0x9334DB;
    VB6StrPtr& options = *(VB6StrPtr*)0x93353D;
    VB6StrPtr& exit = *(VB6StrPtr*)0x93359F;
}

namespace optionsMenu{
    VB6StrPtr& player1Controls = *(VB6StrPtr*)0x936DD7;
    VB6StrPtr& player2Controls = *(VB6StrPtr*)0x936E39;
    VB6StrPtr& fullscreenMode = *(VB6StrPtr*)0x936F10;
    VB6StrPtr& windowedMode = *(VB6StrPtr*)0x936EAC;
    VB6StrPtr& viewCredits = *(VB6StrPtr*)0x936F72;
}

void GameConfiguration::runPatchByIni(INIReader& reader)
{
    if (reader.ParseError() != 0)
        return;


    gameCharacters::marioGame = reader.Get("game-characters", "mario-game", gameCharacters::marioGame);
    gameCharacters::luigiGame = reader.Get("game-characters", "luigi-game", gameCharacters::luigiGame);
    gameCharacters::peachGame = reader.Get("game-characters", "peach-game", gameCharacters::peachGame);
    gameCharacters::toadGame = reader.Get("game-characters", "toad-game", gameCharacters::toadGame);
    gameCharacters::linkGame = reader.Get("game-characters", "link-game", gameCharacters::linkGame);

    mainMenu::player1Game = reader.Get("main-menu", "player1-game", mainMenu::player1Game);
    mainMenu::player2Game = reader.Get("main-menu", "player2-game", mainMenu::player2Game);
    mainMenu::battleGame = reader.Get("main-menu", "battle-game", mainMenu::battleGame);
    mainMenu::options = reader.Get("main-menu", "options", mainMenu::options);
    mainMenu::exit = reader.Get("main-menu", "exit", mainMenu::exit);

    optionsMenu::player1Controls = reader.Get("option-menu", "player1-controls", optionsMenu::player1Controls);
    optionsMenu::player2Controls = reader.Get("option-menu", "player2-controls", optionsMenu::player2Controls);
    optionsMenu::fullscreenMode = reader.Get("option-menu", "fullscreen-mode", optionsMenu::fullscreenMode);
    optionsMenu::windowedMode = reader.Get("option-menu", "windowed-mode", optionsMenu::windowedMode);
    optionsMenu::viewCredits = reader.Get("option-menu", "view-credits", optionsMenu::viewCredits);

}
