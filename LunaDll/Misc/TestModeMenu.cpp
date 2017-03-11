#include <map>

#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Sound.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/ImageLoader.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../Rendering/RenderOps/RenderRectOp.h"
#include "../Rendering/RenderOps/RenderSpriteOp.h"
#include "../SMBXInternal/CustomGraphics.h"

#include "TestMode.h"
#include "TestModeMenu.h"
#include "PGEEditorCmdSender.h"

static void sendTestSettingsToPgeEditor(STestModeSettings &settings)
{
    PGE_EditorCommandSender feedBack;
    std::string cmd = "CMD:testSetup:";
    cmd += std::to_string(0) + ",";
    cmd += std::to_string(settings.players[0].identity)     + ",";
    cmd += std::to_string(settings.players[0].powerup)      + ",";
    cmd += std::to_string(settings.players[0].mountType)    + ",";
    cmd += std::to_string(settings.players[0].mountColor);
    feedBack.sendCommandUTF8(cmd);
}


///////////////////////////////////////////////
//============ MENU ITEM CLASSES ============//
///////////////////////////////////////////////

class MenuItem
{
public:
    MenuItem() {}
    virtual ~MenuItem() {}
    virtual float Render(float x, float y, bool selected) = 0;
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
    virtual ~TextMenuItem() {}
    virtual std::wstring GetText() = 0;
    virtual float Render(float x, float y, bool selected)
    {
        if (selected && renderCounter < 20) {
            gLunaRender.AddOp(new RenderStringOp(L">", 4, x, y));
        }
        gLunaRender.AddOp(new RenderStringOp(GetText(), 4, x + 20, y));

        renderCounter = (renderCounter + 1) % 40;
        return 20.0f;
    }
};
class RunnableMenuItem : public virtual MenuItem
{
public:
    RunnableMenuItem() {}
    virtual ~RunnableMenuItem() {}
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
    {}
    virtual ~TextSelectorItem() {}
    virtual short GetSelection() = 0;
    virtual void ProcessSelection(short option) = 0;
    virtual float Render(float x, float y, bool selected)
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
        return 20.0f;
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
    virtual float Render(float x, float y, bool selected) { return TextMenuItem::Render(x, y, selected); }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) { return RunnableMenuItem::ProcessInput(keymap, lastKeymap); }
    virtual std::wstring GetText() { return L"Continue"; }
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
    virtual float Render(float x, float y, bool selected) { return TextMenuItem::Render(x, y, selected); }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) { return RunnableMenuItem::ProcessInput(keymap, lastKeymap); }
    virtual std::wstring GetText() { return L"Restart Level"; }
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
    virtual float Render(float x, float y, bool selected) { return TextMenuItem::Render(x, y, selected); }
    virtual bool ProcessInput(const KeyMap& keymap, const KeyMap& lastKeymap) { return RunnableMenuItem::ProcessInput(keymap, lastKeymap); }
    virtual std::wstring GetText() { return L"Quit"; }
    virtual bool Run() {
        // Exit
        if (!TestModeCheckHideWindow())
        {
            _exit(0);
        }
        return true;
    };
    static MenuItemQuit inst;
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
    virtual ~PowerupSelectorItem() {}
    virtual short GetSelection()
    {
        STestModeSettings settings = getTestModeSettings();
        return settings.players[0].powerup;
    }

    virtual void ProcessSelection(short option)
    {
        STestModeSettings settings = getTestModeSettings();
        settings.players[0].powerup = option;
        sendTestSettingsToPgeEditor(settings);
        setTestModeSettings(settings);
    }

    static PowerupSelectorItem player1Inst;
};

class CharacterSelectorItem : public TextSelectorItem
{
private:
    int mPlayerIdx;
public:
    CharacterSelectorItem(int playerIdx) :
        TextSelectorItem(),
        mPlayerIdx(playerIdx)
    {
        options[1] = L"";
        options[2] = L"";
        options[3] = L"";
        options[4] = L"";
        options[5] = L"";
    }
    virtual ~CharacterSelectorItem() {}
    virtual short GetSelection()
    {
        STestModeSettings settings = getTestModeSettings();
        return static_cast<short>(settings.players[mPlayerIdx].identity);
    }

    virtual void ProcessSelection(short option)
    {
        STestModeSettings settings = getTestModeSettings();
        settings.players[mPlayerIdx].identity = static_cast<Characters>(option);
        sendTestSettingsToPgeEditor(settings);
        setTestModeSettings(settings);
    }
    virtual float Render(float x, float y, bool selected)
    {
        STestModeSettings settings = getTestModeSettings();
        short charId = static_cast<short>(settings.players[mPlayerIdx].identity);
        short powerup = settings.players[mPlayerIdx].powerup;

        int offX = SMBX_CustomGraphics::getOffsetX(static_cast<Characters>(charId), 1, static_cast<PowerupID>(powerup));
        int offY = SMBX_CustomGraphics::getOffsetY(static_cast<Characters>(charId), 1, static_cast<PowerupID>(powerup));
        int w = SMBX_CustomGraphics::getPlayerHitboxWidth(static_cast<PowerupID>(powerup), static_cast<Characters>(charId));
        int h = SMBX_CustomGraphics::getPlayerHitboxHeight(static_cast<PowerupID>(powerup), static_cast<Characters>(charId));
        int padding = 6;

        std::shared_ptr<LunaImage> sprite = ImageLoader::GetCharacterSprite(charId, powerup);
        if (sprite != nullptr)
        {
            RenderSpriteOp* maskedRenderOp = new RenderSpriteOp();
            maskedRenderOp->sprite = sprite;
            maskedRenderOp->sx = x + 20 + offX + padding;
            maskedRenderOp->sy = y + offY + padding;
            maskedRenderOp->sw = 100;
            maskedRenderOp->sh = 100;
            maskedRenderOp->x = 500;
            maskedRenderOp->y = 0;
            maskedRenderOp->sceneCoords = false;
            gLunaRender.AddOp(maskedRenderOp);
        }

        if (selected && renderCounter < 20) {
            gLunaRender.AddOp(new RenderStringOp(L"<", 4, x, y + (h + padding * 2) / 2 - 10));
            gLunaRender.AddOp(new RenderStringOp(L">", 4, x + 20 + w + padding * 2, y + (h + padding * 2) / 2 - 10));
        }

        renderCounter = (renderCounter + 1) % 40;
        return static_cast<float>(h + padding * 2);
    }

    static CharacterSelectorItem player1Inst;
};

MenuItemContinue MenuItemContinue::inst;
MenuItemRestartLevel MenuItemRestartLevel::inst;
MenuItemQuit MenuItemQuit::inst;
CharacterSelectorItem CharacterSelectorItem::player1Inst(0);
PowerupSelectorItem PowerupSelectorItem::player1Inst;

//////////////////////////////////////////////
//========== MENU RUNNER FUNCTION ==========//
//////////////////////////////////////////////

void testModePauseMenu(bool allowContinue)
{
    gIsTestModePauseActive = true;
    KeyMap lastKeymap = gRawKeymap[0];

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
        short oldPauseOpen = GM_PAUSE_OPEN;
        GM_PAUSE_OPEN = COMBOOL(true);
        native_updateInput();
        GM_PAUSE_OPEN = oldPauseOpen;

        const KeyMap& keymap = gRawKeymap[0];

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
        rect->fillColor     = RenderOpColor(0.0f, 0.0f, 0.0f, 0.7f);
        rect->borderColor   = RenderOpColor(0.5f, 0.0f, 0.0f, 1.0f);
        gLunaRender.AddOp(rect);

        gLunaRender.AddOp(new RenderStringOp(L"Level Testing Menu", 4, menuX, menuY));
        float yIdx = menuY + lineSpacing*1.5f;
        for (unsigned int i = 0; i < menuItems.size(); i++)
        {
            yIdx += menuItems[i]->Render(menuX, yIdx, selectedOption == i);
        }

        // Render the frame and wait
        LunaDllRenderAndWaitFrame();

        // Exit pause if we're trying to switch
        if (GM_EPISODE_MODE) break;
    }
    gIsTestModePauseActive = false;
}
