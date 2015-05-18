local DBG = {}
local doStart = false
local running = false


--Private Funcs
local function internalStart()
    package.path = package.path .. ";./LuaScriptsLib/ext/debugger/?.lua"

    --First make it temporarly global
    _G["require"] = require
    _G["package"] = package

    local tDbg = require("debugger")


    local tEnv = {require = _G["require"], package =  _G["package"]}
	setmetatable( tEnv, { __index = _G } )
    setfenv(tDbg, tEnv)

    tDbg("127.0.0.1", "10001", "LunaLua")

    --Now make is safe again
    _G["require"] = nil
    _G["package"] = nil

    running = true
    doStart = false
end
function DBG.checkForDbgFlag()
    if(doStart)then
        internalStart()
        unregisterEvent(DBG, "onLoop", "checkForDbgFlag") --Unregister that command to safe some preformance
    end
end

--Public Funcs
function DBG.stop()
    if(running)then
        local testArgs = {}
        testArgs.i = 0
        local cmdMgr = require("debugger.commands")
        cmdMgr.stop(cmdMgr, testArgs)
        running = false
    end
end
function DBG.start()
    doStart = true
    registerEvent(DBG, "onLoop", "checkForDbgFlag") --We need to do that to be sure that it gets started when the game starts
end


return DBG