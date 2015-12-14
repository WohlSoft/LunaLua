-----------------------------------------------
-----------------------------------------------
--*******************************************--
--*           _ _   _         _     _       *--
--* _____ _ _| | |_|_|___ ___|_|___| |_ ___ *--
--*|     | | | |  _| | . | . | |   |  _|_ -|*--
--*|_|_|_|___|_|_| |_|  _|___|_|_|_|_| |___|*--
--*                  |_|                    *--
--*******************************************--
-----------------------------------------------
-----------------------------------------------
-----------Created by Hoeloe - 2015------------
-----------With help from Rednaxella-----------
--------Open-Source Checkpoint Library---------
------------For Super Mario Bros X-------------
-------------------v3.0.11----------------------
local multipoints = {}

local playerMP = { sec = 0, x = 0, y = 0, act = nil }
multipoints.midpointCount = 0;

local mpData = {};
local mpTrigs = {};
local mpLuaTrigs = {};
multipoints.IMG_PATH = getSMBXPath().."\\LuaScriptsLib\\multipoints\\mp.png";
multipoints.IMG_SIZE = { width = 32, height = 32 }

local dataFile = Data(Data.DATA_LEVEL, "Multipoints");

local firstRun = true;
local preRun = true;

--NOTE: If and when this function gets added to mainV2, remove from here.
local function registerCustomEvent(obj, eventName)
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

function multipoints.onInitAPI()
	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
	multipoints.setImage();
	registerEvent(multipoints, "onLoad", "onLoad", true) --Register the init event
	
	dataFile = Data(Data.DATA_LEVEL, "Multipoints");
	if(dataFile:get("MPCheck_ID") ==  nil) then
		multipoints.resetMidpoints();
	end
	preRun = true;
	firstRun = true;
	
	registerEvent(multipoints, "onLoop", "update", true) --Register the loop event
	registerEvent(multipoints, "onEvent", "onEvent", true) --Register the SMBX triggers event
	
	--Register curstom events
	registerCustomEvent(multipoints, "onCollected"); --multipoints:onCollected(id) (self = checkpoint object)
	registerCustomEvent(multipoints, "onLevelStart"); --multipoints.onLevelStart()
end

function multipoints.setImage(path)
	if(path ~= nil) then multipoints.IMG_PATH = path end;
	
	multipoints.IMG = Graphics.loadImage(multipoints.IMG_PATH);
end

local function getMP(i)
		return function(plyr)
		
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
					if(plyr == nil) then
						if(player:mem(0x112,FIELD_WORD) < p and player:mem(0x122,FIELD_WORD) ~= 1 and player:mem(0x122,FIELD_WORD) ~= 2) then
							player:mem(0x112,FIELD_WORD, p);
						end
						if(player2 ~= nil and player2:mem(0x112,FIELD_WORD) < p and player2:mem(0x122,FIELD_WORD) ~= 1 and player2:mem(0x122,FIELD_WORD) ~= 2) then
							player2:mem(0x112,FIELD_WORD, p);
						end
					elseif(plyr:mem(0x112,FIELD_WORD) < p and plyr:mem(0x122,FIELD_WORD) ~= 1 and plyr:mem(0x122,FIELD_WORD) ~= 2) then
						plyr:mem(0x112,FIELD_WORD, p)
					end
				end
				
				dataFile:set("MPCheck_Get"..i,tostring(1));
				dataFile:set("MPCheck_ID",tostring(i));
				dataFile:save()
				
				
				mem(0x00B250B0, FIELD_STRING, mem(0x00B2C618, FIELD_STRING))
				
				multipoints.onCollected(multipoints.getCheckpoint(i), i)
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
					return function(plyr) 
						if(not t.collected) then
							mpLuaTrigs[t.id].func(plyr);
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
					return function(plyr) 
						if(not t.collected) then
							mpTrigs[t.id].func(plyr);
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
		
		local obj = { visible = true, silent = false, power = PLAYER_BIG };
		setmetatable(obj,createLuaMT(i));
		
		mpLuaTrigs[i] = { x = x, y = y, spawnAt = false, func = getMP(i), obj = obj };
		mpData[i] = { sec = section, x = spawnX, y = spawnY, actions = extraActions }
		multipoints.midpointCount = i + 1;
		
		return obj;
end

function multipoints.addCheckpoint(event, section, spawnX, spawnY, hideEvent, extraActions)
		local i = multipoints.midpointCount; 
		
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

function multipoints.getCheckpoint(id)
	if(mpLuaTrigs[i] ~= nil) then return mpLuaTrigs[i].obj;
	elseif(mpTrigs[i] ~= nil) then return mpTrigs[i].obj;
	else return nil end;
end

function multipoints.resetMidpoints()
			dataFile:set("MPCheck_ID",tostring(-1));
			local i = 0;
			while(i < multipoints.midpointCount) do
				dataFile:set("MPCheck_Get"..i,tostring(0));
				i = i + 1;
			end
			dataFile:save()
			if(multipoints.midpointCount > 0) then
				mem(0x00B250B0, FIELD_STRING, "")
			end
end

local function warpPlayer()
	if(playerMP == nil) then return end;
	
	player.x = playerMP.x;
	player.y = playerMP.y - player:mem(0xD0,FIELD_DFLOAT) + 32;
	player:mem(0x15A,FIELD_WORD,playerMP.sec);
	playMusic(playerMP.sec);
	if(player2 ~= nil) then
		player.x = player.x + 16;
		player2:mem(0x15A,FIELD_WORD,playerMP.sec);
		player2.x = playerMP.x - 16;
		player2.y = playerMP.y - player2:mem(0xD0,FIELD_DFLOAT) + 32;
	end
end

function multipoints.update()
	if(preRun) then
		if(multipoints.midpointCount > 0) then
			local i = 0;
			while(i > -1 and mpData[i] ~= nil) do
				if(multipoints.getCheckpointID() == i) then
					playerMP = {sec = mpData[i].sec, x = mpData[i].x, y = mpData[i].y, act = mpData[i].actions }
					i = -1;
				else
					i = i + 1;
				end
			end
			
			if (tostring(mem(0x00B250B0, FIELD_STRING)) == tostring(mem(0x00B2C618, FIELD_STRING))) then
				warpPlayer();
			end
		end
		preRun = false;
		return;
	end
	if(firstRun) then
		if(multipoints.midpointCount > 0) then
			if (multipoints.getCheckpointID() == nil) or (multipoints.getCheckpointID() < 0) or tostring(mem(0x00B250B0, FIELD_STRING)) == "" then
				multipoints.resetMidpoints()
				firstRun = false;
				multipoints.onLevelStart();
				return
			end
			
			if (tostring(mem(0x00B250B0, FIELD_STRING)) == tostring(mem(0x00B2C618, FIELD_STRING))) then
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
				warpPlayer();
				if(playerMP.act ~= nil) then
					playerMP.act();
				end
			else
				multipoints.resetMidpoints();
			end
		end
		
		firstRun = false;
		multipoints.onLevelStart();
	end
	
	for k,v in pairs(mpLuaTrigs) do
		if(not v.spawnAt and not multipoints.getCheckpointStatus(k) and v.obj.visible) then
			Graphics.drawImageToScene(multipoints.IMG,v.x,v.y);
			if(player.x < v.x+multipoints.IMG_SIZE.width and player.x+player:mem(0xD8, FIELD_DFLOAT) > v.x and player.y < v.y+multipoints.IMG_SIZE.height and player.y + player:mem(0xD0, FIELD_DFLOAT) > v.y)then
				v.func(player);
			end
			if(player2 ~= nil and player2.x < v.x+multipoints.IMG_SIZE.width and player2.x+player2:mem(0xD8, FIELD_DFLOAT) > v.x and player2.y < v.y+multipoints.IMG_SIZE.height and player2.y + player2:mem(0xD0, FIELD_DFLOAT) > v.y) then
				v.func(player2);
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