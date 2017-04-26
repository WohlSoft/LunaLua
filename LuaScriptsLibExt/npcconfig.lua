-- Wrapper around the core version of npcconfig, which translates true/false
-- values returned back to -1/0
local npcconfig_core = API.load("core\\npcconfig_core")

-- Define metatable for handling 
local npcmt = {
	__newindex = function (tbl, key, value)
		npcconfig_core[tbl.id][key] = value
	end,
	__index = function (tbl, key)
		local value = npcconfig_core[tbl.id][key]
		
		if (value == true) then
			value = -1
		elseif (value == false) then
			value = 0
		end
		
		return value
	end
}

local npcconfig_legacy = {}
setmetatable(npcconfig_legacy, {
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

return npcconfig_legacy
