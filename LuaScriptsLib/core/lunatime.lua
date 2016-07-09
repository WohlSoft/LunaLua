local lunatime = {}
local timer = 0;

function lunatime.onInitAPI()
    registerEvent(lunatime, "onTick", "onTick", true);
end

function lunatime.onTick()
	timer = timer + 1;
end

-- Get the current game tick
function lunatime.tick()
	return timer;
end

-- Get the current game time in seconds
function lunatime.time()
	return (timer * 39) / 2500;
end

-- Convert game seconds to game ticks
function lunatime.toTicks(seconds)
	return math.floor(((seconds * 2500) / 39) + 0.5);
end

-- Convert game ticks to game seconds
function lunatime.toSeconds(ticks)
	return (ticks * 39) / 2500;
end


return lunatime;
