local internalProfiler = require("jit.profile")
local profilerAPI = {}

local isProfilerActive = false
local collectedSample = {}
local collectedSampleLines = {}
local collectedSampleMode = {N={}, I={}, C={}, G={}, J={}}
local totalSamples = 0
local vmModeNames = {N="JIT Compiled", I="Interpreted", C="C/C++ Code", G="Garbage Collection", J="JIT Overhead"}

local function profilerDump(th, samples, vmmode)
	local stackStr = internalProfiler.dumpstack(th, "F`l;", -100)
	
	local samplesCounted = false
	for s in string.gmatch(stackStr, "[^;]+") do
		local parts = {}
		for p in string.gmatch(s, "[^`]+") do
			table.insert(parts, p)
		end
		local func = parts[1]
		local line = parts[2]
		if (string.find(func, "mainV2") == nil) then
			collectedSample[func] = (collectedSample[func] or 0) + samples
			collectedSampleMode[vmmode][func] = (collectedSampleMode[vmmode][func] or 0) + samples
			if (collectedSampleLines[func] == nil) then
				collectedSampleLines[func] = {}
			end
			collectedSampleLines[func][line] = (collectedSampleLines[func][line] or 0) + samples
			samplesCounted = true
		end
	end
	if (samplesCounted) then
		totalSamples = totalSamples + samples
	end
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
	if(isProfilerActive)then return false end	-- Do not start profiling when profiler is already 
	profilerAPI.resetVars()
	isProfilerActive = true
	internalProfiler.start("li1", profilerDump)
	playSFX(4) -- For now (maybe remove later?)
end

local function perc(count, total)
	return string.format("%.1f%%", 100.0 * count / total)
end

function profilerAPI.stop()
	if(not isProfilerActive)then return false end	-- Cannot stop, if the profiler isn't even running
	isProfilerActive = false
	internalProfiler.stop()
	playSFX(6) -- For now (maybe remove later?)
	
	local ord = {}
	for d,v in pairs(collectedSample) do -- Change collectedSample to collectedSampleMode.I to sort by interpreted sample count instead
		table.insert(ord, {v, d})
	end
	table.sort(ord, function(arg1, arg2)
		return arg1[1] > arg2[1]
	end)
	output = ""
	linecnt = 0
	for _,x in ipairs(ord) do
		local funcCnt = collectedSample[x[2]]
		local func = x[2]
		
		output = output .. "\n" .. (perc(funcCnt, totalSamples) .. "\t" ..  func)
		local firstMode = true
		
		for _, vmMode in ipairs({"N", "I", "C", "G", "J"}) do
			local modeCnt = collectedSampleMode[vmMode][func]
			if (modeCnt ~= nil) then
				if (firstMode) then
					output = output .. "\t"
				else
					output = output .. ", "
				end
				output = output .. perc(modeCnt, funcCnt) .. " " .. vmModeNames[vmMode]
				firstMode = false
			end
		end
		
		output = output .. "\n"
		local lines = {}
		for line,count in pairs(collectedSampleLines[func]) do
			if (count * 20 >= funcCnt) then
				table.insert(lines, {count, line})
			end
		end
		if (#lines > 1) then
			table.sort(lines, function(arg1, arg2)
				return arg1[1] > arg2[1]
			end)
			for _,line in ipairs(lines) do
				output = output .. "\t" ..  perc(line[1], funcCnt) .. "\t" .. line[2] .. "\n"
			end
		end
		
		linecnt = linecnt + 1
		if (linecnt > 200) then break end
	end
	local f = io.open("profiler.log", "w")
	f:write(output)
	f:close()
	windowDebug(output)
end

function profilerAPI.resetVars()
	if(isProfilerActive)then return false end	 -- Cannot reset, if the profiler is running.

	collectedSample = {}
	collectedSampleLines = {}
	collectedSampleMode = {N={}, I={}, C={}, G={}, J={}}
	totalSamples = 0
end


return profilerAPI

