local cinematX = {} --Package table

function cinematX.onInitAPI() --Is called when the api is loaded by loadAPI.
	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
	
	registerEvent(cinematX, "onLoad", "initLevel", true) --Register the init event
	registerEvent(cinematX, "onLoadSection", "initSection", false) --Register the init event
	--registerEvent(cinematX, "onLoad", "delayedInit", false) --Register the init event
	registerEvent(cinematX, "onLoop", "update", true) --Register the loop event
	registerEvent(cinematX, "onJump", "onJump", true) --Register the jump event
	registerEvent(cinematX, "onKeyDown", "onKeyDown", true) --Register the input event
	registerEvent(cinematX, "onKeyUp", "onKeyUp", false) --Register the input event
end

--***************************************************************************************************
--                                                                                                  *
--              CONSTANTS AND ENUMS															    	*
--                                                                                                  *
--***************************************************************************************************
do	
	-- Resource path
	cinematX.resourcePath = "..\\..\\..\\LuaScriptsLib\\cinematX\\"

	-- Checkpoint reached
	cinematX.midpointReached = false
	
	-- Animation states enum
	cinematX.ANIMSTATE_NUMFRAMES =  0
	cinematX.ANIMSTATE_IDLE      =  1
	cinematX.ANIMSTATE_TALK      =  2
	cinematX.ANIMSTATE_TALK1   	 =  3
	cinematX.ANIMSTATE_TALK2   	 =  4
	cinematX.ANIMSTATE_TALK3   	 =  5
	cinematX.ANIMSTATE_TALK4   	 =  6
	cinematX.ANIMSTATE_TALK5   	 =  7
	cinematX.ANIMSTATE_TALK6   	 =  8
	cinematX.ANIMSTATE_TALK7   	 =  9
	cinematX.ANIMSTATE_WALK    	 = 10
	cinematX.ANIMSTATE_RUN    	 = 11
	cinematX.ANIMSTATE_JUMP    	 = 12
	cinematX.ANIMSTATE_FALL    	 = 13
	cinematX.ANIMSTATE_DEFEAT  	 = 14
	cinematX.ANIMSTATE_ATTACK 	 = 15
	cinematX.ANIMSTATE_ATTACK1 	 = 16
	cinematX.ANIMSTATE_ATTACK2 	 = 17
	cinematX.ANIMSTATE_ATTACK3 	 = 18
	cinematX.ANIMSTATE_ATTACK4 	 = 19
	cinematX.ANIMSTATE_ATTACK5 	 = 20
	cinematX.ANIMSTATE_ATTACK6 	 = 21
	cinematX.ANIMSTATE_ATTACK7 	 = 22
	

	-- Actor animation state table
	cinematX.npcAnimStates = {}
end



--***************************************************************************************************
--                                                                                                  *
--              ACTOR WRAPPER CLASS																    *
--                                                                                                  *
--***************************************************************************************************
do 
	Actor = {}
	Actor.__index = Actor

	function Actor.create(smbxObjRef, smbxClass)
		
		local thisActorObj = {}             					-- our new object
		setmetatable (thisActorObj, Actor)     		-- make Actor handle lookup
		thisActorObj.smbxObjRef = smbxObjRef     			-- initialize our object
		thisActorObj.smbxClass = smbxClass       			 

		thisActorObj.npcid = -1       			 
		if (thisActorObj.smbxClass == "NPC") then
			thisActorObj.npcid = smbxObjRef.id
		end
		
		thisActorObj.name = "UNNAMED"
		thisActorObj.uid = -1
		thisActorObj.wasMismatched = false
		thisActorObj.isDirty = true
		
		thisActorObj.animState = cinematX.ANIMSTATE_IDLE
		
		thisActorObj.shouldFacePlayer = false
		thisActorObj.closeIdleAnim = cinematX.ANIMSTATE_TALK
		thisActorObj.farIdleAnim = cinematX.ANIMSTATE_IDLE
		thisActorObj.talkAnim = cinematX.ANIMSTATE_TALK
		thisActorObj.walkAnim = cinematX.ANIMSTATE_WALK
		thisActorObj.runAnim = cinematX.ANIMSTATE_RUN

		thisActorObj.shouldDespawn = true
		thisActorObj.isDespawned = false
		thisActorObj.isDead = false
		thisActorObj.savestateX = {}
		thisActorObj.savestateY = {}
		thisActorObj.savestateSpeedX = {}
		thisActorObj.savestateSpeedY = {}	   
		thisActorObj.savestateDir = {}	   

		thisActorObj.isUnderwater = false
		thisActorObj.isResurfacing = false


		thisActorObj.helloVoice = ""
		thisActorObj.goodbyeVoice = ""
		thisActorObj.saidHello = false
		thisActorObj.helloCooldown = 0

		thisActorObj.actorToFollow = nil
		thisActorObj.shouldTeleportToTarget = false
		thisActorObj.distanceToFollow = 64
		thisActorObj.destWalkSpeed = 0
		thisActorObj.walkSpeed = 0
		thisActorObj.walkDestX = 0
		thisActorObj.shouldWalkToDest = false

		thisActorObj.framesSinceJump = 0
		thisActorObj.jumpStrength = 0

		thisActorObj.isInteractive = false
		thisActorObj.sceneString = ""
		thisActorObj.routineString = ""
		thisActorObj.messagePointer = ""
		thisActorObj.messageString = ""
		thisActorObj.nameString = ""
		thisActorObj.talkTypeString = ""
		thisActorObj.wordBubbleIcon = nil
		thisActorObj.messageIsNew = true

		thisActorObj.invincible = false
		thisActorObj.onGround = true
		
		--thisActorObj.x = 0
		--thisActorObj.y = 0
		--thisActorObj.speedX = 0
		--thisActorObj.speedY = 0
		--thisActorObj.direction = 0

		
		--windowDebug ("Actor created: "..thisActorObj.smbxClass)

		return thisActorObj
	end

	
	
	-- Memory functions
	do
		function Actor:getMem(offset,field)
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end
			
			return self.smbxObjRef:mem (offset, field)
		end
		
		function Actor:setMem(offset,field,value)
			if  (self.smbxObjRef == nil)  then  
				return;			
			end
			
			self.smbxObjRef:mem (offset, field, value)
		end
		
		function Actor:UIDCheck ()
			if  self.smbxClass == "Player"  then
				return true
			end
		
			if   (self:getUIDMem ()  ~=  self.uid)  then
				if  (self.wasMismatched == false)  then
					self.wasMismatched = true
					cinematX.toConsoleLog ("UID MISMATCH: MEM " .. tostring(self:getUIDMem()) .. ", VAR " .. tostring(self.uid) .. "; NPCID MEM " .. self.smbxObjRef.id .. ", VAR " .. self.npcid)
				end
				return false;
			else
				return true;
				end
		end
		
		function Actor:getUIDMem ()
			if  self.smbxClass == "Player"  then
				return 0
			end
		
			return self:getMem (cinematX.ID_MEM, FIELD_WORD)
		end
	end

	-- Getters and setters for movement vars
	do
	    function Actor:getDirection()
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end
			
			if  self.smbxClass == "Player"  then
				return self.smbxObjRef:mem (0x106, FIELD_WORD)
			
			else
				return self.smbxObjRef.direction
			end
	    end
	   
	    function Actor:setDirection(newDir)
			if  (self.smbxObjRef == nil)  then  
				return;			
			end
			
			if self.smbxClass == "Player" then
				self.smbxObjRef:mem (0x106, FIELD_WORD, newDir)
			else
				self.smbxObjRef.direction = newDir
			end
	    end
	   
	    function Actor:getX()
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end
		
			local val = self.smbxObjRef.x
			return val
	    end
	   
	    function Actor:setX(newX)
			if  (self.smbxObjRef == nil)  then  
				return;			
			end
		
			self.smbxObjRef.x = newX
	    end
	   
	    function Actor:getY()
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end
		
			return self.smbxObjRef.y	   
	    end	   
	   
	    function Actor:setY(newY)
			if  (self.smbxObjRef == nil)  then  
				return;			
			end

			self.smbxObjRef.y = newY	   
	    end	   
	   
	    function Actor:getSpeedX()
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end
		
			return self.smbxObjRef.speedX	   
	    end
	   
	    function Actor:setSpeedX(newSpd)
			if  (self.smbxObjRef == nil)  then  
				return;			
			end

			self.smbxObjRef.speedX = newSpd
	    end
	   
	    function Actor:getSpeedY()
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end

			return self.smbxObjRef.speedY	   
	    end
	   
	    function Actor:setSpeedY(newSpd)
			if  (self.smbxObjRef == nil)  then  
				return;			
			end
		
			self.smbxObjRef.speedY = newSpd
	    end

	end
	
	-- Animation
	do
	
		function Actor:getAnimFrame ()
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end
		
			return self.smbxObjRef:mem (0xE4, FIELD_WORD) --:getAnimFrame ()
		end
		
		function Actor:setAnimFrame (newFrame)
			if  (self.smbxObjRef == nil)  then  
				return nil;			
			end

			self.smbxObjRef:mem (0xE4, FIELD_WORD, newFrame)  --.setAnimFrame (newFrame)
		end
	
		function Actor:setAnimState (state)
		   self.animState = state
		end
		
		function Actor:getAnimState ()
		   return self.animState
		end
		
		function Actor:overrideAnimation (animDataTable)
			--windowDebug ("BEGINNING WORKS")
			local myState = self:getAnimState ()
			
			local boundsString = animDataTable [myState]
			if (boundsString == nil) then
				--windowDebug ("BOUNDS STRING GATE")
				return;
			end
			
			local dashCharPos = string.find (boundsString, "-")
			local minVal = tonumber(string.sub (boundsString, 0, dashCharPos-1)) 
			local maxVal = tonumber(string.sub (boundsString, dashCharPos+1))
		  
			-- Change to the appropriate animation based on the current movement
			self:setAnimStateByMovement ()
			
			--windowDebug ("ANIM STATE BY MOVEMENT")
			
			-- Clamp animation frames accordingly
			self:clampAnim (minVal, maxVal, animDataTable [cinematX.ANIMSTATE_NUMFRAMES])
		end

		function Actor:clampAnim (minVal, maxVal, dirOffsetFrames)
		  
			-- Add an offset for flipped animations
			local dirOffset = dirOffsetFrames
			if (self:getDirection() == DIR_LEFT) then
				dirOffset = 0
			end

			-- Get the current animation frame
			animFrame = self:getAnimFrame ()
		  
			-- Clamp to the loop  
			if (animFrame < (minVal + dirOffset)  or  animFrame > (maxVal+dirOffset)) then
				self:setAnimFrame (minVal + dirOffset)
			end
		end
		
		function Actor:setAnimStateByMovement ()

			-- Don't disrupt unique animations
			if self:getAnimState() < cinematX.ANIMSTATE_ATTACK1 then

				-- If on ground
				if    (self.onGround == true)   then
					if    math.abs (self:getSpeedX()) < 1 then
						if    cinematX.dialogSpeaker == self   then
							self:setAnimState (self.talkAnim)
						else
						
							if (self:distanceActor (cinematX.playerActor) < 64) then
								self:setAnimState (self.closeIdleAnim)
							else
								self:setAnimState (self.farIdleAnim)
							end
								
						end
					else
						if  math.abs (self:getSpeedX()) > 2  then
							self:setAnimState (cinematX.ANIMSTATE_RUN)
						else
							self:setAnimState (cinematX.ANIMSTATE_WALK)
						end
					end

				-- If jumping or falling
				elseif  self:getSpeedY() > 0 then
					self:setAnimState (cinematX.ANIMSTATE_FALL)
				else
					self:setAnimState (cinematX.ANIMSTATE_JUMP)
				end
			end
		end
			
	end
	
	-- Relational
	do
		function Actor:getScreenX ()
			local myX = self:getX () - (player.x - player.screen.left)
			return myX
		end
		
		function Actor:getScreenY ()
			local myY = self:getY () - (player.y - player.screen.top)
			return myY
		end
		
		function Actor:isOnScreen ()
			local withinRegion = false
			
			if  (self:getScreenX () > -32   and
				 self:getScreenY () > -32   and
				 self:getScreenX () < 832   and
				 self:getScreenY () < 632)  then
				 
				 withinRegion = true
			end
			
			return withinRegion
		end

		function Actor:relativeX (posX)
			return (posX - self.smbxObjRef.x)
		end
		 
		function Actor:relativeY (posY)
			return (posY - self.smbxObjRef.y)
		end
		 
		function Actor:distanceX (posX)
			return math.abs(self:relativeX (posX))
		end
		 
		function Actor:distanceY (posY)
			return math.abs(self:relativeY (posY))
		end
		 
		function Actor:distanceDestX ()
			return self:distanceX (self.walkDestX)
		end
		 		 
		function Actor:relativeActorX (targetActor)
			local dist = self:relativeX (targetActor:getX())
			return dist
		end
		 
		function Actor:relativeActorY (targetActor)
			local dist = self:relativeY (targetActor:getY())
			return dist
		end

		function Actor:distanceActorX (targetActor)
			local dist = self:distanceX (targetActor:getX())
			return dist
		end
		 
		function Actor:distanceActorY (targetActor)
			return self:distanceY (targetActor:getY())
		end	 
		  
		function Actor:distancePos (xPos, yPos)
			local xDist = self:distanceX (xPos)
			local yDist = self:distanceY (yPos)
		 
			local diagDist = math.sqrt ((xDist^2) + (yDist^2))
			return diagDist
		end
		
		function Actor:distanceActor (targetActor)
			return self:distancePos (targetActor:getX(), targetActor:getY())
		end
		 
		function Actor:dirToX (posX)
			if (self:relativeX (posX) > 0) then
				return DIR_RIGHT
			else
				return DIR_LEFT
			end
		end
		 	 
		function Actor:dirToActorX (targetActor)
			return self:dirToX (targetActor:getX())
		end
	end
	
	-- Movement
	do
		function Actor:followActor (targetActor, speed, howClose, shouldTeleport)
			self.shouldWalkToDest = true
			self.actorToFollow = targetActor
			self.distanceToFollow = howClose
			self.destWalkSpeed = speed
			self.shouldTeleportToTarget = shouldTeleport
		end
	
		function Actor:stopFollowing ()
			self.shouldWalkToDest = false
			self.actorToFollow = nil
		end
	
		function Actor:setSpawnX (newX)
			self.smbxObjRef:mem(0xAC, FIELD_WORD, newX)
		end
		
		function Actor:setSpawnY (newY)
			self.smbxObjRef:mem(0xB4, FIELD_WORD, newY)
		end
	
	
		function Actor:setSpawnToCurrent ()
			local newSpawnX = cinematX.coordToSpawnX(self:getX())
			local newSpawnY = cinematX.coordToSpawnY(self:getY())
			
			self:setSpawnX (newSpawnX)
			self:setSpawnY (newSpawnY)
		end
	
		function Actor:saveState (slot)
			--cinematX.toConsoleLog ("Saved state " .. tostring(slot))
			
			self.savestateX [slot] = self:getX ()
			self.savestateY [slot] = self:getY ()
			self.savestateSpeedX [slot] = self:getSpeedX ()
			self.savestateSpeedY [slot] = self:getSpeedY ()
			self.savestateDir [slot] = self:getDirection ()
		end

		function Actor:loadState (slot)
			if  (self.savestateX [slot] == nil)  then
				return
			end
		
			cinematX.toConsoleLog ("Loaded state" .. tostring(slot))
			self:setX (self.savestateX [slot])
			self:setY (self.savestateY [slot])
			self:setSpeedX (self.savestateSpeedX [slot])
			self:setSpeedY (self.savestateSpeedY [slot])
			self:setDirection (self.savestateDir [slot])
		end

		
		function Actor:lookAtPlayer ()
			local newDir = self:dirToActorX (cinematX.playerActor)
			local oldDir = self:getDirection ()
			
			if  newDir ~= oldDir  then
				self:setDirection (newDir)
			end
		end
		
		function Actor:turnAround ()
			local newDir = self:getDirection ()
			
			if  newDir == DIR_LEFT then
				newDir = DIR_RIGHT
			else
				newDir = DIR_LEFT					
			end
			self:setDirection (newDir)
		end
		
		function Actor:lookAwayFromPlayer ()
			self:lookAtPlayer ()
			self:turnAround ()
		end
		
		function Actor:walk (speed)
			self.shouldWalkToDest = false
			self.walkSpeed = speed		
		end

		function Actor:walkForward (speed)
			self.shouldWalkToDest = false
			self.walkSpeed = speed * self:getDirection ()
		end
		
		function Actor:walkToX (dest, speed)
			self.shouldWalkToDest = true
			self.walkDestX = dest
			self.destWalkSpeed = speed
			self.distanceToFollow = 8
		end
		
		function Actor:jump (strength)
			self:setSpeedY (-1 * strength)
			playSFX (1)	
			self.framesSinceJump = 0
			self.jumpStrength = strength
		end
	end
		
	-- Update
	function Actor:update ()
		
		-- Skip update if the corresponding NPC/Player is destroyed
		local skipUpdate = false
		
		if     (self.smbxObjRef == nil  or  self.smbxObjRef == 0  or  self.smbxObjRef == -1)  then  
			skipUpdate = true
		elseif (self.smbxObjRef:mem (0x122, FIELD_WORD) > 0   or   self:UIDCheck () == false  or self.smbxObjRef.id == 0)  then
			skipUpdate = true
		end	
	
		if  (skipUpdate == true)  then
			self.isDead = true
			return;
		end
	
		
		-- Display the actor's UID variables
		if  (cinematX.showDebugInfo == true)  then
			printText (tostring(self:getUIDMem()) .. ", " .. self.uid, 4, self:getScreenX(), self:getScreenY()-32)
		end
		
		-- Update invincible
		if  (self.invincible == true)  then
			self:setMem (0x156, FIELD_WORD, 2)
		end

		-- Update on ground
		self.onGround = false
			
		if  (self:getMem (0x00A, FIELD_WORD) == 2  or  self:getMem (0x120, FIELD_WORD) == 0xFFFF)  then
			self.onGround = true
		end
		
		
		-- Update jump signal
		self.framesSinceJump = self.framesSinceJump + 1
		
		-- Check if underwater
		self.isUnderwater = false
		if       self.smbxObjRef:mem (0x1C, FIELD_WORD) == 2   then
			self.isUnderwater = true
		end
		
		
		-- If following another actor, set their position as the destination X
		if (self.actorToFollow ~= nil) then
			local leadActor = self.actorToFollow
			
			self.walkDestX = leadActor:getX()
			
			-- If the actor being followed just jumped and they are above me, jump shortly after
			if  (leadActor.framesSinceJump == 13	and  
				 self:getSpeedY() == 0				and
				 self:getY() > leadActor:getY()+64)	then
				 
				self:jump (leadActor.jumpStrength * 0.5)			
			
			-- Swim
			elseif  (self:getY() > leadActor:getY()+32) 	then
				if      (self.isUnderwater == true)  then
					self:jump (5)
					self.isResurfacing = true
				elseif 	(self.isResurfacing == true)  then
					self.isResurfacing = false
					self:jump (7)
				end
			end
			
			
			-- Teleport to the actor's position if too far away
			if  (self:distanceActor(leadActor) > 350  and  self.shouldTeleportToTarget == true)   then
				
				-- Teleport
				self:setX (leadActor:getX () - self.distanceToFollow * leadActor:getDirection ())
				self:setY (leadActor:getY () - 32)
				
				-- Smoke puffs
				runAnimation (10, self:getX(), self:getY(), 0)
				runAnimation (10, self:getX()-8, self:getY()+16, 0)
				runAnimation (10, self:getX()+8, self:getY()+16, 0)
				runAnimation (10, self:getX(), self:getY()+24, 0)
			end
		end
		
		
		-- Prevent despawning  --NPCMemSet,NPC ID,0x12A,55,0,0,w, thanks Willhart!
		if (self.shouldDespawn == false) then
			self.smbxObjRef:mem (0x12A, FIELD_WORD, 55)
		end
		
		
		-- Check whether or not the NPC is despawned
		self.isDespawned = true

		if (self.smbxObjRef:mem (0x12A, FIELD_WORD) == 55) then
			self.isDespawned = false
		end
		
		
		-- Old method of preventing despawning
		--[[
			-- Check to see if the actor is despawned
			self.isDespawned = false
			
			if (self:isOnScreen () == false) then
				self.isDespawned = true
			end
			
			-- If despawned, load from state
			if  (self.shouldDespawn == false  and  self.isDespawned == true)  then
				self:loadState(0)
			end
		--]]
		
		-- Look at player if allowed to
		if  self.shouldFacePlayer == true   then
			self:lookAtPlayer ()
		end
		
		-- Say hello if player approaches
		self.helloCooldown = self.helloCooldown - 1
		
		if (cinematX.currentSceneState ~= cinematX.SCENESTATE_CUTSCENE)  then
			if  self:distanceActorX (cinematX.playerActor) < 64  then
				if (self.saidHello == false  and  self.helloCooldown <= 0) then
					if  (self.helloVoice ~= "")  then
						playSFXSDL (self.helloVoice)
					end
					self.saidHello = true
				end
				
			elseif  (self:distanceActorX (cinematX.playerActor) < 196)  then
				--
			elseif  (self:distanceActorX (cinematX.playerActor) < 400)  then
				if  (self.saidHello == true)  then
					if  (self.goodbyeVoice ~= "")  then
						playSFXSDL (self.goodbyeVoice)
					end
					self.saidHello = false
					self.helloCooldown = 300
				end
				
			elseif  (self:distanceActorX (cinematX.playerActor) < 800)  then
				self.saidHello = false
				self.helloCooldown = 0
			end
		end
		
		-- Walk to destination
		if  (self.shouldWalkToDest == true)  then
			
			-- Get distance
			local destDist = self:distanceDestX () - self.distanceToFollow
			if (destDist < 0) then destDist = 0 end
			
			-- Get direction multiplier
			local dirMult = -1
			if self:dirToX (self.walkDestX) == DIR_RIGHT then 
				dirMult = 1
			end
			
			-- Get acceleration multiplier
			local accelMult = invLerp (0,128, destDist)
			
			-- Walk
			--self.walkSpeed = 0
			
			self.walkSpeed = dirMult * accelMult * math.min(self.destWalkSpeed, destDist * 0.125)
		end
		
		-- Walk
		if (self.walkSpeed ~= 0) then
			self:setSpeedX (self.walkSpeed)
		end
		
		-- Save state to prevent despawning
		if  (self.shouldDespawn == false  and  self.isDespawned == false)  then
			self:setSpawnToCurrent () 
			--self:saveState(0)
		end
		
		
		-- If this actor is a generator, disable interactions
		if  (self.smbxClass == "NPC")  then
			if  (self.smbxObjRef:mem (0x64, FIELD_WORD) == 1)  then
				isInteractive = false
			end
		end
	end
	
end


			

--***************************************************************************************************
--                                                                                                  *
--              CINEMATX CONFIG														    			*
--       You may call this before cinematX.init() to toggle specific components of the library		*
--                                                                                                  *
--       Override NPC Messages 	-- Replaces standard SMBX NPC dialog system with cinematX-based 	*
--         							dialogue system; set an NPC's message to "c[function name]" 	*
--									to call that function as a coroutine. (WORK IN PROGRESS)		*
--                                                                                                  *
--       Show debug 			-- Pretty self-explanatory, displays a bunch of debug info so you	*
--									can see if cinematX is behaving properly.						*
--                                                                                                  *
--***************************************************************************************************

do
	cinematX.overrideNPCMessages = false
	cinematX.showDebugInfo = true--false
	cinematX.shouldGenerateActors = true
	cinematX.actorCriteria = nil
	--cinematX.npcsToIgnore = {}
	
	function cinematX.config (--[[genActors, genCriteria,--]] toggleOverrideMsg, showDebug)
		--cinematX.shouldGenerateActors = genActors
		--cinematX.actorGenQualifier = genCriteria
		--cinematX.npcsToIgnore = ignoreTheseNPCs
		cinematX.overrideNPCMessages = toggleOverrideMsg
		cinematX.showDebugInfo = showDebug		
	end
end



--***************************************************************************************************
--                                                                                                  *
--              CINEMATX INIT FUNCTIONS														    	*
--                                                                                                  *
--***************************************************************************************************

do
	cinematX.delayedInitCounter = 0
	cinematX.delayedInitCalledFromUpdate = false
	cinematX.delayedInitSectionNum = -1
	
	cinematX.initCalledYet = false
	
	
	
	function cinematX.initLevel ()
		-- Prevent this function from being called twice
		if  (cinematX.initCalledYet == true)  then  return  end
	
		-- Call all of the sub init functions	
		cinematX.initTiming ()
		cinematX.initHUD ()
		cinematX.initCamera ()
		cinematX.initDialog ()
		cinematX.initCutscene ()
		cinematX.initRace ()
		cinematX.initBoss ()
		cinematX.initQuestSystem ()
		cinematX.initDebug ()
		
		cinematX.initCalledYet = true
	end
	
	function cinematX.initSection ()
		cinematX.initActors ()
	end
	
	function cinematX.delayedInitLevel ()
		--windowDebug ("TEST LEVEL")
		cinematX.updateMidpointCheck ()
		
		cinematX.indexActors (false)
		cinematX.delayedInitCounter = cinematX.delayedInitCounter + 1
		
		-- Play a cutscene if specified in an onLoad event
		if  (cinematX.levelStartScene ~= nil  and  cinematX.midpointReached == false)  then
			cinematX.runCutscene (cinematX.levelStartScene)
		end
	end
	
	
	function cinematX.delayedInitSection ()
		--windowDebug ("TEST SECTION")
		cinematX.updateMidpointCheck ()
	
		-- Play a cutscene if specified in an onLoad event
		if  (cinematX.sectionStartScene[cinematX.delayedInitSectionNum] ~= nil)  then
			cinematX.runCutscene (cinematX.sectionStartScene [cinematX.delayedInitSectionNum])
		end
		
		--[[
		-- Play level start scene
		if  (cinematX.levelStartScene ~= nil  and  cinematX.levelStartScenePlayed == false  and  cinematX.midpointReached == false)  then
			cinematX.levelStartScenePlayed = true
			cinematX.runCutscene (cinematX.levelStartScene)
		end
		--]]
	end
	
	cinematX.currentFrameTime = 0
	
	
	function cinematX.initTiming ()
		cinematX.currentFrameTime = os.clock()
		cinematX.deltaTime = 0
	end
	
	
	cinematX.playerActor = Actor.create(player, "Player")
	
	cinematX.actorCount = 0
	cinematX.npcCount = 0
	
	cinematX.indexedActors = {}
	cinematX.indexedNPCRefs = {}
	cinematX.npcMessageKeyIndexes = {}

	cinematX.nilNPCPointer = nil
	cinematX.currentMessageNPCObj = nil
	cinematX.currentMessageNPCIndex = nil
	cinematX.currentMessageActor = nil
		
	function cinematX.initDialog ()
	
		-- Speaker object
		cinematX.dialogSpeaker = nil

		-- _____
		--cinematX.dialogSpeakerFrame = 0

		-- Text display countdown
		cinematX.dialogTextTime = 0

		-- Speaker animation countdown
		cinematX.dialogSpeakerTime = 0

		-- Dialogue update rate (characters per frame)
		cinematX.dialogTextSpeed = 1

		-- Check whether a line of dialogue or subtitle is being processed
		cinematX.dialogOn = false

		-- Speaker name
		cinematX.dialogName = ""

		-- Full line of dialogue being spoken with timing commands
		cinematX.dialogTextFull = ""

		-- Currently-revealed dialogue string
		cinematX.dialogText = ""
		
		-- Line of dialogue being spoken with timing commands
		cinematX.dialogTextCommands = ""

		-- Individual lines of the line of dialogue being spoken
		--cinematX.dialogTextLine1 = ""
		--cinematX.dialogTextLine2 = ""
		--cinematX.dialogTextLine3 = ""
		--cinematX.dialogTextLine4 = ""

		-- Total number of characters in the dialogue string
		cinematX.dialogNumCharsTotal = 0

		-- Number of characters revealed in the displayed dialogue
		-- Number of characters revealed in the displayed dialogue
		cinematX.dialogNumCharsCurrent = 0

		-- Number of characters revealed w/ bypassing commands
		cinematX.dialogNumCharsCommands = 0

		-- Can the player press the key to skip the current line?
		cinematX.dialogSkippable = true

		-- Is the current line of dialogue a question?
		cinematX.dialogIsQuestion = false

		-- Player's choice for the question
		cinematX.questionPlayerResponse = nil
		
		-- Does the player have to press a key to continue?
		cinematX.dialogEndWithInput = true
		
		
		-- Has this function been called?
		cinematX.dialogInitCalled = true
	end

	function cinematX.initCamera ()
		
		-- Memory addresses of the camera's position		
		cinematX.cameraXAddress = 0x00B2B984
		cinematX.cameraYAddress = 0x00B2B9A0
		
		cinematX.cameraFocusX = 0
		cinematX.cameraFocusY = 0
		
		cinematX.cameraTargetActor = -1
		
		cinematX.cameraOffsetX = 0
		cinematX.cameraOffsetY = 0
		
		cinematX.cameraXSpeed = 0
		cinematX.cameraYSpeed = 0
		
		cinematX.cameraControlOn = false
		
		cinematX.cameraSize = 1
	end


	-- Cutscene to play at the beginning of the level
	cinematX.levelStartScene = nil
	cinematX.levelStartScenePlayed = false
	
	-- Cutscenes to play at the beginning of each section
	cinematX.sectionStartScene = {}
	
	function cinematX.initCutscene ()
	  
		-- Cutscene/Boss AI timing variables
		cinematX.cutsceneFrame = 0
	  
		-- Can the entire cutscene be skipped?
		cinematX.cutsceneSkippable = true

		-- Player input currently active?
		cinematX.playerInputActive = true

		-- Current cutscene time in frames
		cinematX.cutsceneFrame = 0

		-- Current cutscene if there are multiple
		cinematX.cutsceneIndex = -1

		-- Scene state
		cinematX.SCENESTATE_PLAY = 0
		cinematX.SCENESTATE_CUTSCENE = 1
		cinematX.SCENESTATE_BATTLE = 2
		cinematX.SCENESTATE_RACE = 3

		cinematX.changeSceneMode (cinematX.SCENESTATE_PLAY)
	end

	function cinematX.initRace ()
		-- The opponent in a race
		cinematX.raceEnemyActor = nil
		
		cinematX.raceActive = false
		cinematX.raceWinRoutine = nil
		cinematX.raceLoseRoutine = nil
		
		cinematX.raceStartX = nil
		cinematX.raceEndX = nil
		
		cinematX.racePlayerPos = 0.000000
		cinematX.raceEnemyPos = 0.000000		
	end
	
	
	function cinematX.initBoss ()
		-- Current attack pattern time in frames
		cinematX.battleFrame = 0  

		-- "Phase" of the boss' attack pattern
		cinematX.battlePhase = 0

		-- For sequential attack patterns, this is the current step of the pattern
		cinematX.bossAttackPattern = 0

		-- Name for boss' HP bar
		cinematX.bossName = "BOSS NAME"

		-- Boss HP
		cinematX.bossHPMax = 8
		cinematX.bossHP = 8
	
		-- Boss health display modes
		cinematX.BOSSHPDISPLAY_NONE = 0
		cinematX.BOSSHPDISPLAY_HITS = 1
		cinematX.BOSSHPDISPLAY_BAR1 = 2
		
		-- The current display mode
		cinematX.bossHPDisplayType = cinematX.BOSSHPDISPLAY_HITS		
	end

	function cinematX.initHUD ()

		-- Image slot and filename constants
		cinematX.IMGSLOT_HUD 				=	999998
		cinematX.IMGSLOT_HUDBOX				=	999997
		cinematX.IMGSLOT_NPCICON_T_O		=	999996
		cinematX.IMGSLOT_NPCICON_T_N		=	999995
		cinematX.IMGSLOT_NPCICON_Q_O		=	999994
		cinematX.IMGSLOT_NPCICON_Q_N		=	999993
		cinematX.IMGSLOT_NPCICON_PRESSUP	=	999992
		
		cinematX.IMGSLOT_RACEOPPONENT		=	999991
		cinematX.IMGSLOT_RACEPLAYER			=	999990
		
		
		-- Color code constants
		cinematX.COLOR_TRANSPARENT = 0xFFFFFF--0xFB009D
		
		-- Filename constants
		cinematX.IMGNAME_BLANK	 				=	cinematX.resourcePath.."blankImage.bmp"
		cinematX.IMGNAME_LETTERBOX 				=	cinematX.resourcePath.."letterbox.bmp"
		cinematX.IMGNAME_FULLOVERLAY			=	cinematX.resourcePath.."fullScreenOverlay.bmp"
		cinematX.IMGNAME_PLAYDIALOGBOX			=	cinematX.resourcePath.."playSubtitleBox.bmp"
		cinematX.IMGNAME_BOSSHP_RIGHT 			= 	cinematX.resourcePath.."bossHP_right.bmp"
		cinematX.IMGNAME_BOSSHP_LEFT 			=	cinematX.resourcePath.."bossHP_left.bmp"
		cinematX.IMGNAME_BOSSHP_EMPTY 			= 	cinematX.resourcePath.."bossHP_midE.bmp"
		cinematX.IMGNAME_BOSSHP_FULL 			=	cinematX.resourcePath.."bossHP_midF.bmp"
		cinematX.IMGNAME_BOSSHP_BG 				=	cinematX.resourcePath.."bossHP_bg.bmp"
		
		cinematX.IMGNAME_RACEBG 				=	cinematX.resourcePath.."raceBg.bmp"
		cinematX.IMGNAME_RACEPLAYER				=	cinematX.resourcePath.."racePlayer.bmp"
		cinematX.IMGNAME_RACEOPPONENT			=	cinematX.resourcePath.."raceOpponent.bmp"
		
		cinematX.IMGNAME_HUDBOX					=	cinematX.resourcePath.."hudBox.bmp"
		
		cinematX.IMGNAME_NPCICON_TALK_O	 		=	cinematX.resourcePath.."npcIcon_TalkOld.bmp"
		cinematX.IMGNAME_NPCICON_TALK_N 		=	cinematX.resourcePath.."npcIcon_TalkNew.bmp"
		cinematX.IMGNAME_NPCICON_QUEST_O 		=	cinematX.resourcePath.."npcIcon_QuestOld.bmp"
		cinematX.IMGNAME_NPCICON_QUEST_N 		=	cinematX.resourcePath.."npcIcon_QuestNew.bmp"
		cinematX.IMGNAME_NPCICON_PRESSUP 		=	cinematX.resourcePath.."npcIcon_PressUp.bmp"		
		
		
		-- Stores the filename of the image loaded into IMGSLOT_HUD
		cinematX.currentHudOverlay = ""
		
		
		-- Set up the HUD overlay sprites
		cinematX.refreshHUDOverlay ()
		placeSprite (1, cinematX.IMGSLOT_HUD,    0, 0)
		placeSprite (1, cinematX.IMGSLOT_HUDBOX, 0, 0)

		
		-- Set up NPC icon sprites
		loadImage (cinematX.IMGNAME_NPCICON_TALK_O,  cinematX.IMGSLOT_NPCICON_T_O,  cinematX.COLOR_TRANSPARENT)
		loadImage (cinematX.IMGNAME_NPCICON_TALK_N,  cinematX.IMGSLOT_NPCICON_T_N,  cinematX.COLOR_TRANSPARENT)
		loadImage (cinematX.IMGNAME_NPCICON_QUEST_O,  cinematX.IMGSLOT_NPCICON_Q_O,  cinematX.COLOR_TRANSPARENT)
		loadImage (cinematX.IMGNAME_NPCICON_QUEST_N,  cinematX.IMGSLOT_NPCICON_Q_N,  cinematX.COLOR_TRANSPARENT)
		loadImage (cinematX.IMGNAME_NPCICON_PRESSUP,  cinematX.IMGSLOT_NPCICON_PRESSUP,  cinematX.COLOR_TRANSPARENT)
		
		--loadImage (cinematX.IMGNAME_RACEBG,  		cinematX.IMGSLOT_RACEBG,	  	cinematX.COLOR_TRANSPARENT)
		loadImage (cinematX.IMGNAME_RACEOPPONENT,	cinematX.IMGSLOT_RACEOPPONENT,  cinematX.COLOR_TRANSPARENT)
		loadImage (cinematX.IMGNAME_RACEPLAYER,  	cinematX.IMGSLOT_RACEPLAYER,  	cinematX.COLOR_TRANSPARENT)
	end
	
	function cinematX.initActors ()
		--cinematX.playerDummyActor = 
		
		--[[ 
			Player control mode: How to handle the player during cutscenes
				_PLAY 		= Player is active and visible, dummy actor is invisible and snapped to the 
								center of the screen
				_SCENE 		= Player and dummy actor swap positions and visibility; dummy actor serves
								as more flexibly-animated player, can change player position to pan camera
		--]]
		cinematX.PLAYERMODE_PLAY = 0
		cinematX.PLAYERMODE_SCENE = 1
		
		cinematX.playerControlMode = cinematX.PLAYERMODE_PLAY
	end
	
	function cinematX.initQuestSystem ()
		--cinematX.defineQuest ("test", "Test Quest", "Test the quest the quest the test system")
	end
	

	cinematX.debugLogTable = {}
	cinematX.debugCurrentLine = 00
			
	function cinematX.initDebug ()
		cinematX.showConsole = false
		
		cinematX.toConsoleLog ("Console test")
	end
end




--***************************************************************************************************
--                                                                                                  *
--              CINEMATX UPDATE FUNCTIONS													    	*
--                                                                                                  *
--***************************************************************************************************

do

	function cinematX.update ()
		-- Call level init
		if  (cinematX.initCalledYet == false)  then
			cinematX.initLevel ()
		end
	
		-- Keep track of how long the player holds the jump button
		--if (cinematX.playerHoldingJump == true) then
		--	cinematX.playerActor.jumpStrength = cinematX.playerActor.jumpStrength + 1
		--end
	
		cinematX.updateMidpointCheck ()
		cinematX.updateTiming ()
		cinematX.updateScene ()
		cinematX.updateActors ()
		cinematX.updateNPCMessages ()
		cinematX.updateDialog ()
		cinematX.updateRace ()
		cinematX.updateUI ()
		cinematX.updateCheats ()
		--cinematX.updateInput ()
		
		
		-- Call delayed init (level)
		if 	cinematX.delayedInitCalledFromUpdate == false  then
			cinematX.delayedInitLevel ()
			cinematX.delayedInitCalledFromUpdate = true
		end
	
		-- Call delayed init (section)
		if  cinematX.delayedInitSectionNum ~= player.section  then
		
			cinematX.delayedInitSection ()
			cinematX.delayedInitSectionNum = player.section
		end
	end

	function cinematX.updateMidpointCheck ()	
		local midpoint = findnpcs(192, -1) [0]
		
		if midpoint == nil then
			return
		end
		
		if (midpoint:mem (0x44, FIELD_WORD) ~= 0)  or  (midpoint:mem(0x122, FIELD_WORD) ~= 0)   then
			cinematX.midpointReached = true
			cinematX.toConsoleLog ("MIDPOINT")
		end
	end
	
	
	function cinematX.updateTiming ()
		local lastFrameTime = cinematX.currentFrameTime
		cinematX.currentFrameTime = os.clock ()
		cinematX.deltaTime = cinematX.currentFrameTime - lastFrameTime
		cinematX.wakeUpWaitingThreads (cinematX.deltaTime)  
	end

	function cinematX.updateScene ()

		
		-- Camera
		--cinematX.cameraFocusX = cinematX.cameraFocusX + cinematX.cameraXSpeed
		--cinematX.cameraFocusY = cinematX.cameraFocusY + cinematX.cameraYSpeed		
		
		if (cinematX.cameraControlOn == true) then
			--player.x = cinematX.cameraFocusX
			--player.y = cinematX.cameraFocusY
			--player.speedX = 0
			--player.speedY = 0
			--player:mem (0x112, FIELD_WORD, 500)
		end
		
		--mem (cinematX.cameraXAddress, FIELD_DWORD, cinematX.cameraOffsetX)
		--mem (cinematX.cameraYAddress, FIELD_DWORD, cinematX.cameraOffsetY)

		

		-- Unique behavior per cutscene mode
		if      cinematX.currentSceneState == cinematX.SCENESTATE_CUTSCENE   then
			cinematX.freezePlayerInput ()
		elseif  cinematX.currentSceneState == cinematX.SCENESTATE_BATTLE     then  
			--
		elseif  cinematX.currentSceneState == cinematX.SCENESTATE_PLAY       then    
			cinematX.unfreezePlayerInput ()
		end  
		
		--windowDebug ("UPDATE SCENE")
	end

	
	-- Process a cinematX tag from a valid NPC string
	function cinematX.parseTagFromNPCMessage (msgStr, tagName)
		local pTagStart = nil
		local pTagEnd = nil
		pTagStart, pTagEnd = string.find (msgStr, tagName.."=[%w_ ]+[,}]")
		
		if  (pTagStart == nil  or  pTagEnd == nil)  then
			return nil
		end
		
		pTagStart = pTagStart + string.len (tagName) + 1
		pTagEnd = pTagEnd - 1

		local pTagStr = string.sub (msgStr, pTagStart, pTagEnd)
		
		return pTagStr
	end
	
	
	-- Swap this for the location to store and read the NPC unique ID. 0x08 appears unused, but this can be changed to any unused word.
	cinematX.ID_MEM =  0x08
	
	function cinematX.indexActors (onlyIndexNew)
		-- If configured not to generate actors, abort this function
		if  (cinematX.shouldGenerateActors == false)  then
			return
		end
		
		-- Loop through every NPC, create an indexed actor instance for each one and store the messages & other info
		--local i = 0
		
		for k,v in pairs (npcs()) do
    
			local uid = v:mem (cinematX.ID_MEM, FIELD_WORD);
			local isDying = false
			if  (v:mem (0x122, FIELD_WORD)  >  0)  then
				isDying = true
			end
			
			
			--Assign a new unique ID to the NPC (this applies to all NPCs, not just CinematX enabled ones.
			if (uid == 0  and  isDying == false)  then
				uid = cinematX.npcCount + 1;
				v:mem (cinematX.ID_MEM, FIELD_WORD, uid);
				--windowDebug ("CHECK NPC ID: " .. v.id);
				cinematX.npcCount = cinematX.npcCount + 1;
			elseif 	isDying == true  then
				--windowDebug("Killed: "..v.msg.str);
			end
			
			--Have we already defined this actor? If so, then we update the SMBX reference accordingly.
			if (cinematX.indexedActors[uid] ~= nil) then
				cinematX.indexedActors[uid].smbxObjRef = v;
				cinematX.indexedActors[uid].uid = uid;
				cinematX.indexedActors[uid].isDirty = true
				
			
			--Otherwise, create a new actor, if necessary.
			else

				--Validity check message string to ensure we don't follow null pointers.
				local msgStr = nil
				if(v:mem (0x4C, FIELD_DWORD) > 0) then 
					msgStr = v.msg.str
				end

				if  (msgStr ~= nil  and   msgStr ~= "") then

					if (v:mem(0x122, FIELD_WORD) ~= 0)   then
						cinematX.toConsoleLog ("STILLBORN ACTOR")
					end

					
					-- Run qualifier function if it exists
					local shouldBeActor = true
					
					if  (cinematX.actorCriteria ~= nil)  then
						shouldBeActor = cinematX.actorCriteria (v)
					end
					
					if  (shouldBeActor == true)  then
					
						-- Create the actor and add it to the table
						local thisActor = Actor.create (v, "NPC")
						thisActor.uid = uid
						thisActor.messageNew = true   
						thisActor.messagePointer = v:mem (0x4C, FIELD_DWORD)
						cinematX.indexedActors[uid] = thisActor
					   
					   
						-- Get the message string
						thisActor.messageString = msgStr      
					   
					   
						-- Parse the message string						
					
						-- Get the substring between the first and last characters
						local checkStringA = string.sub  (msgStr, 2, string.len(msgStr)-1)
						-- Get JUST the first and last characters
						local checkStringB = string.gsub (msgStr, checkStringA, "")

						-- If this is false, not a valid cinematX message
						local shouldClearMessage = false
						
						if       (string.find (checkStringA, "[{}]") == nil
								  and  checkStringB == "{}")  then

							shouldClearMessage = true
								  
							-- Parse tags
							local parsedKey       = cinematX.parseTagFromNPCMessage (msgStr, "key")
							local parsedName      = cinematX.parseTagFromNPCMessage (msgStr, "name")
							local parsedTalkType  = cinematX.parseTagFromNPCMessage (msgStr, "verb")
							local parsedIcon      = cinematX.parseTagFromNPCMessage (msgStr, "icon")
							local parsedScene     = cinematX.parseTagFromNPCMessage (msgStr, "scene")
							local parsedRoutine   = cinematX.parseTagFromNPCMessage (msgStr, "routine")
							 
							 
							-- Store key for use in getNPCFromKey() if parsed
							if (parsedKey ~= nil) then
								cinematX.npcMessageKeyIndexes[parsedKey] = uid
								--windowDebug ("key = "..parsedKey..", "..tostring(cinematX.npcMessageKeyIndexes[parsedKey])..", "..tostring(cinematX.npcMessageKeyIndexes["calleoca"]))
							end
						 
							-- Store name if parsed
							if (parsedName == nil) then
								parsedName = ""
							end
							thisActor.nameString = parsedName
						  
							-- Store talk type string if parsed
							if (parsedTalkType == nil) then
								parsedTalkType = "talk"
							end
							thisActor.talkTypeString = parsedTalkType

							-- Store icon if parsed
							thisActor.wordBubbleIcon = tonumber (parsedIcon)

							-- Store scene
							thisActor.sceneString = parsedScene
							--windowDebug (thisActor.sceneString)

							-- Store routine
							thisActor.routineString = parsedRoutine

							-- Store whether the actor is interactive
							if  (parsedRoutine ~= nil   or   parsedScene ~= nil)  then
								thisActor.isInteractive = true
							end
						end
					   
					   
					   -- If set to override the SMBX message system, clear the NPC's message after storing it
						if   (cinematX.overrideNPCMessages == true  and  shouldClearMessage == true)   then
							local message = msgStr
							v.msg:clear()
							cinematX.indexedActors[uid] = thisActor
						end
						
						-- Increment the actor count
						cinematX.actorCount = cinematX.actorCount + 1;
						--i = i + 1;
					end					
				end
			end
        end
		
		
		-- Check dirty
		for k,v in pairs (cinematX.indexedActors) do
			if  (v.isDirty == true)  then
				cinematX.indexedActors[k].isDirty = false
				--windowDebug ("Actor cleaned")
			else
				cinematX.toConsoleLog ("Actor destroyed: "..(v.name)..", "..(v.npcid))
				cinematX.indexedActors[k].smbxObjRef = nil
				cinematX.indexedActors[k] = nil
			end
		end
    end
	
	
	function cinematX.updateActors ()
		
		-- If no actors are generated, skip the update
		if (cinematX.shouldGenerateActors == false) then
			return
		end
		
		
		-- Index new actors
		cinematX.indexActors (true)
		
		
		-- Loop through every actor and call their update methods
		cinematX.playerActor:update ()
		
		for k,v in pairs (cinematX.indexedActors) do
			if  (v ~= nil)  then
				if (v.smbxObjRef == nil) then
					cinematX.toConsoleLog ("ERROR: NPC DESTROYED")
					v = nil
				else
					v:update ()
				end
			end
		end
		
		
		-- Freeze the player
		
		if  playerInputActive == false   then
			player:mem(0x122, FIELD_WORD, 0)
			--[[
			player.UKeyState = 0;
			player.DKeyState = 0;
			player.LKeyState = 0;
			player.RKeyState = 0;
			player.JKeyState = 0;
			player.SJKeyState = 0;
			player.RKeyState = 0;
			
			player:mem(0x04, FIELD_WORD, 1)
			player:mem(0x06, FIELD_WORD, 0)
			
			player:mem(0xF2, FIELD_WORD, 0)
			player:mem(0xF4, FIELD_WORD, 0)
			player:mem(0xF6, FIELD_WORD, 0)
			player:mem(0xF8, FIELD_WORD, 0)
			player:mem(0xFA, FIELD_WORD, 0)
			player:mem(0xFC, FIELD_WORD, 0)
			player:mem(0x100, FIELD_WORD, 0)
			
			player:mem(0x118, FIELD_WORD, 0)
			player:mem(0x11E, FIELD_WORD, 0)
			player:mem(0x120, FIELD_WORD, 0)
			player:mem(0x122, FIELD_WORD, -1)
			--]]
		else
			--[[
			player:mem(0x04, FIELD_WORD, 0)
			player:mem(0x06, FIELD_WORD, 1)
			player:mem(0x122, FIELD_WORD, 0)
			--]]
		end  
	end

	function cinematX.updateNPCMessages ()
		cinematX.currentMessageActor = nil
			
		if   (cinematX.overrideNPCMessages == true   and   cinematX.currentSceneState == cinematX.SCENESTATE_PLAY)   then

			cinematX.refreshHUDOverlay ()

			cinematX.dialogOn = false
			cinematX.subtitleBox = false
			
			for k,v in pairs (cinematX.indexedActors) do
				--if cinematX.playerActor ~= nil then
				--	windowDebug ("SHOULD BE NO PROBLEM")
				--end
				
				
				if  (v ~= nil)  then
					if  (v.smbxObjRef ~= nil)  then
						if  (v:distanceActor (cinematX.playerActor) < 800   and   
							 (v.isInteractive == true))						then
							
							-- Check interaction type and whether the player has already spoken to the NPC
							local tempIconType = 0
							local tempIconNew = 0
							local tempIcon = cinematX.IMGSLOT_NPCICON_T_N
							
							if  (v.wordBubbleIcon ~= nil)   then
								tempIconType = 10*v.wordBubbleIcon
							end

							if  v.messageIsNew == false  then
								tempIconNew = 1
							end					
							
							
							-- Determine the icon type based on the above
							local tempAdd = tempIconType + tempIconNew
							
							if  	(tempAdd == 00)  then  tempIcon = cinematX.IMGSLOT_NPCICON_T_N
							elseif	(tempAdd == 01)  then  tempIcon = cinematX.IMGSLOT_NPCICON_T_O
							elseif 	(tempAdd == 10)  then  tempIcon = cinematX.IMGSLOT_NPCICON_Q_N
							elseif	(tempAdd == 11)  then  tempIcon = cinematX.IMGSLOT_NPCICON_Q_O
							end
								
							
							-- NPC Interaction indicators
							if 	(cinematX.showConsole == false)  then					
							
								-- If the player is close enough, store the NPC's index in 	currentMessageNPCIndex
								if  v:distanceActor (cinematX.playerActor) < 48  then
									cinematX.currentMessageNPCObj = v.smbxObjRef
									cinematX.currentMessageNPCIndex = k
									cinematX.currentMessageActor = v
									
									--tempIcon = cinematX.IMGSLOT_NPCICON_PRESSUP
									cinematX.subtitleBox = true
									cinematX.displayNPCSubtitle (v.nameString, "[UP] to "..v.talkTypeString..".")
								end
									
								
									-- Display the icon above the NPC
								if  (v.wordBubbleIcon ~= nil)  then
									placeSprite (2, tempIcon, v:getX()-8, v:getY()-64, "", 2)
								end
							end
						end
					end
					--[[
					if  (Actor_distanceActor (player, v) < 64   and   cinematX.npcMessageStrings[k] ~= "")  then
						--cinematX.dialogOn = true
						cinematX.displayNPCSubtitle ("Name", cinematX.npcMessageStrings[k])
					end
					--]]
				end
			end
		end
	end
	
	
	
	function cinematX.updateRace ()
		if  (cinematX.raceEnemyActor ~= nil
		and  cinematX.raceActive == true) 
		then
		
			-- Calculate relative position of player and opponent
			cinematX.racePlayerPos = invLerp (cinematX.raceStartX, cinematX.raceEndX, cinematX.playerActor:getX ())
			cinematX.raceEnemyPos = invLerp (cinematX.raceStartX, cinematX.raceEndX, cinematX.raceEnemyActor:getX ())		
			
			--[[
			printText (tostring(cinematX.racePlayerPos), 4, 5, 300)
			printText (tostring(cinematX.raceEnemyPos), 4, 5, 320)
			
			printText (tostring(cinematX.raceStartX), 4, 5, 340)
			printText (tostring(cinematX.raceEndX), 4, 5, 360)
			printText (tostring(cinematX.playerActor:getX ()), 4, 5, 380)
			printText (tostring(cinematX.raceEnemyActor:getX ()), 4, 5, 400)
			--]]
			
			-- Call win/lose coroutines
			if (cinematX.racePlayerPos >= 1) then
				cinematX.raceActive = false
				cinematX.runCoroutine (cinematX.raceWinRoutine)
			end
			
			if (cinematX.raceEnemyPos >= 1) then
				cinematX.raceActive = false
				cinematX.runCoroutine (cinematX.raceLoseRoutine)
			end
		end
	end

	
	
	cinematX.memMonitorAddress = 0x00B25068	
	cinematX.memMonitorField = 1
	cinematX.memMonitorScroll = 32
	
	
	function cinematX.updateUI ()
		
		--testThisThing ()
		
		-- MAIN HUD OVERLAY IS CHANGED WHEN cinematX.refreshHUDOverlay () IS CALLED
		-- BELOW ARE ADDITIONAL UI ELEMENTS BASED ON THE SCENE STATE
		
		-- RACE PROGRESS
		if   cinematX.currentSceneState == cinematX.SCENESTATE_RACE   then
			local raceMeterLeft = 56
			local raceMeterRight = 800-72
			local racePlayerIconX = lerp (raceMeterLeft, raceMeterRight, cinematX.racePlayerPos)
			local raceEnemyIconX = lerp (raceMeterLeft, raceMeterRight, cinematX.raceEnemyPos)
			local barY = 520
			
			if (racePlayerIconX > raceEnemyIconX)  then
				placeSprite (1, cinematX.IMGSLOT_RACEOPPONENT,    raceEnemyIconX, barY, "", 2)
				placeSprite (1, cinematX.IMGSLOT_RACEPLAYER,    racePlayerIconX, barY, "", 2)
			else
				placeSprite (1, cinematX.IMGSLOT_RACEPLAYER,    racePlayerIconX, barY, "", 2)
				placeSprite (1, cinematX.IMGSLOT_RACEOPPONENT,    raceEnemyIconX, barY, "", 2)			
			end
		end
			
		-- BOSS HP BAR
		if  cinematX.currentSceneState == cinematX.SCENESTATE_BATTLE   then

			-- Boss Name
			cinematX.printCenteredText (cinematX.bossName, 4, 400, 500)
			
			
			-- Different HP bar types
			
			-- BAR1 -- horizontal, unit-based, center-aligned (broken)
			if		(cinematX.bossHPDisplayType == cinematX.BOSSHPDISPLAY_BAR1)		then
			
				-- Bar sides
				local barLeft = player.screen.left + 400 - (cinematX.bossHPMax * 16)
				local barRight = barLeft + (cinematX.bossHPMax * 32)
				local barY = 520

				--placeSprite (1, IMG_BOSSHP_LEFT, barLeft, barY, "", 1)
				--placeSprite (1, IMG_BOSSHP_RIGHT, barRight, barY, "", 1)

				-- Bar units
				for i = 0, cinematX.bossHPMax-1 do
					local sprX = barLeft + (i+1)*32
					local sprImg = IMG_BOSSHP_EMPTY

					if (i <= cinematX.bossHP-1) then
						sprImg = IMG_BOSSHP_FULL
					end

					--placeSprite (1, sprImg, sprX, barY, "", 1)
				end

				
			-- HITS -- Just display the current and max hits
			elseif	(cinematX.bossHPDisplayType == cinematX.BOSSHPDISPLAY_HITS)		then
				cinematX.printCenteredText (cinematX.bossHP.."/"..cinematX.bossHPMax, 4, 400, 550)
			end
		end


		-- DISPLAY DIALOGUE/SUBTITLES
		if   cinematX.dialogOn == true  and  (cinematX.currentSceneState == cinematX.SCENESTATE_PLAY  or  cinematX.currentSceneState == cinematX.SCENESTATE_CUTSCENE) then

			if(cinematX.dialogName ~= "") then
			printText (cinematX.dialogName..":", 4, 5, 475)  
			end
			printText (string.sub (cinematX.dialogText, 1, 42), 4, 15, 495)
			printText (string.sub (cinematX.dialogText, 43, 85), 4, 15, 515)
			
			if (cinematX.dialogIsQuestion == true) then
				local 	tempBottomLine = "      YES                       NO          "
				
				if cinematX.getResponse() == true then
						tempBottomLine = "    > YES <                     NO          "
				end
				
				if cinematX.getResponse() == false then
						tempBottomLine = "      YES                     > NO <        "
				end
				
				printText (tempBottomLine, 4, 15, 555)
			
			else
				printText (string.sub (cinematX.dialogText, 86, 128), 4, 15, 535)
				printText (string.sub (cinematX.dialogText, 129, 171), 4, 15, 555)
				
				if   (cinematX.dialogEndWithInput == true  and  cinematX.dialogTextTime <= 0)   then
					printText("(PRESS X TO CONTINUE)", 4, 400, 580)
					cinematX.unfreezePlayerInput ()
				end

			end			
		end

		
		-- QUEST STUFF
		if   cinematX.displayQuestTimer > 0   then
			cinematX.displayQuestTimer = cinematX.displayQuestTimer - 1
			cinematX.displayQuestState (cinematX.currentQuestKey)
		end
		
		-- DEBUG STUFF
		if   (cinematX.showDebugInfo == true)   then
		
			-- Display console
			if  (cinematX.showConsole == true)  then	
				local i = 0
				for k,v in pairs (cinematX.debugLogTable) do
					--if (cinematX.debugCurrentLine > 15)
					
					printText (cinematX.debugLogTable[i], 4, 20, (550 - 20*cinematX.debugCurrentLine)+20*i)  
					i = i + 1
				end

				printText ("ACTORS: "..cinematX.actorCount, 4, 550, 100)  

				
				-- Display cheat input string
				local cheatStr = getInput().str
				if  (cheatStr ~= nil)  then
					printText ("INPUT: " .. getInput().str, 4, 20, 580)
				end
				
			else
				-- Disable cheating when the console is not open
				getInput():clear ()
			end
			--]]
			
			--[[
			printText ("Delta Time: "..string.format("%.3f", cinematX.deltaTime), 4, 20, 100)  
			printText ("Current Time: "..string.format("%.3f", cinematX.CURRENT_TIME), 4, 20, 120)  
			printText ("Cutscene mode: "..tostring(cinematX.currentSceneState), 4, 20, 140)  
			printText ("Coroutine: "..tostring(cinematX.currentCoroutine), 4, 20, 160)  
			printText ("Hud img path: "..cinematX.currentHudOverlay, 4, 20, 180)
			--]]
			
			-- NPC animation frames
			
			--[[
			local i = 0
			for k,v in pairs (cinematX.indexedActors) do
				
				printText (""..tostring(k).." animFrame "..tostring(v.animFrame).." animState "..tostring(v.animState), 4, 20, 100+20*i)

				i = i+1
			end
			--]]
			
			--printText ("delayedInit calls: "..tostring(cinematX.delayedInitCounter), 4, 20, 100)
				

			--cinematX.displayDebug_indexedKeys ()
			
			
			
			-- Display memory values - GLOBAL
			--[[
			local myX = 300
			local myY = 300	
			
			local tempMemAdr = 0x00
			local tempMemType = FIELD_WORD
			local tempMemVal = mem(tempMemAdr, tempMemType)
			
			local hexStr = string.format("%X", tempMemAdr)
			local valueStr = mem (tempMemAdr, tempMemType) --string.format("%X", v:mem(memAdrIterated, memField))
			
			printText (hexStr.."="..valueStr, 4, myX-64, myY-96) 
			--]]
			
			--[[
			local memScroll = cinematX.memMonitorScroll
			local memAdr = cinematX.memMonitorAddress
			local memFieldTypes = {FIELD_BYTE,FIELD_WORD,FIELD_DWORD,FIELD_FLOAT,FIELD_DFLOAT,FIELD_STRING}
			local memFieldNames = {"BYTE","WORD","DWORD","FLOAT","DFLOAT","STRING"}
			local memField = memFieldTypes [cinematX.memMonitorField] --DWORD
			local memSize = 2--1
			
			local myX = 300
			local myY = 500			
				
			printText (memFieldNames [cinematX.memMonitorField], 4, myX-192, myY-96)
				
			for i=0,16,1 do 
				local memAdrIterated = memAdr+memSize*i
				local hexStr = string.format("%X", memAdrIterated)
				local valueStr = mem (memAdrIterated, memField)
				
				if memField == FIELD_STRING then
					if valueStr ~= nil then
						valueStr = valueStr.str
					else
						valueStr = ""
					end
				end
				
				--string.format("%X", v:mem(memAdrIterated, memField))
				printText (hexStr.."="..valueStr, 4, myX-64, myY-96-(16*i)) 
			end	
			
				
			for i=0,16,1 do 
				local memAdrIterated = memScroll+memAdr+memSize*i
				local hexStr = string.format("%X", memAdrIterated)
				local valueStr = mem (memAdrIterated, memField)
				
				if memField == FIELD_STRING then
					if valueStr ~= nil then
						valueStr = valueStr.str
					else
						valueStr = ""
					end
				end
				
				--string.format("%X", v:mem(memAdrIterated, memField))
				printText (hexStr.."="..valueStr, 4, myX+256, myY-96-(16*i)) 
			end	
			--]]
			
			
			-- Display memory values - NPCs
			
			--[[
			for k,v in pairs (npcs()) do
				--local myX = v.x - (player.x - player.screen.left)
				--local myY = v.y - (player.y - player.screen.top)
			
				local spawnX = v:mem (0xAC, FIELD_WORD)
				local spawnY = v:mem (0xB4, FIELD_WORD)
				local currentX = v:mem (0x78, FIELD_DFLOAT)
				local currentY = v:mem (0x80, FIELD_DFLOAT)
			--]]
			
				--[[
				printText ("Y pos    "..tostring(currentY), 4, myX-160, myY-16*10)
				printText ("CY to SY "..tostring(currentY*-(8) - 1572864.08), 4, myX-160, myY-16* 9)
				--printText ("Y pos    "..tostring(currentY), 4, myX-160, myY-16* 9)
				
				printText ("Spawn Y  "..tostring(spawnY),   4, myX-160, myY-16* 7)
				--printText ("Spawn Y  "..tostring(spawnY),   4, myX-160, myY-16* )
								
				--]]
				--printText ("_"..v.msg.str.."_", 4, myX, myY-96)
				

				--if (v:mem (0x64, FIELD_WORD) == -1) then
					--v:mem(0x6A, FIELD_WORD, v:mem(0x6A, FIELD_WORD)-1)
				--end
				
				

				-- 1572863.88
				-- 1572863.98
				-- 1572864.08
			
				--[[
				printText ("X            = "..tostring(v.x),          						4, myX-160, myY-16* 10)  
				printText ("X (mem)      = "..tostring(currentX),     						4, myX-160, myY-16* 9)
				printText ("SpawnX       = "..tostring(spawnX),       						4, myX-160, myY-16* 8)  
				printText ("X To Spawn   = "..tostring(cinematX.coordToSpawnX (v.x)),      	4, myX-160, myY-16* 7)  
				printText ("Spawn To X   = "..tostring(cinematX.spawnToCoordX (spawnX)),   	4, myX-160, myY-16* 6)  
				--]]
				
				
				--[[
				local memAdr = cinematX.memMonitorAddress
				local memFieldTypes = {FIELD_BYTE,FIELD_WORD,FIELD_DWORD,FIELD_FLOAT,FIELD_DFLOAT}
				local memFieldNames = {"BYTE","WORD","DWORD","FLOAT","DFLOAT"}
				local memField = memFieldTypes[cinematX.memMonitorField] --DWORD
				local memSize = 2--1
				
				
				printText (memFieldNames[cinematX.memMonitorField], 4, myX-160, myY-96)
				
				for i=0,8,1 do 
					local memAdrIterated = memAdr+memSize*i
					local hexStr = string.format("%X", memAdrIterated)
					local valueStr = v:mem(memAdrIterated, memField)--string.format("%X", v:mem(memAdrIterated, memField))
					printText (hexStr.."="..valueStr, 4, myX-64, myY-96-(16*i)) 
				end	
				--]]				
			--end
			
			
			
			-- Coroutine threads
			--[[
			i = 0			
			for k,v in pairs (cinematX.WAITING_ON_TIME) do

				printText (tostring(k).." wait until "..string.format("%.3f", v), 4, 20, 200+20*i)  
				i = i+1
			end

			if  (i <= 0)  then
				printText ("No active coroutines", 4, 20, 180)  
			else
				printText ("Num coroutines = "..tostring(i), 4, 20, 180)  
			end
			
			
			
			--]]
			
			--[[			
			-- NPC Messages
			if   (cinematX.overrideNPCMessages == true)   then	

				for k,v in pairs (cinematX.npcMessageStrings) do

					printText (tostring(k), 								4,  20, 220 + 20*i)
					printText (tostring(cinematX.npcMessagePointers[i]),	4,  60, 220 + 20*i)
					printText (" -> "..v, 									4, 240, 220 + 20*i)
					i = i + 1
				end
			end
			--]]
			--windowDebug ("UPDATE UI")
		end

	end
	
	function cinematX.displayDebug_indexedKeys ()
		local i = 0
		for k,v in pairs (cinematX.npcMessageKeyIndexes) do
			printText ("K: "..tostring(k)..", V: "..tostring(v), 4, 20, 120+20*i)
			i = i+1
		end
		
		i = 0
		for k,v in pairs (cinematX.indexedActors) do
				printText ("NO KEY "..tostring(k)..", "..v.smbxObjRef.msg.str, 4, 400, 120+20*i)
			i = i+1
		end
		
		printText ("NUM ACTORS: "..tostring(i)..", "..tostring(cinematX.actorCount), 4, 20, 100)
		
		if i == 0 then
			printText ("ERROR: INDEXED KEYS NOT FOUND", 4, 20, 120+20*i)
		end
	end
	
	function cinematX.updateDialog ()

		-- Display the text only revealed by the typewriter effect
		local currentCharNum = math.floor (cinematX.dialogNumCharsCurrent)

		cinematX.dialogText = string.sub (cinematX.dialogTextFull, 0, currentCharNum)
		local currentChar = string.sub (cinematX.dialogTextFull, currentCharNum-1, currentCharNum)

		-- Decrement the dialogue timer and increment the typewriter effect.
		if   (cinematX.dialogTextTime > 0)   then
			cinematX.dialogTextTime = cinematX.dialogTextTime - 1
		end

		if (cinematX.dialogNumCharsCurrent < string.len (cinematX.dialogTextFull)) then
			cinematX.dialogNumCharsCurrent = cinematX.dialogNumCharsCurrent + cinematX.dialogTextSpeed
			--if (currentChar ~= " "  and  dialogNumCharsCurrent % 2 == 0) then
				--playSFX (35) -- 10 = skid, 14 = coin, 23 = shckwup, 24 = boing, 26 = skittish ticking, 29 = menu option, 33 = whoosh, 35 = blip
			--end
		end


		-- When the speaker timer reaches zero, reset the speaker.
		cinematX.dialogSpeakerTime = cinematX.dialogSpeakerTime - 1

		if(cinematX.dialogSpeakerTime == 0) then
			cinematX.dialogSpeaker = nil
		end		
	end

	-- cinematX-specific cheats
	function cinematX.updateCheats ()
		
		-- Check for non-despawned NPCs
		--if (cinematX.processCheat ("cinecheattest")) then
			--windowDebug ("CHEAT TEST")
		--end
	end
	
	function cinematX.processCheat (cheatString)
		local tempStr = getInput().str
		local isTrue = false
		
		if  (string.find (tempStr, cheatString..""))  then
			cinematX.showConsole = false
			playSFX (0)
			isTrue = true
			getInput():clear ()
		end
		
		return isTrue
	end
end



--***************************************************************************************************
--                                                                       		                    *
-- 				CINEMATX INPUT MANAGEMENT															*
--                                                                                                  *
--***************************************************************************************************

do
	-- REGISTER JUMP FOR PLAYER ACTOR ---------------
	
	function cinematX.onJump ()		
		cinematX.playerHoldingJump = true
		cinematX.playerActor.jumpStrength = 17
		cinematX.playerActor.framesSinceJump = 0
	end
	
	function cinematX.onKeyUp (keycode)
		
		-- Stop gauging the strength of the player's jump
		if   (keycode == KEY_JUMP  and  cinematX.playerHoldingJump == true)   then
			cinematX.playerHoldingJump = false
			--cinematX.playerActor.jumpStrength = 8
		end
	end
	
	function cinematX.onKeyDown (keycode)
		--cinematX.toConsoleLog ("Key pressed: "..tostring(keycode))
		
		
		-- DIALOG CONTROLS ------------------------------
		if (cinematX.dialogOn == true) then
			
			-- Choose a response to a yes/no question
			if  (cinematX.dialogIsQuestion == true)  then
				if (keycode == KEY_LEFT) then
					cinematX.questionPlayerResponse = true
					playSFX (3)
				end
			
				if (keycode == KEY_RIGHT) then
					cinematX.questionPlayerResponse = false
					playSFX (3)
				end
			end
			
			-- Skip dialog if allowed
			if  (keycode == KEY_X)  then
				if (cinematX.dialogNumCharsCurrent < string.len (cinematX.dialogTextFull)
						and  cinematX.dialogSkippable == true)  then
					
					cinematX.dialogNumCharsCurrent = string.len (cinematX.dialogTextFull)
					cinematX.dialogTextTime = 0
					cinematX.dialogSpeakerTime = 0
				
				elseif  (cinematX.dialogEndWithInput == true)   then
				
					if  (cinematX.dialogIsQuestion == true  and  cinematX.questionPlayerResponse == nil)  then
						playSFX (10) -- 10 = skid, 14 = coin, 23 = shckwup
					else
						playSFX (23) -- 10 = skid, 14 = coin, 23 = shckwup
						cinematX.endDialogLine ()
					end
				end
			end
		end
		
		-- Speak to NPCs (if SMBX messages are being overridden)
		if (keycode == KEY_UP    										and    
			cinematX.overrideNPCMessages ==  true  						and
			cinematX.currentMessageActor ~=  nil 	  					and
			cinematX.currentSceneState   ==  cinematX.SCENESTATE_PLAY)  then
			
				cinematX.processNPCMessage (cinematX.currentMessageNPCIndex)
		end
		
		-- DEBUG: CONSOLE
		if  (keycode == KEY_SEL  and  cinematX.showDebugInfo == true)  then
			if  (cinematX.showConsole == false)  then
				cinematX.showConsole = true
			else
				cinematX.showConsole = false
			end
		end
		
		-- DEBUG: MEMORY MONITOR
		if (keycode == KEY_UP) then
			cinematX.memMonitorAddress = cinematX.memMonitorAddress + cinematX.memMonitorScroll
		end
		
		if (keycode == KEY_DOWN  and  cinematX.memMonitorAddress > 0) then
			cinematX.memMonitorAddress = cinematX.memMonitorAddress - cinematX.memMonitorScroll
		end
		
		if (keycode == KEY_LEFT   and  cinematX.memMonitorField > 1) then
			cinematX.memMonitorField = cinematX.memMonitorField - 1
		end
		
		if (keycode == KEY_RIGHT  and  cinematX.memMonitorField < 6) then
			cinematX.memMonitorField = cinematX.memMonitorField + 1
		end
	end
	
	
	--cinematX.tempVKeyTable = mem(0x00B25068, FIELD_DWORD)
	
	function cinematX.unfreezePlayerInput ()
		cinematX.playerInputActive = true
		--mem(0x00B25068, FIELD_DWORD, cinematX.tempVKeyTable)
	end

	function cinematX.freezePlayerInput ()
		cinematX.playerInputActive = false
		
		--cinematX.tempVKeyTable = mem(0x00B25068, FIELD_DWORD)
		--mem(0x00B25068, FIELD_DWORD, 0x00B00000)

		--[[
		if  playerInputActive == 0   then
			player.UKeyState = 0;
			player.DKeyState = 0;
			player.LKeyState = 0;
			player.RKeyState = 0;
			player.JKeyState = 0;
			player.SJKeyState = 0;
			player.RKeyState = 0;
			
			player:mem(0x04, FIELD_WORD, 1)
			player:mem(0x06, FIELD_WORD, 0)
			
			player:mem(0xF2, FIELD_WORD, 0)
			player:mem(0xF4, FIELD_WORD, 0)
			player:mem(0xF6, FIELD_WORD, 0)
			player:mem(0xF8, FIELD_WORD, 0)
			player:mem(0xFA, FIELD_WORD, 0)
			player:mem(0xFC, FIELD_WORD, 0)
			player:mem(0x100, FIELD_WORD, 0)
			
			player:mem(0x118, FIELD_WORD, 0)
			player:mem(0x11E, FIELD_WORD, 0)
			player:mem(0x120, FIELD_WORD, 0)
		else
			player:mem(0x04, FIELD_WORD, 0)
			player:mem(0x06, FIELD_WORD, 1)
		end  
	--]]
	end


end
  
  
  
  
--***************************************************************************************************
--                                                                       		                    *
--              COROUTINE FUNCTIONS                                         		                *
--                                                                           		                *
--              THIS SECTION OF CODE BLATANTLY COPIED & EDITED FROM 								*
--				http://www.mohiji.org/2012/12/14/lua-coroutines/             						*
--                                                                                                  *
--***************************************************************************************************
 
do
	-- This table is indexed by coroutine and simply contains the time at which the coroutine
	-- should be woken up.
	cinematX.WAITING_ON_TIME = {}
	 
	-- Keep track of how long the game has been running.
	cinematX.CURRENT_TIME = 0
	
	
	function cinematX.waitSeconds (seconds)
		-- Grab a reference to the current running coroutine.
		local co = coroutine.running ()
		
		-- Store it in cinematX for good measure.
		cinematX.currentCoroutine = co
	 
	 
		-- If co is nil, that means we're on the main process, which isn't a coroutine and can't yield
		assert (co ~= nil, "The main thread cannot wait!")
	 
		-- Store the coroutine and its wakeup time in the WAITING_ON_TIME table
		local wakeupTime = cinematX.CURRENT_TIME + seconds
		cinematX.WAITING_ON_TIME [co] = wakeupTime
	    
		cinematX.toConsoleLog ("Begin waiting for " .. tostring(seconds) .. " seconds")

		-- And suspend the process
		return coroutine.yield (co)
	end
	
	
	function cinematX.wakeUpWaitingThreads (deltaTimeParam)
		-- This function should be called once per game logic update with the amount of time
		-- that has passed since it was last called
		cinematX.CURRENT_TIME = cinematX.CURRENT_TIME + deltaTimeParam
	 
		-- First, grab a list of the threads that need to be woken up. They'll need to be removed
		-- from the WAITING_ON_TIME table which we don't want to try and do while we're iterating
		-- through that table, hence the list.
		local threadsToWake = {}
		for co, wakeupTime in pairs(cinematX.WAITING_ON_TIME) do
			
			--windowDebug (wakeupTime.."/"..cinematX.CURRENT_TIME)
			
			if wakeupTime < cinematX.CURRENT_TIME then	
				table.insert (threadsToWake, co)
			end
		end
	 
		-- Now wake them all up.
		for _, co in ipairs(threadsToWake) do
			cinematX.WAITING_ON_TIME[co] = nil -- Setting a field to nil removes it from the table
			cinematX.toConsoleLog ("Waking up")
			coroutine.resume (co)
		end
	end
	 
	function cinematX.runCoroutine (func)
		-- This function is just a quick wrapper to start a coroutine.
		if (func ~= nil) then
			local co = coroutine.create (func)
			return coroutine.resume (co)
		end
	end
	 
	 
	--[[    DEMONSTRATION
	 
	runCoroutine (function ()
		print ("Hello world. I will now astound you by waiting for 2 seconds.")
		waitSeconds(2)
		print ("Haha! I did it!")
	end)
	 
	 
	From the original author of this script:
	"And that’s it. Call wakeUpWaitingThreads from your game logic loop and you’ll be able to have a bunch of functions waking up after sleeping for some period of time.
	 
	Note: this might not scale to thousands of coroutines. You might need to store them in a priority queue or something at that point."
	--]]
	  
	 
	cinematX.WAITING_ON_SIGNAL = {}

	function cinematX.waitSignal (signalName)
		-- Same check as in waitSeconds; the main thread cannot wait
		local co = coroutine.running ()
		assert (co ~= nil, "The main thread cannot wait!")

		if cinematX.WAITING_ON_SIGNAL[signalStr] == nil then
			-- If there wasn't already a list for this signal, start a new one.
			cinematX.WAITING_ON_SIGNAL[signalName] = { co }
		else
			table.insert (cinematX.WAITING_ON_SIGNAL[signalName], co)
		end
		
		return coroutine.yield ()
	end

	function cinematX.signal (signalName)
	
		local threads = cinematX.WAITING_ON_SIGNAL[signalName]
		if threads == nil then return end

		cinematX.WAITING_ON_SIGNAL[signalName] = nil
		for _, co in ipairs (threads) do
			coroutine.resume (co)
		end
	end
	
	function cinematX.waitForDialog ()
		cinematX.toConsoleLog ("Begin waiting for dialog")
		
		cinematX.waitSignal ("endDialog")
	end
end
	 
 
 
 
--***************************************************************************************************
-- 																									*
--				TEST ROUTINES																		*
-- 																									*
--***************************************************************************************************

do
	function cutscene_TestBreak ()
		cinematX.waitSeconds (2.0)
		npcToCoins ()
	end
	
	function cutscene_TestSceneStates ()		
		
		cinematX.toConsoleLog ("Begin testing screen modes")
		
		--windowDebug ("BEGIN TESTING SCENE MODES")
		cinematX.cycleSceneMode ()
		cinematX.waitSeconds (1.0)
		
		cinematX.cycleSceneMode ()	
		cinematX.waitSeconds (1.0)
				
		cinematX.cycleSceneMode ()	
		cinematX.waitSeconds (1.0)
		
		cinematX.toConsoleLog ("Finished testing screen modes")

		cinematX.endCutscene ()
	end
	
	function cutscene_LevelStartTest ()
	
		cinematX.setDialogSkippable (true)
		cinematX.setDialogInputWait (true)
	
		--cinematX.waitSeconds (1)
		cinematX.startDialog  (-1, "TEST", "THIS WORKS", 140, 120, "voice_talk1.wav")
		cinematX.waitForDialog ()		
		cinematX.endCutscene ()
	end
	
	function cutscene_TestDialog ()
	
		cinematX.setDialogSkippable (true)
		cinematX.setDialogInputWait (true)
	
		--cinematX.waitSeconds (1)
		cinematX.startDialog  (NPCID_BROADSWORD, cinematX.getActorName_Key("goopa1"), "TEST DIALOG TEST DIALOG TEST DIALOG TEST DIALOG TEST DIALOG TEST DIALOG TEST DIALOG TEST DIALOG TEST DIALOG", 140, 120, "voice_talk1.wav")
		cinematX.waitForDialog ()
		
		cinematX.waitSeconds (1)
		cinematX.startDialog  (NPCID_BROADSWORD, cinematX.getActorName_Key("goopa1"), "Hello, this is a test of the cutscene system.", 120, 100, "voice_talk2.wav")
		cinematX.waitForDialog ()

		cinematX.startDialog  (NPCID_BROADSWORD, cinematX.getActorName_Key("goopa1"), "Please remain calm and keep your arms and legs inside the cutscene at all times.", 120, 100, "voice_talk3.wav")	
		cinematX.waitForDialog ()
		
		cinematX.waitSeconds (1)
		cinematX.endCutscene ()
	end

end

 
 
--***************************************************************************************************
--                                                                       		                    *
-- 				DIALOGUE MANAGEMENT																	*
--                                                                                                  *
--***************************************************************************************************

do

	--function cinematX.setDialogRules (pause, skippable, needInput, textSpeed)
	function cinematX.configDialog (skippable, needInput, textSpeed)
		--cinematX.dialogPause = pause
		cinematX.setDialogSkippable (skippable)
		cinematX.setDialogInputWait (needInput)
		cinematX.setDialogSpeed (textSpeed)
	end
	
	function cinematX.setDialogSpeed (textSpeed)
		cinematX.dialogTextSpeed = textSpeed
	end

	function cinematX.setDialogSkippable (skippable)
		cinematX.dialogSkippable = skippable
	end	
	
	function cinematX.setDialogInputWait (needInput)
		cinematX.dialogEndWithInput = needInput
	end
	
	function cinematX.startQuestion (speakerActor, name, text, textTime, speakTime, sound)
		cinematX.startDialog (speakerActor, name, text, textTime, speakTime, sound)
		cinematX.dialogIsQuestion = true
		cinematX.questionPlayerResponse = nil
	end

	function cinematX.getResponse ()
		return cinematX.questionPlayerResponse
	end

	
	function cinematX.formatDialogForWrapping (str)
		local tl = str;
		local hd = "";
		local i = 1;
		while (string.len(tl)>42) do
			local split = cinematX.wrapString(tl,42);
			split.hd = split.hd:gsub("^%s*", "")
			split.tl = split.tl:gsub("^%s*", "")
			local c = 42;
			if(i > 1) then c = 43; end
			while (string.len(split.hd) < c) do
				split.hd = split.hd.." ";
			end
			hd = hd..split.hd;
			tl = split.tl;
			i = i + 1;
		end
		return hd..tl;
	end

	function cinematX.wrapString (str, l)
		local head = "";
		local tail = "";
		local wrds = {}
		local i = 0;
		for j in string.gmatch(str, "%S+") do
			wrds[i] = j;
			i = i + 1
		end
		i = 0;
		while(wrds[i] ~= nil) do
			local newHd = head.." "..wrds[i];
			if(string.len(newHd) <= l) then
				head = newHd;
				i = i + 1
			else
				break;
			end
		end

		while(wrds[i] ~= nil) do
			tail = tail.." "..wrds[i];
			i = i + 1
		end
      
		return { hd = head, tl = tail };
	end


	
	function cinematX.startDialog (speakerActor, name, text, textTime, speakTime, sound)
		--windowDebug ("TEST C")
				
		-- Voice clip
		if  cinematX.dialogOn == false  and  sound ~= ""  then
			playSFXSDL (sound)
		end
		
		--NPC speaking animation
		if   (speakerActor ~= nil  and  speakerActor ~= -1)   then
			cinematX.triggerDialogSpeaker (speakerActor, speakTime)
		end
		
		cinematX.triggerDialogText (name, text, textTime)
		cinematX.dialogOn = true
	end
	
	function cinematX.triggerDialogSpeaker (speakerActor, timeVal)
		cinematX.dialogSpeaker = speakerActor
		cinematX.dialogSpeakerTime = timeVal
	end

	function cinematX.triggerDialogText (name,text,textTime)
		cinematX.dialogName = name
		cinematX.dialogTextFull = cinematX.formatDialogForWrapping (text)
		cinematX.dialogTextTime = textTime
	end
	
	function cinematX.endDialogLine ()
		cinematX.dialogOn = false
		cinematX.dialogTextTime = 0
		cinematX.dialogSpeakerTime = 0
		cinematX.dialogSpeaker = 0
		cinematX.dialogNumCharsCurrent = 0
		cinematX.dialogNumCharsTotal = 0
		cinematX.dialogTextFull = ""
		
		cinematX.dialogIsQuestion = false
		
		cinematX.signal ("endDialog")
	end

	function cinematX.displayNPCSubtitle (name, text)
		--cinematX.dialogOn = true
		cinematX.printCenteredText (name, 4, 400, 500)
		cinematX.printCenteredText (text, 4, 400, 520)
	end
	
	function cinematX.processNPCMessage (npcIndex)
		
		-- Parse the NPC's message
		--local tempMsgStr = cinematX.npcMessageStrings [npcIndex]
		local thisActor = cinematX.indexedActors[npcIndex]
		thisActor.messageIsNew = false
		
		local tempFunctStr = ""
		local tempFunctA = nil
		local tempFunctB = nil
		
		--windowDebug ("R: " .. cinematX.npcMessageRoutines [npcIndex] .. ", C: " .. cinematX.npcMessageScenes [npcIndex])
		--windowDebug ("R: " .. cinematX.npcMessageRoutines [npcIndex] .. ", C: " .. cinematX.npcMessageScenes [npcIndex])
		
		
		-- Call the coroutine or cutscene
		if  	(thisActor.sceneString ~= nil)   then
			tempFunctStr = thisActor.sceneString
			tempFunctA = loadstring ("return __lunalocal."..tempFunctStr)
			tempFunctB = tempFunctA ()
			--windowDebug (tempFunctStr .. " " .. type(tempFunctB))
			cinematX.runCutscene (tempFunctB)
			
			--loadstring ("cinematXMain.runCutscene (__lunalocal."..tempFunctString..")") ()
			
		elseif  (thisActor.routineString ~= nil) then
			tempFunctStr = thisActor.sceneString
			tempFunctA = loadstring ("return __lunalocal."..tempFunctStr)
			tempFunctB = tempFunctA ()
			--windowDebug (tempFunctStr .. " " .. type(tempFunctB))
			cinematX.runCoroutine (tempFunctB)
		end
		
		
		-- LOG TO CONSOLE
		if  (tempFunctB == nil)  then
			cinematX.toConsoleLog ("ERROR: Function '" .. tempFunctStr .. "' does not exist")
		else
			cinematX.toConsoleLog ("Actor " .. tostring(npcIndex) .. " calling scene " .. tempFunctStr)
		end
	end
	
	cinematX.playerNameASXT = function ()
		return cinematX.playerNames ("Demo", "Iris", "Kood", "raocow", "Sheath")
	end
	
	cinematX.playerNameASXT2 = function ()
		return cinematX.playerNames ("Nevada", "Pily", "Alt P3", "Alt P4", "Broadsword")
	end
	
	cinematX.playerNameSMBX = function ()
		return cinematX.playerNames ("Mario", "Luigi", "Peach", "Toad", "Link")
	end
	
	function cinematX.playerNames (marioName, luigiName, peachName, toadName, linkName)
		local indexVal = player:mem(0xF0, FIELD_WORD) --player.Identity

		if      indexVal == 0  then
			return "NONE"
		elseif  indexVal == 1  then
			return marioName
		elseif  indexVal == 2  then
			return luigiName
		elseif  indexVal == 3  then
			return peachName
		elseif  indexVal == 5  then
			return linkName
		else
			return toadName
		end
	end
	
	function cinematX.printCenteredText (text, font, xPos, yPos)
	    if text ~= nil then
		    printText (text, font, xPos-9 * string.len(text), yPos)
	    end
	end
end 



--***************************************************************************************************
--                                                                       		                    *
-- 				BOSS/DYNAMIC SEQUENCE MANAGEMENT													*
--                                                                                                  *
--***************************************************************************************************

do
	function cinematX.beginRace (otherActor, startX, endX, raceFunc, loseFunc, winFunc)
		
		cinematX.raceEnemyActor = otherActor
		cinematX.raceStartX = startX
		cinematX.raceEndX = endX
		
		cinematX.raceLoseRoutine = loseFunc
		cinematX.raceWinRoutine = winFunc
		
		cinematX.changeSceneMode (cinematX.SCENESTATE_RACE)		
		cinematX.raceActive = true
		
		cinematX.runCoroutine (raceFunc)
	end
	
	
	function cinematX.beginBattle (name, hits, barType, func)
		cinematX.bossName = name
		
		cinematX.bossHPMax = hits
		cinematX.bossHP = cinematX.bossHPMax
		
		cinematX.bossHPDisplayType = barType
		cinematX.changeSceneMode (cinematX.SCENESTATE_BATTLE)
		
		cinematX.runCoroutine (func)
	end
	
	function cinematX.getBattleProgress ()
		return cinematX.bossHP
	end
	
	function cinematX.getBattleProgressPercent ()
		return (cinematX.bossHPMax - cinematX.bossHP) / cinematX.bossHPMax
	end	
	
	function cinematX.setBattleProgress (amount)
		cinematX.bossHP = cinematX.bossHP - amount
		
		if cinematX.bossHP <= 0 then
			cineatX.winBattle ()
		end
	end
		
	function cinematX.winBattle ()
	end
	
	function cinematX.loseBattle ()
	end
end

 
 
 
--***************************************************************************************************
--                                                                       		                    *
-- 				SIDEQUEST MANAGEMENT																*
--                                                                                                  *
--***************************************************************************************************

do
	cinematX.questName = {}
	cinematX.questDescr = {}
	cinematX.currentQuestKey = ""
	cinematX.displayQuestTimer = 0
	

	function cinematX.defineQuest (questKey, missionName, missionText)
		cinematX.setQuestName (questKey, missionName)
		cinematX.setQuestDescription (questKey, missionText)
	end

	function cinematX.setQuestName (questKey, missionName)
		cinematX.questName[questKey] = missionName
	end

	function cinematX.setQuestDescription (questKey, missionText)
		cinematX.questDescr[questKey] = missionText
	end

	function cinematX.displayQuestState (questKey)
		local tempState = cinematX.getQuestState (questKey)
		
		if		tempState == 1  then
			cinematX.printCenteredText ("QUEST ACCEPTED:", 4, 400, 200)
		elseif	tempState == 2  then
			cinematX.printCenteredText ("QUEST COMPLETED:", 4, 400, 200)
		end
		
		cinematX.printCenteredText (cinematX.questName[questKey], 4, 400, 240)
		cinematX.printCenteredText (cinematX.questDescr[questKey], 4, 400, 260)
		--cinematX.printCenteredText (tostring(cinematX.getQuestState(questKey)), 4, 400, 380, 60)
	end
	
	function cinematX.setQuestState (questKey, questState)
		UserData.setValue("questState_" .. questKey, questState)
		UserData.save()
		
		if   questState ~= 0   then
			cinematX.currentQuestKey = questKey
			cinematX.displayQuestTimer = 120
		end
	end
	
	function cinematX.setQuestProgress (questKey, questProgress, newMessage)
		UserData.setValue("questProg_" .. questKey, questProgress)
		UserData.save()
				
		if (newMessage ~= "") then
			cinematX.setQuestDescription (questKey, newMessage)
		end
	end
	
	function cinematX.initQuest (questKey)
		cinematX.setQuestState (questKey, 0)
		cinematX.setQuestProgress (questKey, 0)
	end

	function cinematX.beginQuest (questKey)
		cinematX.setQuestState (questKey, 1)
	end

	function cinematX.finishQuest (questKey)
		cinematX.setQuestState (questKey, 2)
	end
	
	function cinematX.getQuestState (questKey)
		return UserData.getValue("questState_" .. questKey)
	end
	
	function cinematX.isQuestStarted (questKey)
		if  cinematX.getQuestState (questKey) > 0  then
			return true
		else
			return false
		end
	end
	
	function cinematX.isQuestFinished (questKey)
		if  cinematX.getQuestState (questKey) == 2  then
			return true
		else
			return false
		end
	end
end




--***************************************************************************************************
--                                                                       		                    *
-- 				CUTSCENE MANAGEMENT																	*
--                                                                                                  *
--***************************************************************************************************

do
	cinematX.tempPlayerX = 0
	cinematX.tempPlayerY = 0
	cinematX.tempPlayerXSpeed = 0
	cinematX.tempPlayerYSpeed = 0
	cinematX.tempPlayerPowerup = 0
	cinematX.tempPlayerState = 0
	
	function cinematX.savePlayerPosition ()
		cinematX.tempPlayerX = player.x
		cinematX.tempPlayerY = player.y
		cinematX.tempPlayerXSpeed = player.speedX
		cinematX.tempPlayerYSpeed = player.speedY
		cinematX.tempPlayerPowerup = player.powerup
		cinematX.tempPlayerState = player:mem (0x112, FIELD_WORD)
	end	
	
	function cinematX.restorePlayerPosition ()
		player.x = cinematX.tempPlayerX
		player.y = cinematX.tempPlayerY
		player.speedX = cinematX.tempPlayerXSpeed
		player.speedY = cinematX.tempPlayerYSpeed
		player.powerup = cinematX.tempPlayerPowerup
		player:mem (0x112, FIELD_WORD, cinematX.tempPlayerState)
	end
	
	function cinematX.enterCameraMode ()
		cinematX.savePlayerPosition ()
		cinematX.cameraFocusX = player.x --0.5*(Player.screen.left + Player.screen.right)
		cinematX.cameraFocusY = player.y --0.5*(Player.screen.top + Player.screen.bottom)
		cinematX.cameraControlOn = true
	end
	
	function cinematX.exitCameraMode ()
			cinematX.cameraControlOn = false
			player:mem (0x112, FIELD_WORD, 0)
			cinematX.restorePlayerPosition ()
	end
	
	function cinematX.runCutscene (func)
		cinematX.changeSceneMode (cinematX.SCENESTATE_CUTSCENE)
		--cinematX.enterCameraMode ()
		
		return cinematX.runCoroutine (func)
	end
		
	function cinematX.endCutscene ()
		cinematX.changeSceneMode (cinematX.SCENESTATE_PLAY)
		cinematX.unfreezePlayerInput ()
		--cinematX.exitCameraMode ()
	end
	
	function cinematX.cycleSceneMode ()
		cinematX.changeSceneMode ((cinematX.currentSceneState + 1) % 4)
	end
		
	function cinematX.changeSceneMode (sceneModeType)	
		cinematX.currentSceneState = sceneModeType
		cinematX.refreshHUDOverlay ()
		cinematX.toConsoleLog ("SWITCH TO STATE "..cinematX.currentSceneState)

	end
	
	function cinematX.refreshHUDOverlay ()
		-- Enable the hud
		hud (true)

		
		-- DEBUG CONSOLE
		if  	(cinematX.showConsole == true)									then
			hud (false)
			cinematX.changeHudOverlay (cinematX.IMGNAME_FULLOVERLAY)		
		
		-- Race mode
		elseif  (cinematX.currentSceneState  ==  cinematX.SCENESTATE_RACE)      then
			cinematX.changeHudOverlay (cinematX.IMGNAME_RACEBG)
		
		-- Boss battle mode
		elseif	(cinematX.currentSceneState  ==  cinematX.SCENESTATE_BATTLE)	then
			cinematX.changeHudOverlay (cinematX.IMGNAME_BOSSHP_BG)		
		
		-- Cutscene mode
		elseif	(cinematX.currentSceneState  ==  cinematX.SCENESTATE_CUTSCENE)	then
			cinematX.changeHudOverlay (cinematX.IMGNAME_LETTERBOX)
			
			-- Disable the hud
			hud (false)

		
		-- Play mode
		elseif	(cinematX.currentSceneState  ==  cinematX.SCENESTATE_PLAY)		then

			if      (cinematX.dialogOn == true)       then
				cinematX.changeHudOverlay (cinematX.IMGNAME_PLAYDIALOGBOX)
			elseif  (cinematX.subtitleBox == true)    then
				cinematX.changeHudOverlay (cinematX.IMGNAME_BOSSHP_BG)
			else
				cinematX.changeHudOverlay (cinematX.IMGNAME_BLANK)
			end
		end
	end
	
	function cinematX.changeHudOverlay (imageName)
		
		if   (cinematX.currentHudOverlay  ~=  imageName)   then
			-- Hud box
			if   (imageName == cinematX.IMGNAME_LETTERBOX)  then
				loadImage (cinematX.IMGNAME_BLANK,  cinematX.IMGSLOT_HUDBOX,  cinematX.COLOR_TRANSPARENT)
			else
				loadImage (cinematX.IMGNAME_HUDBOX,  cinematX.IMGSLOT_HUDBOX,  cinematX.COLOR_TRANSPARENT)
			end

			
			-- Other overlay sprites	
			loadImage (imageName,  cinematX.IMGSLOT_HUD,  cinematX.COLOR_TRANSPARENT)
			cinematX.currentHudOverlay  =  imageName
		end		
	end	
end

	

	
--***************************************************************************************************
--                                                                       		                    *
-- 				DEBUG STUFF																			*
--                                                                                                  *
--***************************************************************************************************

do
	function cinematX.toConsoleLog (text)
		cinematX.debugLogTable [cinematX.debugCurrentLine] = text
		cinematX.debugCurrentLine = cinematX.debugCurrentLine + 1
	end
	
	function cinematX.displayDebugText (text)
		if  (cinematX.showDebugInfo == true)  then
			cinematX.printCenteredText (text, 4, 400, 300+math.random(-1,1))
		end
		
		cinematX.toConsoleLog (text)
	end

end




--***************************************************************************************************
--                                                                       		                    *
-- 				MATH																				*
--                                                                                                  *
--***************************************************************************************************

do
	function lerp (minVal, maxVal, percentVal)
		return (1-percentVal) * minVal + percentVal*maxVal;
	end
	
	function invLerp (minVal, maxVal, amountVal)			
		return  math.min(1.00000, math.max(0.0000, math.abs(amountVal-minVal) / math.abs(maxVal - minVal)))
	end
	
	function cinematX.coordToSpawnX (xPos)
		local newX = xPos*-(8) - 1572863.88
		return newX
	end
	
	function cinematX.coordToSpawnY (yPos)
		local newY = yPos*-(8) - 1572864.08
		return newY
	end

	function cinematX.spawnToCoordX (xPos)
		local newX = -(xPos + 1572863.88)/8
		return newX
	end
	
	function cinematX.spawnToCoordY (yPos)
		local newY = -(yPos + 1572864.08)/8
		return newY
	end
end


	
--***************************************************************************************************
--                                                                       		                    *
-- 				NPC MANAGEMENT																		*
--                                                                                                  *
--***************************************************************************************************

do	
	function cinematX.getNPCFromKey (keyStr)
		return cinematX.getActorFromKey (keyStr).smbxObjRef
	end

	function cinematX.getActorFromKey (keyStr)
		local thisIndex = cinematX.getNPCIndexFromKey (keyStr)
		return cinematX.indexedActors[cinematX.npcMessageKeyIndexes[keyStr]]  --thisIndex]
	end

	function cinematX.getNPCIndexFromKey (keyStr)
		local thisIndex = cinematX.npcMessageKeyIndexes[keyStr]
		--cinematX.toConsoleLog ("GOT INDEX "..tostring(thisIndex).." FROM KEY "..keyStr)
		--windowDebug ("key = '"..keyStr.."', "..tostring(cinematX.npcMessageKeyIndexes[keyStr])..", "..tostring(cinematX.npcMessageKeyIndexes["calleoca"]))
		return thisIndex
	end

	
	function cinematX.resetNPCMessageNew_Key (keyStr)
		--cinematX.resetNPCMessageNew_Index (cinematX.getNPCIndexFromKey (keyStr))
	end
	
	function cinematX.resetNPCMessageNew_Index (index)
		--cinematX.npcMessageNew [index] = true
	end

	
	function cinematX.setNPCName_Key (keyStr, nameStr)
		--cinematX.setNPCName_Index (cinematX.getNPCIndexFromKey (keyStr), nameStr)
	end
	
	function cinematX.setNPCName_Index (index, nameStr)
		--cinematX.npcMessageNames [index] = nameStr
	end

	
	function cinematX.getNPCName_Key (keyStr)
		return cinematX.getNPCName_Index (cinematX.getNPCIndexFromKey (keyStr))
	end
	
	function cinematX.getActorName_Key (keyStr)
		return cinematX.getActorName_Index (cinematX.getNPCIndexFromKey (keyStr))
	end
	
	function cinematX.getNPCName_Index (index)
		return cinematX.npcMessageNames [index]
	end
	
	function cinematX.getActorName_Index (index)
		return cinematX.indexedActors [index].nameString
	end
		
end

	
return cinematX