--LevelData.lua
--v1.0.1
--Created by Hoeloe, 2015
--DEPRECATED IN LUNALUA v0.6 : Use the Data class instead.
local LevelData = {}

local function prefix()
	if(isOverworld) then
		return ".MAP.lvl - "
	else
		return getLevelFilename().." - ";
	end
end

local function otherPrefix(name)
	return name..".lvl - "
end

local function filterValues(startString)
	local vars = UserData.values();
	local rtrn = {}
	for k,v in pairs(vars) do
		if(string.sub(k,1,string.len(startString))==startString) then
			rtrn[string.sub(k,string.len(startString))] = v;
		end
	end
	return rtrn;
end

function LevelData.externalData(levelName)
	local otherLevel = {}
	otherLevel.getValue = function(key) return UserData.getValue(otherPrefix(levelName)..key); end
	otherLevel.setValue = function(key, value) UserData.setValue(otherPrefix(levelName)..key, value); end
	otherLevel.isValueSet = function(key) return UserData.isValueSet(otherPrefix(levelName)..key); end
	otherLevel.values = function() return filterValues(otherPrefix(levelName)); end
	otherLevel.save = function() UserData.save(); end
	return otherLevel;
end

function LevelData.getValue(key)
	return UserData.getValue(prefix()..key);
end

function LevelData.setValue(key, value)
	UserData.setValue(prefix()..key, value);
end

function LevelData.isValueSet(key)
	return UserData.isValueSet(prefix()..key);
end

function LevelData.values()
	return filterValues(prefix());
end

function LevelData.save()
	UserData.save();
end

return LevelData;