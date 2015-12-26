--Advanced Player Collisions API (WIP - Not reccomended for personal use)
--Created by Sambo
--Requires Colliders and Vectors libraries

--API values
local apc = {}
colliders = loadSharedAPI("colliders")
local r = 0
local ropeCheck = 0
local ropeBox = {}

function apc.onInitAPI()	
	registerEvent(apc, "onLoop")
end

function apc.onLoop()
	--Player 1
	local p1hitbox = colliders.getAABB(player) --Load P1's hitbox
	--Create a 8 px thick collision sensor on each side of the player
	p1colliders = {
				top = colliders.Box(player.x, player.y - 8, p1hitbox.width, 8),
				left = colliders.Box(player.x - 8, player.y, 8, p1hitbox.height),
				right = colliders.Box(player.x + p1hitbox.width, player.y, 8, p1hitbox.height),
				bottom = colliders.Box(player.x, player.y + p1hitbox.height, p1hitbox.width, 8),
				}
	if apc.debug then
		p1colliders["top"]:Draw(0xff000099)
		p1colliders["left"]:Draw(0x00ff0099)
		p1colliders["right"]:Draw(0x0000ff99)
		p1colliders["bottom"]:Draw(0xffff0099)
	end
	if ropeBlocks then
		for k,v in pairs(ropeBlocks) do
			for a,b in pairs(Block.get(v)) do
				if (b:collidesWith(player) == 1) and (player.downKeyPressing) then
					player.y = player.y + 2 --Move the player down 2 pixels. This effectively causes the player to "fall through" a cloud block, but is too small to glitch solid blocks. :)
				end
			end
		end
	end
	if bouncySpikes then
		for k,v in pairs(bouncySpikes) do
			for a,b in pairs(Block.get(v)) do
				if (b:collidesWith(player) == 1) then
					if player:mem(0x50, FIELD_WORD) == -1 then
						colliders.bounceResponse(player)
					else
						player:harm()
					end
				end
			end
		end
	end
--Player 2 (if there is one)
	if player2 then
		local p2hitbox = colliders.getAABB(player2) --Load P2's hitbox
		--Create a 8 px thick collision sensor on each side of player2
		p2colliders = {
					top = colliders.Box(player2.x, player2.y - 8, p2hitbox.width, 8),
					left = colliders.Box(player2.x - 8, player2.y, 8, p2hitbox.height),
					right = colliders.Box(player2.x + p2hitbox.width, player2.y, 8, p2hitbox.height),
					bottom = colliders.Box(player2.x, player2.y + p2hitbox.height, p2hitbox.width, 8),
					}
		if apc.debug then
			p2colliders["top"]:Draw(0xff000099)
			p2colliders["left"]:Draw(0x00ff0099)
			p2colliders["right"]:Draw(0x0000ff99)
			p2colliders["bottom"]:Draw(0xffff0099)
		end
		if ropeBlocks then
			for k,v in pairs(ropeBlocks) do
				for a,b in pairs(Block.get(v)) do
					if (b:collidesWith(player2) == 1) and (player2.downKeyPressing) then
						player2.y = player2.y + 2 --Move the player2 down 2 pixels. This effectively causes the player2 to "fall through" a cloud block, but is too small to glitch solid blocks. :)
					end
				end
			end
		end
		if bouncySpikes then
			for k,v in pairs(bouncySpikes) do
				for a,b in pairs(Block.get(v)) do
					if (b:collidesWith(player2) == 1) then
						if player2:mem(0x50, FIELD_WORD) == -1 then
							colliders.bounceResponse(player2)
						else
							player2:harm()
						end
					end
				end
			end
		end
	end
end

--This function makes a block behave like a "rope-type" block. If you play SMW hacks, you may have seen these. A "rope-type" block behaves like a cloud block, but you can go through it if you press DOWN. This only works if used on a cloud-type block.
function apc.rope(ids) --Argument must be a table of integers between 1 and 638
	ropeBlocks = ids
end

--This function makes a block act like an upward pointing spike block, but spinjumping on it will bounce
--the player instead of harming him. The block must NOT already harm the player, or it won't work.
--It is not reccomended to place this block on a moving layer, as it may not work correctly.
function apc.bouncySpike(ids) --Argument must be a table of integers between 1 and 638
	bouncySpikes = ids
end

return apc