#include "GameConfiguration.h"
#include "../Misc/VB6StrPtr.h"

namespace gameCharacters{
    VB6StrPtr* marioGame = 0;
    VB6StrPtr* luigiGame = 0;
    VB6StrPtr* peachGame = 0;
    VB6StrPtr* toadGame = 0;
    VB6StrPtr* linkGame = 0;
}

namespace mainMenu{
    VB6StrPtr* player1Game = 0;
    VB6StrPtr* player2Game = 0;
    VB6StrPtr* battleGame = 0;
    VB6StrPtr* options = 0;
    VB6StrPtr* exit = 0;
}

namespace optionsMenu{
    VB6StrPtr* player1Controls = 0;
    VB6StrPtr* player2Controls = 0;
    VB6StrPtr* fullscreenMode = 0;
    VB6StrPtr* windowedMode = 0;
    VB6StrPtr* viewCredits = 0;
}

#define TEXTPATCH(d_variable, d_reader, d_section, d_name, d_defaultVal, d_address) d_variable = new VB6StrPtr(d_reader.Get(d_section, d_name, d_defaultVal)); \
    memcpy((void*)d_address, d_variable, 4)

void GameConfiguration::runPatchByIni(INIReader& reader)
{
    if (reader.ParseError() != 0)
        return;


    TEXTPATCH(gameCharacters::marioGame, reader, "game-characters", "mario-game", "MARIO GAME ", 0x9339C2);
    TEXTPATCH(gameCharacters::luigiGame, reader, "game-characters", "luigi-game", "LUIGI GAME ", 0x933B00);
    TEXTPATCH(gameCharacters::peachGame, reader, "game-characters", "peach-game", "PEACH GAME ", 0x933C3D);
    TEXTPATCH(gameCharacters::toadGame, reader, "game-characters", "toad-game", "TOAD GAME ", 0x933D79);
    TEXTPATCH(gameCharacters::linkGame, reader, "game-characters", "link-game", "LINK GAME ", 0x933EB7);

    TEXTPATCH(mainMenu::player1Game, reader, "main-menu", "player1-game", "1 PLAYER GAME", 0x933417);
    TEXTPATCH(mainMenu::player2Game, reader, "main-menu", "player2-game", "2 PLAYER GAME", 0x933479);
    TEXTPATCH(mainMenu::battleGame, reader, "main-menu", "battle-game", "BATTLE GAME", 0x9334DB);
    TEXTPATCH(mainMenu::options, reader, "main-menu", "options", "OPTIONS", 0x93353D);
    TEXTPATCH(mainMenu::exit, reader, "main-menu", "exit", "EXIT", 0x93359F);

    TEXTPATCH(optionsMenu::player1Controls, reader, "option-menu", "player1-controls", "PLAYER 1 CONTROLS", 0x936DD7);
    TEXTPATCH(optionsMenu::player2Controls, reader, "option-menu", "player2-controls", "PLAYER 2 CONTROLS", 0x936E39);
    TEXTPATCH(optionsMenu::fullscreenMode, reader, "option-menu", "fullscreen-mode", "FULLSCREEN MODE", 0x936F10);
    TEXTPATCH(optionsMenu::windowedMode, reader, "option-menu", "windowed-mode", "WINDOWED MODE", 0x936EAC);
    TEXTPATCH(optionsMenu::viewCredits, reader, "option-menu", "view-credits", "VIEW CREDITS", 0x936F72);


}
