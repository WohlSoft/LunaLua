--eventu.lua 
--v1.0.4
--Created by Hoeloe, 2015
local eventu = {}

local secondsQueue = {}
local framesQueue = {}
local inputQueue = {}
local signalQueue = {}
local eventQueue = {}

local pausedQueue = {}
local breakQueue = {}

eventu.deltaTime = 0;
eventu.time = 0;
eventu.timeFrozen = 0;

eventu.deltaTimeMax = 0.1;

function eventu.onInitAPI()
	registerEvent(eventu, "onLoad", "init", true) --Register the init event
	registerEvent(eventu, "onLoop", "update", true) --Register the loop event
	registerEvent(eventu, "onKeyDown", "onKey", true)
	registerEvent(eventu, "onEvent", "onEvent", true)
end

function eventu.init()
	eventu.time = os.clock();
end

local function resumeCoroutine(co)
		 local coStatus, coMsg = coroutine.resume(co)
         if (not coStatus) then
            -- Ignore dead coroutines because that's just the result of a normal non-error exit
            if (coMsg ~= "cannot resume dead coroutine") then
               -- Throw error up the chain, including a stack trace of the coroutine that we otherwise wouldn't get
               error(coMsg .. "\n=============\ncoroutine " .. debug.traceback(co))
            end
         end
		 return coStatus, coMsg;
end

function eventu.update()
	eventu.deltaTime = os.clock() - eventu.time;
	if(eventu.deltaTime > eventu.deltaTimeMax) then
		eventu.timeFrozen = eventu.deltaTime;
		eventu.deltaTime = 0;
	else
		eventu.timeFrozen = 0;
	end
	
	for k,v in pairs(secondsQueue) do
		if(pausedQueue[k] == nil) then
			secondsQueue[k] = v-eventu.deltaTime;
			if(v <= 0) then
				secondsQueue[k] = nil;
				resumeCoroutine (k)
			end
		end
	end
	
	for k,v in pairs(framesQueue) do
		if(pausedQueue[k] == nil) then
			framesQueue[k] = v-1;
			if(v <= 0) then
				framesQueue[k] = nil;
				resumeCoroutine (k)
			end
		end
	end
	
	eventu.time = os.clock();
end

function eventu.onKey(key)
	for k,v in pairs(inputQueue) do
		if(v == key) then
			inputQueue[k] = nil;
			resumeCoroutine (k)
		end
	end
end

function eventu.onEvent(name)
		local waketable = {}
		if(eventQueue[name] ~= nil) then
			for k,v in pairs(eventQueue[name]) do
				eventQueue[name][k] = nil;
				table.insert(waketable, v);
			end
			eventQueue[name] = nil;
			
			for _,v in ipairs(waketable) do
				resumeCoroutine (v)
			end
		end
end

local function getCurrentCoroutine()
		local co = coroutine.running ()
		
		-- If co is nil, that means we're on the main process, which isn't a coroutine and can't yield
		assert (co ~= nil, "The main thread cannot wait!")
		
		return co;
end

function eventu.run(func)
		local co = coroutine.create (func)
		return resumeCoroutine (co), co
end

function eventu.waitSeconds(secs)
		local co = getCurrentCoroutine();
		secondsQueue[co] = secs;

		-- And suspend the process
		return coroutine.yield (co)
end

function eventu.waitFrames(frames)
		local co = getCurrentCoroutine();
		framesQueue[co] = frames-1;

		-- And suspend the process
		return coroutine.yield (co)
end

function eventu.waitForInput(key)
		local co = getCurrentCoroutine();
		inputQueue[co] = key;

		-- And suspend the process
		return coroutine.yield (co)
end

function eventu.waitSignal(name)
		local co = getCurrentCoroutine();
		
		if(signalQueue[name] == nil) then
			signalQueue[name] = {};
		end
		
		table.insert(signalQueue[name], co);
		
		-- And suspend the process
		return coroutine.yield (co)
end


function eventu.waitEvent(name)
		local co = getCurrentCoroutine();
		
		if(eventQueue[name] == nil) then
			eventQueue[name] = {};
		end
		
		table.insert(eventQueue[name], co);
		
		-- And suspend the process
		return coroutine.yield (co)
end

function eventu.signal(name)
		local waketable = {}
		for k,v in pairs(signalQueue[name]) do
			signalQueue[name][k] = nil;
			table.insert(waketable, v);
		end
		signalQueue[name] = nil;
		
		for _,v in ipairs(waketable) do
			resumeCoroutine (v)
		end
end

function eventu.setTimer(secs, func, repeated)
		repeated = repeated or false;
		local _,c = eventu.run(function()
						repeat
							eventu.waitSeconds(secs);
							func();
							local co = getCurrentCoroutine();
							if(breakQueue[co] == true) then
								breakQueue[co] = nil;
								repeated = false;
							end
						until repeated == false;
					end);
		return c;
end

function eventu.setFrameTimer(frames, func, repeated)
		repeated = repeated or false;
		local _,c = eventu.run(function()
						repeat
							eventu.waitFrames(frames);
							func();
							local co = getCurrentCoroutine();
							if(breakQueue[co] == true) then
								breakQueue[co] = nil;
								repeated = false;
							end
						until repeated == false;
					end);
		return c;
end

function eventu.pauseTimer(co)
		pausedQueue[co] = true;	
end

function eventu.resumeTimer(co)
		pausedQueue[co] = nil;
end

function eventu.getTimer(co)
		if(secondsQueue[co] ~= nil) then
			return secondsQueue[co];
		elseif(framesQueue[co] ~= nil) then
			return framesQueue[co];
		else
			return 0;
		end
end

function eventu.breakTimer()
		local co = getCurrentCoroutine();
		breakQueue[co] = true;
end

function eventu.registerKeyEvent(key, func, consume)
		consume = consume or false;
		eventu.run(function()
						repeat
							eventu.waitForInput(key);
							func();
							local co = getCurrentCoroutine();
							if(breakQueue[co] == true) then
								breakQueue[co] = nil;
								consume = true;
							end
						until consume == true;
					end);
end

function eventu.registerSMBXEvent(event, func, repeated)
		repeated = repeated or false;
		eventu.run(function()
						repeat
							eventu.waitEvent(event);
							func();
							local co = getCurrentCoroutine();
							if(breakQueue[co] == true) then
								breakQueue[co] = nil;
								repeated = false;
							end
						until repeated == false;
					end);
end

return eventu;