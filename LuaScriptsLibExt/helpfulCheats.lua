---------------------------
---- helpfulCheats.lua ----
------ version 0.1 --------
-- Created by Pyro, 2015 --
---------------------------

local rng = loadSharedAPI("rng")
local horikawaTools = loadSharedAPI("horikawaTools")

local helpfulCheats = {}

local rinkaCheat = false;
local rinkaCheat2 = false;
local rinkaCheat3 = false;
local horikawarad = false;

local function switchBoolean(butts)
	if butts == true then
		return false
	else
		return true
	end
end

function helpfulCheats.onInitAPI()
	registerEvent(helpfulCheats, "onLoop", "onLoop", false)
	registerEvent(helpfulCheats, "onNPCKill", "onNPCKill", false)
	registerEvent(helpfulCheats, "onKeyDown", "onKeyDown", false)
end

function helpfulCheats.onLoop()
	currentGravity = gravity()
	cheatBuffer = Misc.cheatBuffer()
	
    killcheat = string.find(cheatBuffer, "suicide", 1)
	if (killcheat ~= 0 and killcheat ~= nil) then
        player:kill()
        Misc.cheatBuffer("")
		Misc.doBombExplosion(player.x,player.y,3)
		earthquake(10)
		playSFX(22)
    end
	
	lesssupercheat = string.find(cheatBuffer, "holytrinity", 1)
	if (lesssupercheat ~= 0 and lesssupercheat ~= nil) then
        Misc.cheatBuffer("")
		Defines.cheat_donthurtme = switchBoolean(Defines.cheat_donthurtme);
		Defines.cheat_ahippinandahoppin = switchBoolean(Defines.cheat_ahippinandahoppin);
		Defines.cheat_shadowmario = switchBoolean(Defines.cheat_shadowmario);
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(34)
    end
	
	supercheat = string.find(cheatBuffer, "theessentials", 1)
	if (supercheat ~= 0 and supercheat ~= nil) then
        Misc.cheatBuffer("")
		Defines.cheat_donthurtme = switchBoolean(Defines.cheat_donthurtme);
		Defines.cheat_sonictooslow = switchBoolean(Defines.cheat_sonictooslow);
		Defines.cheat_ahippinandahoppin = switchBoolean(Defines.cheat_ahippinandahoppin);
		Defines.cheat_shadowmario = switchBoolean(Defines.cheat_shadowmario);
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(34)
    end
	
	jumpman = string.find(cheatBuffer, "jumpman", 1)
	if (jumpman ~= 0 and jumpman ~= nil) then
        Misc.cheatBuffer("")
		Defines.cheat_ahippinandahoppin = switchBoolean(Defines.cheat_ahippinandahoppin);
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(6)
    end
	
	bootrocket = string.find(cheatBuffer, "horikawaisradicola", 1)
	if (bootrocket ~= 0 and bootrocket ~= nil) then
		radicola = switchBoolean(radicola)
        Misc.cheatBuffer("")
		earthquake(10)
		playSFX(34)
    end
	
	instantlives = string.find(cheatBuffer, "liveforever", 1)
	if (instantlives ~= 0 and instantlives ~= nil) then
        Misc.cheatBuffer("")
		mem(0x00B2C5AC,FIELD_FLOAT,99)
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(15)
    end
	
	rinka = string.find(cheatBuffer, "rinkamania", 1)
	if (rinka ~= 0 and rinka ~= nil) then
        Misc.cheatBuffer("")
		rinkaCheat = switchBoolean(rinkaCheat);
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(65)
    end
	
	rinka2 = string.find(cheatBuffer, "jumpforrinka", 1)
	if (rinka2 ~= 0 and rinka2 ~= nil) then
        Misc.cheatBuffer("")
		rinkaCheat2 = switchBoolean(rinkaCheat2);
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(65)
    end
	
	rinka3 = string.find(cheatBuffer, "rinkamadness", 1)
	if (rinka3 ~= 0 and rinka3 ~= nil) then
        Misc.cheatBuffer("")
		rinkaCheat3 = switchBoolean(rinkaCheat3);
		player:mem(0x140, FIELD_WORD, 30)
		playSFX(65)
    end
	
	launcher = string.find(cheatBuffer, "launchme", 1)
	if (launcher ~= 0 and launcher ~= nil) then
        Misc.cheatBuffer("")
		player.speedY = -30;
		playSFX(61)
    end
	
	powe = string.find(cheatBuffer, "boomtheroom", 1)
	if (powe ~= 0 and powe ~= nil) then
        Misc.cheatBuffer("")
		Misc.doPOW()
		playSFX(34)
    end
	
	powe2 = string.find(cheatBuffer, "instantswitch", 1)
	if (powe2 ~= 0 and powe2 ~= nil) then
        Misc.cheatBuffer("")
		Misc.doPSwitch()
		playSFX(34)
    end
	
	powe3 = string.find(cheatBuffer, "butinspace", 1)
	if (powe3 ~= 0 and powe3 ~= nil) then
        Misc.cheatBuffer("")
		if currentGravity == 12 then
			gravity(2)
			jumpheight(60)
		else	
			gravity(12)
			jumpheight(20)
		end
		playSFX(34)
    end
	
	powe4 = string.find(cheatBuffer, "murder", 1)
	if (powe4 ~= 0 and powe4 ~= nil) then
        Misc.cheatBuffer("")
		for k, v in pairs (NPC.get(horikawaTools.hittableNPCs,player.section)) do
			if v:mem(0x128,FIELD_WORD) ~= -1 then
				v:kill()
			end
		end
		playSFX(22)
		earthquake(50)
    end
	
	--mem(0x00B2C5AC,FIELD_FLOAT,99)
	
	Defines.player_hasCheated = false;
	
	if rinkaCheat3 == true and rng.randomInt(1,20) == 20 then
		NPC.spawn(210,player.x + rng.randomInt(-800,800),player.y + rng.randomInt(-600,600),player.section)
	end
	
	if radicola == true then
		player:mem(0x108, FIELD_WORD, 1) 
		player:mem(0x10A, FIELD_WORD, 1)
	end

end

function helpfulCheats.onNPCKill(eventObj,killedNPC,killReason)
	if rinkaCheat == true and killReason ~= 9 then
		NPC.spawn(210,killedNPC.x,killedNPC.y,player.section)
	end
end

function helpfulCheats.onKeyDown(keycode)
	if rinkaCheat2 == true and keycode == KEY_JUMP then
		NPC.spawn(210,player.x,player.y + 64,player.section)
	end
end

return helpfulCheats