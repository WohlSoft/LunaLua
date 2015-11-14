--npcconfig.lua 
--v0.0.1a
--Created by Hoeloe, 2015
local npcconfig = {}

local npcmt = {}

local ptrs = { 
	gfxoffsetx = 0x00B25B70, 
	gfxoffsety = 0x00B25B8C, 
	width = 0x00B25BA8, 
	height = 0x00B25BC4, 
	gfxwidth = 0x00B25BE0, 
	gfxheight = 0x00B25BFC,
	speed = 0x00B25C18,
	isshell = 0x00B25C34,
	blocknpc = 0x00B25C50,
	blocknpctop = 0x00B25C6C,
	isinteractable = 0x00B25C88,
	iscoin = 0x00B25CA4,
	isvine = 0x00B25CC0,
	iscollectablegoal = 0x00B25CDC,
	isflying = 0x00B25CF8,
	iswaternpc = 0x00B25D14,
	jumphurt = 0x00B25D30,
	noblockcollision = 0x00B25D4C,
	score = 0x00B25D68,
	blockplayertop = 0x00B25D84,
	grabtop = 0x00B25DA0,
	cliffturn = 0x00B25DBC,
	nohurt = 0x00B25DD8,
	blockplayer = 0x00B25DF4,
	grabside = 0x00B25E2C,
	isshoe = 0x00B25E48,
	isyoshi = 0x00B25E64,
	noyoshi = 0x00B25E9C,
	foreground = 0x00B25EB8,
	isbot = 0x00B25ED4,
	isvegetable = 0x00B25F0C,
	nofireball = 0x00B25F28,
	noiceball = 0x00B25F44,
	nogravity = 0x00B25F60,
	frames = 0x00B25F7C,
	framespeed = 0x00B25F98,
	framestyle = 0x00B25FB4,
	
	--Default values
	def_gfxoffsetx = 0x00B25FC0, 
	def_gfxoffsety = 0x00B2621A, 
	def_width = 0x00B26474, 
	def_height = 0x00B266CE, 
	def_gfxwidth = 0x00B26928, 
	def_gfxheight = 0x00B26B82,
	def_speed = 0x00B2AD5C,
	def_isshell = 0x00B26DDC,
	def_blocknpc = 0x00B27036,
	def_blocknpctop = 0x00B27290,
	def_isinteractable = 0x00B274EA,
	def_iscoin = 0x00B27744,
	def_isvine = 0x00B2799E,
	def_iscollectablegoal = 0x00B27BF8,
	def_isflying = 0x00B27E52,
	def_iswaternpc = 0x00B280AC,
	def_jumphurt = 0x00B28306,
	def_noblockcollision = 0x00B28560,
	def_score = 0x00B287BA,
	def_blockplayertop = 0x00B28A14,
	def_grabtop = 0x00B28C6E,
	def_cliffturn = 0x00B28EC8,
	def_nohurt = 0x00B29122,
	def_blockplayer = 0x00B2937C,
	def_grabside = 0x00B29830,
	def_isshoe = 0x00B29A8A,
	def_isyoshi = 0x00B29CE4,
	def_noyoshi = 0x00B2A198,
	def_foreground = 0x00B2A3F2,
	def_isbot = 0x00B2A64C,
	def_isvegetable = 0x00B2AB00,
	def_nofireball = 0x00B2B210,
	def_noiceball = 0x00B2B46A,
	def_nogravity = 0x00B2B6C4
	}
	
local function getBytes(field)
	if(field == FIELD_WORD) then return 2;
	elseif(field == FIELD_DWORD) then return 4;
	elseif(field == FIELD_BYTE) then return 1;
	elseif(field == FIELD_FLOAT) then return 4;
	elseif(field == FIELD_DFLOAT) then return 8;
	elseif(field == FIELD_STRING) then return 4;
	else
		error("ERROR: Unknown data type when accessing NPC config.", 2);
	end
end

local function getDataType(key)
	if(key == "speed" or key == "def_speed") then 
		return FIELD_FLOAT 
	else 
		return FIELD_WORD 
	end;
end

npcmt = {
	__newindex = function (tbl, key, value)
		if(ptrs[key] ~= nil) then
			local ptr = mem(ptrs[key], FIELD_DWORD);
			local dat = getDataType(key);
		    mem(ptr + getBytes(dat)*(tbl.id),dat, value);
		end
	end,
	__index = function (tbl, key)
		if(ptrs[key] ~= nil) then
			local ptr = mem(ptrs[key], FIELD_DWORD);
			local dat = getDataType(key);
		    return mem(ptr + getBytes(dat)*(tbl.id),dat);
		else
		    return nil;
		end
	end
}

setmetatable(npcconfig, {
	__newindex = function (tbl, key, value)
			error("Cannot assign directly to NPC config. Try assigning to a field instead.", 2)
	end,
	__index = function (tbl, key)
		if(type(key) == "number" and key >= 1 and key <= 292) then
		    local val = { id = key };
			setmetatable(val, npcmt);
			return val;
		else
		    return nil;
		end
	end
})

return npcconfig;