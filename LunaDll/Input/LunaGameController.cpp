#include <Windows.h>
#include <SDL2/SDL.h>
#include "LunaGameController.h"

#include <stdio.h>

#define CONTROLLER_DEBUG

static JOYINFOEX test1;

// Constructor
LunaGameControllerManager::LunaGameControllerManager() :
    initDone(false)
{
}

// Destructor
LunaGameControllerManager::~LunaGameControllerManager()
{
    if (initDone)
    {
        initDone = false;
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
    }
}

// Init function
void LunaGameControllerManager::init()
{
    // Avoid excess init
    if (initDone) return;

    // Init SDL subssytems
    int ret = SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
    if (ret != 0)
    {
        // Error
        return;
    }

    // Flag init done
    initDone = true;

    int numJoy = SDL_NumJoysticks();
    for (int i = 0; i < numJoy; i++)
    {
        addJoystickEvent(i);
    }
}

// Function to poll inputs
void LunaGameControllerManager::pollInputs()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processSDLEvent(event);
    }
    handleInputs();
}

// Function to process an SDL event that is incoming
void LunaGameControllerManager::processSDLEvent(const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_JOYDEVICEADDED:
            addJoystickEvent(event.jdevice.which);
            break;
        case SDL_JOYDEVICEREMOVED:
            removeJoystickEvent(event.jdevice.which);
            break;
        case SDL_JOYBUTTONDOWN:
            joyButtonDownEvent(event.jbutton);
            break;
        case SDL_JOYBUTTONUP:
            joyButtonUpEvent(event.jbutton);
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            controllerButtonDownEvent(event.cbutton);
            break;
        case SDL_CONTROLLERBUTTONUP:
            controllerButtonUpEvent(event.cbutton);
            break;
    }
}

void LunaGameControllerManager::handleInputs()
{

}

// Function to emulate joyGetPosEx
unsigned int LunaGameControllerManager::emulatedJoyGetPosEx(unsigned int uJoyID, struct joyinfoex_tag* pji)
{
    // Sanity checks
    if (pji == nullptr) return 0;
    if (pji->dwSize < sizeof(JOYINFOEX)) return 0;

    // For our purposes, we ignore dwFlags. We know what we need to fill in.

    return 0;
}

void LunaGameControllerManager::addJoystickEvent(int joyIdx)
{
    // Process adding new device
    SDL_Joystick* joyPtr = nullptr;
    SDL_GameController* ctrlPtr = nullptr;

    // Open as joystick
    joyPtr = SDL_JoystickOpen(joyIdx);
    if (joyPtr == nullptr)
    {
        // Failure to open joystick
        return;
    }

    // Get joystick ID
    SDL_JoystickID joyId = SDL_JoystickInstanceID(joyPtr);
    if ((joyId < 0) || (controllerMap.find(joyId) != controllerMap.end()))
    {
        // Failure to get ID or duplicate
        SDL_JoystickClose(joyPtr);
        return;
    }

    // Open as game controller
    if (SDL_IsGameController(joyIdx))
    {
        ctrlPtr = SDL_GameControllerOpen(joyIdx);
    }

    printf("joyid: %d ", joyId);
    controllerMap.emplace(joyId, LunaGameController(joyPtr, ctrlPtr));
    printf("\n");
}

void LunaGameControllerManager::removeJoystickEvent(SDL_JoystickID joyId)
{
    printf("joyid: %d ", joyId);
    auto it = controllerMap.find(joyId);
    if (it != controllerMap.end())
    {
        controllerMap.erase(it);
    }
    printf("\n");
}

void LunaGameControllerManager::joyButtonDownEvent(const SDL_JoyButtonEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.joyButtonDownEvent(event);
    }
}

void LunaGameControllerManager::joyButtonUpEvent(const SDL_JoyButtonEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.joyButtonUpEvent(event);
    }
}

void LunaGameControllerManager::controllerButtonDownEvent(const SDL_ControllerButtonEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.controllerButtonDownEvent(event);
    }
}

void LunaGameControllerManager::controllerButtonUpEvent(const SDL_ControllerButtonEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.controllerButtonUpEvent(event);
    }
}


//=================================================================================================

LunaGameController::LunaGameController(SDL_Joystick* _joyPtr, SDL_GameController* _ctrlPtr) :
    joyPtr(_joyPtr),
    ctrlPtr(_ctrlPtr),
    name()
{
    // Get controller name
    const char* namePtr = nullptr;
    if (ctrlPtr)
    {
        namePtr = SDL_GameControllerName(ctrlPtr);
    }
    if (joyPtr && (namePtr == nullptr))
    {
        namePtr = SDL_JoystickName(joyPtr);
    }
    if (namePtr == nullptr)
    {
        namePtr = "<NULL>";
    }
    name = namePtr;

    #if defined(CONTROLLER_DEBUG)
        printf("Added controller: %s\n", name.c_str());
    #endif
}

LunaGameController::~LunaGameController()
{
    #if defined(CONTROLLER_DEBUG)
        if (joyPtr || ctrlPtr) printf("Removed controller: %s\n", name.c_str());
    #endif

    if (joyPtr)
    {
        SDL_JoystickClose(joyPtr);
        ctrlPtr = nullptr;
    }

    if (ctrlPtr)
    {
        SDL_GameControllerClose(ctrlPtr);
        ctrlPtr = nullptr;
    }
}

// Move constructors
LunaGameController::LunaGameController(LunaGameController &&other) :
    joyPtr(other.joyPtr),
    ctrlPtr(other.ctrlPtr),
    name(other.name)
{
    other.joyPtr = nullptr;
    other.ctrlPtr = nullptr;
}

// Move assignment
LunaGameController & LunaGameController::operator=(LunaGameController &&other)
{
    if (joyPtr)
    {
        SDL_JoystickClose(joyPtr);
        ctrlPtr = nullptr;
    }

    if (ctrlPtr)
    {
        SDL_GameControllerClose(ctrlPtr);
        ctrlPtr = nullptr;
    }

    joyPtr = other.joyPtr;
    ctrlPtr = other.ctrlPtr;
    name = other.name;
    other.joyPtr = nullptr;
    other.ctrlPtr = nullptr;

    return *this;
}

void LunaGameController::joyButtonDownEvent(const SDL_JoyButtonEvent& event)
{
    // Ignore if this is not a joy-only device
    if ((joyPtr == nullptr) || (ctrlPtr != nullptr)) return;

    #if defined(CONTROLLER_DEBUG)
        printf("JoyButtonDown %s, %d\n", name.c_str(), (int)event.button);
    #endif
}

void LunaGameController::joyButtonUpEvent(const SDL_JoyButtonEvent& event)
{
    // Ignore if this is not a joy-only device
    if ((joyPtr == nullptr) || (ctrlPtr != nullptr)) return;
}

void LunaGameController::controllerButtonDownEvent(const SDL_ControllerButtonEvent& event)
{
    // Ignore if this is not a controller device
    if (ctrlPtr == nullptr) return;

    #if defined(CONTROLLER_DEBUG)
        printf("ControllerButtonDown %s, %d\n", name.c_str(), (int)event.button);
    #endif
}

void LunaGameController::controllerButtonUpEvent(const SDL_ControllerButtonEvent& event)
{
    // Ignore if this is not a controller device
    if (ctrlPtr == nullptr) return;
}
