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





-- ====================================================================
-- =================== NEW REWORKED MAIN FUNTION ======================
-- ====================================================================

--=====================================================================
--[[ Utils ]]--
function string:split(sep)
    local sep, fields = sep or ":", {}
    local pattern = string.format("([^%s]+)", sep)
    self:gsub(pattern, function(c) fields[#fields+1] = c end)
    return fields
end

function __xpcallCheck(returnData)
    if(not returnData[1])then
        Text.windowDebugSimple(returnData[2])
        __isLuaError = true
        return false
    end
    return true
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
    for _,apiSearchPath in pairs(searchInPath) do
        for _,ending in pairs(endings) do
            func, err = loadfile(apiSearchPath..apiPath..ending)
            if(func)then
                loadedAPI = func()
                if(type(loadedAPI) ~= "table")then
                    error("API \""..apiPath.."\" did not return the api-table (got "..type(loadedAPI)..")", 2)
                end
            else
                if(not err:find("such file"))then
                    error(err,2)
                end
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

-- Codefile manager START
function UserCodeManager.addCodeFile(codeFileName, loadedCodeFile, apiTable)
    local newCodeFileEntry = {
        name = codeFileName,
        loadedAPIs = {},
        instance = loadedCodeFile
    }
    table.insert(UserCodeManager.codefiles, newCodeFileEntry)
end

function UserCodeManager.getCodeFile(codeFileName)
    for _, v in pairs(UserCodeManager.codefiles) do
        if(v.name == codeFileName)then
            return v
        end
    end
    return nil
end

-- Codefile manager END

function UserCodeManager.loadCodeFile(codeFileName, codeFilePath)
    console:println("DEBUG: Setting up instance!")
    -- 1. Setup the usercode instance
    local usercodeInstance = {}
    local loadedAPIsTable = {}
    
    console:println("DEBUG: Setting up environment!")
    -- 2. Setup environment
    local usercodeEnvironment = {
        -- 2.1 Add loadAPI function
        loadAPI = function(api)
            APIHelper.doAPI(loadedAPIsTable, api)
        end
    }
    
    console:println("DEBUG: Adding proxy environment!")
    -- 2.2 Add proxy environment
    local eventEnvironment = EventManager.getProxyEnvironment()
    for k,v in pairs(eventEnvironment) do
        usercodeEnvironment[k] = v
    end
    
    console:println("DEBUG: Setup metatable!")
    -- 3. Add access to global environment
    setmetatable( usercodeEnvironment, { __index = _G } )
    
    console:println("DEBUG: Load and check code file!")
    console:println("DEBUG: Loading codefile: "..codeFilePath)
    -- 4. Load the code file and add environment
    local codeFile, err = loadfile(codeFilePath)
    if codeFile then
        console:println("DEBUG: setfenv")
        -- 4.1 Set environment to the usercode
        setfenv( codeFile, usercodeEnvironment )
        -- 4.2 Execute file for initial run.
        console:println("DEBUG: EXECUTE!")
        codeFile()
    else
        -- 4.3 If file not found then ignore error, otherwise throw error!
        if(not err:find("such file"))then
            Text.windowDebugSimple("Error: "..err)
        end
        return false
    end
    
    console:println("DEBUG: Copy environment result!")
    -- 5. Directly add "global" fields to the table.
    for k,v in pairs( usercodeEnvironment ) do
        usercodeInstance[k] =  v
    end
    
    console:println("DEBUG: Call load function!")
    -- 6. Notify usercode file that loading has finished via "onLoad".
    if(type(usercodeInstance["onLoad"]) == "function")then
        usercodeInstance.onLoad()
    end

    console:println("DEBUG: Add to code files!")
    -- 7. Now add the code file to the usercode table
    UserCodeManager.addCodeFile(codeFileName, usercodeEnvironment, loadedAPIsTable)
    
    -- 8. Subscript to all events
    EventManager.addUserListener(usercodeEnvironment)
    
    return true
end

--=====================================================================
--[[ Main Event Manager ]]--
EventManager = {}
EventManager.userListeners = {}
EventManager.apiListeners = {}
EventManager.queuedEvents = {}

-- ====================== Event Management ==============================
function EventManager.callEvent(name, ...)
    local mainName, childName = unpack(name:split("."))
    if(mainName == nil or childName == nil)then
        mainName, childName = unpack(name:split(":"))
    end
    
    for _, nextUserListener in pairs(EventManager.userListeners)do
        local hostObject = nextUserListener
        if(childName)then
            hostObject = nextUserListener[mainName]
            mainName = childName
        end
        if(hostObject[mainName])then
            hostObject[mainName](...)
        end
    end
end
function EventManager.queueEvent(name, ...)
    local newQueueEntry = 
    {
        eventName = name,
        parameters = {...}
    }
    table.insert(EventManager.queuedEvents, newQueueEntry)
end
function EventManager.manageEventObj(eventObj, ...)
    console:println("DEBUG: Manage LunaLua event: " .. eventObj.eventName)
    local directEventName = eventObj.directEventName
    if(directEventName == "")then
        directEventName = eventObj.eventName
    end
    EventManager.callEvent(directEventName, eventObj, ...)
    if(eventObj.loopable)then
        EventManager.queueEvent(eventObj.eventName, ...) 
    end
end



-- ================== Event Distribution ===========================
-- This will add a new listener object.
-- table listenerObject (A code file)
function EventManager.addUserListener(listenerObject)
    table.insert(EventManager.userListeners, listenerObject)
end

function EventManager.addAPIListener(thisTable, event, eventHandler, beforeMainCall)
    eventHandler = eventHandler or event
    beforeMainCall = beforeMainCall or true
    local newApiHandler =
    {
        api = thisTable,
        eventName = event,
        eventHandlerName = eventHandler,
        callBefore = beforeMainCall
    }
    table.insert(EventManager.apiListeners, newApiHandler)
end

-- This will add proxy objects for Events
function EventManager.getProxyEnvironment()
    return {
        NPC = setmetatable({},{__index = _G["NPC"]})
    }
end


function EventManager.doQueue()
    while(#EventManager.queuedEvents > 0)do
        local nextQueuedEvent = table.remove(EventManager.queuedEvents)
        EventManager.callEvent(nextQueuedEvent.eventName, unpack(nextQueuedEvent.parameters))
    end
end





-- ===== FUNCTION USED BY LUNALUA ===== --
-- usage for luabind, always do with event-object
function __callEvent(...)
    console:println("DEBUG: Calling Event!")
    local pcallReturns = {__xpcall(EventManager.manageEventObj, ...)}
    __xpcallCheck(pcallReturns)
end

function __doEventQueue()
    console:println("DEBUG: Running queue")
    local pcallReturns = {__xpcall(EventManager.doQueue)}
    __xpcallCheck(pcallReturns)
end

--Preloading function
--This code segment won't post any errors!
function __onInit(episodePath, lvlName)
    console:println("DEBUG: Start init")
    local pcallReturns = {__xpcall(function()
        console:println("DEBUG: Loading APIs")
        --SEGMENT TO ADD PRELOADED APIS START
        Defines = APIHelper.doAPI(_G, "core\\defines")
        APIHelper.doAPI(_G, "uservar")
        DBG = APIHelper.doAPI(_G, "core\\dbg")
        --SEGMENT TO ADD PRELOADED APIS END
        
        console:println("DEBUG: Loading user files!")
        __episodePath = episodePath
        __customFolderPath = episodePath..string.sub(lvlName, 0, -5).."\\"
        local noFileLoaded = true
        if(not isOverworld)then
            if(UserCodeManager.loadCodeFile("lunadll", __customFolderPath.."lunadll.lua")) then noFileLoaded = false end
            if(UserCodeManager.loadCodeFile("lunaworld", episodePath .. "lunaworld.lua")) then noFileLoaded = false end
        else
            if(UserCodeManager.loadCodeFile("lunaoverworld", episodePath .. "lunaoverworld.lua")) then noFileLoaded = false end
        end
        
        console:println("DEBUG: Loaded user files!")
        if(noFileLoaded)then
            console:println("DEBUG: No user files found... shutting down!")
            __isLuaError = true
            return
        end
    end)}
    __xpcallCheck(pcallReturns)
end
