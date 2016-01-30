local pipecannon = {}

-- Speed at which the player is fired from the pipe
pipecannon.exitspeed = {}
pipecannon.angle = {}
local PIPE_EXIT_SPEED_X = 0
local PIPE_EXIT_SPEED_Y = 0

-- Frame counter for pipe transition
local pipecounter = 0
local jumpcounter = 0
-- Is the player currently in the pipe transition
local enteringPipe = false
-- Warp entrance and exit coordinates
local enterX = 0 
local enterY = 0
local exitX = 0
local exitY = 0
-- Warp entrance and exit directions
local exitDirection = 0
local enterDirection = 0

-- Direction constants
local EXIT_UP = 3
local EXIT_LEFT = 4
local EXIT_DOWN = 1
local EXIT_RIGHT = 2
local ENTRANCE_UP = 1
local ENTRANCE_LEFT = 2
local ENTRANCE_DOWN = 3
local ENTRANCE_RIGHT = 4

-- Cannon fire SFX index
pipecannon.SFX = 22
-- Visual effect
pipecannon.effect = 0

function checksection(x, y)
	for i,section in pairs(Section.get()) do
		bounds = section.boundary
		if x+96 >= bounds.left and x-96 <= bounds.right and y+96 >= bounds.top and y-96 <= bounds.bottom then return i end
	end
	return -1
end

-- Perform logic for pipe cannons
function pipecannon.logic()
	-- If the player just entered a pipe warp transition
	if player:mem(0x122, FIELD_WORD) == 3 and not enteringPipe then
		-- Find the relevant warp
		for _,w in pairs(Warp.getIntersectingEntrance(player.x, player.y, player.x+player.width, player.y+player.height)) do
			-- Check speed and angle
			PIPE_EXIT_SPEED = pipecannon.exitspeed[player:mem(0x15e,FIELD_WORD)]
			ANGLE = pipecannon.angle[player:mem(0x15e,FIELD_WORD)]
			if not PIPE_EXIT_SPEED or PIPE_EXIT_SPEED == 0 then break end
			if not ANGLE then ANGLE = 0 end
			if ANGLE < -90 then ANGLE = -90 end
			if ANGLE > 90 then ANGLE = 90 end
			ANGLE = ANGLE*math.pi/180
			-- Find relevant parameters
			enteringPipe = true
			enterDirection = w:mem(0x80, FIELD_WORD)
			exitDirection = w:mem(0x82, FIELD_WORD)
			-- Tweak player coordinates depending on warp entrance direction
			if enterDirection == ENTRANCE_UP then
				enterX = w.entranceX
				enterY = w.entranceY
			elseif enterDirection == ENTRANCE_DOWN then
				enterX = w.entranceX
				enterY = w.entranceY - player.height + 32
			elseif enterDirection == ENTRANCE_LEFT then
				enterX = w.entranceX
				enterY = w.entranceY + (32 - player.height)/2
			elseif enterDirection == ENTRANCE_RIGHT then
				enterX = w.entranceX + (32 - player.width)
				enterY = w.entranceY + (32 - player.height)/2
			end
			-- Tweak player exit coordinates
			if exitDirection == EXIT_UP then
				exitX = w.exitX + (32 - player.width)/2
				exitY = w.exitY - player.height + 32 + PIPE_EXIT_SPEED/2
				PIPE_EXIT_SPEED_X = PIPE_EXIT_SPEED*math.sin(ANGLE)
				PIPE_EXIT_SPEED_Y = -PIPE_EXIT_SPEED*math.cos(ANGLE)
			elseif exitDirection == EXIT_DOWN then
				exitX = w.exitX + (32 - player.width)/2
				exitY = w.exitY
				PIPE_EXIT_SPEED_X = -PIPE_EXIT_SPEED*math.sin(ANGLE)
				PIPE_EXIT_SPEED_Y = PIPE_EXIT_SPEED*math.cos(ANGLE)
			elseif exitDirection == EXIT_LEFT then
				exitX = w.exitX + (32 - player.width)
				exitY = w.exitY + (32 - player.height)
				PIPE_EXIT_SPEED_X = -PIPE_EXIT_SPEED*math.cos(ANGLE)
				PIPE_EXIT_SPEED_Y = -PIPE_EXIT_SPEED*math.sin(ANGLE)
			elseif exitDirection == EXIT_RIGHT then
				exitX = w.exitX
				exitY = w.exitY + (32 - player.height)
				PIPE_EXIT_SPEED_X = PIPE_EXIT_SPEED*math.cos(ANGLE)
				PIPE_EXIT_SPEED_Y = PIPE_EXIT_SPEED*math.sin(ANGLE)
			end
		end
	end
	-- If in a pipe transition
	if enteringPipe then
		-- Increment pipe frame counter
		pipecounter = pipecounter + 1
		jumpcounter = jumpcounter + 1
		-- Enforce pipe transition animation
		player:mem(0x122, FIELD_WORD, 3)
		-- Set player coordinates
		player.x = enterX
		player.y = enterY
		-- Adjust for player dimensions
		factor = 0.02
		if player.height >= 50 then
			factor = 0.019
		end
		-- Manually override player pipe animation
		if enterDirection == ENTRANCE_UP then enterY = enterY - factor*player.height
		elseif enterDirection == ENTRANCE_DOWN then enterY = enterY + factor*player.height
		elseif enterDirection == ENTRANCE_LEFT then enterX = enterX - 0.021*player.width
		elseif enterDirection == ENTRANCE_RIGHT then enterX = enterX + 0.021*player.width end
	end
	-- If the transition has ended
	if pipecounter >= 60 then
		-- Cancel transition animation
		player:mem(0x122, FIELD_WORD, 0)
		-- Set player coordinates
		player:mem(0x15a, FIELD_WORD, checksection(exitX, exitY)-1)
		player.x = exitX
		player.y = exitY
		enteringPipe = false
		-- Reset counter
		pipecounter = 0
		-- Set player speed
		player.speedX = PIPE_EXIT_SPEED_X
		player.speedY = PIPE_EXIT_SPEED_Y
		-- Play effects
		if pipecannon.SFX > 0 then playSFX(pipecannon.SFX) end
		if pipecannon.effect > 0 then
			anim = Animation.spawn(pipecannon.effect, player.x, player.y)
			animX = player.x + (player.width-anim.width)/2
			animY = player.y + (player.height-anim.height)/2
			anim.x = animX; anim.y = animY
		end
	end
	if jumpcounter >= 60 and jumpcounter < 75 then
		jumpcounter = jumpcounter + 1
	end
end

function pipecannon.onInitAPI()
	registerEvent(pipecannon, "onLoop", "logic")
end

return pipecannon