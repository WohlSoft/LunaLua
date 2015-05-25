--multipoints.lua 
--v2.2.2
--Created by Hoeloe, 2015
local multipoints = {}

local playerMP = { sec = 0, x = 0, y = 0, act = nil }
multipoints.midpointCount = 0;

local mpData;
local mpTrigs;
local mpLuaTrigs;
multipoints.IMG_ID = 156932;
multipoints.IMG_ALPHA = 0xFF00FF;
multipoints.IMG_PATH = getSMBXPath().."\\LuaScriptsLib\\multipoints\\mp.bmp";
multipoints.IMG_SIZE = { width = 32, height = 32 }

local dataFile;

local firstRun = true;

function multipoints.onInitAPI()
	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
	multipoints.setImage();
	registerEvent(multipoints, "onLoad", "onLoad", true) --Register the init event
	
	dataFile = Data(Data.DATA_LEVEL, "Multipoints");
	if(dataFile:get("MPCheck_ID") ==  nil) then
		multipoints.resetMidpoints();
	end
	firstRun = true;
	
	registerEvent(multipoints, "onLoop", "update", true) --Register the loop event
	registerEvent(multipoints, "onEvent", "onEvent", true) --Register the SMBX triggers event
end

function multipoints.setImage(path, id, alpha)
	if(path ~= nil) then multipoints.IMG_PATH = path end;
	if(id ~= nil) then multipoints.IMG_ID = id end;
	if(alpha ~= nil) then multipoints.IMG_ALPHA = alpha end;
	loadImage(multipoints.IMG_PATH, multipoints.IMG_ID, multipoints.IMG_ALPHA);
end

local function getMP(i)
		return function()
				local mp = findnpcs(192, -1)[0]
				if((mpLuaTrigs[i] ~= nil and not mpLuaTrigs[i].obj.silent) or (mpTrigs[i] ~= nil and not mpTrigs[i].obj.silent)) then
					playSFX(58);
				end
				
				local p;
				if(mpLuaTrigs[i] ~= nil) then
					p = mpLuaTrigs[i].obj.power;
				elseif(mpTrigs[i] ~= nil) then
					p = mpTrigs[i].obj.power;
				end
				
				if(p >= PLAYER_SMALL) then
					if(player:mem(0x112,FIELD_WORD) < p and player:mem(0x122,FIELD_WORD) ~= 1 and player:mem(0x122,FIELD_WORD) ~= 2) then
						player:mem(0x112,FIELD_WORD, p)
					end
				end
				
				if(mp ~= nil) then
					mp.x = player.x;
					mp.y = player.y;
				end
				dataFile:set("MPCheck_Get"..i,tostring(1));
				dataFile:set("MPCheck_ID",tostring(i));
				dataFile:save()
		end
end

local function createLuaMT(id)
local mpmt = {}

mpmt.__index = 
			function(t,k)
				if(k == "id") then
					return id;
				elseif(k == "x") then
					return mpLuaTrigs[t.id].x;
				elseif(k=="y") then
					return mpLuaTrigs[t.id].y;
				elseif(k == "collect") then
					return function() 
						if(not t.collected) then
							mpLuaTrigs[t.id].func();
						end
					end
				elseif(k == "collected") then
					return mpLuaTrigs[t.id].spawnAt or multipoints.getCheckpointStatus(t.id);
				end
			end;

mpmt.__newindex =
			function(t,k,v)
				if(k == "x") then
					mpLuaTrigs[t.id].x = v;
				elseif(k == "y") then
					mpLuaTrigs[t.id].y = v;
				end
			end;
			
return mpmt;
end

local function createMT(id)
local mpmt = {}

mpmt.__index = 
			function(t,k)
				if(k == "id") then
					return id;
				elseif(k == "x") then
					return 0;
				elseif(k=="y") then
					return 0;
				elseif(k=="visible") then
					return true;
				elseif(k == "collect") then
					return function() 
						if(not t.collected) then
							mpTrigs[t.id].func();
						end
					end
				elseif(k == "collected") then
					return mpTrigs[t.id].spawnAt or multipoints.getCheckpointStatus(t.id);
				end
			end;

mpmt.__newindex =
			function(t,k,v)
				if(k == "x") then
					return;
				elseif(k == "y") then
					return;
				end
			end;
			
return mpmt;
end

function multipoints.addLuaCheckpoint(x,y,section,spawnX,spawnY,extraActions)
		local i = multipoints.midpointCount;
		if(spawnX == nil) then
			spawnX = x;
		end
		if(spawnY == nil) then
			spawnY = y;
		end 
		
		if(mpData == nil) then
			dataFile = Data(Data.DATA_LEVEL, "Multipoints");
			mpData = {}
		end
		if(mpLuaTrigs == nil) then
			mpLuaTrigs = {} 
		end
		if(mpTrigs == nil) then
			mpTrigs = {} 
		end
		
		local obj = { visible = true, silent = false, power = PLAYER_BIG };
		setmetatable(obj,createLuaMT(i));
		
		mpLuaTrigs[i] = { x = x, y = y, spawnAt = false, func = getMP(i), obj = obj };
		mpData[i] = { sec = section, x = spawnX, y = spawnY, actions = extraActions }
		multipoints.midpointCount = i + 1;
		
		return obj;
end

function multipoints.addCheckpoint(event, section, spawnX, spawnY, hideEvent, extraActions)
		local i = multipoints.midpointCount; 
		if(mpData == nil) then
			dataFile = Data(Data.DATA_LEVEL, "Multipoints");
			mpData = {} 
		end
		if(mpTrigs == nil) then
			mpTrigs = {} 
		end
		if(mpLuaTrigs == nil) then
			mpLuaTrigs = {} 
		end
		
		local obj = { silent = false, power = PLAYER_BIG };
		setmetatable(obj,createMT(i));
		
		mpTrigs[i] = { e = event, spawnAt = false, func = getMP(i), obj = obj };
		mpData[i] = { sec = section, x = spawnX, y = spawnY, hide = hideEvent, actions = extraActions }
		multipoints.midpointCount = i + 1;
		
		return obj;
end

function multipoints.getCheckpointID()
	return tonumber(dataFile:get("MPCheck_ID"));
end

function multipoints.getCheckpointStatus(id)
	return tonumber(dataFile:get("MPCheck_Get"..id)) == 1;
end

function multipoints.resetMidpoints()
			dataFile:set("MPCheck_ID",tostring(-1));
			local i = 0;
			while(i < multipoints.midpointCount) do
				dataFile:set("MPCheck_Get"..i,tostring(0));
				i = i + 1;
			end
			dataFile:save()
end

function multipoints.onLoad()
end

function multipoints.update()
	if(firstRun) then
		local i = 0;
		while(i > -1 and mpData[i] ~= nil) do
			if(multipoints.getCheckpointID() == i) then
				playerMP = {sec = mpData[i].sec, x = mpData[i].x, y = mpData[i].y, act = mpData[i].actions }
				i = -1;
			else
				i = i + 1;
			end
		end
		
		local mp = findnpcs(192, -1)[0]
		if mp == nil then
			return
		end
		
		if (mp:mem (0x44, FIELD_WORD) ~= 0)  or  (mp:mem(0x122, FIELD_WORD) ~= 0)   then
			local i = 0;
			while(i < multipoints.midpointCount) do
				if(multipoints.getCheckpointStatus(i) and mpData[i].hide ~= nil) then
					triggerEvent(mpData[i].hide);
					mpTrigs[i].spawnAt = true;
				elseif(multipoints.getCheckpointStatus(i) and mpData[i].hide == nil and mpLuaTrigs[i] ~= nil) then
					mpLuaTrigs[i].spawnAt = true;
				end
				i = i + 1
			end
			player:mem(0x15A,FIELD_WORD,playerMP.sec);
			player.x = playerMP.x;
			player.y = playerMP.y - player:mem(0xD0,FIELD_DFLOAT) + 32;
			if(playerMP.act ~= nil) then
				playerMP.act();
			end
		else
			multipoints.resetMidpoints()
		end
		
		firstRun = false;
	end
	
	for k,v in pairs(mpLuaTrigs) do
		if(not v.spawnAt and not multipoints.getCheckpointStatus(k) and v.obj.visible) then
			placeSprite(2,multipoints.IMG_ID,v.x,v.y,"",2);
			if(player.x < v.x+multipoints.IMG_SIZE.width and player.x+player:mem(0xD8, FIELD_DFLOAT) > v.x and player.y < v.y+multipoints.IMG_SIZE.height and player.y + player:mem(0xD0, FIELD_DFLOAT) > v.y) then
				v.func();
			end
		end
	end
end

function multipoints.onEvent(event)
	for _,v in pairs(mpTrigs) do
		if(not v.spawnAt and v.e == event) then
			v.func();
		end
	end
end

return multipoints;