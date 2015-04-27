local DBG = {}
local doStart = false
local running = false


--Private Funcs
local function internalStart()
    package.path = package.path .. ";./LuaScriptsLib/ext/debugger/?.lua"
    require("debugger")("127.0.0.1", "10001", "LunaLua")
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