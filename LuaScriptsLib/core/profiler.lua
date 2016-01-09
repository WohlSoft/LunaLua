local internalProfiler = require("jit.profile")
local profilerAPI = {}

local isProfilerActive = false
local collectedSample = {}

local function profilerDump(th, samples, vmmode)
    local stackStr = internalProfiler.dumpstack(th, "F --> l;", -10) .. " with mode " .. vmmode -- Maybe better format?
    collectedSample[stackStr] = (collectedSample[stackStr] or 0) + samples
end

function profilerAPI.onInitAPI()
    registerEvent(profilerAPI, "onKeyboardPress")
end

function profilerAPI.onKeyboardPress(keyCode)
    if(keyCode == VK_F3)then
        if(not isProfilerActive)then
            profilerAPI.start()
        else
            profilerAPI.stop()
        end
    end
end

function profilerAPI.start()
    if(isProfilerActive)then return false end    -- Do not start profiling when profiler is already 
    profilerAPI.resetVars()
    isProfilerActive = true
    internalProfiler.start("i1", profilerDump)
    playSFX(4) -- For now (maybe remove later?)
end

function profilerAPI.stop()
    if(not isProfilerActive)then return false end    -- Cannot stop, if the profiler isn't even running 
    isProfilerActive = false
    internalProfiler.stop()
    playSFX(6) -- For now (maybe remove later?)
end

function profilerAPI.resetVars()
    if(isProfilerActive)then return false end     -- Cannot reset, if the profiler is running.

    collectedSample = {}
end


return profilerAPI

