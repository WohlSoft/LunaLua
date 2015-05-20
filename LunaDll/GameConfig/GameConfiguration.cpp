#include "GameConfiguration.h"
#include "../Misc/VB6StrPtr.h"

namespace gameCharacters{
    VB6StrPtr* marioGame = 0;
    VB6StrPtr* luigiGame = 0;
    VB6StrPtr* peachGame = 0;
    VB6StrPtr* toadGame = 0;
    VB6StrPtr* linkGame = 0;
}


void GameConfiguration::runPatchByIni(INIReader& reader)
{
    if (reader.ParseError() != 0)
        return;

    gameCharacters::marioGame = new VB6StrPtr(reader.Get("game-characters", "mario-game", "MARIO GAME "));
    gameCharacters::luigiGame = new VB6StrPtr(reader.Get("game-characters", "luigi-game", "LUIGI GAME "));
    gameCharacters::peachGame = new VB6StrPtr(reader.Get("game-characters", "peach-game", "PEACH GAME "));
    gameCharacters::toadGame = new VB6StrPtr(reader.Get("game-characters", "toad-game", "TOAD GAME "));
    gameCharacters::linkGame = new VB6StrPtr(reader.Get("game-characters", "link-game", "LINK GAME "));

    memcpy((void*)0x9339C2, gameCharacters::marioGame, 4);
    memcpy((void*)0x933B00, gameCharacters::luigiGame, 4);
    memcpy((void*)0x933C3D, gameCharacters::peachGame, 4);
    memcpy((void*)0x933D79, gameCharacters::toadGame, 4);
    memcpy((void*)0x933EB7, gameCharacters::linkGame, 4);
}
