local SMWCam = {}

local camera = nil			-- Camera object
local active = true			-- SMW camera logic is activated
local sectbounds = nil		-- Current section boundaries
local cambounds = nil		-- Camera boundaries, defaulted to section boundaries
local prevSection = -1		-- Section player was in previous frame

local warpingOut = false	-- If the player coming out of a warp?
local prevpos = {x=0, y=0}	-- Position of player previous frame

local bound = nil			-- Player boundaries, used to guide camera
local prevbound = nil		-- Player boundaries on previous frame
local delta = nil			-- Change in player boundaries

local line = nil			-- Vertical boundary line
local line2 = nil			-- Horizontal boundary line
local groundline = nil		-- Ground level (red)
local debugmode = true		-- Debug mode
--------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------
local LKeyTap = false				-- L button was tapped for a frame
local RKeyTap = false				-- R button was tapped for a frame
local leftKeyWasPressed = false		-- Left key was down on previous frame
local rightKeyWasPressed = false	-- Right key was down on previous frame
local doubleTapCounter = 0			-- Counter for registering double tap
local DOUBLE_TAP_TIME = 25			-- Maximum frames allowed to provide a second tap
local LRDirection = 0				-- L/R button panning direction
local LRPanLocked = false			-- L/R panning has been activated
--------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------
-- Horizontal snapping data
local PANSPEEDX = 25/6			-- Speed for horizontal camera panning
local lastCameraX = 0			-- Camera X position on previous frame

local HORIZ_GAP = 32*PANSPEEDX	-- Pixels allowed when changing direction before camera snaps to player
local directionX = 0			-- Panning direction, -1 is left
local lastBndTouchX = 0			-- Last boundary touched, -1 is left
--------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------
-- Vertical snapping data
local PANSPEEDY = 25/8			-- Speed for vertical camera panning
local targetCameraY = 0			-- Y to pan camera to
local lastCameraY = 0			-- Camera Y position on previous frame

local VERT_GAP = 32*PANSPEEDY	-- Pixels allowed before camera snaps

local yPanning = false			-- Is the camera panning vertically?

local freeform = true			-- Free tracking camera
local grounded = false			-- Is the player on the ground?
local lastPlatformY = 0			-- Y coordinate of last platform touched

local UDPanCounter = 0			-- Frame counter to UD panning
local UDPanDirection = 0		-- Direction to UD pan
local MAX_UD_COUNTER = 75		-- Frames required before UD panning
local UDPanTargetSet = false	-- Target has been set for UD panning
--------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------
local UNLOCKED = 0				-- Camera is restricted only by the section boundaries
local PLAYERLOCKED = 1			-- Camera is restricted by player-defined boundaries
local LOCKING = 2				-- Camera is currently locking into position
local UNLOCKING = 3				-- Camera is returning to the player's control
local AUTOSCROLLING = 4			-- Camera is autoscrolling
local camlockState = UNLOCKED	-- What is the camera doing?
local autoscroll = nil			-- Autoscroll speeds

LOCKTYPE_INSTANT = 0			-- The camera should instantly snap into position
LOCKTYPE_CONSTANT = 1			-- The camera should move into position at a constant speed
LOCKTYPE_SMOOTH = 2				-- The camera should linearly interpolate into position
local lockparam = nil			-- Arguments passed to module when locking camera
--------------------------------------------------------------------------------------------------------------------------

-- Get reference to camera
local function GetCamera()
	-- Get first player camera
	camera = Camera.get()[1]
	if not camera then error("SMW Camera Error: Could not get camera.") end
end
-- Get rectangle defining current section boundaries
local function GetSectionBounds()
	-- Get section boundaries
	sectbounds = Section.get(player.section+1).boundary
	if not sectbounds then error("SMW Camera Error: Could not get section boundaries.") end
end
-- Initialize variables
function SMWCam.Init()
	-- (Mom) get the camera
	GetCamera()
	-- Center camera on player to start
	camera.x = player.x + player.width/2 - camera.width/2
	camera.y = player.y + player.height - camera.height/2
	-- Update section info and camera boundaries
	GetSectionBounds()
	cambounds = {left = sectbounds.left, right = sectbounds.right, top = sectbounds.top, bottom = sectbounds.bottom}
	prevSection = player.section
	
	-- Adjust camera to remain within section
	if camera.x < sectbounds.left then camera.x = sectbounds.left
	elseif camera.x+camera.width > sectbounds.right then camera.x = sectbounds.right-camera.width end
	if camera.y < sectbounds.top then camera.y = sectbounds.top
	elseif camera.y+camera.height > sectbounds.bottom then camera.y = sectbounds.bottom-camera.height end
	
	-- Initialize other variables
	lastCameraX = camera.x
	lastCameraY = camera.y
	
	bound = {left = 0, right = 0, top = 0, bottom = 0}
	bound.left = player.x + player.width/2 - HORIZ_GAP/2
	bound.right = bound.left + HORIZ_GAP
	
	bound.bottom = player.y + player.height
	bound.top = bound.bottom - VERT_GAP
	lastPlatformY = cambounds.bottom
	targetCameraY = camera.y
	
	prevbound = {left = bound.left, right = bound.right, top = bound.top, bottom = bound.bottom}
	delta = {left = 0, right = 0, top = 0, bottom = 0}
	
	lockparam = {}
	autoscroll = {speedX = 0, speedY = 0}
end



-- Logic for double-tapping
function SMWCam.LRButtonLogic()
	-- Abort if in a forced animation state
	if player:mem(0x122, FIELD_WORD) ~= 0 then
		LKeyTap = false; RKeyTap = false
		leftKeyWasPressed = false
		rightKeyWasPressed = false
		doubleTapCounter = 0
		return
	end
	
	-- Reset taps
	if LKeyTap then LKeyTap = false end
	if RKeyTap then RKeyTap = false end
	
	-- If holding the left key this frame but not last frame
	if player.leftKeyPressing then
		-- It was a tap
		if not leftKeyWasPressed then LKeyTap = true
		else LKeyTap = false end
		-- The left key was held this frame
		leftKeyWasPressed = true
	else
		-- The left key was not held this frame
		leftKeyWasPressed = false
	end
	
	-- Same logic for right key
	if player.rightKeyPressing then
		if not rightKeyWasPressed then RKeyTap = true
		else RKeyTap = false end
		rightKeyWasPressed = true
	else
		rightKeyWasPressed = false
	end
	
	-- If the player isn't holding the tanooki button, ignore the tap
	if not player.altRunKeyPressing then
		LKeyTap = false; RKeyTap = false
	end
	
	if RKeyTap then
		if doubleTapCounter <= 0 then doubleTapCounter = DOUBLE_TAP_TIME
		else
			LRDirection = LRDirection + 1
			LRPanLocked = true
			if LRDirection > 1 then LRDirection = 1 end
		end
	end
	if LKeyTap then
		if doubleTapCounter >= 0 then doubleTapCounter = -DOUBLE_TAP_TIME
		else
			LRDirection = LRDirection - 1
			LRPanLocked = true
			if LRDirection < -1 then LRDirection = -1 end
		end
	end
	
	if LRDirection == 0 and Misc.isPausedByLua() and camlockState ~= LOCKING and camlockState ~= UNLOCKING then Misc.unpause() end
	
	if doubleTapCounter > 0 then doubleTapCounter = doubleTapCounter - 1
	elseif doubleTapCounter < 0 then doubleTapCounter = doubleTapCounter + 1 end
	
	if player.altRunKeyPressing and doubleTapCounter ~= 0 then
		player.leftKeyPressing = false
		player.rightKeyPressing = false
	end
end


-- Check if player is grounded
local function CheckIfGrounded()
	-- Is the player in water/quicksand?
	submerged = (player:mem(0x34, FIELD_WORD)~=0)
	-- Is the player flying, climbing, or a fairy?
	flying = (player:mem(0x16e, FIELD_WORD) ~= 0)
	climbing = (player:mem(0x40, FIELD_WORD) ~= 0)
	fairy = (player:mem(0x0c, FIELD_WORD) ~= 0)
	-- Is the player on a slope?
	sloped = (player:mem(0x48, FIELD_WORD) ~= 0)
	-- Is the player standing on an NPC and moving up or down?
	npcStandIndex = player:mem(0x176, FIELD_WORD)
	npcStandMoving = false
	npcStand = NPC.get()[npcStandIndex]
	if npcStand then npcStandMoving = (npcStand.speedY ~= 0) end
	npcStandStationary = (npcStandIndex ~= 0 and not npcStandMoving)
	
	if submerged or flying or fairy or climbing or sloped or npcStandMoving then freeform = true end
	
	-- Is the player Link and bouncing on spikes?
	bouncingOnSpikes = false
	if player.character == CHARACTER_LINK and player.downKeyPressing and not grounded then
		for _,block in pairs(Block.getIntersecting(player.x+4, player.y+1, player.x+player.width-4, player.y+player.height+2.5*math.abs(player.speedY))) do
			bouncingOnSpikes = true
			lastPlatformY = block.y
			break
		end
	end
	
	-- Is the player jumping on enemies?
	jumpingOnNPCs = false
	if not grounded then
		for _,bounceEffect in pairs(Animation.getIntersecting(player.x, player.y, player.x+player.width, player.y+player.height)) do
			if bounceEffect.id == 75 then
				jumpingOnNPCs = true
				lastPlatformY = bounceEffect.y
				break
			end
		end
	end
	
	-- Is the player grounded?
	if player:mem(0x146, FIELD_WORD) == 2 or npcStandStationary or bouncingOnSpikes or jumpingOnNPCs then
		grounded = true
		-- Remember the altitude of the last platform stood on
		if not (bouncingOnSpikes or jumpingOnNPCs) then lastPlatformY = player.y + player.height end
		-- Disable free camera
		freeform = false
	else
		grounded = false
	end
end
-- Horizontal camera movement logic
local function HorizontalCameraLogic()
	-- Ignore panning if the screen is too small
	if cambounds.right - cambounds.left == camera.width then LRPanLocked = false end
	
	-- If not panning with L and R
	if not LRPanLocked and not Misc.isPausedByLua() then
		-- Adjust snap boundaries for section edges
		if player.x + player.width < cambounds.left + camera.width/2 and LRDirection == 0 then
			bound.right = cambounds.left + camera.width/2 + HORIZ_GAP
			bound.left = cambounds.left
			directionX = -1
		elseif player.x > cambounds.right - camera.width/2 and LRDirection == 0 then
			bound.left = cambounds.right - camera.width/2 - HORIZ_GAP
			bound.right = cambounds.right
			directionX = 1
		end
	
		-- Keep player within snap boundaries
		if player.x < bound.left then
			bound.left = player.x
			bound.right = player.x + HORIZ_GAP
			camera.x = camera.x - PANSPEEDX + player.speedX
			if camera.x < bound.left - camera.width/2 then camera.x = bound.left - camera.width/2 end
			directionX = -1
			LRDirection = 0
		elseif player.x + player.width > bound.right then
			bound.right = player.x + player.width
			bound.left = player.x + player.width - HORIZ_GAP
			camera.x = camera.x + PANSPEEDX + player.speedX
			if camera.x > bound.right - camera.width/2 then camera.x = bound.right - camera.width/2 end
			directionX = 1
			LRDirection = 0
		end
	else
		-- Panning to the right
		if LRDirection == 1 then
			if player.x > camera.x + HORIZ_GAP and camera.x + camera.width < cambounds.right then
				if not Misc.isPausedByLua() then Misc.pause() end
				camera.x = camera.x + PANSPEEDX
			else
				Misc.unpause()
				bound.left = player.x
				bound.right = camera.x + camera.width/2 + HORIZ_GAP
				LRPanLocked = false
			end
		elseif LRDirection == -1 then
			if player.x + player.width < camera.x + camera.width - HORIZ_GAP and camera.x > cambounds.left then
				if not Misc.isPausedByLua() then Misc.pause() end
				camera.x = camera.x - PANSPEEDX
			else
				Misc.unpause()
				bound.right = player.x + player.width
				bound.left = camera.x + camera.width/2 - HORIZ_GAP
				LRPanLocked = false
			end
		else
			playerCenterX = player.x + player.width/2
			cameraCenterX = camera.x + camera.width/2
			if playerCenterX <= cambounds.left + camera.width/2 then
				Misc.pause()
				camera.x = camera.x - PANSPEEDX
				if camera.x < cambounds.left then
					LRPanLocked = false
					Misc.unpause()
				end
			elseif playerCenterX >= cambounds.right - camera.width/2 then
				Misc.pause()
				camera.x = camera.x + PANSPEEDX
				if camera.x + camera.width > cambounds.right then
					LRPanLocked = false
					Misc.unpause()
				end
			else
				if playerCenterX ~= cameraCenterX then
					if not Misc.isPausedByLua() then Misc.pause() end
				end
				if playerCenterX < cameraCenterX then
					if cameraCenterX - PANSPEEDX <= playerCenterX then camera.x = playerCenterX - camera.width/2
					else camera.x = camera.x - PANSPEEDX end
				elseif playerCenterX > cameraCenterX then
					if cameraCenterX + PANSPEEDX >= playerCenterX then camera.x = playerCenterX - camera.width/2
					else camera.x = camera.x + PANSPEEDX end
				else
					Misc.unpause()
					bound.left = playerCenterX - HORIZ_GAP/2
					bound.right = playerCenterX + HORIZ_GAP/2
					LRPanLocked = false
				end
			end
		end
	end
	
	if warpingOut then
		if directionX == 1 then
			if LRDirection ~= 0 then
				bound.right = player.x + player.width
				bound.left = bound.right - HORIZ_GAP
			end
			camera.x = bound.right - camera.width/2
		elseif directionX == -1 then
			if LRDirection ~= 0 then
				bound.left = player.x
				bound.right = bound.left + HORIZ_GAP
			end
			camera.x = bound.left - camera.width/2
		end
		if player.x + player.width < cambounds.left + camera.width/2 then camera.x = cambounds.left
		elseif player.x > cambounds.right - camera.width/2 then camera.x = cambounds.right - camera.width end
		LRDirection = 0
		LRPanLocked = false
	end
	
	-- Keep target position within section boundaries
	if camera.x < cambounds.left then camera.x = cambounds.left
	elseif camera.x + camera.width > cambounds.right then camera.x = cambounds.right-camera.width end
	
	-- Did the player just change direction?
	if directionX ~= lastBndTouchX then xPanning = true end
	lastBndTouchX = directionX
	
	-- Update boundary info
	delta.left = bound.left - prevbound.left
	delta.right = bound.right - prevbound.right
	prevbound.left = bound.left
	prevbound.right = bound.right
end
-- Vertical camera movement logic
local function VerticalCameraLogic()
	-- Snap camera if not UD panning
	if UDPanDirection == 0 then
		-- If a large drop in height, activate freecam
		if player.y + player.height >= lastPlatformY + VERT_GAP then freeform = true end
		-- Platform snapping
		if not freeform then
			-- If standing on a platform or on the ground, update snap boundaries
			if grounded then
				-- If a large increase in height, bump up bottom boundary
				if math.abs(lastPlatformY - bound.bottom) >= VERT_GAP then bound.bottom = lastPlatformY end
				bound.top = bound.bottom - VERT_GAP
			end
			-- Focus camera on bottom boundary
			targetCameraY = bound.bottom - camera.height/2
			yPanning = true
		else
			-- Keep player within snap boundaries
			if player.y + player.height > bound.bottom then
				bound.bottom = player.y + player.height
				bound.top = bound.bottom - VERT_GAP
			elseif player.y < bound.top then
				bound.top = player.y
				bound.bottom = bound.top + VERT_GAP
			end
			-- Allow free camera movement within snap boundaries
			if bound.top >= camera.y + camera.height/2 then
				targetCameraY = bound.top - camera.height/2
			elseif bound.bottom <= camera.y + camera.height/2 then
				targetCameraY = bound.bottom - camera.height/2
			end
			yPanning = true
		end
	end
	
	if warpingOut then
		lastPlatformY = player.y + player.height
		bound.bottom = lastPlatformY
		bound.top = bound.bottom - VERT_GAP
		targetCameraY = bound.bottom - camera.height/2
		yPanning = false
		UDPanDirection = 0
		UDPanTargetSet = false
	end
	
	-- Only pan up/down if on the ground
	if grounded then
		-- If looking up and not moving left or right
		if player.upKeyPressing and not player.leftKeyPressing and not player.rightKeyPressing then
			-- Increment counter
			if UDPanCounter < 0 then UDPanCounter = 0 end
			if UDPanCounter < MAX_UD_COUNTER then
				UDPanCounter = UDPanCounter + 1
			else
				UDPanCounter = MAX_UD_COUNTER
				-- If changing direction, reset pan target
				if UDPanDirection == 1 then UDPanTargetSet = false end
				UDPanDirection = -1
				freeform = false
				yPanning = true
			end
		else
			-- If the timer is not finished, reset it
			if UDPanCounter < MAX_UD_COUNTER and not player.downKeyPressing then UDPanCounter = 0 end
		end
		-- If looking down and not moving left or right
		if player.downKeyPressing and not player.leftKeyPressing and not player.rightKeyPressing then
			-- Increment counter
			if UDPanCounter > 0 then UDPanCounter = 0 end
			if UDPanCounter > -MAX_UD_COUNTER then
				UDPanCounter = UDPanCounter - 1
			else
				UDPanCounter = -MAX_UD_COUNTER
				-- If changing direction, reset pan target
				if UDPanDirection == -1 then UDPanTargetSet = false end
				UDPanDirection = 1
				freeform = false
				yPanning = true
			end
		else
			-- If the timer is not finished, reset it
			if UDPanCounter > -MAX_UD_COUNTER and not player.upKeyPressing then UDPanCounter = 0 end
		end
	end
	
	-- If a target hasn't been set and player is on the ground
	if grounded and UDPanDirection ~= 0 and not UDPanTargetSet then
		-- If panning up, pan towards the upper fourth of the screen
		if UDPanDirection == -1 then
			targetCameraY = player.y + player.height - 3*camera.height/4
		-- If panning down, pan towards the lower fourth of the screen
		else
			targetCameraY = player.y - camera.height/4
		end
		UDPanTargetSet = true
	end
	if UDPanDirection ~= 0 then
		-- Fix camera until player moves out of bounds
		if UDPanDirection == -1 then
			if player.y < targetCameraY + camera.height/4 or player.y + player.height > targetCameraY + 3*camera.height/4 then
				UDPanDirection = 0
				UDPanCounter = 0
				UDPanTargetSet = false
				freeform = true
			end
		else
			if player.y + player.height > targetCameraY + 3*camera.height/4 or player.y < targetCameraY + camera.height/4 - 2.5*player.height then
				UDPanDirection = 0
				UDPanCounter = 0
				UDPanTargetSet = false
				freeform = true
			end
		end
	end

	-- Keep target position within section boundaries
	if targetCameraY <= cambounds.top then targetCameraY = cambounds.top
	elseif targetCameraY+camera.height >= cambounds.bottom then targetCameraY = cambounds.bottom-camera.height end
	
	-- Update boundary info
	delta.top = bound.top - prevbound.top
	delta.bottom = bound.bottom - prevbound.bottom
	prevbound.top = bound.top
	prevbound.bottom = bound.bottom
	
	-- Pan camera to current target
	dy = PANSPEEDY
	if freeform then dy = PANSPEEDY + math.abs(player.speedY) end
	if math.abs(targetCameraY-camera.y) <= dy or not yPanning then
		camera.y = targetCameraY
		yPanning = false
	else
		if camera.y < targetCameraY then camera.y = camera.y + dy
		else camera.y = camera.y - dy end
	end
end



-- Set lock for camera boundaries
function SMWCam.lock(args)
	-- Default coordinates to previous frame
	if type(args.x) ~= "number" then args.x = lastCameraX end
	if type(args.y) ~= "number" then args.y = lastCameraY end
	-- Default dimensions to camera width and height
	if type(args.width) ~= "number" or args.width < camera.width then args.width = camera.width end
	if type(args.height) ~= "number" or args.height < camera.height then args.height = camera.height end
	
	-- Check if boundaries lie within section
	if args.x < sectbounds.left or args.x + args.width > sectbounds.right or
		args.y < sectbounds.top or args.y + args.height > sectbounds.bottom then
		error("SMW Camera Error: Specified bounds exceed section boundaries.", 2) end
	
	-- Default camera panning to snap instantly
	if type(args.locktype) ~= "number" or args.locktype < LOCKTYPE_INSTANT or args.locktype > LOCKTYPE_SMOOTH then args.locktype = LOCKTYPE_INSTANT end
	-- Default speed/lerp factor
	if args.locktype == LOCKTYPE_CONSTANT then
		if type(args.speed) ~= "number" or args.speed <= 0 then args.speed = 10 end
	elseif args.locktype == LOCKTYPE_SMOOTH then
		if type(args.lerpfactor) ~= "number" or args.lerpfactor <= 0 or args.lerpfactor > 1 then args.lerpfactor = 0.15 end
	end
	-- Default to not pause when panning
	if type(args.pause) ~= "boolean" then args.pause = false end
	
	-- Check if arguments have updated
	updated = false
	for i,a in pairs(args) do
		for j,b in pairs(lockparam) do
			if i == j then
				if a ~= b then
					updated = true
					break
				end
			end
		end
		if updated then break end
	end
	if not lockparam.x or camlockState == UNLOCKED then updated = true end
	if not updated then return end
	
	-- Copy arguments
	lockparam.x = args.x;	lockparam.y = args.y
	lockparam.width = args.width
	lockparam.height = args.height
	lockparam.locktype = args.locktype
	lockparam.speed = args.speed
	lockparam.lerpfactor = args.lerpfactor
	lockparam.pause = args.pause
	
	-- Set bounds and lock camera
	cambounds = {left = lockparam.x, right = lockparam.x + lockparam.width, top = lockparam.y, bottom = lockparam.y + lockparam.height}
	camlockState = LOCKING
end
-- Reset camera boundaries
function SMWCam.unlock()
	if camlockState ~= PLAYERLOCKED then return end
	camlockState = UNLOCKING
	cambounds = {left = sectbounds.left, right = sectbounds.right, top = sectbounds.top, bottom = sectbounds.bottom}
end

-- Move camera into position when locked
local function LockToPosition()
	if warpingOut then
		if directionX == 1 then camera.x = player.x + player.width - camera.width/2
		elseif directionX == -1 then camera.x = player.x - camera.width/2
		else camera.x = player.x + player.width/2 - camera.width/2 end
		camera.y = player.y + player.height - camera.height/2
		if camera.x < cambounds.left then camera.x = cambounds.left
		elseif camera.x + camera.width > cambounds.right then camera.x = cambounds.right - camera.width end
		if camera.y < cambounds.top then camera.y = cambounds.top
		elseif camera.y + camera.height > cambounds.bottom then camera.y = cambounds.bottom - camera.height end
		camlockState = PLAYERLOCKED
	end

	if lockparam.locktype == LOCKTYPE_INSTANT then
		if camera.x < cambounds.left then camera.x = cambounds.left
		elseif camera.x + camera.width > cambounds.right then camera.x = cambounds.right - camera.width end
		if camera.y < cambounds.top then camera.y = cambounds.top
		elseif camera.y + camera.height > cambounds.bottom then camera.y = cambounds.bottom - camera.height end
		camlockState = PLAYERLOCKED
	elseif lockparam.locktype == LOCKTYPE_CONSTANT then
		dx = 0; dy = 0;
		if camera.x < cambounds.left then dx = cambounds.left - camera.x
		elseif camera.x + camera.width > cambounds.right then dx = cambounds.right - (camera.x + camera.width) end
		if camera.y < cambounds.top then dy = cambounds.top - camera.y
		elseif camera.y + camera.height > cambounds.bottom then dy = cambounds.bottom - (camera.y + camera.height) end
		
		distance = math.sqrt(dx*dx + dy*dy)
		if distance > lockparam.speed then
			dx = dx * lockparam.speed / distance
			dy = dy * lockparam.speed / distance
			if lockparam.pause and not Misc.isPausedByLua() then Misc.pause() end
		else
			camlockState = PLAYERLOCKED
			if lockparam.pause and Misc.isPausedByLua() then Misc.unpause() end
		end
		camera.x = camera.x + dx
		camera.y = camera.y + dy
	elseif lockparam.locktype == LOCKTYPE_SMOOTH then
		dx = 0; dy = 0;
		if camera.x < cambounds.left then dx = cambounds.left - camera.x
		elseif camera.x + camera.width > cambounds.right then dx = cambounds.right - (camera.x + camera.width) end
		if camera.y < cambounds.top then dy = cambounds.top - camera.y
		elseif camera.y + camera.height > cambounds.bottom then dy = cambounds.bottom - (camera.y + camera.height) end
		
		lerpdistance = lockparam.lerpfactor*math.sqrt(dx*dx + dy*dy)
		if lerpdistance > lockparam.lerpfactor then
			dx = dx * lockparam.lerpfactor
			dy = dy * lockparam.lerpfactor
		else
			camlockState = PLAYERLOCKED
		end
		camera.x = camera.x + dx
		camera.y = camera.y + dy
	end
end
-- Unlock camera boundaries and return to player
local function UnlockToPlayer()
	targetpos = {x = player.x + player.width/2 - camera.width/2, y = targetCameraY}
	adj = {x = 0, y = 0, dist = 0}
	if directionX == 1 then
		bound.right = player.x + player.width
		bound.left = bound.right - HORIZ_GAP
		adj.x = delta.right
		targetpos.x = bound.right - camera.width/2
	elseif directionX == -1 then
		bound.left = player.x
		bound.right = bound.left + HORIZ_GAP
		adj.x = delta.left
		targetpos.x = bound.left - camera.width/2
	end
	
	if warpingOut then
		camera.x = targetpos.x
		camera.y = targetpos.y
		if camera.x < cambounds.left then camera.x = cambounds.left
		elseif camera.x + camera.width > cambounds.right then camera.x = cambounds.right - camera.width end
		if camera.y < cambounds.top then camera.y = cambounds.top
		elseif camera.y + camera.height > cambounds.bottom then camera.y = cambounds.bottom - camera.height end
		camlockState = UNLOCKED
	end
	
	if lockparam.locktype == LOCKTYPE_INSTANT then
		camera.x = targetpos.x
		camera.y = targetpos.y
		camlockState = UNLOCKED
	elseif lockparam.locktype == LOCKTYPE_CONSTANT then
		dx = targetpos.x - camera.x
		dy = targetpos.y - camera.y
		
		distance = math.sqrt(dx*dx + dy*dy)
		if lerpdistance > lockparam.speed then
			dx = (dx - adj.x) * lockparam.speed / distance + adj.x
			dy = dy * lockparam.speed / distance
		else
			camlockState = UNLOCKED
		end
		camera.x = camera.x + dx
		camera.y = camera.y + dy
	elseif lockparam.locktype == LOCKTYPE_SMOOTH then
		dx = targetpos.x - camera.x
		dy = targetpos.y - camera.y
		
		lerpdistance = lockparam.lerpfactor*math.sqrt(dx*dx + dy*dy)
		if lerpdistance > lockparam.lerpfactor then
			dx = (dx - adj.x) * lockparam.lerpfactor + adj.x
			dy = (dy - adj.y) * lockparam.lerpfactor + adj.y
		else
			camlockState = UNLOCKED
		end
		camera.x = camera.x + dx
		camera.y = camera.y + dy
	end
end



-- Initialize variables for autoscrolling
function SMWCam.BeginAutoScroll(scrollspeedX, scrollspeedY)
	if scrollspeedX == 0 and scrollspeedY == 0 then return end
	if camlockState ~= LOCKING and camlockState ~= UNLOCKING then camlockState = AUTOSCROLLING end
	autoscroll.speedX = scrollspeedX
	autoscroll.speedY = scrollspeedY
end
-- Scroll camera
local function AutoscrollCamera()
	-- Check if player is offscreen
	offscreen = false
	if player.x < camera.x or player.x + player.width > camera.x + camera.width then offscreen = true end
	if player.y < camera.y or player.y + player.height > camera.y + camera.height then offscreen = true end
	
	if warpingOut and offscreen then
		-- Determine where the player would end up if the camera was not autoscrolling
		targetpos = {x = 0, y = 0}
		if directionX == 1 then targetpos.x = player.x + player.width - camera.width/2
		elseif directionX == -1 then targetpos.x = player.x - camera.width/2
		else targetpos.x = player.x + player.width/2 - camera.width/2 end
		targetpos.y = player.y + player.height - camera.height/2
		
		-- Find closest point en route to destination where the player is still in shot
		src = {x = camera.x, y = -camera.y}
		dx = autoscroll.speedX;		dy = -autoscroll.speedY;
		targetpos.y = -targetpos.y
		
		omega = math.tan((targetpos.y - src.y) / (targetpos.x - src.x))
		phi = math.tan(dy / dx)
		theta = phi - omega
		h = math.sqrt((targetpos.y - src.y)*(targetpos.y - src.y) + (targetpos.x - src.x)*(targetpos.x - src.x))
		dist = h*math.cos(theta)
		mult = dist / math.sqrt(dx*dx + dy*dy)
		
		camera.x = src.x + mult*dx
		camera.y = -(src.y + mult*dy)
		
		-- If the player is still offscreen, center the camera on them
		if player.x < camera.x or player.x + player.width > camera.x + camera.width then offscreen = true
		elseif player.y < camera.y or player.y + player.height > camera.y + camera.height then offscreen = true
		else offscreen = false end
		if offscreen then
			camera.x = player.x + player.width/2 - camera.width/2
			camera.y = player.y + player.height - camera.height/2
		end
	else
		camera.x = camera.x + autoscroll.speedX
		camera.y = camera.y + autoscroll.speedY
	end
	
	-- Keep camera in bounds
	if camera.x < cambounds.left then camera.x = cambounds.left
	elseif camera.x + camera.width > cambounds.right then camera.x = cambounds.right - camera.width end
	if camera.y < cambounds.top then camera.y = cambounds.top
	elseif camera.y + camera.height > cambounds.bottom then camera.y = cambounds.bottom - camera.height end
	
	-- Keep player in bounds if they arent warping
	if not (player:mem(0x122, FIELD_WORD) == 3 or player:mem(0x122, FIELD_WORD) == 7) then
		if player.x < camera.x then
			if player:mem(0x14c, FIELD_WORD) ~= 0 and player:mem(0x13e, FIELD_WORD) == 0 then player:kill() end 
			player.x = camera.x
		elseif player.x + player.width > camera.x + camera.width then
			if player:mem(0x148, FIELD_WORD) ~= 0 and player:mem(0x13e, FIELD_WORD) == 0 then player:kill() end 
			player.x = camera.x + camera.width - player.width
		end
		if player.y < camera.y - 96 then
			if (grounded or player:mem(0x48, FIELD_WORD) ~= 0) and player:mem(0x13e, FIELD_WORD) == 0 then player:kill() end
			player.y = camera.y - 96
		end
		if player.y > camera.y + camera.height + 96 and player:mem(0x13e, FIELD_WORD) == 0 then player:kill() end
	end
end

function SMWCam.StopAutoScroll()
	if camlockState ~= AUTOSCROLLING then return end
	autoscroll = {speedX = 0, speedY = 0}
	camlockState = UNLOCKED
end

local function DrawBounds()
	if not line then line = Graphics.loadImage("line.png") end
	if line then
		Graphics.drawImage(line, bound.left-1-camera.x, 0)
		Graphics.drawImage(line, bound.right-camera.x, 0)
	end
	
	if not line2 then line2 = Graphics.loadImage("line2.png") end
	if not groundline then groundline = Graphics.loadImage("ground.png") end
	if line2 then
		Graphics.drawImage(line2, 0, bound.top-1-camera.y)
		Graphics.drawImage(line2, 0, bound.bottom-camera.y)
		Graphics.drawImage(line2, 0, bound.bottom+VERT_GAP-camera.y)
	end
	if groundline then Graphics.drawImage(groundline, 0, lastPlatformY-camera.y) end
end

function SMWCam.SectionUpdate()
	if player.section ~= prevSection then
		GetSectionBounds()
		SMWCam.StopAutoScroll()
		cambounds = {left = sectbounds.left, right = sectbounds.right, top = sectbounds.top, bottom = sectbounds.bottom}
	end
end

function SMWCam.CameraLogic()
	-- If SMW logic is inactive or the camera is not found, exit
	if not active or not camera then return end	
	camera.x = lastCameraX
	camera.y = lastCameraY
	
	-- Adjust camera for warping
	if warpingOut then
		if (player:mem(0x122, FIELD_WORD) == 3 or player:mem(0x122, FIELD_WORD) == 7) and camlockState ~= AUTOSCROLLING then warpingOut = true else warpingOut = false end
	end
	if math.abs(player.x-prevpos.x) >= HORIZ_GAP or math.abs(player.y-prevpos.y) >= VERT_GAP then warpingOut = true end
	if prevSection ~= player.section then warpingOut = true end
	
	CheckIfGrounded()
	
	if camlockState == LOCKING then LockToPosition()
	elseif camlockState == AUTOSCROLLING then AutoscrollCamera()
	else
		if camlockState == UNLOCKING then UnlockToPlayer() end
		-- Perform horizontal snapping
		HorizontalCameraLogic()
		-- Perform vertical snapping
		VerticalCameraLogic()
	end
	
	if camera.x < sectbounds.left then camera.x = sectbounds.left
	elseif camera.x+camera.width > sectbounds.right then camera.x = sectbounds.right-camera.width end
	if camera.y < sectbounds.top then camera.y = sectbounds.top
	elseif camera.y+camera.height > sectbounds.bottom then camera.y = sectbounds.bottom-camera.height end

	lastCameraX = camera.x
	lastCameraY = camera.y
	prevpos.x = player.x
	prevpos.y = player.y
	prevSection = player.section
	
	if debugmode then DrawBounds() end
end

function SMWCam.activate()
	active = true
end
function SMWCam.deactivate()
	active = false
end
function SMWCam.isActive()
	return active
end

function SMWCam.isLocked()
	return (camlockState ~= UNLOCKED)
end
function SMWCam.isAutoScrolling()
	return (camlockState == AUTOSCROLLING)
end

function SMWCam.onInitAPI()
	registerEvent(SMWCam, "onStart", "Init")
	registerEvent(SMWCam, "onTickEnd", "SectionUpdate")
	registerEvent(SMWCam, "onInputUpdate", "LRButtonLogic")
	registerEvent(SMWCam, "onCameraUpdate", "CameraLogic")
end

return SMWCam