--npcconfig.lua 
--v2.0.0
--Created by Hoeloe, (heavily) modified by Rednaxela
local npcconfig = {}

-- Declare encoder/decoder functions functions
local encodeHarmMask, decodeHarmMask
local maskableHarmTypeMap = {
	[HARM_TYPE_JUMP]=true,
	[HARM_TYPE_FROMBELOW]=true,
	[HARM_TYPE_NPC]=true,
	[HARM_TYPE_PROJECTILE_USED]=true,
	[HARM_TYPE_LAVA]=true,
	[HARM_TYPE_HELD]=true,
	[HARM_TYPE_TAIL]=true,
	[HARM_TYPE_SPINJUMP]=true,
	[HARM_TYPE_OFFSCREEN]=true,
	[HARM_TYPE_SWORD]=true
}
local function decodeHarmMask(mskVal)
	local tblVal = {}
	for v,_ in pairs(maskableHarmTypeMap) do
		if (bit.band(mskVal, bit.lshift(1, v)) ~= 0) then
			table.insert(tblVal, v)
		end
	end
	return tblVal
end
local function encodeHarmMask(tblVal)
	local mskVal = 0
	for _,v in ipairs(tblVal) do
		if maskableHarmTypeMap[v] then
			mskVal = bit.bor(mskVal, bit.lshift(1, v))
		end
	end
	return mskVal
end

-- Encoder for casting to boolean
local function encodeBoolean(value)
	return (value ~= false) and (value ~= 0) and (value ~= nil)
end

-- Main property table
local propertyTables = {
	-- Vanilla properties
	gfxoffsetx        = {ptr=0x00B25B70, t=FIELD_WORD},
	gfxoffsety        = {ptr=0x00B25B8C, t=FIELD_WORD},
	width             = {ptr=0x00B25BA8, t=FIELD_WORD},
	height            = {ptr=0x00B25BC4, t=FIELD_WORD},
	gfxwidth          = {ptr=0x00B25BE0, t=FIELD_WORD},
	gfxheight         = {ptr=0x00B25BFC, t=FIELD_WORD},
	speed             = {ptr=0x00B25C18, t=FIELD_FLOAT},
	isshell           = {ptr=0x00B25C34, t=FIELD_BOOL},
	npcblock          = {ptr=0x00B25C50, t=FIELD_BOOL},
	npcblocktop       = {ptr=0x00B25C6C, t=FIELD_BOOL},
	isinteractable    = {ptr=0x00B25C88, t=FIELD_BOOL},
	iscoin            = {ptr=0x00B25CA4, t=FIELD_BOOL},
	isvine            = {ptr=0x00B25CC0, t=FIELD_BOOL},
	iscollectablegoal = {ptr=0x00B25CDC, t=FIELD_BOOL},
	isflying          = {ptr=0x00B25CF8, t=FIELD_BOOL},
	iswaternpc        = {ptr=0x00B25D14, t=FIELD_BOOL},
	jumphurt          = {ptr=0x00B25D30, t=FIELD_BOOL},
	noblockcollision  = {ptr=0x00B25D4C, t=FIELD_BOOL},
	score             = {ptr=0x00B25D68, t=FIELD_WORD},
	playerblocktop    = {ptr=0x00B25D84, t=FIELD_BOOL},
	grabtop           = {ptr=0x00B25DA0, t=FIELD_BOOL},
	cliffturn         = {ptr=0x00B25DBC, t=FIELD_BOOL},
	nohurt            = {ptr=0x00B25DD8, t=FIELD_BOOL},
	playerblock       = {ptr=0x00B25DF4, t=FIELD_BOOL},
	grabside          = {ptr=0x00B25E2C, t=FIELD_BOOL},
	isshoe            = {ptr=0x00B25E48, t=FIELD_BOOL},
	isyoshi           = {ptr=0x00B25E64, t=FIELD_BOOL},
	noyoshi           = {ptr=0x00B25E9C, t=FIELD_BOOL},
	foreground        = {ptr=0x00B25EB8, t=FIELD_BOOL},
	isbot             = {ptr=0x00B25ED4, t=FIELD_BOOL},
	isvegetable       = {ptr=0x00B25F0C, t=FIELD_BOOL},
	nofireball        = {ptr=0x00B25F28, t=FIELD_BOOL},
	noiceball         = {ptr=0x00B25F44, t=FIELD_BOOL},
	nogravity         = {ptr=0x00B25F60, t=FIELD_BOOL},
	frames            = {ptr=0x00B25F7C, t=FIELD_WORD},
	framespeed        = {ptr=0x00B25F98, t=FIELD_WORD},
	framestyle        = {ptr=0x00B25FB4, t=FIELD_WORD},
	
	-- Extended properties
	vulnerableharmtypes = {name="vulnerableharmtypes", t=FIELD_DWORD, encoder=encodeHarmMask, decoder=decodeHarmMask},
	spinjumpsafe        = {name="spinjumpsafe",        t=FIELD_BOOL},
	}

-- Deprecated aliases
local aliases = {
	blocknpc="npcblock",
	blockplayer="playerblock",
	}

-- Mapping of field types to field sizes
local fieldSizes = {
	[FIELD_WORD]   = 2,
	[FIELD_BOOL]   = 2,
	[FIELD_DWORD]  = 4,
	[FIELD_BYTE]   = 1,
	[FIELD_FLOAT]  = 4,
	[FIELD_DFLOAT] = 8,
	[FIELD_STRING] = 4,
	}

-- Resolve addresses and field sizes ahead of time
for _,tbl in pairs(propertyTables) do
	-- Fill in resolved address for vanilla types
	if (tbl.ptr ~= nil) and (tbl.addr == nil) then
		tbl.addr = mem(tbl.ptr, FIELD_DWORD)
	end
	
	-- Fill in resolved address for extended types
	if (tbl.name ~= nil) and (tbl.addr == nil) then
		tbl.addr = Misc.__getNPCPropertyTableAddress(tbl.name)
	end
	
	-- Boolean field automatically get a casting encoder
	if (tbl.encoder == nil) and (tbl.t == FIELD_BOOL) then
		tbl.encoder = encodeBoolean
	end
	
	-- Fill in stride
	tbl.stride = fieldSizes[tbl.t]
end

-- Copy entries for aliaes
for alias,target in pairs(aliases) do
	propertyTables[alias] = propertyTables[target]
end

-- Define metatable for handling 
local npcmt = {
	__newindex = function (tbl, key, value)
		local prop = propertyTables[key] or propertyTables[string.lower(key)]
		if(prop ~= nil) then
			if (prop.encoder ~= nil) then
				value = prop.encoder(value)
			end
			mem(prop.addr + prop.stride * tbl.id, prop.t, value)
		end
	end,
	__index = function (tbl, key)
		local prop = propertyTables[key] or propertyTables[string.lower(key)]
		if(prop ~= nil) then
			local value = mem(prop.addr + prop.stride * tbl.id, prop.t)
			if (prop.decoder ~= nil) then
				value = prop.decoder(value)
			end
			return value
		else
		    return nil
		end
	end
}

-- Declare the npcconfig object itself
setmetatable(npcconfig, {
	__newindex = function (tbl, key, value)
		error("Cannot assign directly to NPC config. Try assigning to a field instead.", 2)
	end,
	__index = function (tbl, key)
		if(type(key) == "number" and key >= 1 and key <= NPC_MAX_ID) then
		    local val = { id = key }
			setmetatable(val, npcmt)
			return val
		else
		    return nil
		end
	end
})

-- Patch in some semi-legacy properties on NPC class
if (NPC ~= nil) then
	local vulnerableHarmTypesMetatable = {
		__index = function(tbl, id)
			return npcconfig[id].vulnerableharmtypes
		end,
		__newindex = function(tbl, id, val)
			npcconfig[id].vulnerableharmtypes = val
		end
	}
	NPC.vulnerableHarmTypes = setmetatable({}, vulnerableHarmTypesMetatable)

	local spinjumpSafeMetatable = {
		__index = function(tbl, id)
			return npcconfig[id].spinjumpsafe
		end,
		__newindex = function(tbl, id, val)
			npcconfig[id].spinjumpsafe = val
		end
	}
	NPC.spinjumpSafe = setmetatable({}, spinjumpSafeMetatable)
end

-- Patch in a 'config' member variable in the NPC namespace
if (NPC ~= nil) then
	NPC.config = npcconfig
end

return npcconfig
