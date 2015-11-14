--[[

'##:::::::'##::::'##:'##::: ##::::'###::::'##:::::::'##::::'##::::'###::::
 ##::::::: ##:::: ##: ###:: ##:::'## ##::: ##::::::: ##:::: ##:::'## ##:::
 ##::::::: ##:::: ##: ####: ##::'##:. ##:: ##::::::: ##:::: ##::'##:. ##::
 ##::::::: ##:::: ##: ## ## ##:'##:::. ##: ##::::::: ##:::: ##:'##:::. ##:
 ##::::::: ##:::: ##: ##. ####: #########: ##::::::: ##:::: ##: #########:
 ##::::::: ##:::: ##: ##:. ###: ##.... ##: ##::::::: ##:::: ##: ##.... ##:
 ########:. #######:: ##::. ##: ##:::: ##: ########:. #######:: ##:::: ##:
........:::.......:::..::::..::..:::::..::........:::.......:::..:::::..::
    '########:'##::: ##::'######:::'####:'##::: ##:'########::::
     ##.....:: ###:: ##:'##... ##::. ##:: ###:: ##: ##.....:::::
     ##::::::: ####: ##: ##:::..:::: ##:: ####: ##: ##::::::::::
     ######::: ## ## ##: ##::'####:: ##:: ## ## ##: ######::::::
     ##...:::: ##. ####: ##::: ##::: ##:: ##. ####: ##...:::::::
     ##::::::: ##:. ###: ##::: ##::: ##:: ##:. ###: ##::::::::::
     ########: ##::. ##:. ######:::'####: ##::. ##: ########::::
    ........::..::::..:::......::::....::..::::..::........:::::

This is currently a second version of main.lua 

It is a attempt to merge lunaworld.lua and lunadll.lua in one state.

]]

--LunaLua Version

__LUNALUA = "0.7"
__isLuaError = false

--Constants used by Lunadll. Do not modify them, or Lunadll with crash. 
-- ==DANGER ZONE START==
__lapiInit = "__onInit"
__lapiEventMgr = "eventManager"
__lapiSigNative = "detourEv_"
-- ==DANGER ZONE END==
-- Global vars
__loadedAPIs = {}

__lunaworld = {} --lunaworld.lua
__lunaworld.__init = false
__lunaworld.loadAPI = function(api)
    return loadLocalAPI("lunaworld", api)
end
__lunalocal = {} --lunadll.lua
__lunalocal.__init = false
__lunalocal.loadAPI = function(api)
    return loadLocalAPI("lunadll", api)
end

__lunaoverworld = {} --lunaoverworld.lua
__lunaoverworld.__init = false
__lunaoverworld.loadAPI = function(api)
    return loadLocalAPI("lunaoverworld", api)
end

__episodePath = ""
__customFolderPath = ""

-- Modified Native functions: 
os.exit = function() error("Shutdown") end

-- FFI-based APIs (remove direct acccess to FFI though)
local function initFFIBasedAPIs()
    local ffi = require("ffi")
    
    -- Add high resolution clock under the Misc namespace
    ffi.cdef[[
        typedef long            BOOL;
        BOOL QueryPerformanceFrequency(int64_t *lpFrequency);
        BOOL QueryPerformanceCounter(int64_t *lpPerformanceCount);
    ]]
    local kernel32 = ffi.load("kernel32.dll")
    local function GetPerformanceFrequency()
        local anum = ffi.new("int64_t[1]");
        if kernel32.QueryPerformanceFrequency(anum) == 0 then
            return nil
        end
        return tonumber(anum[0])
    end
    local function GetPerformanceCounter()
        local anum = ffi.new("int64_t[1]")
        if kernel32.QueryPerformanceCounter(anum) == 0 then
            return nil
        end
        return tonumber(anum[0])
    end   
    local performanceCounterFreq = GetPerformanceFrequency()
    Misc.clock = function()
        return GetPerformanceCounter() / performanceCounterFreq
    end
    
    -- Add GL engine FFI APIs
    ffi.cdef[[
        float* LunaLuaGlAllocCoords(size_t size);
        void LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count);
    ]]
    local LunaDLL = ffi.load("LunaDll.dll")
    Graphics.glNewCoordArray = function(size)
        return ffi.new("float[?]", size)
    end
    Graphics.glDrawTriangles = function(arg1, arg2, arg3)
        local arrLen = 2*arg3
        local arg1_raw = LunaDLL.LunaLuaGlAllocCoords(arrLen)
        local arg2_raw = LunaDLL.LunaLuaGlAllocCoords(arrLen)
        for i = 0,arrLen-1 do
            arg1_raw[i] = arg1[i] or 0
        end
        for i = 0,arrLen-1 do
            arg2_raw[i] = arg2[i] or 0
        end
        LunaDLL.LunaLuaGlDrawTriangles(arg1_raw, arg2_raw, arg3)
    end
    
    -- Limit access to FFI
    package.preload['ffi'] = nil
    package.loaded['ffi'] = nil
end
initFFIBasedAPIs()

-- We want the JIT running, so it's initially preloaded, but disable access to it
package.preload['jit'] = nil
package.loaded['jit'] = nil

-- ERR HANDLING v2.0, Let's get some more good ol' data
function __xpcall (f, ...)
  return xpcall(f,
    function (msg)
      -- build the error message
      return "==> "..msg..'\n'.."============="..'\n'..debug.traceback()
    end, ...)
end
-- ERR HANDLING END

function isAPILoadedByAPITable(apiTable, api)
    if(type(api)=="table")then
        for _,v in pairs(apiTable) do
            if(v == api)then
                return true
            end
        end
    end
    if(type(api)=="string")then
        if(apiTable[api])then
            return true
        end
    end
    return false
end

function isAPILoaded(api)
    if(isAPILoadedByAPITable(__loadedAPIs, api)) then return true end
    if(__lunaworld.__init)then
        if(isAPILoadedByAPITable(__lunaworld.__loadedAPIs, api)) then return true end
    end
    if(__lunalocal.__init)then
        if(isAPILoadedByAPITable(__lunalocal.__loadedAPIs, api)) then return true end
    end
    if(__lunaoverworld.__init)then
        if(isAPILoadedByAPITable(__lunaoverworld.__loadedAPIs, api)) then return true end
    end
    return false
end

local function findLast(haystack, needle)
    local i=haystack:match(".*"..needle.."()")
    if i==nil then return -1 else return i-1 end
end

local function filenameOfPath(path)
    local lastIndexOfSlash = findLast(path, "/")
    local lastIndexOfBackslash = findLast(path, "\\")
    if(lastIndexOfSlash == -1 and lastIndexOfBackslash == -1)then return path end
    
    local lastIndex = lastIndexOfSlash
    if(lastIndex < lastIndexOfBackslash)then
        lastIndex = lastIndexOfBackslash
    end
    return path:sub(lastIndex + 1)
end

local function doAPI(apiName)
    local searchInPath = {
        __episodePath,
        __customFolderPath,
        getSMBXPath().."\\LuaScriptsLib\\"}
    local endings = {
        ".lua",
        ".dll"}
    local func, err
    for _,apiPath in pairs(searchInPath) do
        for _,ending in pairs(endings) do
            func, err = loadfile(apiPath..apiName..ending)
            if(func)then
                local returnVal = func()
                if(type(returnVal) ~= "table")then
                    error("API \""..apiName.."\" did not return the api-table (got "..type(returnVal)..")", 2)
                end
                return returnVal
            end
            if(not err:find("such file"))then
                error(err,2)
            end
        end
    end
    error("No API found \""..apiName.."\"",2)
end

local function loadCodeFile(tableAddr, path, preDefinedEnv)
    tableAddr.__loadedAPIs = {}
    
    local tEnv = preDefinedEnv or {}
    setmetatable( tEnv, { __index = _G } )
    
    local codeFile, err = loadfile(path)
    if codeFile then
        tableAddr.__init = true
        setfenv( codeFile, tEnv )
        codeFile()
    else
        if(not err:find("such file"))then
            Text.windowDebugSimple("Error: "..err)
        end
        return false
    end
    
    for k,v in pairs( tEnv ) do
        tableAddr[k] =  v
    end
    
    if(type(tableAddr["onLoad"]) == "function")then
        tableAddr.onLoad()
    end
    
    return true
end


--Preloading function.
--This code segment won't post any errors!
function __onInit(episodePath, lvlName)
    local status = {__xpcall(function()
        --SEGMENT TO ADD PRELOADED APIS START
        Defines = loadSharedAPI("core\\defines")
        loadSharedAPI("uservar")
        DBG = loadSharedAPI("core\\dbg")
        --SEGMENT TO ADD PRELOADED APIS END
        
        __episodePath = episodePath
        __customFolderPath = episodePath..string.sub(lvlName, 0, -5).."\\"
        local noFileLoaded = true
        if(not isOverworld)then
            if(loadCodeFile(    __lunalocal,        __customFolderPath.."lunadll.lua",     {loadAPI = __lunalocal.loadAPI})) then noFileLoaded = false end
            if(loadCodeFile(    __lunaworld,        episodePath .. "lunaworld.lua",        {loadAPI = __lunaworld.loadAPI})) then noFileLoaded = false end
        else
            if(loadCodeFile(__lunaoverworld,    episodePath .. "lunaoverworld.lua",        {loadAPI = __lunaoverworld.loadAPI})) then noFileLoaded = false end
        end
        if(noFileLoaded)then
            __isLuaError = true
            return
        end
    end)}
    if(not status[1])then
        console:println("DEBUG: Error thrown!")
        Text.windowDebugSimple(status[2])
        __isLuaError = true
        console:println("DEBUG: Set error flag!")
        return
    end
end

-- Loads shared apis
function loadSharedAPI(api)
    local apiName = filenameOfPath(api)
    if(__loadedAPIs[apiName])then
        return __loadedAPIs[apiName], false
    end
    
    local loadedAPI = doAPI(api)
    __loadedAPIs[apiName] = loadedAPI
    if(type(loadedAPI["onInitAPI"])=="function")then
        loadedAPI.onInitAPI()
    end
    return loadedAPI, true
end

function loadLocalAPI(apiHoster, api)
    local apiName = filenameOfPath(api)
    local tHoster = nil
    if(apiHoster == "lunadll")then
        tHoster = __lunalocal
    elseif(apiHoster == "lunaworld")then
        tHoster = __lunaworld
    elseif(apiHoster == "lunaoverworld")then
        tHoster = __lunaoverworld
    else
        return nil, false
    end
    
    if(tHoster.__loadedAPIs[apiName])then
        return tHoster.__loadedAPIs[apiName], false
    end
    
    local loadedAPI = doAPI(api)

    tHoster.__loadedAPIs[apiName] = loadedAPI
    if(type(loadedAPI["onInitAPI"])=="function")then
        loadedAPI.onInitAPI()
    end
    return loadedAPI, true
end

--[[
value = api
]]
function registerEvent(apiTable, event, eventHandler, beforeMainCall)
    if(type(apiTable)=="string")then
        error("\nOutdated version of API is trying to use registerEvent with string\nPlease contact the api developer to fix this issue!",2)
    end
    eventHandler = eventHandler or event
    beforeMainCall = beforeMainCall or true
    if(isAPILoaded(apiTable))then
        if(beforeMainCall)then
            if(not eventManager.eventHosterBefore[event])then
                eventManager.eventHosterBefore[event] = {}
            end
            local tHandler = {}
            tHandler.eventHandler = eventHandler
            tHandler.apiTable = apiTable
            table.insert(eventManager.eventHosterBefore[event], tHandler)
        else
            if(not eventManager.eventHosterAfter[event])then
                eventManager.eventHosterAfter[event] = {}
            end
            local tHandler = {}
            tHandler.eventHandler = eventHandler
            tHandler.apiTable = apiTable
            table.insert(eventManager.eventHosterAfter[event], tHandler)
        end
    end    
end

function unregisterEvent(apiTable, event, eventHandler)
    eventHandler = eventHandler or event
    if(isAPILoaded(apiTable))then
        for index, eventHandlerStruct in pairs(eventManager.eventHosterBefore[event]) do
            if(eventHandlerStruct.apiTable == apiTable and eventHandlerStruct.eventHandler == eventHandler)then
                table.remove(eventManager.eventHosterBefore[event], index)
                return true
            end
        end
        for index, eventHandlerStruct in pairs(eventManager.eventHosterAfter[event]) do
            if(eventHandlerStruct.apiTable == apiTable and eventHandlerStruct.eventHandler == eventHandler)then
                table.remove(eventManager.eventHosterAfter[event], index)
                return true
            end
        end
    end
    return false
end

detourEventQueue = {
    collectedEvents = {},
    -- Collect an native event for the onLoop event
    collectEvent = function(eventName, ...)
        local args = {...}
        local eventInfo = args[1]
        local directEventName = ""
        if(eventInfo.loopable)then
            local collectedEvent = {eventName, detourEventQueue.transformArgs(eventName, ...)}
            table.insert(detourEventQueue.collectedEvents, collectedEvent)
        end
        if(eventInfo.directEventName == "")then
            directEventName = eventName.."Direct"
        else
            directEventName = eventInfo.directEventName
        end
        return directEventName
    end,
    transformArgs = function(eventName, ...)
        local fArgs = {...} -- Original Functions Args
        local rArgs = {...} -- New Functions Args
        --- ALL HARDCODED EVENTS ---
        if(eventName == "onEvent")then
            -- NOTE: Index starts with 1!
            -- Original Signature: Event eventObj, String name
            -- New Signature: String name
            rArgs = {fArgs[2]}
        else
            table.remove(rArgs[1]) --Remove the event object, as it cannot be used for loop events
        end
        --- ALL HARDCODED EVENTS END ---
        return rArgs
    end,
    -- Dispatch the new event
    dispatchEvents = function()
         for idx, collectedEvent in ipairs(detourEventQueue.collectedEvents) do
             eventManager[collectedEvent[1]](unpack(collectedEvent[2]))
        end
        detourEventQueue.collectedEvents = {}
    end
    
}


--Event Manager
eventManager = setmetatable({ --Normal table
    
    nextEvent = "",
    eventHosterBefore = {},
    eventHosterAfter = {},
    
    manageEvent = function(...)
        if(eventManager.nextEvent:len() > __lapiSigNative:len())then
            if(eventManager.nextEvent:sub(0, __lapiSigNative:len()) == __lapiSigNative)then
                eventManager.nextEvent = eventManager.nextEvent:sub(__lapiSigNative:len()+1)
                eventManager.nextEvent = detourEventQueue.collectEvent(eventManager.nextEvent, ...)
            end
        end
        
        
        
        local eventReturn = nil
    
        --Event host before    
        if(eventManager.eventHosterBefore[eventManager.nextEvent])then
            for k,v in pairs(eventManager.eventHosterBefore[eventManager.nextEvent]) do
                if(type(v.apiTable[v.eventHandler])=="function")then
                    local returns = {__xpcall(v.apiTable[v.eventHandler],...)} --Call event
                    if(not returns[1])then
                        Text.windowDebugSimple(returns[2])
                        __isLuaError = true
                        return
                    end
                    if(eventReturn ~= nil)then
                        table.remove(returns, 1)
                        eventReturn = returns
                    end
                end
            end
        end
        --Call global script event
        
        if(__lunaworld.__init)then
            if(type(__lunaworld[eventManager.nextEvent])=="function")then
                local returns = {__xpcall(__lunaworld[eventManager.nextEvent],...)}
                if(not returns[1])then
                    Text.windowDebugSimple(returns[2])
                    __isLuaError = true
                    return
                end
                table.remove(returns, 1)
                eventReturn = returns
            end
        end
        
        if(__lunalocal.__init)then
            if(type(__lunalocal[eventManager.nextEvent])=="function")then
                local returns = {__xpcall(__lunalocal[eventManager.nextEvent],...)}
                if(not returns[1])then
                    Text.windowDebugSimple(returns[2])
                    __isLuaError = true
                    return
                end
                table.remove(returns, 1)
                eventReturn = returns
            end
        end
        
        if(__lunaoverworld.__init)then
            if(type(__lunaoverworld[eventManager.nextEvent])=="function")then
                local returns = {__xpcall(__lunaoverworld[eventManager.nextEvent],...)}
                if(not returns[1])then
                    Text.windowDebugSimple(returns[2])
                    __isLuaError = true
                    return
                end
                table.remove(returns, 1)
                eventReturn = returns
            end
        end
        
        --Event host after
        if(eventManager.eventHosterAfter[eventManager.nextEvent])then
            for k,v in pairs(eventManager.eventHosterAfter[eventManager.nextEvent]) do
                if(type(v.apiTable[v.eventHandler])=="function")then
                    local returns = {__xpcall(v.apiTable[v.eventHandler],...)} --Call event
                    if(not returns[1])then
                        Text.windowDebugSimple(returns[2])
                        __isLuaError = true
                        return
                    end
                    if(eventReturn ~= nil)then
                        table.remove(returns, 1)
                        eventReturn = returns
                    end
                end
            end
        end
        
        if(eventManager.nextEvent == "onLoop")then
            detourEventQueue.dispatchEvents()
        end
        
        if(type(eventReturn) == "table")then
            return unpack(eventReturn)
        end
        return eventReturn
    end
},    
{ --Metatable
    __newindex = function (tarTable, key, value)
        --Text.windowDebugSimple("newindex: "..tostring(key))
    end,

    __index = function (tarTable, tarKey)
        if(type(rawget(eventManager,tarKey))=="nil")then --Handle a event
            rawset(eventManager, "nextEvent", tarKey)
            return rawget(eventManager, "manageEvent")
        else
            return rawget(eventManager, tarKey) --Get a already set field
        end
    end
});

--=====================================================================
--[[ API Functions ]]--
local APIHelper = {}
function APIHelper.doAPI(apiTableHolder, apiPath)
    local apiName = filenameOfPath(apiPath)
    if(apiTableHolder[apiName])then
        return apiTableHolder[apiName], false
    end
    
    local loadedAPI = nil
    local searchInPath = {
    __episodePath,
    __customFolderPath,
    getSMBXPath().."\\LuaScriptsLib\\"}
    local endings = {
        ".lua",
        ".dll"}
    local func, err
    for _,apiPath in pairs(searchInPath) do
        for _,ending in pairs(endings) do
            func, err = loadfile(apiPath..apiPath..ending)
            if(func)then
                loadedAPI = func()
                if(type(loadedAPI) ~= "table")then
                    error("API \""..apiPath.."\" did not return the api-table (got "..type(loadedAPI)..")", 2)
                end
            end
            if(not err:find("such file"))then
                error(err,2)
            end
            if(loadedAPI) then break end
        end
        if(loadedAPI) then break end
    end
    if(not loadedAPI) then error("No API found \""..apiPath.."\"",2) end
    
    apiTableHolder[apiName] = loadedAPI
    if(type(loadedAPI["onInitAPI"])=="function")then
        loadedAPI.onInitAPI()
    end
    return loadedAPI, true
end



--=====================================================================
--[[ Main User Code Manager ]]--
UserCodeManager = {}
UserCodeManager.codefiles = {}

function UserCodeManager.loadCodeFile(codeFileName, codeFilePath)
    -- 1. Setup the usercode instance
    local usercodeInstance = {}
    usercodeInstance.__loadedAPIs = {}
    
    -- 2. Setup environment
    local usercodeEnvironment = {
        -- 2.1 Add loadAPI function
        loadAPI = function(api)
            APIHelper.doAPI(usercodeInstance.__loadedAPIs, api)
        end
    }
    -- 3. Add access to global environment
    setmetatable( usercodeEnvironment, { __index = _G } )
    
    -- 4. Load the code file and add environment
    local codeFile, err = loadfile(path)
    if codeFile then
        -- 4.1 Set environment to the usercode
        setfenv( codeFile, usercodeEnvironment )
        -- 4.2 Execute file for initial run.
        codeFile()
    else
        -- 4.3 If file not found then ignore error, otherwise throw error!
        if(not err:find("such file"))then
            Text.windowDebugSimple("Error: "..err)
        end
        return false
    end
    
    -- 5. Directly add "global" fields to the table.
    for k,v in pairs( usercodeEnvironment ) do
        tableAddr[k] =  v
    end
    
    -- 6. Notify usercode file that loading has finished via "onLoad".
    if(type(tableAddr["onLoad"]) == "function")then
        tableAddr.onLoad()
    end
    
    return true
end

--=====================================================================
--[[ Main Event Manager ]]--
EventManager = {}
EventManager.userListeners = {}
EventManager.apiListeners = {}

EventManager.queuedEvents = {}

-- ===================== Event Calling =============================
-- This will call a new event
function EventManager.manageEventObj(eventObj, ...)

end

function EventManager.doQueue()

end

-- ================== Event Distribution ===========================
-- This will add a new listener object.
-- table listenerObject (A code file)
function EventManager.addUserListener(listenerObject)
    table.insert(EventManager.userListeners, listenerObject)
end

-- This will add proxy objects for Events
function EventManager.getProxyEnvironment()
    return {
        NPC = setmetatable({},{__index = _G["NPC"]})
    }
end

-- usage for luabind, always do with event-object
function __CallEvent(...)
    EventManager.manageEventObj(...)
end


