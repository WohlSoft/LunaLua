#ifndef LUNA_GAME_CONTROLLER_H_
#define LUNA_GAME_CONTROLLER_H_

#include <SDL2/SDL.h>
#include <vector>
#include <tuple>
#include <unordered_map>

struct joyinfoex_tag;
class LunaGameControllerManager;

class LunaGameControllerPlayer
{
public:
    bool haveKeyboard;
    bool haveController;
    SDL_JoystickID joyId;
    unsigned int xAxis;
    unsigned int yAxis;
    unsigned int buttonState;
    // true between the controller disconnecting and a new input device being picked up
    bool controllerJustDisconnected;
public:
    inline LunaGameControllerPlayer() :
        haveKeyboard(false),
        haveController(false),
        controllerJustDisconnected(false),
        joyId(0),
        xAxis(0x7FFF),
        yAxis(0x7FFF),
        buttonState(0)
    {}
};

class LunaGameController
{
public:
    LunaGameController(LunaGameControllerManager* _managerPtr, SDL_JoystickID _joyId, SDL_Joystick* _joyPtr, SDL_GameController* _ctrlPtr);
    ~LunaGameController();

    LunaGameController(const LunaGameController&) = delete;
    LunaGameController& operator=(const LunaGameController&) = delete;
    LunaGameController(LunaGameController&&);
    LunaGameController& operator=(LunaGameController&&);

    void close();

    void joyButtonEvent(const SDL_JoyButtonEvent& event, bool down);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event, bool down);
    void joyAxisEvent(const SDL_JoyAxisEvent& event);
    void joyHatEvent(const SDL_JoyHatEvent& event);
    void controllerAxisEvent(const SDL_ControllerAxisEvent& event);

    inline std::string getName() const { return name; }
    inline bool isActive() const { return activeFlag; }
    inline void clearActive() { activeFlag = false; }
    inline unsigned int getPadState() const { return padState; }
    inline unsigned int getButtonState() const { return buttonState; }
    inline std::tuple<int, int> getStickPosition() const { return {xAxis, yAxis}; }

    SDL_JoystickPowerLevel getPowerLevel();

    void rumble(int ms, float strength);

    inline SDL_JoystickID getJoyId() { return joyId;  }
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

    enum ECtrlButtons {
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
        CONTROLLER_BUTTON_GUIDE = 12,
        CONTROLLER_BUTTON_DPAD_UP = 13,
        CONTROLLER_BUTTON_DPAD_DOWN = 14,
        CONTROLLER_BUTTON_DPAD_LEFT = 15,
        CONTROLLER_BUTTON_DPAD_RIGHT = 16,
        CONTROLLER_BUTTON_JOY0 = 17
    }; // Translated values from SDL_GameControllerButton

    static const int axisThresholdOn = (30 * 0x7FFF) / 100;
    static const int axisThresholdOff = (25 * 0x7FFF) / 100;
    static bool axisValueToState(int value, bool oldState);
private:
    LunaGameControllerManager* managerPtr;
    SDL_JoystickID joyId;
    SDL_Joystick* joyPtr;
    SDL_GameController* ctrlPtr;
    SDL_Haptic* hapticPtr;
    std::string name;
    unsigned int dirPadState;
    unsigned int axisPadState;
    unsigned int padState;
    unsigned int buttonState;
    int xAxis;
    int yAxis;
    bool activeFlag;
    std::vector<int> joyButtonMap;
};


class LunaGameControllerManager
{
public:
    const static int CONTROLLER_MAX_PLAYERS = 2;
private:
    bool initDone;
    std::unordered_map<SDL_JoystickID, LunaGameController> controllerMap;

    LunaGameControllerPlayer players[CONTROLLER_MAX_PLAYERS];

    std::vector<std::pair<SDL_JoystickID, int>> pressQueue;
    std::vector<std::pair<SDL_JoystickID, int>> releaseQueue;

    unsigned int reconnectTimeout;
    bool reconnectFlag;
public:
    LunaGameControllerManager();
    ~LunaGameControllerManager();
    LunaGameControllerManager(const LunaGameControllerManager&) = delete;
    LunaGameControllerManager& operator=(const LunaGameControllerManager&) = delete;
    void init();
    void pollInputs();
    void processSDLEvent(const SDL_Event& event);
#if !defined(BUILDING_SMBXLAUNCHER)
public:
    void handleInputs();
private:
    void handleInputsForPlayer(int playerNum);
public:
    unsigned int emulatedJoyGetPosEx(unsigned int uJoyID, struct joyinfoex_tag* pji);
    void notifyKeyboardPress(int keycode);
    void LunaGameControllerManager::sendSelectedController(const std::string& name, int playerNum, bool changeTriggeredByDisconnect);
#endif // !define(BUILDING_SMBXLAUNCHER)
public:
    SDL_JoystickPowerLevel getSelectedControllerPowerLevel(int playerNum);
    std::string getSelectedControllerName(int playerNum);
    std::tuple<int, int> getSelectedControllerStickPosition(int playerNum);
    void rumbleSelectedController(int playerNum, int ms, float strength);
    LunaGameController* getController(int playerNum);
private:
    void addJoystickEvent(int joyIdx);
    void removeJoystickEvent(SDL_JoystickID joyId);
    void joyButtonEvent(const SDL_JoyButtonEvent& event, bool down);
    void controllerButtonEvent(const SDL_ControllerButtonEvent& event, bool down);
    void joyAxisEvent(const SDL_JoyAxisEvent& event);
    void joyHatEvent(const SDL_JoyHatEvent& event);
    void controllerAxisEvent(const SDL_ControllerAxisEvent& event);
public:
    inline void storePressEvent(SDL_JoystickID joyId, int which) { pressQueue.emplace_back(joyId, which); }
    inline const std::vector<std::pair<SDL_JoystickID, int>>& getPressQueue() { return pressQueue; }
    inline void clearPressQueue() { pressQueue.clear(); }
    inline void storeReleaseEvent(SDL_JoystickID joyId, int which) { releaseQueue.emplace_back(joyId, which); }
    inline const std::vector<std::pair<SDL_JoystickID, int>>& getReleaseQueue() { return releaseQueue; }
    inline void clearReleaseQueue() { releaseQueue.clear(); }
    std::string getControllerName(SDL_JoystickID joyId);
};

#endif
