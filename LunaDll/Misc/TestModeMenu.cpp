#include <map>

#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Sound.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../Rendering/RenderOps/RenderRectOp.h"

#include "TestMode.h"
#include "TestModeMenu.h"

///////////////////////////////////////////////
//============ MENU ITEM CLASSES ============//
///////////////////////////////////////////////

class MenuItem
{
public:
    MenuItem() {};
    virtual ~MenuItem() {};
    virtual void Render(float x, float y, bool selected) = 0;
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) = 0;
};
class TextMenuItem : public virtual MenuItem
{
protected:
    int renderCounter;
public:
    TextMenuItem() :
        renderCounter(0)
    {};
    virtual ~TextMenuItem() {};
    virtual std::wstring GetText() = 0;
    virtual void Render(float x, float y, bool selected)
    {
        if (selected && renderCounter < 20) {
            gLunaRender.AddOp(new RenderStringOp(L">", 4, x, y));
        }
        gLunaRender.AddOp(new RenderStringOp(GetText(), 4, x + 20, y));

        renderCounter = (renderCounter + 1) % 40;
    }
};
class RunnableMenuItem : public virtual MenuItem
{
public:
    RunnableMenuItem() {};
    virtual ~RunnableMenuItem() {};
    virtual bool Run() = 0;
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap)
    {
        if ((keymap.jumpKeyState && !lastKeymap.jumpKeyState) || (GetKeyState(VK_RETURN) & 0x1000))
        {
            return Run();
        }
        return false;
    }
};

class TextSelectorItem : public virtual MenuItem
{
protected:
    int renderCounter;
    std::map<short, std::wstring> options;
public:
    TextSelectorItem() :
        renderCounter(0), options()
    {};
    virtual ~TextSelectorItem() {};
    virtual short GetSelection() = 0;
    virtual void ProcessSelection(short option) = 0;
    virtual void Render(float x, float y, bool selected)
    {
        short option = GetSelection();
        auto optionIt = options.find(option);
        if (optionIt == options.end()) optionIt = options.begin();
        std::wstring text = optionIt->second; 

        if (selected && renderCounter < 20) {
            gLunaRender.AddOp(new RenderStringOp(L"<", 4, x, y));
            gLunaRender.AddOp(new RenderStringOp(L">", 4, x+20+text.size()*18, y));
        }
        gLunaRender.AddOp(new RenderStringOp(text, 4, x + 20, y));

        renderCounter = (renderCounter + 1) % 40;
    }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap)
    {
        short option = GetSelection();
        auto optionIt = options.find(option);
        if (optionIt == options.end()) optionIt = options.begin();

        if (keymap.rightKeyState && !lastKeymap.rightKeyState)
        {
            optionIt++;
            if (optionIt == options.end()) optionIt = options.begin();
        }
        else if (keymap.leftKeyState && !lastKeymap.leftKeyState)
        {
            if (optionIt == options.begin()) optionIt = options.end();
            optionIt--;
        }
        else
        {
            return false;
        }

        SMBXSound::PlaySFX(76);
        ProcessSelection(optionIt->first);
        return false;
    }
};

class MenuItemContinue : public TextMenuItem, public RunnableMenuItem
{
public:
    virtual void Render(float x, float y, bool selected) { TextMenuItem::Render(x, y, selected); }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) { return RunnableMenuItem::ProcessInput(keymap, lastKeymap); }
    virtual std::wstring GetText() { return L"Continue"; };
    virtual bool Run() {
        // Pause Sound
        SMBXSound::PlaySFX(30);
        return true;
    };
    static MenuItemContinue inst;
};
class MenuItemRestartLevel : public TextMenuItem, public RunnableMenuItem
{
public:
    virtual void Render(float x, float y, bool selected) { TextMenuItem::Render(x, y, selected); }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) { return RunnableMenuItem::ProcessInput(keymap, lastKeymap); }
    virtual std::wstring GetText() { return L"Restart Level"; };
    virtual bool Run() {
        // Restart Level
        testModeRestartLevel();
        return true;
    };
    static MenuItemRestartLevel inst;
};
class MenuItemQuit : public TextMenuItem, public RunnableMenuItem
{
public:
    virtual void Render(float x, float y, bool selected) { TextMenuItem::Render(x, y, selected); }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) { return RunnableMenuItem::ProcessInput(keymap, lastKeymap); }
    virtual std::wstring GetText() { return L"Quit"; };
    virtual bool Run() {
        // Exit
        // TODO: If launched by something with IPC (PGE Editor) hide window instead of exit
        _exit(0);
        return true;
    };
    static MenuItemQuit inst;
};

class CharacterSelectorItem : public TextSelectorItem
{
public:
    CharacterSelectorItem() :
        TextSelectorItem()
    {
        options[1] = L"Mario";
        options[2] = L"Luigi";
        options[3] = L"Peach";
        options[4] = L"Toad";
        options[5] = L"Link";
    };
    virtual ~CharacterSelectorItem() {};
    virtual short GetSelection()
    {
        STestModeSettings settings = getTestModeSettings();
        return static_cast<short>(settings.players[0].identity);
    }

    virtual void ProcessSelection(short option)
    {
        STestModeSettings settings = getTestModeSettings();
        settings.players[0].identity = static_cast<Characters>(option);
        setTestModeSettings(settings);
    }

    static CharacterSelectorItem player1Inst;
};

class PowerupSelectorItem : public TextSelectorItem
{
public:
    PowerupSelectorItem() :
        TextSelectorItem()
    {
        options[1] = L"Small";
        options[2] = L"Big";
        options[3] = L"Fire";
        options[4] = L"Leaf";
        options[5] = L"Tanookie";
        options[6] = L"Hammer";
        options[7] = L"Ice";
    };
    virtual ~PowerupSelectorItem() {};
    virtual short GetSelection()
    {
        STestModeSettings settings = getTestModeSettings();
        return settings.players[0].powerup;
    }

    virtual void ProcessSelection(short option)
    {
        STestModeSettings settings = getTestModeSettings();
        settings.players[0].powerup = option;
        setTestModeSettings(settings);
    }

    static PowerupSelectorItem player1Inst;
};

MenuItemContinue MenuItemContinue::inst;
MenuItemRestartLevel MenuItemRestartLevel::inst;
MenuItemQuit MenuItemQuit::inst;
CharacterSelectorItem CharacterSelectorItem::player1Inst;
PowerupSelectorItem PowerupSelectorItem::player1Inst;

//////////////////////////////////////////////
//========== MENU RUNNER FUNCTION ==========//
//////////////////////////////////////////////

void testModePauseMenu(bool allowContinue)
{
    KeyMap lastKeymap = Player::Get(1)->keymap;

    unsigned int selectedOption = 0;
    std::vector<MenuItem*> menuItems;
    if (allowContinue)
    {
        menuItems.push_back(&MenuItemContinue::inst);
    }
    menuItems.push_back(&MenuItemRestartLevel::inst);
    menuItems.push_back(&CharacterSelectorItem::player1Inst);
    menuItems.push_back(&PowerupSelectorItem::player1Inst);
    menuItems.push_back(&MenuItemQuit::inst);

    float menuX = 200.0f;
    float menuY = 200.0f;
    float menuW = 400.0f;
    float menuH = 200.0f;
    float lineSpacing = 20.0f;
    float charWidth = 20.0f;

    // Pause Sound
    SMBXSound::PlaySFX(30);

    while (true)
    {
        // Read input...
        native_updateInput();

        KeyMap keymap = Player::Get(1)->keymap;

        // Process selected item input
        if ((menuItems[selectedOption] != nullptr) && menuItems[selectedOption]->ProcessInput(keymap, lastKeymap))
        {
            break;
        }

        if (keymap.downKeyState && !lastKeymap.downKeyState)
        {
            if (selectedOption + 1 < menuItems.size())
            {
                selectedOption++;
            }
            else
            {
                selectedOption = 0;
            }
            SMBXSound::PlaySFX(71);
        }
        if (keymap.upKeyState && !lastKeymap.upKeyState)
        {
            if (selectedOption > 0)
            {
                selectedOption--;
            }
            else
            {
                selectedOption = menuItems.size() - 1;
            }
            SMBXSound::PlaySFX(71);
        }
        lastKeymap = keymap;

        RenderRectOp* rect = new RenderRectOp();
        rect->x1 = menuX - 5;
        rect->y1 = menuY - 5;
        rect->x2 = menuX + menuW + 5;
        rect->y2 = menuY + menuH + 5;
        rect->fillColor = RenderOpColor(0.0f, 0.0f, 0.0f, 0.7f);
        rect->borderColor = RenderOpColor(0.5f, 0.0f, 0.0f, 1.0f);
        gLunaRender.AddOp(rect);


        gLunaRender.AddOp(new RenderStringOp(L"Level Testing Menu", 4, menuX, menuY));
        for (unsigned int i = 0; i < menuItems.size(); i++)
        {
            float yIdx = menuY + lineSpacing*(i + 1.5f);
            menuItems[i]->Render(menuX, yIdx, selectedOption == i);
        }

        // Render the frame and wait
        LunaDllRenderAndWaitFrame();

        // Exit pause if we're trying to switch
        if (GM_EPISODE_MODE) break;
    }
}
