#include "GameConfiguration.h"
#include "../Misc/VB6StrPtr.h"
#include "../Misc/AsmPatch.h"
#include "../Defines.h"
#include "../Globals.h"

namespace strAddressRange {
    // Range to unlock
    std::uintptr_t minPage = 0x8BD000;
    std::size_t lenBytes = 0xAE000;
}

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

namespace general {
    VB6StrPtr& gameTitle_1 = *(VB6StrPtr*)0x8BD869;
    VB6StrPtr& gameTitle_2 = *(VB6StrPtr*)0x8BE25A;
    VB6StrPtr& gameTitle_3 = *(VB6StrPtr*)0x96AF26;
}

void GameConfiguration::runPatchByIni(IniProcessing &&reader)
{
    if (!reader.isOpened())
        return;

    // Special case: in softwareGL mode, append to the title to it's clear.
    std::string gameTitleSuffix = "";
    if (gStartupSettings.softwareGL)
    {
        gameTitleSuffix = " (Software Renderer)";
    }

    #ifdef __clang__
        gameTitleSuffix += " (Clang)";
    #endif

    {
        MemoryUnlock lock(strAddressRange::minPage, strAddressRange::lenBytes);
        if (lock.IsValid()) {
            reader.beginGroup("game-characters");
            gameCharacters::marioGame = reader.value("mario-game", std::string(gameCharacters::marioGame)).toString();
            gameCharacters::luigiGame = reader.value("luigi-game", std::string(gameCharacters::luigiGame)).toString();
            gameCharacters::peachGame = reader.value("peach-game", std::string(gameCharacters::peachGame)).toString();
            gameCharacters::toadGame = reader.value("toad-game", std::string(gameCharacters::toadGame)).toString();
            gameCharacters::linkGame = reader.value("link-game", std::string(gameCharacters::linkGame)).toString();
            reader.endGroup();

            reader.beginGroup("main-menu");
            mainMenu::player1Game = reader.value("player1-game", std::string(mainMenu::player1Game)).toString();
            mainMenu::player2Game = reader.value("player2-game", std::string(mainMenu::player2Game)).toString();
            mainMenu::battleGame = reader.value("battle-game", std::string(mainMenu::battleGame)).toString();
            mainMenu::options = reader.value("options", std::string(mainMenu::options)).toString();
            mainMenu::exit = reader.value("exit", std::string(mainMenu::exit)).toString();
            reader.endGroup();

            reader.beginGroup("option-menu");
            optionsMenu::player1Controls = reader.value("player1-controls", std::string(optionsMenu::player1Controls)).toString();
            optionsMenu::player2Controls = reader.value("player2-controls", std::string(optionsMenu::player2Controls)).toString();
            optionsMenu::fullscreenMode = reader.value("fullscreen-mode", std::string(optionsMenu::fullscreenMode)).toString();
            optionsMenu::windowedMode = reader.value("windowed-mode", std::string(optionsMenu::windowedMode)).toString();
            optionsMenu::viewCredits = reader.value("view-credits", std::string(optionsMenu::viewCredits)).toString();
            reader.endGroup();

            // References the same string memory, so do not destruct those.
            reader.beginGroup("general");
            general::gameTitle_1.assignNoDestruct(reader.value("game-title", std::string(general::gameTitle_1)).toString() + gameTitleSuffix);
            general::gameTitle_2.assignNoDestruct(reader.value("game-title", std::string(general::gameTitle_2)).toString() + gameTitleSuffix);
            general::gameTitle_3.assignNoDestruct(reader.value("game-title", std::string(general::gameTitle_3)).toString() + gameTitleSuffix);
            reader.endGroup();
        }
    }
}
