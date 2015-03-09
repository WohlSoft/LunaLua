

-- The define Table
local defines = {
	gravity           = {defValue = 12, minVal = 0, maxVal = nil, address = 0x00B2C6F4, size = FIELD_WORD},
	earthquake        = {defValue = 0 , minVal = 0, maxVal = nil, address = 0x00B250AC, size = FIELD_WORD},
	jumpheight        = {defValue = 20, minVal = 0, maxVal = nil, address = 0x00B2C6DC, size = FIELD_WORD},
	jumpheight_bounce = {defValue = 20, minVal = 0, maxVal = nil, address = 0x00B2C6E2, size = FIELD_WORD}
}

--(Re)sets a define
local function setDefine(defTable, value)
	local theValue = value or defTable.defValue
	mem(defTable.address, defTable.size, theValue)
end

local function getDefine(defTable)
	return mem(defTable.address, defTable.size)
end

--The actual host code
local definesLib  = setmetatable({
	-- On Level startup reset all defines
	onInitAPI = function()
		for _,defineTable in pairs(defines) do
			setDefine(defineTable)
		end
	end

}, {
	--Neat function to modify a define
	__newindex = function (tarTable, key, value)
		--A bunch of error checking
		assert(key)
		local theDefine = defines[key]
		if not theDefine then
			error("Field \""..tostring(key).."\" does not exist!", 2)
		end
		if type(value) ~= "number" and type(value) ~= "nil" then
			error("Value is not a number: Need number, got "..type(value).."!", 2)
		end
		if theDefine.minVal then
			if theDefine.minVal > value then
				error("Value "..value.." is smaller than the minimum value of "..theDefine.minVal.."!", 2)
			end
		end
		if theDefine.maxVal then
			if theDefine.maxVal < value then
				error("Value "..value.." is bigger than the maximum value of "..theDefine.maxVal.."!", 2)
			end
		end
		--Set the actual define
		setDefine(theDefine, value)
	end,

	__index = function (tarTable, key)
		assert(key)
		local theDefine = defines[key]
		if not theDefine then
			error("Field \""..tostring(key).."\" does not exist!", 2)
		end
		
		return getDefine(theDefine)
	end
})

return definesLib

