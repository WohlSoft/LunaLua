--[[
·▄▄▄▄  ▄▄▄ . ▄▄▄· ▄▄▄▄▄ ▄ .▄▄▄▄▄▄▄▄▄   ▄▄▄·  ▄▄· ▄ •▄ ▄▄▄ .▄▄▄     ▄▄▌  ▄• ▄▌ ▄▄▄· 
██▪ ██ ▀▄.▀·▐█ ▀█ •██  ██▪▐█•██  ▀▄ █·▐█ ▀█ ▐█ ▌▪█▌▄▌▪▀▄.▀·▀▄ █·   ██•  █▪██▌▐█ ▀█ 
▐█· ▐█▌▐▀▀▪▄▄█▀▀█  ▐█.▪██▀▐█ ▐█.▪▐▀▀▄ ▄█▀▀█ ██ ▄▄▐▀▀▄·▐▀▀▪▄▐▀▀▄    ██▪  █▌▐█▌▄█▀▀█ 
██. ██ ▐█▄▄▌▐█ ▪▐▌ ▐█▌·██▌▐▀ ▐█▌·▐█•█▌▐█ ▪▐▌▐███▌▐█.█▌▐█▄▄▌▐█•█▌   ▐█▌▐▌▐█▄█▌▐█ ▪▐▌
▀▀▀▀▀•  ▀▀▀  ▀  ▀  ▀▀▀ ▀▀▀ · ▀▀▀ .▀  ▀ ▀  ▀ ·▀▀▀ ·▀  ▀ ▀▀▀ .▀  ▀ ▀ .▀▀▀  ▀▀▀  ▀  ▀ 
--V. 1.0
--written by Enjl
]]
local lvlDeath = Data(Data.DATA_LEVEL, "deathTracker", true)

local lastPlayerX = 0
local lastPlayerY = 0
local deathCounter = 0

local hasDied = false

local deathTracker = {}

local shownIcons = {}

deathTracker.ICON_STANDARD = Graphics.loadImage(Misc.resolveFile("deathTracker\\deathIcon.png"))
deathTracker.ICON_STANDARD2X = Graphics.loadImage(Misc.resolveFile("deathTracker\\deathIcon2x.png"))
deathTracker.ICON_ARROW = Graphics.loadImage(Misc.resolveFile("deathTracker\\arrowIcon.png"))
deathTracker.ICON_ARROW2X = Graphics.loadImage(Misc.resolveFile("deathTracker\\arrowIcon2x.png"))
deathTracker.ICON_PLAYER1 = Graphics.loadImage(Misc.resolveFile("deathTracker\\player1.png"))
deathTracker.ICON_PLAYER2 = Graphics.loadImage(Misc.resolveFile("deathTracker\\player2.png"))
deathTracker.ICON_PLAYER3 = Graphics.loadImage(Misc.resolveFile("deathTracker\\player3.png"))
deathTracker.ICON_PLAYER4 = Graphics.loadImage(Misc.resolveFile("deathTracker\\player4.png"))
deathTracker.ICON_PLAYER5 = Graphics.loadImage(Misc.resolveFile("deathTracker\\player5.png"))
deathTracker.ICON_TRUMP = Graphics.loadImage(Misc.resolveFile("deathTracker\\trump.png"))
deathTracker.ICON_SWEAT = Graphics.loadImage(Misc.resolveFile("deathTracker\\sweat smile.png"))
deathTracker.ICON_CAT = Graphics.loadImage(Misc.resolveFile("deathTracker\\cat.png"))

deathTracker.iconSprite = deathTracker.ICON_STANDARD2X

local function iconHandler()
	for i=1, deathCounter do
		local entry = {}
		entry.x = tonumber(lvlDeath:get("deathX" .. tostring(i)))
		entry.y = tonumber(lvlDeath:get("deathY" .. tostring(i)))
		entry.timer = 0
		entry.opacity = 0
		entry.sprite = deathTracker.iconSprite
		table.insert(shownIcons, entry)
	end
end

function deathTracker.onInitAPI()
	registerEvent(deathTracker, "onTick", "onTick", false)
	registerEvent(deathTracker, "onDraw", "onDraw", false)
	registerEvent(deathTracker, "onStart", "onStart", false)
end

function deathTracker.onStart()
	if lvlDeath:get("deaths") == nil then
		lvlDeath:set("deaths", 0)
		lvlDeath:save()
	end
	deathCounter = tonumber(lvlDeath:get("deaths")) or 0
end

function deathTracker.onDraw()
	local cam = Camera.get()[1]
	for k,v in ipairs(shownIcons) do
		if v.x + 0.5 * v.sprite.width > cam.x and v.x -0.5 * v.sprite.width < cam.x + 800 and v.y + 0.5 * v.sprite.height > cam.y and v.y -0.5 * v.sprite.height < cam.y + 600 then
			Graphics.drawImageToScene(v.sprite, v.x - 0.5 * v.sprite.width, v.y - v.sprite.height, v.opacity)
		end
	end
end

function deathTracker.onTick()
	local cam = Camera.get()[1]
	--track player position for onscreen depiction of offscreen deaths
	if player.x + player.width > cam.x and player.x < cam.x + 800 then
		lastPlayerX = player.x + 0.5 * player.width
	end
	if player.y + player.height > cam.y and player.y < cam.y + 600 then
		lastPlayerY = player.y + 0.5 * player.height
	end
	--add to counter
	if player:mem(0x13E, FIELD_WORD) > 0 then
		if hasDied == false then
			hasDied = true
			deathCounter = deathCounter + 1
			lvlDeath:set("deaths", tostring(deathCounter))
			lvlDeath:set("deathX" .. tostring(deathCounter), tostring(lastPlayerX))
			lvlDeath:set("deathY" .. tostring(deathCounter), tostring(lastPlayerY))
			lvlDeath:save()
			iconHandler()
		end
		for k,v in ipairs(shownIcons) do
			v.timer = v.timer + 1
			if v.timer > 50 then
				v.opacity = v.opacity + 0.1
			end
		end
	end
end

return deathTracker;
--cat planet cat planet