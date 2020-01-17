#ifndef LUNA_GAME_CONTROLLER_H_
#define LUNA_GAME_CONTROLLER_H_

#include <SDL2/SDL.h>
#include <unordered_map>

struct joyinfoex_tag;

class LunaGameController
{
public:
    LunaGameController(SDL_Joystick* _joyPtr, SDL_GameController* _ctrlPtr);
    ~LunaGameController();

    LunaGameController(const LunaGameController&) = delete;
    LunaGameController& operator=(const LunaGameController&) = delete;
    LunaGameController(LunaGameController&&);
    LunaGameController& operator=(LunaGameController&&);

    void joyButtonDownEvent(const SDL_JoyButtonEvent& event);
    void joyButtonUpEvent(const SDL_JoyButtonEvent& event);
    void controllerButtonDownEvent(const SDL_ControllerButtonEvent& event);
    void controllerButtonUpEvent(const SDL_ControllerButtonEvent& event);
private:
    SDL_Joystick* joyPtr;
    SDL_GameController* ctrlPtr;
    std::string name;
};

class LunaGameControllerManager
{
private:
    bool initDone;
    std::unordered_map<SDL_JoystickID, LunaGameController> controllerMap;
public:
    LunaGameControllerManager();
    ~LunaGameControllerManager();
    LunaGameControllerManager(const LunaGameControllerManager&) = delete;
    LunaGameControllerManager& operator=(const LunaGameControllerManager&) = delete;
    void init();
    void pollInputs();
    void processSDLEvent(const SDL_Event& event);
    void handleInputs();
    unsigned int emulatedJoyGetPosEx(unsigned int uJoyID, struct joyinfoex_tag* pji);
private:
    void addJoystickEvent(int joyIdx);
    void removeJoystickEvent(SDL_JoystickID joyId);
    void joyButtonDownEvent(const SDL_JoyButtonEvent& event);
    void joyButtonUpEvent(const SDL_JoyButtonEvent& event);
    void controllerButtonDownEvent(const SDL_ControllerButtonEvent& event);
    void controllerButtonUpEvent(const SDL_ControllerButtonEvent& event);
};

#endif