#include <Windows.h>
#include <SDL2/SDL.h>
#include <memory>
#include "LunaGameController.h"
#include "../LuaMain/LunaLuaMain.h"
#include "../SMBXInternal/NativeInput.h"
#include "../Globals.h"

//#define CONTROLLER_DEBUG
#if defined(CONTROLLER_DEBUG)
    #include <stdio.h>
#endif

static JOYINFOEX test1;

// Constructor
LunaGameControllerManager::LunaGameControllerManager() :
    initDone(false),
    controllerMap(),
    players()
{
}

// Destructor
LunaGameControllerManager::~LunaGameControllerManager()
{
    if (initDone)
    {
        initDone = false;
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
    }
}

// Init function
void LunaGameControllerManager::init()
{
    // Avoid excess init
    if (initDone) return;

    // Init SDL subssytems
    int ret = SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
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
            joyButtonEvent(event.jbutton, true);
            break;
        case SDL_JOYBUTTONUP:
            joyButtonEvent(event.jbutton, false);
            break;
        case SDL_JOYAXISMOTION:
            joyAxisEvent(event.jaxis);
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            controllerButtonEvent(event.cbutton, true);
            break;
        case SDL_CONTROLLERBUTTONUP:
            controllerButtonEvent(event.cbutton, false);
            break;
        case SDL_CONTROLLERAXISMOTION:
            controllerAxisEvent(event.caxis);
            break;
    }
}

void LunaGameControllerManager::handleInputs()
{
    if (GM_PLAYERS_COUNT <= 1) {
        // For the case of 1-player, use a controll switching style where player 1 is always controlled by the most recently active control source

        // Get selected controller and whether it was active
        LunaGameController* selectedController = getController(1);
        bool wasSelectedActive = (selectedController != nullptr) ? selectedController->isActive() : selectedController;

        // Get active controller and clear flags
        LunaGameController* activeController = nullptr;
        SDL_JoystickID activeJoyId;
        for (std::pair<const SDL_JoystickID, LunaGameController>& it : controllerMap)
        {
            if (it.second.isActive())
            {
                it.second.clearActive();
                if (activeController == nullptr)
                {
                    activeJoyId = it.first;
                    activeController = &it.second;
                }
            }
        }

        // Handle controller switch, if the selected one wasn't active but now it is active
        if (!wasSelectedActive && (activeController != nullptr))
        {
            selectedController = activeController;
            players[0].joyId = activeJoyId;
            players[0].haveController = true;

            SMBXInput::setPlayerInputType(1, 1); // Set player 1 input type to 'joystick 1'
            #if defined(CONTROLLER_DEBUG)
                printf("Selected controller: %s\n", selectedController->getName().c_str());
            #endif

            sendSelectedController(selectedController->getName(), 1);
        }

        // Nothing for further players
        for (int playerNum = 2; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
        {
            players[playerNum - 1].haveController = false;
            players[playerNum - 1].joyId = 0;
        }
    }
    else
    {
        // For 2+(?) players up to player count
        for (int playerNum = 1; (playerNum <= CONTROLLER_MAX_PLAYERS) && (playerNum <= GM_PLAYERS_COUNT); playerNum++)
        {
            LunaGameController* selectedController = getController(playerNum);

            // If already have a controller, we're done
            if (selectedController != nullptr) continue;

            // Check if we have an active controller that is not selected already
            SDL_JoystickID newJoyId;
            for (std::pair<const SDL_JoystickID, LunaGameController>& it : controllerMap)
            {
                if (it.second.isActive())
                {
                    it.second.clearActive();
                    bool alreadyUsed = false;
                    for (int otherPlayerNum = 1; (otherPlayerNum <= CONTROLLER_MAX_PLAYERS) && (otherPlayerNum <= GM_PLAYERS_COUNT); otherPlayerNum++)
                    {
                        if (otherPlayerNum == playerNum) continue;
                        if (players[otherPlayerNum - 1].haveController && players[otherPlayerNum - 1].joyId == it.first)
                        {
                            alreadyUsed = true;
                            break;
                        }
                    }

                    if (!alreadyUsed)
                    {
                        newJoyId = it.first;
                        selectedController = &it.second;
                        break;
                    }
                }
            }

            // If we got a new selected controller
            if (selectedController != nullptr)
            {
                players[playerNum - 1].joyId = newJoyId;
                players[playerNum - 1].haveController = true;

                SMBXInput::setPlayerInputType(playerNum, playerNum); // Set player n input type to 'joystick n'
                #if defined(CONTROLLER_DEBUG)
                    printf("Selected controller: %s\n", selectedController->getName().c_str());
                #endif

                sendSelectedController(selectedController->getName(), playerNum);
            }
        }

        // Clear stray activity flags
        for (std::pair<const SDL_JoystickID, LunaGameController>& it : controllerMap)
        {
            if (it.second.isActive())
            {
                it.second.clearActive();
            }
        }

        // Nothing for further(?) players
        for (int playerNum = GM_PLAYERS_COUNT + 1; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
        {
            players[playerNum - 1].haveController = false;
            players[playerNum - 1].joyId = 0;
        }
    }

    // Update controller state for each player
    for (int playerNum = 1; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
    {
        handleInputsForPlayer(playerNum);
    }
}

// Function to process inputs for a player
void LunaGameControllerManager::handleInputsForPlayer(int playerNum)
{
    // Sanity check
    if ((playerNum < 1) || (playerNum > CONTROLLER_MAX_PLAYERS))
    {
        return;
    }

    // Get and player controller
    LunaGameControllerPlayer& player = players[playerNum - 1];
    LunaGameController* controller = getController(playerNum);

    // No selected controller? Nothing to do
    if (controller == nullptr)
    {
        if (SMBXInput::getPlayerInputType(playerNum) != 0)
        {
            SMBXInput::setPlayerInputType(playerNum, 0); // Set player 1 input type to 'keyboard'
            #if defined(CONTROLLER_DEBUG)
                printf("Selected controller: Keyboard\n");
            #endif
            sendSelectedController("Keyboard", playerNum);
        }
        return;
    }


    // Convert pad state to axis for later emulatedJoyGetPosEx calls
    unsigned int padState = controller->getPadState();
    if ((padState & (LunaGameController::CONTROLLER_PAD_UP_MASK | LunaGameController::CONTROLLER_PAD_DOWN_MASK)) == LunaGameController::CONTROLLER_PAD_UP_MASK)
    {
        player.yAxis = 0x0000; // Up
    }
    else if ((padState & (LunaGameController::CONTROLLER_PAD_UP_MASK | LunaGameController::CONTROLLER_PAD_DOWN_MASK)) == LunaGameController::CONTROLLER_PAD_DOWN_MASK)
    {
        player.yAxis = 0xFFFF; // Down
    }
    else
    {
        player.yAxis = 0x7FFF; // Neutral
    }

    if ((padState & (LunaGameController::CONTROLLER_PAD_LEFT_MASK | LunaGameController::CONTROLLER_PAD_RIGHT_MASK)) == LunaGameController::CONTROLLER_PAD_LEFT_MASK)
    {
        player.xAxis = 0x0000; // Left
    }
    else if ((padState & (LunaGameController::CONTROLLER_PAD_LEFT_MASK | LunaGameController::CONTROLLER_PAD_RIGHT_MASK)) == LunaGameController::CONTROLLER_PAD_RIGHT_MASK)
    {
        player.xAxis = 0xFFFF; // Right
    }
    else
    {
        player.xAxis = 0x7FFF; // Neutral
    }

    // Copy button state
    player.buttonState = controller->getButtonState() & 0xFFFF;
}

// Function to emulate joyGetPosEx
unsigned int LunaGameControllerManager::emulatedJoyGetPosEx(unsigned int uJoyID, struct joyinfoex_tag* pji)
{
    // Sanity checks
    if (pji == nullptr) return 0;
    if (pji->dwSize < sizeof(JOYINFOEX)) return 0;

    // For our purposes, we ignore dwFlags. We know what we need to fill in.

    // Get player number from uJoyID
    int playerNum = uJoyID + 1;

    if ((playerNum >= 1) && (playerNum <= CONTROLLER_MAX_PLAYERS))
    {
        LunaGameControllerPlayer& player = players[playerNum - 1];
        pji->dwXpos = player.xAxis;
        pji->dwYpos = player.yAxis;
        pji->dwPOV = 0x7FFF;
        pji->dwButtons = player.buttonState;
        pji->dwButtonNumber = __popcnt(player.buttonState);
    }
    else
    {
        pji->dwXpos = 0x7FFF;
        pji->dwYpos = 0x7FFF;
        pji->dwPOV = 0x7FFF;
        pji->dwButtons = 0;
        pji->dwButtonNumber = 0;
    }

    return 0;
}

void LunaGameControllerManager::notifyKeyboardPress(int keycode)
{
    // Return if init not done
    if (!initDone)
    {
        return;
    }

    for (int playerNum = 1; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
    {
        // If the selected input type is not keyboard, maybe switch to keyboard
        SMBXNativeKeyboard* keyboardConfig = SMBXNativeKeyboard::Get(playerNum);
        bool isConfiguredKey = (
            (keycode == keyboardConfig->up) ||
            (keycode == keyboardConfig->down) ||
            (keycode == keyboardConfig->left) ||
            (keycode == keyboardConfig->right) ||
            (keycode == keyboardConfig->jump) ||
            (keycode == keyboardConfig->run) ||
            (keycode == keyboardConfig->dropitem) ||
            (keycode == keyboardConfig->pause) ||
            (keycode == keyboardConfig->altjump) ||
            (keycode == keyboardConfig->altrun)
            );

        // If the key that is pressed is configured as an input, switch to keyboard control
        if (isConfiguredKey)
        {
            if (SMBXInput::getPlayerInputType(playerNum) != 0)
            {
                // Clear selected flag if set
                players[playerNum - 1].haveController = false;

                SMBXInput::setPlayerInputType(playerNum, 0); // Set player 1 input type to 'keyboard'
                #if defined(CONTROLLER_DEBUG)
                    printf("Selected controller: Keyboard\n");
                #endif
                sendSelectedController("Keyboard", playerNum);
            }

            // Don't consider this input in switching player 2 if it's set for player 1
            break;
        }
    }
}

SDL_JoystickPowerLevel LunaGameControllerManager::getSelectedControllerPowerLevel(int playerNum)
{
    LunaGameController* controller = getController(playerNum);
    if (controller != nullptr)
    {
        return controller->getPowerLevel();
    }
    return SDL_JOYSTICK_POWER_UNKNOWN;
}

LunaGameController* LunaGameControllerManager::getController(int playerNum)
{
    if ((playerNum >= 1) && (playerNum <= CONTROLLER_MAX_PLAYERS) && players[playerNum - 1].haveController)
    {
        auto& it = controllerMap.find(players[playerNum - 1].joyId);
        if (it != controllerMap.end())
        {
            return &it->second;
        }
    }
    return nullptr;
}

void LunaGameControllerManager::sendSelectedController(const std::string& name, int playerNum)
{
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> changeControllerEvent = std::make_shared<Event>("onChangeController", false);
        changeControllerEvent->setDirectEventName("onChangeController");
        changeControllerEvent->setLoopable(false);
        gLunaLua.callEvent(changeControllerEvent, name, playerNum);
    }
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

    controllerMap.emplace(joyId, LunaGameController(joyPtr, ctrlPtr));
}

void LunaGameControllerManager::removeJoystickEvent(SDL_JoystickID joyId)
{
    // Unassign if assigned
    for (int playerNum = 1; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
    {
        if (players[playerNum - 1].haveController && (players[playerNum - 1].joyId == joyId))
        {
            players[playerNum - 1].haveController = false;
            players[playerNum - 1].joyId = 0;
        }
    }

    // Delete joystick
    auto it = controllerMap.find(joyId);
    if (it != controllerMap.end())
    {
        controllerMap.erase(it);
    }
}

void LunaGameControllerManager::joyButtonEvent(const SDL_JoyButtonEvent& event, bool down)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.joyButtonEvent(event, down);
    }
}

void LunaGameControllerManager::controllerButtonEvent(const SDL_ControllerButtonEvent& event, bool down)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.controllerButtonEvent(event, down);
    }
}

void LunaGameControllerManager::joyAxisEvent(const SDL_JoyAxisEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.joyAxisEvent(event);
    }
}

void LunaGameControllerManager::controllerAxisEvent(const SDL_ControllerAxisEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.controllerAxisEvent(event);
    }
}

//=================================================================================================

LunaGameController::LunaGameController(SDL_Joystick* _joyPtr, SDL_GameController* _ctrlPtr) :
    joyPtr(_joyPtr),
    ctrlPtr(_ctrlPtr),
    hapticPtr(nullptr),
    name(),
    dirPadState(0),
    axisPadState(0),
    padState(0),
    buttonState(0),
    activeFlag(false)
{
    // Open for haptic feedback if possible, because at least checking sometimes this helps keep a controller awake (really?)
    if (joyPtr && SDL_JoystickIsHaptic(joyPtr))
    {
        hapticPtr = SDL_HapticOpenFromJoystick(joyPtr);
    }

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

    close();
}

// Move constructors
LunaGameController::LunaGameController(LunaGameController &&other)
{
    joyPtr          = other.joyPtr;
    ctrlPtr         = other.ctrlPtr;
    hapticPtr       = other.hapticPtr;
    name            = other.name;
    dirPadState     = other.dirPadState;
    axisPadState    = other.axisPadState;
    padState        = other.padState;
    buttonState     = other.buttonState;
    activeFlag      = other.activeFlag;
    other.joyPtr    = nullptr;
    other.ctrlPtr   = nullptr;
    other.hapticPtr = nullptr;
}

// Move assignment
LunaGameController & LunaGameController::operator=(LunaGameController &&other)
{
    close();

    joyPtr          = other.joyPtr;
    ctrlPtr         = other.ctrlPtr;
    hapticPtr       = other.hapticPtr;
    name            = other.name;
    dirPadState     = other.dirPadState;
    axisPadState    = other.axisPadState;
    padState        = other.padState;
    buttonState     = other.buttonState;
    activeFlag      = other.activeFlag;
    other.joyPtr    = nullptr;
    other.ctrlPtr   = nullptr;
    other.hapticPtr = nullptr;

    return *this;
}

void LunaGameController::close()
{
    if (hapticPtr)
    {
        SDL_HapticClose(hapticPtr);
        hapticPtr = nullptr;
    }

    if (joyPtr)
    {
        SDL_JoystickClose(joyPtr);
        joyPtr = nullptr;
    }

    if (ctrlPtr)
    {
        SDL_GameControllerClose(ctrlPtr);
        ctrlPtr = nullptr;
    }
}

void LunaGameController::joyButtonEvent(const SDL_JoyButtonEvent& event, bool down)
{
    // Ignore if this is not a joy-only device
    if ((joyPtr == nullptr) || (ctrlPtr != nullptr)) return;

    #if defined(CONTROLLER_DEBUG_LOWLEVEL)
        printf("JoyButton%s %s, %d\n", down ? "Down" : "Up", name.c_str(), (int)event.button);
    #endif
    
    if (event.button < 32)
    {
        buttonEvent(event.button, down);
    }
}

void LunaGameController::controllerButtonEvent(const SDL_ControllerButtonEvent& event, bool down)
{
    // Ignore if this is not a controller device
    if (ctrlPtr == nullptr) return;

    #if defined(CONTROLLER_DEBUG_LOWLEVEL)
        printf("ControllerButton%s %s, %d\n", down ? "Down" : "Up", name.c_str(), (int)event.button);
    #endif

    switch (event.button)
    {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        directionalEvent(CONTROLLER_PAD_UP, down, false);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        directionalEvent(CONTROLLER_PAD_DOWN, down, false);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        directionalEvent(CONTROLLER_PAD_LEFT, down, false);
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        directionalEvent(CONTROLLER_PAD_RIGHT, down, false);
        break;
    case SDL_CONTROLLER_BUTTON_A:
        buttonEvent(CONTROLLER_BUTTON_A, down);
        break;
    case SDL_CONTROLLER_BUTTON_B:
        buttonEvent(CONTROLLER_BUTTON_B, down);
        break;
    case SDL_CONTROLLER_BUTTON_Y:
        buttonEvent(CONTROLLER_BUTTON_Y, down);
        break;
    case SDL_CONTROLLER_BUTTON_X:
        buttonEvent(CONTROLLER_BUTTON_X, down);
        break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        buttonEvent(CONTROLLER_BUTTON_LEFTSHOULDER, down);
        break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        buttonEvent(CONTROLLER_BUTTON_RIGHTSHOULDER, down);
        break;
    case SDL_CONTROLLER_BUTTON_BACK:
        buttonEvent(CONTROLLER_BUTTON_SELECT, down);
        break;
    case SDL_CONTROLLER_BUTTON_START:
        buttonEvent(CONTROLLER_BUTTON_START, down);
        break;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        buttonEvent(CONTROLLER_BUTTON_LEFTSTICK, down);
        break;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        buttonEvent(CONTROLLER_BUTTON_RIGHTSTICK, down);
        break;
    case SDL_CONTROLLER_BUTTON_GUIDE:
        buttonEvent(CONTROLLER_BUTTON_GUIDE, down);
        break;
    default:
        break;
    }
}


void LunaGameController::joyAxisEvent(const SDL_JoyAxisEvent& event)
{
    // Ignore if this is not a joy-only device
    if ((joyPtr == nullptr) || (ctrlPtr != nullptr)) return;

    #if defined(CONTROLLER_DEBUG_LOWLEVEL)
        printf("JoyAxis %s, %d, %d\n", name.c_str(), (int)event.axis, (int)event.value);
    #endif

    bool axisAsDirectional = false;
    int posPadNumber = -1;
    int negPadNumber = -1;
    switch (event.axis)
    {
    case 0:
        axisAsDirectional = true;
        posPadNumber = CONTROLLER_PAD_RIGHT;
        negPadNumber = CONTROLLER_PAD_LEFT;
        break;
    case 1:
        axisAsDirectional = true;
        posPadNumber = CONTROLLER_PAD_DOWN;
        negPadNumber = CONTROLLER_PAD_UP;
        break;
    default:
        break;
    }

    // Translate axis to directional
    translateFromAxis(false, axisAsDirectional, posPadNumber, negPadNumber, event.value);
}

void LunaGameController::controllerAxisEvent(const SDL_ControllerAxisEvent& event)
{
    // Ignore if this is not a controller device
    if (ctrlPtr == nullptr) return;

    #if defined(CONTROLLER_DEBUG_LOWLEVEL)
        printf("ControllerAxis %s, %d, %d\n", name.c_str(), (int)event.axis, (int)event.value);
    #endif

    // Translation of axis to buttons or directional pads
    bool axisAsButton = false;
    bool axisAsDirectional = false;
    int posPadNumber = -1;
    int negPadNumber = -1;
    switch (event.axis)
    {
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        axisAsButton = true;
        posPadNumber = CONTROLLER_BUTTON_TRIGGERLEFT;
        break;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        axisAsButton = true;
        posPadNumber = CONTROLLER_BUTTON_TRIGGERRIGHT;
        break;
    case SDL_CONTROLLER_AXIS_LEFTX:
        axisAsDirectional = true;
        posPadNumber = CONTROLLER_PAD_RIGHT;
        negPadNumber = CONTROLLER_PAD_LEFT;
        break;
    case SDL_CONTROLLER_AXIS_LEFTY:
        axisAsDirectional = true;
        posPadNumber = CONTROLLER_PAD_DOWN;
        negPadNumber = CONTROLLER_PAD_UP;
        break;
    default:
        break;
    }

    // Translate axis to button or directional
    translateFromAxis(axisAsButton, axisAsDirectional, posPadNumber, negPadNumber, event.value);
}

void LunaGameController::translateFromAxis(bool axisAsButton, bool axisAsDirectional, int posPadNumber, int negPadNumber, int value)
{
    unsigned int* maskPtr = nullptr;
    if (axisAsButton)
    {
        maskPtr = &buttonState;
    }
    else if (axisAsDirectional)
    {
        maskPtr = &axisPadState;
    }
    else
    {
        // Ignored axis
        return;
    }

    // Get value
    if (posPadNumber != -1)
    {
        bool oldState = (*maskPtr & (1UL << posPadNumber)) != 0;
        bool newState = axisValueToState(value, oldState);
        if (oldState != newState)
        {
            if (axisAsDirectional)
            {
                directionalEvent(posPadNumber, newState, true);
            }
            else if (axisAsButton)
            {
                buttonEvent(posPadNumber, newState);
            }
        }
    }
    if (negPadNumber != -1)
    {
        bool oldState = (*maskPtr & (1UL << negPadNumber)) != 0;
        bool newState = axisValueToState(-value, oldState);
        if (oldState != newState)
        {
            if (axisAsDirectional)
            {
                directionalEvent(negPadNumber, newState, true);
            }
            else if (axisAsButton)
            {
                buttonEvent(negPadNumber, newState);
            }
        }
    }
}

SDL_JoystickPowerLevel LunaGameController::getPowerLevel()
{
    if (joyPtr)
    {
        return SDL_JoystickCurrentPowerLevel(joyPtr);
    }
    return SDL_JOYSTICK_POWER_UNKNOWN;
}

void LunaGameController::directionalEvent(int which, bool newState, bool fromAnalog)
{
    unsigned int* maskPtr = fromAnalog ? &axisPadState : &dirPadState;
    if (newState)
    {
        *maskPtr |= (1UL << which);
    }
    else
    {
        *maskPtr &= 0xFFFFFFFF ^ (1UL << which);
    }

    padState = *maskPtr; // Assign current source of pad state
    activeFlag = activeFlag || newState;
}

void LunaGameController::buttonEvent(int which, bool newState)
{
    if (newState)
    {
        buttonState |= (1UL << which);
    }
    else
    {
        buttonState &= 0xFFFFFFFF ^ (1UL << which);
    }

    activeFlag = activeFlag || newState;
}

/*static*/ bool LunaGameController::axisValueToState(int value, bool oldState)
{
    if (oldState)
    {
        // Already pressed? Use threshold for turning off
        return value >= axisThresholdOff;
    }
    else
    {
        // Not pressed? Use threshold for turning on
        return value >= axisThresholdOn;
    }
}
