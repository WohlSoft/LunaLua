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

-- Make safe
local unsafe_require = _G["require"] _G["require"] = nil
local unsafe_package = _G["package"] _G["package"] = nil

-- FFI-based APIs (remove direct acccess to FFI though) [DONE]
local function initFFIBasedAPIs()
    local ffi = unsafe_require("ffi")
    
    -- Add high resolution clock under the Misc namespace
    ffi.cdef[[
        typedef long			BOOL;
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
    
end
initFFIBasedAPIs()

-- ERR HANDLING v2.0, Let's get some more good ol' data
function __xpcall (f, ...)
  return xpcall(f,
    function (msg)
      -- build the error message
      return "==> "..msg..'\n'.."============="..'\n'..debug.traceback()
    end, ...)
end
-- ERR HANDLING END

function isAPILoaded(api)
	if(type(api)=="table")then
		for k,v in pairs(__loadedAPIs) do
			if(v == api)then
				return true
			end
		end
		
		if(__lunaworld.__init)then
			for k,v in pairs(__lunaworld.__loadedAPIs) do
				if(v == api)then
					return true
				end
			end
		end
		
		if(__lunalocal.__init)then
			for k,v in pairs(__lunalocal.__loadedAPIs) do 
				if(v == api)then
					return true
				end
			end
		end
        
        if(__lunaoverworld.__init)then
            for k,v in pairs(__lunaoverworld.__loadedAPIs) do
                if(v == api)then
                    return true
                end
            end
        end
	elseif(type(api)=="string")then
		if(__loadedAPIs[api])then
			return true
		end
		
		if(__lunaworld.__init)then
			if(__lunaworld.__loadedAPIs[api])then
				return true
			end
		end
		
		if(__lunalocal.__init)then
			if(__lunalocal.__loadedAPIs[api])then
				return true
			end
		end
        
        if(__lunaoverworld.__init)then
            if(__lunaoverworld.__loadedAPIs[api])then
				return true
			end
        end
	end
	
	return false
end

local function doAPI(apiName, preDefinedEnv)
    local tEnv = preDefinedEnv or {}
	setmetatable( tEnv, { __index = _G } )

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
                setfenv(func, tEnv)
				return func()
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
			windowDebug("Error: "..err)
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
function __onInit(lvlPath, lvlName)
    
	--Load default libs
	if(not isOverworld)then
		local isLunaworld = true
		local isLunadll = true
		
		local status = {pcall(function() --Safe code: This code segment can post errors
			__episodePath = lvlPath
			__customFolderPath = lvlPath..string.sub(lvlName, 0, -5).."\\"
			local doLunaworld = true
			local doLunadll = true
			
			--SEGMENT TO ADD PRELOADED APIS START
			loadSharedAPI("uservar")
			Defines = loadSharedAPI("core\\defines")
			DBG = loadSharedAPI("core\\dbg", {require = unsafe_require, package = unsafe_package}) -- Here we have to pass the "unsafe" function for the debugger sockets.
            --SEGMENT TO ADD PRELOADED APIS END
			
			local localLuaFile = nil
			local glLuaFile = lvlPath .. "lunaworld.lua"
			if(lvlName:find("."))then
				localLuaFile = lvlPath..string.sub(lvlName, 0, -5).."\\lunadll.lua"
			end	

			if(not loadCodeFile(__lunaworld, glLuaFile, {loadAPI = __lunaworld.loadAPI}))then
				doLunaworld = false
			end

			if(not loadCodeFile(__lunalocal, localLuaFile, {loadAPI = __lunalocal.loadAPI}))then
				doLunadll = false
			end
			
			return doLunaworld, doLunadll
		end)}
		
		if(not status[1])then
			windowDebug(status[2])
			error()
		end
		table.remove(status, 1)
		isLunaworld, isLunadll = unpack(status)
		if((not isLunaworld) and (not isLunadll))then
			error() --Shutdown Lua module as it is not used.
		end
	else
		local isOverworld = true
		local status = {pcall(function() --Safe code: This code segment can post errors
			__episodePath = lvlPath
			local doOverworld = true
			
			--SEGMENT TO ADD PRELOADED APIS START
			Defines = loadSharedAPI("core\\defines")
            DBG = loadSharedAPI("core\\dbg", {require = unsafe_require, package = unsafe_package}) -- Here we have to pass the "unsafe" function for the debugger sockets.
			--SEGMENT TO ADD PRELOADED APIS END

			local overworldLuaFile = lvlPath .. "lunaoverworld.lua"

			if(not loadCodeFile(__lunaoverworld, overworldLuaFile, {loadAPI = __lunaoverworld.loadAPI}))then
				doOverworld = false
			end
			return doOverworld
		end)}
		if(not status[1])then
			windowDebug(status[2])
			error()
		end
		table.remove(status, 1)
		isOverworld = unpack(status)
		if(not isOverworld)then
			error() --Shutdown Lua module as it is not used.
		end
	end
end

-- Loads shared apis
function loadSharedAPI(api, preDefEnv)
	if(__loadedAPIs[api])then
		return __loadedAPIs[api], false
	end
	
	local loadedAPI = doAPI(api, preDefEnv)
	__loadedAPIs[api] = loadedAPI
	if(type(loadedAPI["onInitAPI"])=="function")then
		loadedAPI.onInitAPI()
	end
	return loadedAPI, true
end

function loadLocalAPI(apiHoster, api)
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
	
	if(tHoster.__loadedAPIs[api])then
		return tHoster.__loadedAPIs[api], false
	end
	
	local loadedAPI = doAPI(api)

	tHoster.__loadedAPIs[api] = loadedAPI
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
						windowDebug(returns[2])
						error()
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
					windowDebug(returns[2])
					error()
				end
				table.remove(returns, 1)
				eventReturn = returns
			end
		end
		
		if(__lunalocal.__init)then
			if(type(__lunalocal[eventManager.nextEvent])=="function")then
				local returns = {__xpcall(__lunalocal[eventManager.nextEvent],...)}
				if(not returns[1])then
					windowDebug(returns[2])
					error()
				end
				table.remove(returns, 1)
				eventReturn = returns
			end
		end
		
		if(__lunaoverworld.__init)then
			if(type(__lunaoverworld[eventManager.nextEvent])=="function")then
				local returns = {__xpcall(__lunaoverworld[eventManager.nextEvent],...)}
				if(not returns[1])then
					windowDebug(returns[2])
					error()
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
						windowDebug(returns[2])
						error()
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
		--windowDebug("newindex: "..tostring(key))
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