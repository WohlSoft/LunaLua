
if __wikiGEN then
	FIELD_BYTE = 0
	FIELD_WORD = 1
	FIELD_DWORD = 2
	FIELD_FLOAT = 3
	FIELD_DFLOAT = 4
end


-- The define Table
-- Group and desc are only for the wiki generator
local defines = {
	gravity           = {defValue = 12, minVal = 0, maxVal = nil, address = 0x00B2C6F4, size = FIELD_WORD,
	                     n = 1, group = "Movement", desc = "The gravity of the player."},
	earthquake        = {defValue = 0 , minVal = 0, maxVal = nil, address = 0x00B250AC, size = FIELD_WORD,
	                     n = 2, group = "Movement", desc = "The earthquake factor of the Level. It resets to 0 after time."},
	jumpheight        = {defValue = 20, minVal = 0, maxVal = nil, address = 0x00B2C6DC, size = FIELD_WORD,
	                     n = 3, group = "Movement", desc = "The jumpheight of the player."},
	jumpheight_bounce = {defValue = 20, minVal = 0, maxVal = nil, address = 0x00B2C6E2, size = FIELD_WORD,
	                     n = 4, group = "Movement", desc = "The jumpheight of the player when bouncing of a enemy."}
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

if not __wikiGEN then
	return definesLib
end

--[[

To generate the Wiki:
1. Set up any base lua intepreter.
2. Define __wikiGEN to true
3. Loadup this file
]]


local function generateWiki()
	print("Starting to generate wiki...")
	
	local wfile = io.open("GenWiki.txt", "w+")
	print("Generate Header Template...")
	
	wfile:write("Following code for \"Template:Defines table\":\n")
	
	wfile:write("<table style=\"width: 100%; border: solid 1px black; margin: 2px; border-spacing: 0px;\">\n")
	wfile:write("<tr><th colspan=\"5\" style=\"font-weight: bold; font-size: large; padding-bottom: .3em; border-bottom: solid #C9C9C9 1px; background: #D3FFC2; line-height:28px; text-align: left\">\n")
	wfile:write("{{{1}}}</th></tr>\n")
	wfile:write("<tr>\n")
	wfile:write("<td style=\"width: 350px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Name of Define</td>\n")
	wfile:write("<td style=\"width: 150px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Type</td>\n")
	wfile:write("<td style=\"width: 150px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Default value</td>\n")
	wfile:write("<td style=\"width: 150px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Min/Max Size</td>\n")
	wfile:write("<td style=\"background: #E0E0E0; padding: .4em; font-weight:bold;\">Description</td></tr>\n")
	wfile:write("{{{2}}}\n")
	wfile:write("</table>")

	wfile:write("\n=============================\n\n")
	io.flush(wfile)
	
	
	print("Generate Row Template...")
	wfile:write("Following code for \"Template:Defines table/row\":\n")
	
	wfile:write("<tr style=\"background-color: {{#ifeq:{{{4|}}}|odd|#FFFFFF|#E8E8E8}};\">\n")
	wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{1}}}</td>\n")
	wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{2}}}</td>\n")
	wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{3}}}</td>\n")
	wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{4}}}</td>\n")
	wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{5|}}}</td></tr>\n")
	wfile:write("\n=============================\n\n")
	io.flush(wfile)
	
	
	print("Generate SUPER AMAZING CONTENT...")
	wfile:write("Following code for the main page: \n")
	
	wfile:write("[[Category:LunaLua API]]\n")
	wfile:write("Since LunaLua v0.6.1 you can use these powerful defines to modify SMBX values. They are safe to use and will reset on every level.\n")
	
	
	local sortedTable = {}
	
	for defineName, defineTable in pairs(defines) do
		local defineGroup = defineTable.group or "Misc"
		if not sortedTable[defineGroup] then
			sortedTable[defineGroup] = {}
		end
		sortedTable[defineGroup][defineTable["n"]] = {name = defineName, dt = defineTable}
	end
	
	for defineGroup, defineGroupTable in pairs(sortedTable) do
		wfile:write("{{Defines table|"..defineGroup.."||2=\n\n")
		local oddness = true
		for _, defineTablePack in pairs(defineGroupTable) do
			local defineName = defineTablePack.name
			local defineTable = defineTablePack.dt
			wfile:write("{{Defines table/row|")
			wfile:write("Defines."..defineName.."|")
			if defineTable.size == FIELD_BYTE then
				wfile:write("BYTE|")
			elseif defineTable.size == FIELD_WORD then
				wfile:write("WORD|")
			elseif defineTable.size == FIELD_DWORD then
				wfile:write("DWORD|")
			elseif defineTable.size == FIELD_FLOAT then
				wfile:write("FLOAT|")
			elseif defineTable.size == FIELD_DFLOAT then
				wfile:write("DFLOAT (double)|")
			end
			wfile:write(tostring(defineTable.defValue).."|")
			if defineTable.minVal and defineTable.maxVal then
				wfile:write("Min. Value: "..defineTable.minVal.."<br>")
				wfile:write("Max. Value: "..defineTable.maxVal.."|")
			elseif defineTable.minVal then
				wfile:write("Min. Value: "..defineTable.minVal.."|")
			elseif defineTable.maxVal then
				wfile:write("Max. Value: "..defineTable.maxVal.."|")
			else
				wfile:write("---|")
			end
			wfile:write(defineTable.desc)
			if oddness then
				wfile:write("|odd")
			end
			wfile:write("}}\n")
			oddness = not oddness
		end
		wfile:write("\n}}\n\n\n")
		
		
	end
	io.close(wfile)
	print("Done writing and closed file")
end

if __wikiGEN then generateWiki() end
