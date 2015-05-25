--------------------------------------------------------------------
--============== Persistent NPC Reference Framework ==============--
--------------------------------------------------------------------

local pNPC = {}
local ID_FIELD = 0xE6 -- (Presumably) padding before animation timer. Different from the padding location cinematX uses
local __activeRefs = {}
local __nextId = 1
local __nextUID = 1

-- Runs an update of references
local function runReferenceUpdate()
	-- Create a new table of up-to-date references
	local newActiveTable = {}
	for i,npc in pairs(findnpcs(-1, -1)) do
		-- Check if the NPC has a Id and it matches one we know about
		local id = npc:mem(ID_FIELD, FIELD_WORD)
		if (id ~= 0 and __activeRefs[id] ~= nil) then
			-- Copy the reference into the new table and update it
			newActiveTable[id] = __activeRefs[id]
			newActiveTable[id].__id = id
			newActiveTable[id].__ref = npc
		end
	end

	-- Iterate over old table to invalidate references which are no longer valid
	for id,tbl in pairs(__activeRefs) do
		if (newActiveTable[id] ~= tbl) then
			tbl.__id = -1
			tbl.__ref = 0
		end
	end
	
	-- Replace the old table of active references
	__activeRefs = newActiveTable
end

-- Internal function to assign a new Id. It will re-use old Ids after wrapping
-- around, if they're detected to not be in use
local function getNewId()
	for i = 1,2 do
		local startingIndex = __nextId
		repeat
			local newId = __nextId
			if (newId > 0x7FFF) then
				newId = 1
			end
			__nextId = newId + 1
			if (__activeRefs[newId] == nil) then
				local newUID = __nextUID
				__nextUID = __nextUID + 1
				return newId, newUID
			end
		until __nextId == startingIndex
		
		-- See if we can clear up old references and then try again
		runReferenceUpdate()
	end
	
	error("pNPC could not get new Id! Shouldn't happen because we support more Ids than SMBX supports NPCs!")
	return nil
end

-- Function to check if a reference's currently cached NPC object is valid,
-- and if not, trigger a global NPC reference update
local function validateRef(tbl)
	-- If already invalid, exit validation quick
	if (tbl.__id == -1) then
		return
	end

	-- Check cached reference for validity, because it will stay valid if the
	-- NPC array hasn't shifted around.
	if ((tbl.__ref.isValid ~= false) and tbl.__id == tbl.__ref:mem(ID_FIELD, FIELD_WORD)) then
		return
	end
	
	-- Our reference was invalid, so force an update of all references
	runReferenceUpdate()
end

-- Metatable definition for our persistent NPC references
local NPCMetaTable = {}
function NPCMetaTable.__newindex(tbl, key, value)
	-- Make sure our reference is up-to-date
	validateRef(tbl)

	-- Check the reference validity
	if (tbl.__id == -1) then error("Invalid access of pNPC reference") end
	
	-- Set the value
	tbl.__ref[key] = value
end
function NPCMetaTable.__index(tbl, key)
	-- Redirect mem/kill calls, only because we can't pass our wrapper as
	-- 'self' to the C++ based functions.
	if (key == "mem") then return NPCMetaTable.mem end
	if (key == "kill") then return NPCMetaTable.kill end
	
	-- Make sure our reference is up-to-date
	validateRef(tbl)
	
	-- Map "isValid" to our internal reference validity
	if (key == "isValid") then return (tbl.__id ~= -1) end
	
	-- Read-only ID related properties
	if (key == "uid") then return tbl.__uid end
	if (key == "pid") then return tbl.__id end
	if (key == "pidIsDirty") then return (tbl.__uid < 0x8000) end
	
	-- Check the reference validity
	if (tbl.__id == -1) then error("Invalid access of pNPC reference") end
	
	-- Get the normal value
	local val = tbl.__ref[key]
	
	-- Wrap functions that were patched on
	if (type(val) == "function") then
		return (function(self, ...)
			validateRef(self)
			if (self.__id == -1) then error("Invalid access of pNPC reference") end
			return val(self.__ref, ...)
		end)
	end
	
	-- Get the normal NPC value
	return val
end
function NPCMetaTable:mem(...)
	validateRef(self)
	if (self.__id == -1) then error("Invalid access of pNPC reference") end
	return self.__ref:mem(...)
end
function NPCMetaTable:kill(...)
	validateRef(self)
	if (self.__id == -1) then error("Invalid access of pNPC reference") end
	return self.__ref:kill(...)
end

-- Function to get an existing wrapper for a NPC without trying to create a new one
function pNPC.getExistingWrapper(npc)
	local id = npc:mem(ID_FIELD, FIELD_WORD)
	return __activeRefs[id]
end

-- Function a user uses to get a reference
function pNPC.wrap(npc)
	local ref = pNPC.getExistingWrapper(npc)
	
	if (ref ~= nil) then
		return ref
	end
	
	-- Assign new Id
	id, uid = getNewId()
	npc:mem(ID_FIELD, FIELD_WORD, id)
		
	-- Create our new persistent reference
	local ref = {
		__id = id,
		__uid = uid,
		__ref = npc,
		data = {}
	}
	setmetatable(ref, NPCMetaTable);
	__activeRefs[id] = ref
	
	return ref
end

return pNPC
