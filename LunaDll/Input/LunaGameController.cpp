#include <SDL2/SDL.h>
#include <memory>
#include <tuple>
#include "LunaGameController.h"
#if !defined(BUILDING_SMBXLAUNCHER)
#   ifdef _WIN32
#       include <windows.h>
#   endif
#   include "../LuaMain/LunaLuaMain.h"
#   include "../SMBXInternal/NativeInput.h"
#   include "../Globals.h"
#endif

//#define CONTROLLER_DEBUG
#if defined(CONTROLLER_DEBUG)
    #include <stdio.h>
#endif

// Constructor
LunaGameControllerManager::LunaGameControllerManager() :
    initDone(false),
    controllerMap(),
    players(),
    pressQueue(),
    releaseQueue(),
    reconnectTimeout(0),
    reconnectFlag(false)
{
}

// Destructor
LunaGameControllerManager::~LunaGameControllerManager()
{
    if (initDone)
    {
        initDone = false;
        controllerMap.clear();
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
    }
}

// Init function
void LunaGameControllerManager::init()
{
    // Avoid excess init
    if (initDone) return;

    // Set hints
    SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1", SDL_HINT_OVERRIDE);

    // Init SDL subssytems
    int ret = SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
    if (ret != 0)
    {
        // Error
        return;
    }

    // Attempt to load extra controller mappings
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

    // Enable events
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);

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
    // If we needed to connect things in a deferred fashion, handle that
    if (reconnectFlag && (SDL_GetTicks() >= reconnectTimeout))
    {
        reconnectFlag = false;
        int numJoy = SDL_NumJoysticks();
        for (int i = 0; i < numJoy; i++)
        {
            addJoystickEvent(i);
        }
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processSDLEvent(event);
    }

    #if !defined(BUILDING_SMBXLAUNCHER)
        handleInputs();
    #endif // !defined(BUILDING_SMBXLAUNCHER)
}

// Function to process an SDL event that is incoming
void LunaGameControllerManager::processSDLEvent(const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_JOYDEVICEADDED:
        {
            const char* joyName = SDL_JoystickNameForIndex(event.jdevice.which);
            if (joyName && (strcmp(joyName, "Nintendo Switch Pro Controller") == 0))
            {
                reconnectTimeout = SDL_GetTicks() + 4000;
                reconnectFlag = true;
                break;
            }
            addJoystickEvent(event.jdevice.which);
            break;
        }
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
        case SDL_JOYHATMOTION:
            joyHatEvent(event.jhat);
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


#if !defined(BUILDING_SMBXLAUNCHER)
void LunaGameControllerManager::handleInputs()
{
    int playerCount = GM_PLAYERS_COUNT;

    if (playerCount <= 1) {
        // For the case of 1-player, use a controll switching style where player 1 is always controlled by the most recently active control source

        // Get selected controller and whether it was active
        LunaGameController* selectedController = getController(1);
        bool wasSelectedActive = (selectedController != nullptr) ? selectedController->isActive() : false;

        // Get active controller and clear flags
        LunaGameController* activeController = nullptr;
        for (std::pair<const SDL_JoystickID, LunaGameController>& it : controllerMap)
        {
            if (it.second.isActive())
            {
                it.second.clearActive();
                if (activeController == nullptr)
                {
                    activeController = &it.second;
                }
            }
        }

        // Handle controller switch, if the selected one wasn't active but now it is active
        if (!wasSelectedActive && (activeController != nullptr))
        {
            selectedController = activeController;
            players[0].joyId = selectedController->getJoyId();
            players[0].haveKeyboard = false;
            players[0].haveController = true;

            SMBXInput::setPlayerInputType(1, 1); // Set player 1 input type to 'joystick 1'
            #if defined(CONTROLLER_DEBUG)
                printf("Selected controller: %s\n", selectedController->getName().c_str());
            #endif

            sendSelectedController(selectedController->getName(), 1, false);
        }

        // Nothing for further players
        for (int playerNum = 2; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
        {
            players[playerNum - 1].haveKeyboard = false;
            players[playerNum - 1].haveController = false;
            players[playerNum - 1].joyId = 0;
        }
    }
    else
    {
        // For 2+(?) players up to player count
        for (int playerNum = 1; (playerNum <= CONTROLLER_MAX_PLAYERS) && (playerNum <= playerCount); playerNum++)
        {
            LunaGameController* selectedController = getController(playerNum);

            // If already have a controller, we're done
            if (selectedController != nullptr) continue;

            // If we're already locked to keyboard, we're done
            if (players[playerNum - 1].haveKeyboard) continue;

            // Check if we have an active controller that is not selected already
            for (std::pair<const SDL_JoystickID, LunaGameController>& it : controllerMap)
            {
                if (it.second.isActive())
                {
                    it.second.clearActive();
                    bool alreadyUsed = false;
                    for (int otherPlayerNum = 1; (otherPlayerNum <= CONTROLLER_MAX_PLAYERS) && (otherPlayerNum <= playerCount); otherPlayerNum++)
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
                        selectedController = &it.second;
                        break;
                    }
                }
            }

            // If we got a new selected controller
            if (selectedController != nullptr)
            {
                players[playerNum - 1].joyId = selectedController->getJoyId();
                players[playerNum - 1].haveKeyboard = false;
                players[playerNum - 1].haveController = true;

                SMBXInput::setPlayerInputType(playerNum, playerNum); // Set player n input type to 'joystick n'
                #if defined(CONTROLLER_DEBUG)
                    printf("Selected controller: %s\n", selectedController->getName().c_str());
                #endif

                sendSelectedController(selectedController->getName(), playerNum, false);
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
        for (int playerNum = playerCount + 1; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
        {
            players[playerNum - 1].haveKeyboard = false;
            players[playerNum - 1].haveController = false;
            players[playerNum - 1].joyId = 0;
        }
    }

    // Update controller state for each player
    for (int playerNum = 1; (playerNum <= CONTROLLER_MAX_PLAYERS) && (playerNum <= playerCount); playerNum++)
    {
        handleInputsForPlayer(playerNum);
    }

    // Send button press/release events
    for (int queueToSend = 0; queueToSend <= 1; queueToSend++)
    {
        // send press events first, then release events
        auto theQueue = queueToSend ? releaseQueue : pressQueue;
        const char* eventName = queueToSend ? "onControllerButtonRelease" : "onControllerButtonPress";
        //
        for (auto& stateChange : theQueue)
        {
            SDL_JoystickID joyId = stateChange.first;
            int which = stateChange.second;

            auto&& it = controllerMap.find(joyId);
            if (it != controllerMap.end())
            {
                if (gLunaLua.isValid()) {
                    // Get associated playerNum, or 0 if unassociated
                    int playerNum;
                    for (playerNum = 1; playerNum <= LunaGameControllerManager::CONTROLLER_MAX_PLAYERS; playerNum++)
                    {
                        if (players[playerNum - 1].haveController && (players[playerNum - 1].joyId == joyId))
                        {
                            break;
                        }
                    }
                    if (playerNum > LunaGameControllerManager::CONTROLLER_MAX_PLAYERS)
                    {
                        playerNum = 0;
                    }

                    std::shared_ptr<Event> changeControllerEvent = std::make_shared<Event>(eventName, false);
                    changeControllerEvent->setDirectEventName(eventName);
                    changeControllerEvent->setLoopable(false);
                    gLunaLua.callEvent(changeControllerEvent, which, playerNum, it->second.getName());
                }
            }
        }
    }
    pressQueue.clear();
    releaseQueue.clear();
}
#endif // !defined(BUILDING_SMBXLAUNCHER)

// Function to process inputs for a player
#if !defined(BUILDING_SMBXLAUNCHER)
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
            sendSelectedController("Keyboard", playerNum, player.controllerJustDisconnected);
        }
        player.controllerJustDisconnected = false;
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
    player.buttonState = controller->getButtonState();
}
#endif // !defined(BUILDING_SMBXLAUNCHER)

// Function to emulate joyGetPosEx
#if !defined(BUILDING_SMBXLAUNCHER)
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
    }
    else
    {
        pji->dwXpos = 0x7FFF;
        pji->dwYpos = 0x7FFF;
        pji->dwPOV = 0x7FFF;
        pji->dwButtons = 0;
    }

    return 0;
}
#endif // !defined(BUILDING_SMBXLAUNCHER)

#if !defined(BUILDING_SMBXLAUNCHER)
void LunaGameControllerManager::notifyKeyboardPress(int keycode)
{
    // Return if init not done
    if (!initDone)
    {
        return;
    }

    for (int playerNum = 1; playerNum <= CONTROLLER_MAX_PLAYERS; playerNum++)
    {
        // If multiplayer, and already locked to controller, ignore
        if ((GM_PLAYERS_COUNT > 1) && (players[playerNum - 1].haveController))
        {
            continue;
        }

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
            if ((GM_PLAYERS_COUNT <= 1) || (!players[playerNum - 1].haveController)) {
                if (SMBXInput::getPlayerInputType(playerNum) != 0)
                {
                    // Clear selected flag if set
                    players[playerNum - 1].haveKeyboard = true;
                    players[playerNum - 1].haveController = false;
                    players[playerNum - 1].controllerJustDisconnected = false;

                    SMBXInput::setPlayerInputType(playerNum, 0); // Set player 1 input type to 'keyboard'
                    #if defined(CONTROLLER_DEBUG)
                        printf("Selected controller: Keyboard\n");
                    #endif
                    sendSelectedController("Keyboard", playerNum, false);
                }
            }

            // Don't consider this input in switching player 2 if it's set for player 1
            break;
        }
    }
}
#endif // !defined(BUILDING_SMBXLAUNCHER)

SDL_JoystickPowerLevel LunaGameControllerManager::getSelectedControllerPowerLevel(int playerNum)
{
    LunaGameController* controller = getController(playerNum);
    if (controller != nullptr)
    {
        return controller->getPowerLevel();
    }
    return SDL_JOYSTICK_POWER_UNKNOWN;
}

std::string LunaGameControllerManager::getSelectedControllerName(int playerNum)
{
    LunaGameController* controller = getController(playerNum);
    if (controller != nullptr)
    {
        return controller->getName();
    }
    return "Keyboard";
}

std::tuple<int, int> LunaGameControllerManager::getSelectedControllerStickPosition(int playerNum)
{
    LunaGameController* controller = getController(playerNum);
    if (controller != nullptr)
    {
        return controller->getStickPosition();
    }
    return {0, 0};
}

void LunaGameControllerManager::rumbleSelectedController(int playerNum, int ms, float strength)
{
    LunaGameController* controller = getController(playerNum);
    if (controller != nullptr)
    {
        return controller->rumble(ms, strength);
    }
}

LunaGameController* LunaGameControllerManager::getController(int playerNum)
{
    if ((playerNum >= 1) && (playerNum <= CONTROLLER_MAX_PLAYERS) && players[playerNum - 1].haveController)
    {
        auto it = controllerMap.find(players[playerNum - 1].joyId);
        if (it != controllerMap.end())
        {
            return &it->second;
        }
    }
    return nullptr;
}

#if !defined(BUILDING_SMBXLAUNCHER)
void LunaGameControllerManager::sendSelectedController(const std::string& name, int playerNum, bool changeTriggeredByDisconnect)
{
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> changeControllerEvent = std::make_shared<Event>("onChangeController", false);
        changeControllerEvent->setDirectEventName("onChangeController");
        changeControllerEvent->setLoopable(false);
        gLunaLua.callEvent(changeControllerEvent, name, playerNum, changeTriggeredByDisconnect);
    }
}
#endif

void LunaGameControllerManager::addJoystickEvent(int joyIdx)
{
    // Process adding new device
    SDL_Joystick* joyPtr = nullptr;
    SDL_GameController* ctrlPtr = nullptr;

    // Get joystick ID
    SDL_JoystickID joyId = SDL_JoystickGetDeviceInstanceID(joyIdx);
    if ((joyId < 0) || (controllerMap.find(joyId) != controllerMap.end()))
    {
        // Failure to get ID or duplicate
        return;
    }

    // Open as joystick
    joyPtr = SDL_JoystickOpen(joyIdx);
    if (joyPtr == nullptr)
    {
        return;
    }

    if (SDL_IsGameController(joyIdx))
    {
        // Open as game controller
        ctrlPtr = SDL_GameControllerOpen(joyIdx);
    }

    controllerMap.emplace(joyId, LunaGameController(this, joyId, joyPtr, ctrlPtr));
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
            players[playerNum - 1].controllerJustDisconnected = true;
        }
    }

    // Remove pending presses associated with this joyId
    for (int queueToClear = 0; queueToClear <= 1; queueToClear++) {
        // which queue to clear?
        auto theQueue = queueToClear ? releaseQueue : pressQueue;
        // remove presses from the queue
        auto it = theQueue.begin();
        while (it != theQueue.end())
        {
            if (it->first == joyId)
            {
                it = theQueue.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    // Delete joystick
    {
        auto it = controllerMap.find(joyId);
        if (it != controllerMap.end())
        {
            controllerMap.erase(it);
        }
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

void LunaGameControllerManager::joyHatEvent(const SDL_JoyHatEvent& event)
{
    auto it = controllerMap.find(event.which);
    if (it != controllerMap.end())
    {
        it->second.joyHatEvent(event);
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

std::string LunaGameControllerManager::getControllerName(SDL_JoystickID joyId)
{
    auto it = controllerMap.find(joyId);
    if (it != controllerMap.end())
    {
        return it->second.getName();
    }
    return "<NULL>";
}

//=================================================================================================

LunaGameController::LunaGameController(LunaGameControllerManager* _managerPtr, SDL_JoystickID _joyId, SDL_Joystick* _joyPtr, SDL_GameController* _ctrlPtr) :
    managerPtr(_managerPtr),
    joyId(_joyId),
    joyPtr(_joyPtr),
    ctrlPtr(_ctrlPtr),
    hapticPtr(nullptr),
    name(),
    dirPadState(0),
    axisPadState(0),
    padState(0),
    buttonState(0),
    xAxis(0),
    yAxis(0),
    activeFlag(false),
    joyButtonMap()
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

    // Get buttom mapping
    if (joyPtr)
    {
        int buttonCount = SDL_JoystickNumButtons(joyPtr);
        joyButtonMap.resize(buttonCount);

        if (ctrlPtr)
        {
            // Detected as gamepad, so need to see if we have unbound buttons to map to extra button numbers

            // Clear values
            for (int i = 0; i < buttonCount; i++)
            {
                joyButtonMap[i] = 0;
            }

            // Set values to -1 when they're already handled with a gamepad binding
            for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
            {
                SDL_GameControllerButtonBind bind = SDL_GameControllerGetBindForButton(ctrlPtr, (SDL_GameControllerButton)i);
                if ((bind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON) && (bind.value.button < buttonCount))
                {
                    joyButtonMap[bind.value.button] = -1;
                }
            }

            // Set new indicies for buttons that aren't flagged with -1
            int nextJoyButtonIdx = CONTROLLER_BUTTON_JOY0;
            for (int i = 0; i < buttonCount; i++)
            {
                if (joyButtonMap[i] == 0)
                {
                    joyButtonMap[i] = nextJoyButtonIdx;
                    nextJoyButtonIdx++;
                }
            }
        }
        else
        {
            // Plain joystick, use unity mapping
            for (int i = 0; i < buttonCount; i++)
            {
                joyButtonMap[i] = i;
            }
        }

        #if defined(CONTROLLER_DEBUG)
            // Dump for testing
            printf("==== UNBOUND CONTROLS FOR %s ====\n", namePtr);
            for (int i = 0; i < buttonCount; i++)
            {
                printf(": %d%s\n", joyButtonMap[i], (joyButtonMap[i] >= CONTROLLER_BUTTON_JOY0) ? "****" : "");
            }
            printf("====\n");
        #endif
    }
    else
    {
        joyButtonMap.clear();
    }

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
    managerPtr      = other.managerPtr;
    joyId           = other.joyId;
    joyPtr          = other.joyPtr;
    ctrlPtr         = other.ctrlPtr;
    hapticPtr       = other.hapticPtr;
    name            = other.name;
    dirPadState     = other.dirPadState;
    axisPadState    = other.axisPadState;
    padState        = other.padState;
    buttonState     = other.buttonState;
    xAxis           = other.xAxis;
    yAxis           = other.yAxis;
    activeFlag      = other.activeFlag;
    joyButtonMap    = other.joyButtonMap;
    other.joyPtr    = nullptr;
    other.ctrlPtr   = nullptr;
    other.hapticPtr = nullptr;
}

// Move assignment
LunaGameController & LunaGameController::operator=(LunaGameController &&other)
{
    close();

    managerPtr      = other.managerPtr;
    joyId           = other.joyId;
    joyPtr          = other.joyPtr;
    ctrlPtr         = other.ctrlPtr;
    hapticPtr       = other.hapticPtr;
    name            = other.name;
    dirPadState     = other.dirPadState;
    axisPadState    = other.axisPadState;
    padState        = other.padState;
    buttonState     = other.buttonState;
    xAxis           = other.xAxis;
    yAxis           = other.yAxis;
    activeFlag      = other.activeFlag;
    joyButtonMap    = other.joyButtonMap;
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
    if (joyPtr == nullptr) return;

    #if defined(CONTROLLER_DEBUG_LOWLEVEL)
        printf("JoyButton%s %s, %d\n", down ? "Down" : "Up", name.c_str(), (int)event.button);
    #endif

    // Ignore if out of range
    if (event.button >= joyButtonMap.size()) return;

    // Get mapping
    int idx = joyButtonMap[event.button];

    if (idx >= 0)
    {
        buttonEvent(idx, down);
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

void LunaGameController::joyHatEvent(const SDL_JoyHatEvent& event)
{
    // Ignore if this is not a joy-only device
    if ((joyPtr == nullptr) || (ctrlPtr != nullptr)) return;

#if defined(CONTROLLER_DEBUG_LOWLEVEL)
    printf("JoyHat %s, idx=%d, %d\n", name.c_str(), (int)event.hat, (int)event.value);
#endif

    // Ignore any hat except hat 0 for now (in the future could map these to buttons maybe?)
    if (event.hat != 0) return;

    directionalEvent(CONTROLLER_PAD_UP,    event.value & SDL_HAT_UP,    false);
    directionalEvent(CONTROLLER_PAD_DOWN,  event.value & SDL_HAT_DOWN,  false);
    directionalEvent(CONTROLLER_PAD_LEFT,  event.value & SDL_HAT_LEFT,  false);
    directionalEvent(CONTROLLER_PAD_RIGHT, event.value & SDL_HAT_RIGHT, false);
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
        xAxis = (int)event.value;
        break;
    case SDL_CONTROLLER_AXIS_LEFTY:
        axisAsDirectional = true;
        posPadNumber = CONTROLLER_PAD_DOWN;
        negPadNumber = CONTROLLER_PAD_UP;
        yAxis = (int)event.value;
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

void LunaGameController::rumble(int ms, float strength)
{
    if (hapticPtr)
    {
        if (SDL_HapticRumblePlay(hapticPtr, strength, ms) == 0) return;
    }
    int intStrength = (int)(0xFFFF * strength + 0.5f);
    if (intStrength > 0xFFFF) intStrength = 0xFFFF;
    if (intStrength < 0) intStrength = 0;
#if SDL_VERSION_ATLEAST(2, 0, 12)
    if (ctrlPtr)
    {
        if (SDL_GameControllerRumble(ctrlPtr, intStrength, intStrength, ms) == 0) return;
    }
    if (joyPtr)
    {
        SDL_JoystickRumble(joyPtr, intStrength, intStrength, ms);
    }
#endif
}

void LunaGameController::directionalEvent(int which, bool newState, bool fromAnalog)
{
    unsigned int* maskPtr = fromAnalog ? &axisPadState : &dirPadState;

    unsigned int mask = *maskPtr;
    if (newState)
    {
        mask |= (1UL << which);
    }
    else
    {
        mask &= 0xFFFFFFFF ^ (1UL << which);
    }

    if (*maskPtr != mask)
    {
        *maskPtr = mask;
        padState = mask; // Assign current source of pad state
        activeFlag = activeFlag || newState;
    }
}

void LunaGameController::buttonEvent(int which, bool newState)
{
    // Up to 32 buttons supported currently
    if (which >= 32) return;

    if (newState)
    {
        buttonState |= (1UL << which);
    }
    else
    {
        buttonState &= 0xFFFFFFFF ^ (1UL << which);
    }

    activeFlag = activeFlag || newState;

    if (newState)
    {
        managerPtr->storePressEvent(joyId, which);
    }
    else
    {
        managerPtr->storeReleaseEvent(joyId, which);
    }
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
