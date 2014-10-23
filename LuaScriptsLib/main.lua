--Constants used by Lunadll. Do not modify them, or Lunadll with crash. 
-- ==DANGER ZONE START==
__lapiInit = "__onInit"
__lapiEventMgr = "eventManager"
-- ==DANGER ZONE END==
-- Global vars
__loadedAPIs = {}

-- ERR HANDLING v2.0, Let's get some more good ol' data
function __xpcall (f, ...)
  return xpcall(function () return f(unpack(arg)) end,
    function (msg)
      -- build the error message
      return "==> "..msg..'\n'.."============="..'\n'..debug.traceback()
    end)
end
-- ERR HANDLING END


--Preloading function.
function __onInit()
	--Do currently nothing
	loadAPI("uservar")
end

function loadAPI(api)
	local loadedAPI = require(api)
	_G[api] = loadedAPI
	__loadedAPIs[api] = loadedAPI
	if(type(loadedAPI["onInitAPI"])=="function")then
		loadedAPI.onInitAPI()
	end
end

function registerEvent(apiName, event, eventHandler, beforeMainCall)
	eventHandler = eventHandler or event
	beforeMainCall = beforeMainCall or true
	if(__loadedAPIs[apiName])then
		if(beforeMainCall)then
			if(not eventManager.eventHosterBefore[event])then
				eventManager.eventHosterBefore[event] = {}
			end
			eventManager.eventHosterBefore[event][eventHandler] = apiName
		else
			if(not eventManager.eventHosterAfter[event])then
				eventManager.eventHosterAfter[event] = {}
			end
			eventManager.eventHosterAfter[event][eventHandler] = apiName
		end
	end
end

--Event Manager
eventManager = setmetatable({ --Normal table
	
	nextEvent = "",
	eventHosterBefore = {},
	eventHosterAfter = {},
	
	manageEvent = function(...)
		--Event host before	
		if(eventManager.eventHosterBefore[eventManager.nextEvent])then
			for k,v in pairs(eventManager.eventHosterBefore[eventManager.nextEvent]) do
				if(__loadedAPIs[v])then
					if(type(__loadedAPIs[v][k])=="function")then
						local returns = {__xpcall(__loadedAPIs[v][k],...)} --Call event
						if(not returns[1])then
							windowDebug(returns[2])
							error()
						end
					end
				end
			end
		end
		--Call global script event
		if(type(_G[eventManager.nextEvent])=="function")then
			local returns = {__xpcall(_G[eventManager.nextEvent],...)}
			if(not returns[1])then
				windowDebug(returns[2])
				error()
			end
			table.remove(returns, 1)
			return unpack(returns)
		else
			return
		end
		--Event host after
		if(eventManager.eventHosterAfter[eventManager.nextEvent])then
			for k,v in pairs(eventManager.eventHosterAfter[eventManager.nextEvent]) do
				if(__loadedAPIs[v])then
					if(type(__loadedAPIs[v][k])=="function")then
						local returns = {__xpcall(__loadedAPIs[v][k],...)} --Call event
						if(not returns[1])then
							windowDebug(returns[2])
							error()
						end
					end
				end
			end
		end
	end
},	
{ --Metatable
	__newindex = function (tarTable, key, value)
		--windowDebug("newindex: "..tostring(key))
	end,

	__index = function (tarTable, tarKey)
		if(type(rawget(eventManager,tarKey))=="nil")then
			rawset(eventManager, "nextEvent", tarKey)
			return rawget(eventManager, "manageEvent")
		else
			return rawget(eventManager, tarKey)
		end
	end
});