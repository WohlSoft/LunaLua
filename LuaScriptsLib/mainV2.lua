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
    
	local function convertGlArray(arr, arr_len)
		if (arr == nil) then return 0 end
		local arr_offset = 0
		if (arr[0] == nil) then arr_offset = 1 end
        local arr_raw = LunaDLL.LunaLuaGlAllocCoords(arr_len)
        for i = 0,arr_len-1 do
            arr_raw[i] = arr[i+arr_offset] or 0
        end
		return tonumber(ffi.cast("unsigned int", arr_raw))
	end
	
	local function getGlElementCount(arr, divisor)
		local len_offset = 0
		if (arr[0] ~= nil) then len_offset = 1 end
		return math.floor((#arr + len_offset) / divisor)
	end
	
	Graphics.glDraw = function(args)
		local priority = args['priority'] or 1.0
		local texture = args['texture']
		local color = args['color'] or {1.0, 1.0, 1.0, 1.0}
		if (type(color) == "number") then
			error("Numeric colors support not yet existing")
		elseif (#color == 3) then
			color = {color[1], color[2], color[3], 1.0}
		end
		local vertCoords = args['vertexCoords']
		local texCoords = args['textureCoords']
		local vertColor = args['vertexColors']
		local arr_len = nil
		if (vertCoords == nil) then
			error("vertexCoords is required")
		end
		local arr_len = getGlElementCount(vertCoords, 2)
		if (texCoords ~= nil) then
			if (arr_len ~= getGlElementCount(texCoords, 2)) then
				error("Incorrect textureCoords len")
			end
		end
		if (vertColor ~= nil) then
			if (arr_len ~= getGlElementCount(vertColor, 4)) then
				error("Incorrect vertexColors len")
			end
		end
		vertCoords = convertGlArray(vertCoords, arr_len*2)
		texCoords = convertGlArray(texCoords, arr_len*2)
		vertColor = convertGlArray(vertColor, arr_len*4)
	
		Graphics.__glInternalDraw(
			priority, texture,
			color[1], color[2], color[3], color[4],
			vertCoords, texCoords, vertColor, arr_len)
	end
	
    -- Limit access to FFI
    package.preload['ffi'] = nil
    package.loaded['ffi'] = nil
end

local function initJSON()
    package.path = package.path .. ";./LuaScriptsLib/ext/?.lua"
    _G["json"] = require("lunajson")
end

local nativeIO = io.open
local nativeIsSamePath = Misc.isSamePath
local function initSafeIO()
    io.open = function(filename, mode)
        local badFiles = {
            "./config/luna.ini",
            "./config/game.ini",
            "./config/autostart.ini",
            "./luna.ini",
            "./game.ini",
            "./autostart.ini",
            "./LuaScriptsLib/mainV2.lua"
        }
        local hasSame = false
        for _, nextPath in pairs(badFiles) do
            if(nativeIsSamePath(nextPath, filename))then
                hasSame = true
            end
        end
        if hasSame then
            error("You cannot access this path: " .. filename);
        end
        return nativeIO(filename, mode)
    end
end

initSafeIO()
initJSON()
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
    if(isAPILoadedByAPITable(UserCodeManager.sharedAPIs, api)) then return true end
    for _, nextCodeFile in pairs(UserCodeManager.codefiles) do
        if(isAPILoadedByAPITable(nextCodeFile.loadedAPIs, api))then
            return true
        end
    end
    return false
end


-- Loads shared apis
function loadSharedAPI(api)
    return APIHelper.doAPI(UserCodeManager.sharedAPIs, api)
end
--[[
value = api
]]
function registerEvent(apiTable, event, eventHandler, beforeMainCall)
    EventManager.addAPIListener(apiTable, event, eventHandler, beforeMainCall)
end

function unregisterEvent(apiTable, event, eventHandler)
    return EventManager.removeAPIListener(apiTable, event, eventHandler)
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

function registerCustomEvent(obj, eventName)
    local queue = {};
    local mt = getmetatable(obj);
    if(mt == nil) then
        mt = {__index = function(tbl,key) return rawget(tbl,key) end, __newindex = function(tbl,key,val) rawset(tbl,key,val) end}
    end
    local index_f = mt.__index;
    local newindex_f = mt.__newindex;
    
    mt.__index = function(tbl, key)
        if(key == eventName) then
            return function(...)
                for _,v in ipairs(queue) do
                    v(...);
                end
            end
        else
            return index_f(tbl, key);
        end
    end
    
    mt.__newindex = function (tbl,key,val)
        if(key == eventName) then
            table.insert(queue, val);
        else
            newindex_f(tbl,key,val);
        end
    end
    
    setmetatable(obj,mt);
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
UserCodeManager.sharedAPIs = {}

-- Codefile manager START
function UserCodeManager.addCodeFile(codeFileName, loadedCodeFile, apiTable)
    local newCodeFileEntry = {
        name = codeFileName,
        loadedAPIs = apiTable,
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
    -- 1. Setup the usercode instance
    local usercodeInstance = {}
    local loadedAPIsTable = {}
    
    -- 2. Setup environment
    local usercodeEnvironment = {
        -- 2.1 Add loadAPI function
        loadAPI = function(api)
            return APIHelper.doAPI(loadedAPIsTable, api)
        end
    }
    
    -- 2.2 Add custom environment (FIXME: Add proxy environment later!)
    local eventEnvironment = {}
    
    -- 3. Add access to global environment
    setmetatable( usercodeEnvironment, { __index = _G } )
    
    -- 4. Load the code file and add environment
    local codeFile, err = loadfile(codeFilePath)
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
        usercodeInstance[k] =  v
    end
    
    -- 6. Notify usercode file that loading has finished via "onLoad".
    if(type(usercodeInstance["onLoad"]) == "function")then
        usercodeInstance.onLoad()
    end

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
function EventManager.callApiListeners(isBefore, ...)
    for _, nextAPIToHandle in pairs(EventManager.apiListeners) do
        if(nextAPIToHandle.callBefore == isBefore)then
            if(nextAPIToHandle.eventName == name)then
                local hostObject = nextAPIToHandle.api
                hostObject[nextAPIToHandle.eventHandlerName](...)
            end
        end
    end
end
    
function EventManager.callEvent(name, ...)
    local mainName, childName = unpack(name:split("."))
    if(mainName == nil or childName == nil)then
        mainName, childName = unpack(name:split(":"))
    end
	
    -- Call API listeners before usercodes.
	EventManager.callApiListeners(true, ...)
	
    -- Call usercode files
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
	
    -- Call API Listeners after usercodes.
	EventManager.callApiListeners(false, ...)
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
    local directEventName = eventObj.directEventName
    if(directEventName == "")then
        directEventName = eventObj.eventName .. "Direct"
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
    if(type(thisTable) == "string")then
        error("\nOutdated version of API is trying to use registerEvent with string\nPlease contact the api developer to fix this issue!",2)
    end
    eventHandler = eventHandler or event --FIXME: Handle ==> NPC:onKill
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

-- FIXME: Check also if "beforeMainCall"
function EventManager.removeAPIListener(apiTable, event, eventHandler)
    for i=1,#EventManager.apiListeners do
        local apiObj = EventManager.apiListeners[i]
        if(apiObj.api == apiTable and 
            apiObj.eventName == event and
            apiObj.eventHandlerName == eventHandler)then
            table.remove(apiTable, i)
            return true
        end
    end
    return false
end

function EventManager.doQueue()
    while(#EventManager.queuedEvents > 0)do
        local nextQueuedEvent = table.remove(EventManager.queuedEvents)
        EventManager.callEvent(nextQueuedEvent.eventName, unpack(nextQueuedEvent.parameters))
    end
end









-- ===== FUNCTION USED BY LUNALUA ===== --
-- usage for luabind, always do with event-object

--[[
    The new core uses three functions:
        * __callEvent(...)  
            - This function is called when LunaLua grabs a new event. This event is then futher processed and queued if possible.
              The first argument is always an event object by LunaLua core.
              The function which process it is called 'EventManager.manageEventObj'.
        * __doEventQueue()
            - This function is called when LunaLua should process the queued events.
              The function which process it is called 'EventManager.doQueue'.
        * __onInit(episodePath, lvlName)
            - This function is doing the initializing.
 
]] 
function __callEvent(...)
    local pcallReturns = {__xpcall(EventManager.manageEventObj, ...)}
    __xpcallCheck(pcallReturns)
end

function __doEventQueue()
    local pcallReturns = {__xpcall(EventManager.doQueue)}
    __xpcallCheck(pcallReturns)
end

--Preloading function
--This code segment won't post any errors!
function __onInit(episodePath, lvlName)
    local pcallReturns = {__xpcall(function()
        --SEGMENT TO ADD PRELOADED APIS START
        Defines = APIHelper.doAPI(_G, "core\\defines")
        APIHelper.doAPI(_G, "uservar")
        DBG = APIHelper.doAPI(_G, "core\\dbg")
        --SEGMENT TO ADD PRELOADED APIS END
        
        __episodePath = episodePath
        __customFolderPath = episodePath..string.sub(lvlName, 0, -5).."\\"
        local noFileLoaded = true
        if(not isOverworld)then
            if(UserCodeManager.loadCodeFile("lunadll", __customFolderPath.."lunadll.lua")) then noFileLoaded = false end
            if(UserCodeManager.loadCodeFile("lunaworld", episodePath .. "lunaworld.lua")) then noFileLoaded = false end
        else
            if(UserCodeManager.loadCodeFile("lunaoverworld", episodePath .. "lunaoverworld.lua")) then noFileLoaded = false end
        end
        
        if(noFileLoaded)then
            __isLuaError = true
            return
        end
    end)}
    __xpcallCheck(pcallReturns)
end

