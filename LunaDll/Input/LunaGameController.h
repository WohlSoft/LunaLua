#ifndef LUNA_GAME_CONTROLLER_H_
#define LUNA_GAME_CONTROLLER_H_

#include <SDL2/SDL.h>
#include <unordered_map>

struct joyinfoex_tag;
class LunaGameController;

class LunaGameControllerPlayer
{
public:
    bool haveController;
    SDL_JoystickID joyId;
    unsigned int xAxis;
    unsigned int yAxis;
    unsigned int buttonState;
public:
    inline LunaGameControllerPlayer() :
        haveController(false),
        joyId(0),
        xAxis(0x7FFF),
        yAxis(0x7FFF),
        buttonState(0)
    {}
};

class LunaGameControllerManager
{
private:
    const static int CONTROLLER_MAX_PLAYERS = 2;
private:
    bool initDone;
    std::unordered_map<SDL_JoystickID, LunaGameController> controllerMap;

    LunaGameControllerPlayer players[CONTROLLER_MAX_PLAYERS];
public:
    LunaGameControllerManager();
    ~LunaGameControllerManager();
    LunaGameControllerManager(const LunaGameControllerManager&) = delete;
    LunaGameControllerManager& operator=(const LunaGameControllerManager&) = delete;
    void init();
    void pollInputs();
    void processSDLEvent(const SDL_Event& event);
    void handleInputs();
private:
    void handleInputsForPlayer(int playerNum);
public:
    unsigned int emulatedJoyGetPosEx(unsigned int uJoyID, struct joyinfoex_tag* pji);
    void notifyKeyboardPress(int keycode);
    SDL_JoystickPowerLevel getSelectedControllerPowerLevel(int playerNum);
private:
    LunaGameController* getController(int playerNum);
    void LunaGameControllerManager::sendSelectedController(const std::string& name, int playerNum);
    void addJoystickEvent(int joyIdx);
    void removeJoystickEvent(SDL_JoystickID joyId);
    void joyButtonEvent(const SDL_JoyButtonEvent& event, bool down);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event, bool down);
    void joyAxisEvent(const SDL_JoyAxisEvent& event);
    void controllerAxisEvent(const SDL_ControllerAxisEvent& event);
};

class LunaGameController
{
public:
    LunaGameController(SDL_Joystick* _joyPtr, SDL_GameController* _ctrlPtr);
    ~LunaGameController();

    LunaGameController(const LunaGameController&) = delete;
    LunaGameController& operator=(const LunaGameController&) = delete;
    LunaGameController(LunaGameController&&);
    LunaGameController& operator=(LunaGameController&&);

    void close();

    void joyButtonEvent(const SDL_JoyButtonEvent& event, bool down);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event, bool down);
    void joyAxisEvent(const SDL_JoyAxisEvent& event);
    void controllerAxisEvent(const SDL_ControllerAxisEvent& event);

    inline std::string getName() const { return name; }
    inline bool isActive() const { return activeFlag; }
    inline void clearActive() { activeFlag = false; }
    inline unsigned int getPadState() const { return padState; }
    inline unsigned int getButtonState() const { return buttonState; }

    SDL_JoystickPowerLevel getPowerLevel();
private:
    void translateFromAxis(bool axisAsButton, bool axisAsDirectional, int posPadNumber, int negPadNumber, int value);
    void directionalEvent(int which, bool newState, bool fromAnalog);
    void buttonEvent(int which, bool newState);
public:
    enum EPadDir {
        CONTROLLER_PAD_UP = 0,
        CONTROLLER_PAD_DOWN,
        CONTROLLER_PAD_LEFT,
        CONTROLLER_PAD_RIGHT
    };

    enum EPadDirMask {
        CONTROLLER_PAD_UP_MASK    = (1UL << CONTROLLER_PAD_UP),
        CONTROLLER_PAD_DOWN_MASK  = (1UL << CONTROLLER_PAD_DOWN),
        CONTROLLER_PAD_LEFT_MASK  = (1UL << CONTROLLER_PAD_LEFT),
        CONTROLLER_PAD_RIGHT_MASK = (1UL << CONTROLLER_PAD_RIGHT)
    };

    enum ECtrlButtons{
        CONTROLLER_BUTTON_A = 0,
        CONTROLLER_BUTTON_B = 1,
        CONTROLLER_BUTTON_Y = 2,
        CONTROLLER_BUTTON_X = 3,
        CONTROLLER_BUTTON_LEFTSHOULDER = 4,
        CONTROLLER_BUTTON_RIGHTSHOULDER = 5,
        CONTROLLER_BUTTON_SELECT = 6,
        CONTROLLER_BUTTON_START  = 7,
        CONTROLLER_BUTTON_LEFTSTICK = 8,
        CONTROLLER_BUTTON_RIGHTSTICK = 9,
        CONTROLLER_BUTTON_TRIGGERLEFT  = 10,
        CONTROLLER_BUTTON_TRIGGERRIGHT = 11,
        CONTROLLER_BUTTON_GUIDE = 12
    }; // Translated values from SDL_GameControllerButton

    static const int axisThresholdOn = (30 * 0x7FFF) / 100;
    static const int axisThresholdOff = (25 * 0x7FFF) / 100;
    static bool axisValueToState(int value, bool oldState);
private:
    SDL_Joystick* joyPtr;
    SDL_GameController* ctrlPtr;
    SDL_Haptic* hapticPtr;
    std::string name;
    unsigned int dirPadState;
    unsigned int axisPadState;
    unsigned int padState;
    unsigned int buttonState;
    bool activeFlag;
};

#endif