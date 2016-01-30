--***************************************************************************************
--                                                                                      *
--  playerGfx.lua                                                                       *
--  v2.0b                                                                               *
--  Documentation: http://engine.wohlnet.ru/pgewiki/PlayerGfx.lua                       *
--                                                                                      *
--***************************************************************************************



local playerGfx = {} --Package table
local inputs = loadSharedAPI("inputs");
--local textblox = loadSharedAPI("textblox");

function playerGfx.onInitAPI () --Is called when the api is loaded by loadAPI.
	registerEvent(playerGfx, "onLoop", "update", true) --Register the loop event
	registerEvent(playerGfx, "onHUDDraw", "onHUDDraw", false) --Register the loop event
	--registerEvent(playerGfx, "onMessageBox", "onMessageBox", true) --Register the loop event
end

--***************************************************************************************************
--                                                                                                  *
--              INITIALIZATION                                                                      *
--                                                                                                  *
--***************************************************************************************************
do
	isMessageBoxActive = false

	playerGfx.defPlayerOffsetsX = {}
	playerGfx.defPlayerOffsetsY = {}
	playerGfx.defPlayerHitboxW = {}
	playerGfx.defPlayerHitboxH = {}
	playerGfx.defPlayerHitboxDuckH = {}

	
	playerGfx.currentSet = {}
	
	playerGfx.currentSet[CHARACTER_MARIO] = nil
	playerGfx.currentSet[CHARACTER_LUIGI] = nil
	playerGfx.currentSet[CHARACTER_PEACH] = nil
	playerGfx.currentSet[CHARACTER_TOAD] = nil
	playerGfx.currentSet[CHARACTER_LINK] = nil

	
	playerGfx.currentAccessories = {}
	
	playerGfx.currentAccessories[CHARACTER_MARIO] = {}
	playerGfx.currentAccessories[CHARACTER_LUIGI] = {}
	playerGfx.currentAccessories[CHARACTER_PEACH] = {}
	playerGfx.currentAccessories[CHARACTER_TOAD] = {}
	playerGfx.currentAccessories[CHARACTER_LINK] = {}

	
	playerGfx.isOn = true
	playerGfx.setsOn = true
	playerGfx.accessoriesOn = true

	playerGfx.initDone = false


	playerGfx.charTable = {CHARACTER_MARIO, CHARACTER_LUIGI, CHARACTER_PEACH, CHARACTER_TOAD, CHARACTER_LINK}
	playerGfx.powerupTable = {PLAYER_SMALL, PLAYER_BIG, PLAYER_FIREFLOWER, PLAYER_LEAF, PLAYER_TANOOKIE, PLAYER_HAMMER, PLAYER_ICE}
	
	playerGfx.debug = false
end	



	
--***************************************************************************************************
--                                                                                                  *
--              ACCESSORY CLASS                                                                     *
--      This stuff's not finished yet so I can't guarantee it'll work.  Sorry!                      *
--                                                                                                  *
--***************************************************************************************************	
do
	playerGfx.ACCALIGN_NONE = 0
	playerGfx.ACCALIGN_HEAD = 1
	playerGfx.ACCALIGN_FEET = 2
	playerGfx.ACCALIGN_BODY = 3

	
	PlayerAccessory = {}
	PlayerAccessory.__index = PlayerAccessory

	function PlayerAccessory.create (filename, properties)
		local thisAcc = {}
		setmetatable (thisAcc, PlayerAccessory)
		
		thisAcc.filename = {}
		thisAcc.images = {}
		
		for  k,v in pairs (imageArray)  do
			thisAcc.filenames[k] = v
			thisAcc.images[k] = Graphics.loadImage (filenameArray[k])
		end
		
		thisAcc.currentFrame = 0
		thisAcc.frameCount = properties["frames"] or 1
		thisAcc.alignMode = properties["align"] or playerGfx.ACCALIGN_HEAD
		thisAcc.priority = properties["priority"] or 1
		thisAcc.offsetX = properties["offsetX"] or 0
		thisAcc.offsetY = properties["offsetY"] or 0
		
		return thisAcc
	end	
	
	
	function PlayerAccessory:draw (charIndex)
		local playSet = player:getCurrentPlayerSetting()
		
		
		for  k,v  in pairs (Player.get())  do
			
			-- Check to make sure the character index is correct
			if  v.character == charIndex  and  v:mem(0x13E, FIELD_WORD) < 1  and  v:mem(0x10, FIELD_WORD) < 1  then
				local gfxIndexX, gfxIndexY = v:getCurrentSpriteIndex ()
				local powerupIndex = v.powerup
				
				if self.powerupIndex > PLAYER_ICE  then
					powerupIndex = self.powerupIndex
				else
					self.powerupIndex = powerupIndex
				end
				
				local defOffsetX = playerGfx.defPlayerOffsetsX [charIndex*10000 + powerupIndex*100 + 10*gfxIndexX + gfxIndexY] or 0
				local defOffsetY = playerGfx.defPlayerOffsetsY [charIndex*10000 + powerupIndex*100 + 10*gfxIndexX + gfxIndexY] or 0
				local hitboxW = playerGfx.defPlayerHitboxW [charIndex*10 + powerupIndex*1]
				local hitboxH = playerGfx.defPlayerHitboxH [charIndex*10 + powerupIndex*1]
				local hitboxSmallH = playerGfx.defPlayerHitboxH [charIndex*10 + 1]
				local hitboxBigH = playerGfx.defPlayerHitboxH [charIndex*10 + 2]
				local hitboxDuckH = playerGfx.defPlayerHitboxDuckH [charIndex*10 + powerupIndex*1]
				
				
				-- Figure out cropping for pipes, mounts, etc.
				local cropLeft = 0
				local cropRight = 0
				local cropTop = 0
				local cropBottom = 0
				
				local shouldCropLeft = false
				local shouldCropRight = false
				local shouldCropTop = false
				local shouldCropBottom = false
				
				local forcedAnimState = v:mem(0x122, FIELD_WORD)
				local pipeEnterTimer = v:mem(0x17A, FIELD_WORD)
				local pipeExitTimer = v:mem(0x124, FIELD_DFLOAT)
				local warpId = v:mem(0x15E, FIELD_WORD)
				
				local slopeMod = v:mem(0x48, FIELD_WORD)
				local mountOffsetY = v:mem(0x10E, FIELD_WORD)
				local mountId = v:mem (0x108, FIELD_WORD)
				local playerDucking = v:mem (0x12E, FIELD_WORD)
							
				local copterOffsetX = v.width*0.5-10

				
				local hitboxCurrentH = hitboxH
				if  playerDucking == true  then
					hitboxCurrentH = hitboxDuckH
				end
				
				
				-- Debug stuff
				if  playerGfx.debug == true  then
					Text.print ("forcedAnim: "..tostring(forcedAnimState), 4, 30, 60 + 100*k)
					Text.print ("enterTimer: "..tostring(pipeEnterTimer), 4, 30, 80 + 100*k)
					Text.print ("exitTimer:  "..tostring(pipeExitTimer), 4, 30, 100 + 100*k)
					Text.print ("mountOffY:  "..tostring(mountOffsetY), 4, 30, 120 + 100*k)
					Text.print ("height:     "..tostring(hitboxCurrentH), 4, 30, 140 + 100*k)

					Text.print ("defOffsets: "..tostring(defOffsetX)..", "..tostring(defOffsetY), 4, 30, 160 + 100*k)
				end
				
				
				-- calculate offsets
				local tempOffsetX = self.offsetX + defOffsetX-- + v.speedX
				local tempOffsetY = self.offsetY + defOffsetY + mountOffsetY-- + v.speedY
				
				--- kuribo's shoe fix
				if  mountId == 1  then
					shouldCropBottom = true
					tempOffsetY = tempOffsetY - mountOffsetY
					if  powerupIndex == PLAYER_SMALL  then
						cropBottom = 5 --79-self.offsetY+defOffsetY
					else
						cropBottom = 29 --75-self.offsetY+defOffsetY
					end
				
				--- clown copter fix
				elseif  mountId == 2  then
					tempOffsetX = tempOffsetX + copterOffsetX
					shouldCropBottom = true
					tempOffsetY = tempOffsetY - (hitboxCurrentH-20)
					cropBottom = hitboxCurrentH-35
					
					if  hitboxCurrentH == hitboxSmallH  and  hitboxSmallH ~= hitboxBigH  then
						tempOffsetY = tempOffsetY - (hitboxBigH - hitboxSmallH - 8)
						cropBottom = cropBottom + 8
					end					
					
					--64-self.offsetY+defOffsetY
					
					--[[if  powerupIndex == PLAYER_SMALL  then
						tempOffsetY = tempOffsetY - 24
						cropBottom = 5--78-self.offsetY+defOffsetY
					else
						tempOffsetY = tempOffsetY - 36
						cropBottom = 17--64-self.offsetY+defOffsetY
					end]]

				elseif  mountID ~= 0  then

				end
				
				
				-- Move the bottom crop up to the feet
				if  shouldCropBottom == true  then
					cropBottom = cropBottom + (100 - hitboxCurrentH + self.offsetY + defOffsetY)
				end

				-- Move the right crop up to the feet
				if  shouldCropRight == true  then
					cropRight = cropRight + (100 - hitboxW + self.offsetX + defOffsetX)
				end				
				
				-- Perform the draw
				if  v:mem (0x142, FIELD_WORD) == 0  					and  -- Powerup blinking state
					v:mem(0x122, FIELD_WORD) ~= 8   					and  -- Forced anim state (invisible)
					(mountId == 1  and  playerDucking == -1) == false  	and
					(cropTop + cropBottom) < 100						and
					(cropLeft + cropRight) < 100						--and
					then   
					if  self.images[powerupIndex] ~= nil  then
						Graphics.drawImageToSceneWP (self.images[powerupIndex], 
													v.x + tempOffsetX + cropLeft, 
													v.y + tempOffsetY + cropTop, 
													100*gfxIndexX,
													100*gfxIndexY + cropTop, 
													100,
													math.max(0, 100 - cropBottom - cropTop),
													0.5)
					else
						Text.windowDebug("Nil image error: "..self.filenames[powerupIndex])
					end
				end
			end
			
		end
	end

	
	
	function playerGfx.assignAccessory (playerChar, accessory)
		table.insert(playerGfx.currentAccessories[playerChar], accessory)
	end
	
	function playerGfx.removeAccessory (playerChar, accessory)
		--table.remove()
	end
	
end



	
--***************************************************************************************************
--                                                                                                  *
--              GFX SET CLASS                                                                       *
--                                                                                                  *
--***************************************************************************************************
do

	PlayerGfxSet = {}
	PlayerGfxSet.__index = PlayerGfxSet

	function PlayerGfxSet.create (filenameArray, offX, offY, overwr)
		return PlayerGfxSet.createEx (filenameArray, {offsetX=offX, offsetY=offY, overwrite=overwr})
	end
	
	function PlayerGfxSet.createEx (playerImageArray, properties)
		local thisSet = {}
		setmetatable (thisSet, PlayerGfxSet)
		
		thisSet.filenames = {}
		thisSet.images = {}
		
		for  k,v in pairs (playerImageArray)  do
			thisSet.filenames[k] = v
			thisSet.images[k] = Graphics.loadImage (playerImageArray[k])
		end

		
		thisSet.powerupIndex = 0
		
		thisSet.npcs = properties["npcs"] or {}
		thisSet.inis = properties["inis"] or {}
		thisSet.frameRemaps = properties["frameRemaps"] or {}
		thisSet.fixedFrameX = nil
		thisSet.fixedFrameY = nil
		thisSet.offsetX = properties["offsetX"] or 0
		thisSet.offsetY = properties["offsetY"] or 0
		
		thisSet.overwrite = properties["overwrite"]
		if  thisSet.overwrite == nil  then
			thisSet.overwrite = true
		end
		
		return thisSet
	end	


	function PlayerGfxSet:readIni (path)
		local p = Misc.resolveFile (path);
		if (p == nil) then return nil; end
		
		if (string.sub(p,-3) ~= ".ini") then
				p = p..".ini";
		end
   
		local ad = {};
		local f = io.open(p, "r");
   
		for t in f:lines() do
			--[[
			local ts = split(t,"=");
	   
			if(#ts ~= 2) then
				error("Parse error. Invalid line structure: "..t,2)
			end
					   
			for k,v in ipairs(ts) do
				ts[k] = string.lower(trim(v));
			end
	   
			local cnt = false;
			local index = 1;
	   
			if(ts[1]== "frames") then
				ad[cinematX.ANIMSTATE_NUMFRAMES] = tonumber(ts[2]);
				cnt=true;
				if(ad[cinematX.ANIMSTATE_NUMFRAMES] == nil) then
					error("Parse error. Not a valid frame number: "..ts[2],2)
				end
			elseif(ts[1] == "idle") then
				index = cinematX.ANIMSTATE_IDLE;
			elseif(ts[1] == "talk") then
				index = cinematX.ANIMSTATE_TALK;
			elseif(ts[1] == "talk1") then
				index = cinematX.ANIMSTATE_TALK1;
			elseif(ts[1] == "talk2") then
				index = cinematX.ANIMSTATE_TALK2;
			elseif(ts[1] == "talk3") then
				index = cinematX.ANIMSTATE_TALK3;
			elseif(ts[1] == "talk4") then
				index = cinematX.ANIMSTATE_TALK4;
			elseif(ts[1] == "talk5") then
				index = cinematX.ANIMSTATE_TALK5;
			elseif(ts[1] == "talk6") then
				index = cinematX.ANIMSTATE_TALK6;
			elseif(ts[1] == "talk7") then
				index = cinematX.ANIMSTATE_TALK7;
			elseif(ts[1] == "walk") then
				index = cinematX.ANIMSTATE_WALK;
			elseif(ts[1] == "run") then
				index = cinematX.ANIMSTATE_RUN;
			elseif(ts[1] == "jump") then
				index = cinematX.ANIMSTATE_JUMP;
			elseif(ts[1] == "fall") then
				index = cinematX.ANIMSTATE_FALL;
			elseif(ts[1] == "climb") then
				index = cinematX.ANIMSTATE_CLIMB;
			elseif(ts[1] == "hurt") then
				index = cinematX.ANIMSTATE_HURT;
			elseif(ts[1] == "stun") then
				index = cinematX.ANIMSTATE_STUN;
			elseif(ts[1] == "defeat") then
				index = cinematX.ANIMSTATE_DEFEAT;
			elseif(ts[1] == "attack") then
				index = cinematX.ANIMSTATE_ATTACK;
			elseif(ts[1] == "attack1") then
				index = cinematX.ANIMSTATE_ATTACK1;
			elseif(ts[1] == "attack2") then
				index = cinematX.ANIMSTATE_ATTACK2;
			elseif(ts[1] == "attack3") then
				index = cinematX.ANIMSTATE_ATTACK3;
			elseif(ts[1] == "attack4") then
				index = cinematX.ANIMSTATE_ATTACK4;
			elseif(ts[1] == "attack5") then
				index = cinematX.ANIMSTATE_ATTACK5;
			elseif(ts[1] == "attack6") then
				index = cinematX.ANIMSTATE_ATTACK6;
			elseif(ts[1] == "attack7") then
				index = cinematX.ANIMSTATE_ATTACK7;
			elseif(ts[1] == "grab") then
				index = cinematX.ANIMSTATE_GRAB;
			elseif(ts[1] == "grabwalk") then
				index = cinematX.ANIMSTATE_GRABWALK;
			elseif(ts[1] == "grabrun") then
				index = cinematX.ANIMSTATE_GRABRUN;
			elseif(ts[1] == "grabjump") then
				index = cinematX.ANIMSTATE_GRABJUMP;
			elseif(ts[1] == "grabfall") then
				index = cinematX.ANIMSTATE_GRABFALL;
			elseif(tonumber(ts[1]) ~= nil) then
				index = tonumber(ts[1]);
			elseif(tonumber(ts[1]) == nil) then
				error("Parse error. Unknown frame label: "..ts[1],2)
			end
	   
			if(not cnt) then
				ad[index] = ts[2];
			end
			]]
		end
	   
		--ad = cinematX.animDataAutofill (ad);
   
		--return ad;
   
	end
	
	
	
		
	local pipeWarpEnter = {}
	
	function PlayerGfxSet:draw (charIndex)
		currentMemOffsetA = 0
		currentMemOffsetB = 0
		startAddressA = 0xD0--166
		startAddressB = 0x6E
		
		local playSet = player:getCurrentPlayerSetting()
		
		if  inputs.state["dropitem"] == inputs.HOLD  then
			if  inputs.state["up"] == inputs.PRESS  then
				currentMemOffsetA = currentMemOffsetA + 8
			end
			if  inputs.state["down"] == inputs.PRESS  then
				currentMemOffsetA = currentMemOffsetA - 8
			end
		end
		
		
		
		for  k,v  in pairs (Player.get())  do
			
			-- Check to make sure the character index is correct
			if  v.character == charIndex  and  v:mem(0x13E, FIELD_WORD) < 1  and  v:mem(0x10, FIELD_WORD) < 1  then
				local gfxIndexX, gfxIndexY = v:getCurrentSpriteIndex ()
				local powerupIndex = v.powerup
				
				if  self.fixedFrameX ~= nil  then
					gfxIndexX = self.fixedFrameX
				end
				if  self.fixedFrameY ~= nil  then
					gfxIndexY = self.fixedFrameY
				end
				
				if self.powerupIndex > PLAYER_ICE  then
					powerupIndex = self.powerupIndex
				else
					self.powerupIndex = powerupIndex
				end
				
				local centerX = v.x + 0.5*v.width
				local centerY = v.y + 0.5*v.height
				local rightX = v.x + v.width
				local bottomY = v.y + v.height
				
				local defOffsetX = playerGfx.defPlayerOffsetsX [charIndex*10000 + powerupIndex*100 + 10*gfxIndexX + gfxIndexY] or 0
				local defOffsetY = playerGfx.defPlayerOffsetsY [charIndex*10000 + powerupIndex*100 + 10*gfxIndexX + gfxIndexY] or 0
				local hitboxW = playerGfx.defPlayerHitboxW [charIndex*10 + powerupIndex*1]
				local hitboxH = playerGfx.defPlayerHitboxH [charIndex*10 + powerupIndex*1]
				local hitboxSmallH = playerGfx.defPlayerHitboxH [charIndex*10 + 1]
				local hitboxBigH = playerGfx.defPlayerHitboxH [charIndex*10 + 2]
				local hitboxDuckH = playerGfx.defPlayerHitboxDuckH [charIndex*10 + powerupIndex*1]
				
				local hitboxDiffH = hitboxBigH - hitboxSmallH
				
				
				-- Figure out cropping for pipes, mounts, etc.
				local cropLeft = 0
				local cropRight = 0
				local cropTop = 0
				local cropBottom = 0
				
				local shouldCropLeft = false
				local shouldCropRight = false
				local shouldCropTop = false
				local shouldCropBottom = false
				
				local forcedAnimState = v:mem(0x122, FIELD_WORD)
				
				local warpExitId = v:mem (0x15E, FIELD_WORD)
				local warpEnterId = nil
				local warpEnterObj = nil
				
				if  forcedAnimState == 3  then
					warpEnterId = pipeWarpEnter[v]
					warpEnterObj = Warp.getIntersectingEntrance (centerX-24,bottomY-hitboxBigH-8, centerX+24,v.y+hitboxBigH+8)[1]
				else
					warpEnterId = v:mem (0x5A, FIELD_WORD)
					warpEnterObj = Warp.get()[warpEnterId]
					pipeWarpEnter[v] = warpEnterId
				end
				
				
				
				local warpExitObj = Warp.get()[warpExitId]
				
				local warpEnterDir = nil
				local warpExitDir = nil
				local warpEnterTimer = nil
				local warpExitTimer = v:mem (0x124, FIELD_DFLOAT)
				
				if  (warpEnterObj ~= nil)  then
					warpEnterDir = warpEnterObj:mem (0x80, FIELD_WORD)
				end
				if  (warpExitObj ~= nil)  then
					warpExitDir = warpExitObj:mem (0x82, FIELD_WORD)
				end
				
								
				local slopeMod = v:mem(0x48, FIELD_WORD)
				local mountOffsetY = v:mem(0x10E, FIELD_WORD)
				local mountId = v:mem (0x108, FIELD_WORD)
				local playerDucking = v:mem (0x12E, FIELD_WORD)
							
				local copterOffsetX = v.width*0.5-10

				
				local hitboxCurrentH = hitboxH
				if  playerDucking == true  then
					hitboxCurrentH = hitboxDuckH
				end
				
				
				-- Debug stuff
				playerGfx.debugPrint ("forcedAnim: "..tostring(forcedAnimState), 4, 30, 60 + 100*k)
				playerGfx.debugPrint ("enterDir: "..tostring(warpEnterDir), 4, 30, 80 + 100*k)
				playerGfx.debugPrint ("exitTimer:  "..tostring(warpExitTimer), 4, 30, 100 + 100*k)
				playerGfx.debugPrint ("mountOffY:  "..tostring(mountOffsetY), 4, 30, 120 + 100*k)
				playerGfx.debugPrint ("height:     "..tostring(hitboxCurrentH), 4, 30, 140 + 100*k)

				playerGfx.debugPrint ("defOffsets: "..tostring(defOffsetX)..", "..tostring(defOffsetY), 4, 30, 160 + 100*k)

				
				
				-- calculate offsets
				local tempOffsetX = self.offsetX + defOffsetX-- + v.speedX
				local tempOffsetY = self.offsetY + defOffsetY + mountOffsetY-- + v.speedY
				
				
				playerGfx.debugPrintToScene ("____v.y", 3, v.x, v.y)
				playerGfx.debugPrintToScene ("____bottomY", 3, v.x, bottomY)
				if  warpEnterObj ~= nil  then
					playerGfx.debugPrintToScene ("             ___WarpEnterY", 3, v.x, warpEnterObj.entranceY)
				end
				if  warpExitObj ~= nil  then
					playerGfx.debugPrintToScene ("             ___WarpExitY", 3, v.x, warpExitObj.exitY)
				end
				
				-- Pipe enter fix
				if  forcedAnimState == 3  then
					
					-- Prevent flickering inside the pipe
					if  warpExitTimer <= 2  then
						
						-- Entering
						if  warpExitTimer == 0  then
							playerGfx.debugPrint ("ENTERING", 4, 400, 200)
						
							-- Enter up
							if  warpEnterDir == 1  then
								shouldCropTop = true
								cropTop = (warpEnterObj.entranceY - v.y) - defOffsetY - self.offsetY
								playerGfx.debugPrint ("UP ("..tostring(cropTop)..")", 4, 400, 220)
							
							-- Enter left
							elseif  warpEnterDir == 2  then
								shouldCropLeft = true
								cropLeft = (warpEnterObj.entranceX - v.x) - self.offsetX + 0.5 - defOffsetX
								playerGfx.debugPrint ("LEFT ("..tostring(cropLeft)..")", 4, 400, 220)

							
							-- Enter down
							elseif  warpEnterDir == 3  then
								shouldCropBottom = true
								cropBottom = (bottomY - (warpEnterObj.entranceY + 32)) + self.offsetY -- defOffsetY
								playerGfx.debugPrint ("DOWN ("..tostring(cropBottom)..")", 4, 400, 220)
							
							-- Enter right
							elseif  warpEnterDir == 4  then
								shouldCropRight = true
								cropRight = math.min(hitboxW+2, (v.x - 2 - warpEnterObj.entranceX) - 8)

								playerGfx.debugPrint ("RIGHT ("..tostring(cropRight)..")", 4, 400, 220)
							end
						
						-- Exiting
						else
							playerGfx.debugPrint ("EXITING", 4, 400, 200)
							
							-- Exit up
							if  warpExitDir == 3  then
								shouldCropBottom = true
								cropBottom = (bottomY - (warpExitObj.exitY + 32)) + self.offsetY -- defOffsetY
								if  hitboxDuckH > 0  then
									--cropBottom = cropBottom + hitboxDuckH--(hitboxDuckH - hitboxSmallH)
								end								
								playerGfx.debugPrint ("UP ("..tostring(cropBottom)..")", 4, 400, 220)
							
							-- Exit left
							elseif  warpExitDir == 4  then
								shouldCropRight = true
								cropRight = math.min(hitboxW+2, (v.x - 2 - warpExitObj.exitX) - 8)

								playerGfx.debugPrint ("RIGHT ("..tostring(cropRight)..")", 4, 400, 220)

							
							-- Exit down
							elseif  warpExitDir == 1  then
								shouldCropTop = true
								cropTop = math.abs(v.y - warpExitObj.exitY) - defOffsetY - self.offsetY
								playerGfx.debugPrint ("DOWN ("..tostring(cropTop)..")", 4, 400, 220)
							
							-- Exit right
							elseif  warpExitDir == 2  then
								shouldCropLeft = true
								cropLeft = (warpExitObj.exitX - v.x) - self.offsetX + 0.5 - defOffsetX
								playerGfx.debugPrint ("RIGHT ("..tostring(cropLeft)..")", 4, 400, 220)

							end
						end
					end
				end
				
				--- kuribo's shoe fix
				if  mountId == 1  then
					shouldCropBottom = true
					tempOffsetY = tempOffsetY - mountOffsetY
					if  powerupIndex == PLAYER_SMALL  then
						cropBottom = 5 --79-self.offsetY+defOffsetY
					else
						cropBottom = 29 --75-self.offsetY+defOffsetY
					end
				
				--- clown copter fix
				elseif  mountId == 2  then
					tempOffsetX = tempOffsetX + copterOffsetX
					shouldCropBottom = true
					tempOffsetY = tempOffsetY - (hitboxCurrentH-20)
					cropBottom = hitboxCurrentH-35
					
					if  hitboxCurrentH == hitboxSmallH  and  hitboxSmallH ~= hitboxBigH  then
						tempOffsetY = tempOffsetY - (hitboxBigH - hitboxSmallH - 8)
						cropBottom = cropBottom + 8
					end					
					
					--64-self.offsetY+defOffsetY
					
					--[[if  powerupIndex == PLAYER_SMALL  then
						tempOffsetY = tempOffsetY - 24
						cropBottom = 5--78-self.offsetY+defOffsetY
					else
						tempOffsetY = tempOffsetY - 36
						cropBottom = 17--64-self.offsetY+defOffsetY
					end]]

				elseif  mountID ~= 0  then

				end
				
				
				-- Debug crop display
				playerGfx.debugPrintToScene ("             ___cropBottom", 3, v.x, bottomY-cropBottom)
				
				-- Move the bottom crop up to the feet
				if  shouldCropBottom == true  then
					cropBottom = cropBottom + (100 - hitboxCurrentH + self.offsetY + defOffsetY)
				end

				-- Move the right crop up to the feet
				if  shouldCropRight == true  then
					cropRight = cropRight + (100 - hitboxW + self.offsetX + defOffsetX)
				end				
				
				
				-- Perform the draw
				if  v:mem (0x142, FIELD_WORD) == 0  							and  -- Powerup blinking state
					v:mem(0x122, FIELD_WORD) ~= 8   							and  -- Forced anim state (invisible)
					(mountId == 1  and  playerDucking == -1) == false		  	and  -- Hiding in boot
					(forcedAnimState == 3  and  warpExitTimer > 2) == false  	and  -- Player completely inside pipe
					(cropTop + cropBottom) < 100								and
					(cropLeft + cropRight) < 100						
					then   
					if  self.images[powerupIndex] ~= nil  then
						Graphics.drawImageToSceneWP (self.images[powerupIndex], 
													v.x + tempOffsetX + cropLeft, 
													v.y + tempOffsetY + cropTop, 
													100*gfxIndexX + cropLeft,
													100*gfxIndexY + cropTop, 
													math.min (math.max(0, 100 - cropRight - cropLeft), 100),
													math.min (math.max(0, 100 - cropBottom - cropTop), 100),
													0.5)
					else
						Text.windowDebug("Nil image error: "..self.filenames[powerupIndex])
					end
				end
			end
			
		end
	end
	
	function PlayerGfxSet:getHeadPosition (row, column)
	end



	function playerGfx.arrayFromPrefix (prefix)
		local newArray = {}
		newArray [PLAYER_SMALL] 		= Misc.resolveFile (prefix.."_1.png");
		newArray [PLAYER_BIG] 			= Misc.resolveFile (prefix.."_2.png");
		newArray [PLAYER_FIREFLOWER] 	= Misc.resolveFile (prefix.."_3.png");
		newArray [PLAYER_LEAF] 			= Misc.resolveFile (prefix.."_4.png");
		newArray [PLAYER_TANOOKIE]		= Misc.resolveFile (prefix.."_5.png");
		newArray [PLAYER_HAMMER] 		= Misc.resolveFile (prefix.."_6.png");
		newArray [PLAYER_ICE] 			= Misc.resolveFile (prefix.."_7.png");
		
		return newArray
	end


	function playerGfx.assignSet (playerChar, gfxSet)
		
		playerGfx.currentSet [playerChar] = gfxSet
		
		if  gfxSet == nil  then
			playerGfx.resetOffsets (playerChar)
		
		elseif  gfxSet.overwrite == true  then
			playerGfx.hideOffsets (playerChar)
		
		else
			playerGfx.resetOffsets (playerChar)
		end
		
	end
	
end
	

	
	
--***************************************************************************************************
--                                                                                                  *
--             OVERRIDE NPC GRAPHICS                                                                *
--                                                                                                  *
--***************************************************************************************************
do
	-- Thanks again for this, Horikawa!
	function playerGfx.overrideNpcAnim (npcRef, image)
		local currentFrame = npcRef:mem(0xE4, FIELD_WORD)
		npcRef:mem(0xE4, FIELD_WORD, 255)
		--npcRef:mem(0xE8, FIELD_DFLOAT, 0)
		Graphics.drawImageToSceneWP (image, 
									v.x + tempOffsetX - cropLeft, 
									v.y + tempOffsetY + cropTop, 
									100*gfxIndexX,
									100*gfxIndexY + cropTop, 
									100,
									math.max(0, 100 - cropBottom - cropTop),
									0.5)
	end

	
end	


	
	
--***************************************************************************************************
--                                                                                                  *
--              TOGGLE FUNCTIONS                                                                    *
--                                                                                                  *
--***************************************************************************************************
do

	function playerGfx.disable ()
		for  k,v in pairs (playerGfx.charTable)  do
			playerGfx.resetOffsets (v)
		end
		playerGfx.isOn = false
	end

	function playerGfx.enable ()
		for  k,v in pairs (playerGfx.charTable)  do
			playerGfx.assignSet (playerGfx.currentSet[v])
		end
		playerGfx.isOn = true
	end

	function playerGfx.toggle ()
		if  playerGfx.isOn == true  then
			playerGfx.disable ()
		else
			playerGfx.enable ()
		end
	end

	
	function playerGfx.disableSets ()
		for  k,v in pairs (playerGfx.charTable)  do
			playerGfx.resetOffsets (v)
		end
		playerGfx.setsOn = false
	end

	function playerGfx.enableSets ()
		for  k,v in pairs (playerGfx.charTable)  do
			playerGfx.assignSet (playerGfx.currentSet[v])
		end
		playerGfx.setsOn = true
	end
		
	function playerGfx.toggleSets ()
		if  playerGfx.setsOn == true  then
			playerGfx.disableSets ()
		else
			playerGfx.enableSets ()
		end
	end
	
	
	function playerGfx.disableAccessories ()
		playerGfx.accessoriesOn = false
	end
	
	function playerGfx.enableAccessories ()
		playerGfx.accessoriesOn = true
	end
	
	function playerGfx.toggleAccessories ()
		playerGfx.accessoriesOn = not playerGfx.accessoriesOn
	end

end



	
--***************************************************************************************************
--                                                                                                  *
--              LOAD AND UPDATE                                                                     *
--                                                                                                  *
--***************************************************************************************************
do
	function playerGfx.storeDefaultOffsets ()
		for  k1,v1 in pairs(playerGfx.charTable)  do			
			for  k2,v2 in pairs(playerGfx.powerupTable)  do
				
				local tempPlayerSet = PlayerSettings.get(v1, v2)
					
				playerGfx.defPlayerHitboxW [v1*10 + v2] = tempPlayerSet.hitboxWidth;
				playerGfx.defPlayerHitboxH [v1*10 + v2] = tempPlayerSet.hitboxHeight;
				playerGfx.defPlayerHitboxDuckH [v1*10 + v2] = tempPlayerSet.hitboxDuckHeight
				
				for  i=0,9  do 
					for  j=0,9  do 
						playerGfx.defPlayerOffsetsX [v1*10000 + v2*100 + 10*i + j] = tempPlayerSet:getSpriteOffsetX(i, j)
						playerGfx.defPlayerOffsetsY [v1*10000 + v2*100 + 10*i + j] = tempPlayerSet:getSpriteOffsetY(i, j)
					end
				end
			end
		end
	end


	function playerGfx.hideOffsets (playerChar)
		local v1 = playerChar
		
		for  k2,v2 in pairs(playerGfx.powerupTable)  do
			local tempPlayerSet = PlayerSettings.get(v1, v2)
			
			for  i=0,9  do 
				for  j=0,9  do 
					tempPlayerSet:setSpriteOffsetX (i, j, 10000)
					tempPlayerSet:setSpriteOffsetY (i, j, 10000)
				end
			end
		end
	end


	function playerGfx.resetOffsets (playerChar)
		local v1 = playerChar
		
		for  k2,v2 in pairs(playerGfx.powerupTable)  do
			local tempPlayerSet = PlayerSettings.get(v1, v2)
			for  i=0,9  do 
				for  j=0,9  do 
					tempPlayerSet:setSpriteOffsetX (i, j, playerGfx.defPlayerOffsetsX [v1*10000 + v2*100 + 10*i + j])
					tempPlayerSet:setSpriteOffsetY (i, j, playerGfx.defPlayerOffsetsY [v1*10000 + v2*100 + 10*i + j])
				end
			end
		end
	end

	
	function playerGfx.onHUDDraw (eventObj, message)
		playerGfx.updateDraws ()
	end

	
	function playerGfx.updateDraws ()
		-- Initialize
		if  playerGfx.initDone == false  then
			playerGfx.initDone = true
			playerGfx.storeDefaultOffsets ()
			return
		end

		-- Cancel if the system is disabled
		if  playerGfx.isOn == false  then
			return;
		end
		
		-- Loop through and draw the graphic sets
		if  playerGfx.setsOn == true  then
			for  k,v in pairs(playerGfx.currentSet)  do
				if  v ~= nil  then
					v:draw(k)
				end
			end		
		end
		
		-- Draw the accessories
		if  playerGfx.accessoriesOn == true  then
			for  k1,v1 in pairs(playerGfx.currentAccessories)  do
				for  k2,v2 in pairs(v1)  do
					if  v2 ~= nil  then
						v2:draw(k1)
					end
				end
			end		
		end
	end
	
	
	function playerGfx.update ()
		isMessageBoxActive = false
	end

end



	
--***************************************************************************************************
--                                                                                                  *
--              DEBUG STUFFS                                                                        *
--                                                                                                  *
--***************************************************************************************************
	--[[
	local testFontProps = {}
	testFontProps ["charWidth"] = 9
	testFontProps ["charHeight"] = 9
	testFontProps ["imagePath"] = "font_default3.png"
	testFontProps ["kerning"] = 0

	local testFont = Font.create (textblox.FONTTYPE_SPRITE, testFontProps)
	]]
	
	
	function playerGfx.debugPrint (text, font, x,y)
		if  playerGfx.debug == true  then
			--textblox.print (text, x,y, testFont)
			Text.print (text, font, x, y)
		end
	end

	function playerGfx.debugPrintToScene (text, font, x,y)
		if  playerGfx.debug == true  then
			local x1,y1 = worldToScreen (x,y)
			--textblox.print (text, x1,y1, testFont)
			Text.print (text, font, x1, y1)
		end
	end
	
	
	function getScreenBounds()
		local h = (player:mem(0xD0, FIELD_DFLOAT));
		local b = { left = player.x-400+player.speedX, right = player.x+400+player.speedX, top = player.y-260+player.speedY, bottom = player.y+340+player.speedY };
		
		local sect = Section(player.section);
		local bounds = sect.boundary;

		if(b.left < bounds.left - 10) then
			b.left = bounds.left - 10;
			b.right = b.left + 800;
		end
		
		if(b.right > bounds.right - 10) then
			b.right = bounds.right - 10;
			b.left = b.right - 800;
		end
		
		if(b.top < bounds.top+40-h) then
			b.top = bounds.top+40-h;
			b.bottom = b.top + 600;
		end
		
		if(b.bottom > bounds.bottom+40-h) then
			b.bottom = bounds.bottom+40-h;
			b.top = b.bottom - 600;
		end
		
		return b;
		
	end	
	
	function worldToScreen(x,y)
		local b = getScreenBounds();
		local x1 = x-b.left;
		local y1 = y-b.top-(player:mem(0xD0, FIELD_DFLOAT))+30;
		return x1,y1;
	end

	






return playerGfx;