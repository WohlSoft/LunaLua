--characters.lua
--v1.1.2
--Created by Horikawa Otane, 2015

local characters = {}

local playerCharacters = {}
playerCharacters[1] = "mario"
playerCharacters[2] = "luigi"
playerCharacters[3] = "peach"
playerCharacters[4] = "toad"
playerCharacters[5] = "link"

local intIds = {}
intIds["mario"] = 1
intIds["luigi"] = 2
intIds["peach"] = 3
intIds["toad"] = 4
intIds["link"] = 5

local playerCharacters = {"mario", "luigi", "peach", "toad", "link"}


local function resetCharacter(characterId, powerupId)
	loadHitboxes(characterId, powerupId, Misc.resolveFile("character_defaults\\" .. playerCharacters[characterId] .. "-" .. tostring(powerupId) .. ".ini"))
end

local function loadAllHitBoxes()
	for _, butts in pairs(playerCharacters) do
		for i = 1, 7, 1 do
			local theIniFile = Misc.resolveFile(butts .. "-" .. i .. ".ini")
			if  theIniFile ~= nil then
				loadHitboxes(intIds[butts], i, theIniFile)
			else
				resetCharacter(intIds[butts], i)
			end
		end
	end
end

function characters.onInitAPI()
	loadAllHitBoxes()
end

return characters