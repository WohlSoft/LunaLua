--***************************************************************************************
--                                                                                      *
--  inputs2.lua                                                               *
--  v1.1                                                                                *
--                                                                                      *
--***************************************************************************************

--Spun off from inputs.lua by Rockythechao - now supports multiplayer!

--[[
    Vars:  
        inputs.locked[playerIndex][keystr] -- if true, SMBX does not process this input, but the state
                                 is still recorded by this library.
							 
        inputs.state[playerIndex][keystr]  -- the current state of that key (read-only)
		
    keystrings are same as the respective fields, all lowercase
    (I.E. player.leftKeyPressing --> "left",  player.dropItemKeyPressing --> "dropItem")
	
    state constants are inputs.UP
                        inputs.PRESS
                        inputs.HOLD
                        inputs.RELEASE
--]]



local inputs2 = {} --Package table

	function inputs2.onInitAPI()
		registerEvent(inputs2, "onLoop", "onLoop", true)
		registerEvent(inputs2, "onInputUpdate", "onInputUpdate", true)
	end


	
	--***********************************************************************************
	--                                                                                  *
	--  State constants                                                                 *
	--                                                                                  *
	--***********************************************************************************
	
	do
		inputs2.UP = 0
		inputs2.PRESS = 1
		inputs2.HOLD = 2
		inputs2.RELEASE = 3
	end

	
	
	--***********************************************************************************
	--                                                                                  *
	--  State table                                                                     *
	--                                                                                  *
	--***********************************************************************************
	
	do
		inputs2.state = {}
		inputs2.state[1] = {}
		inputs2.state[1]["up"] = inputs2.UP
		inputs2.state[1]["down"] = inputs2.UP
		inputs2.state[1]["left"] = inputs2.UP
		inputs2.state[1]["right"] = inputs2.UP
		inputs2.state[1]["jump"] = inputs2.UP
		inputs2.state[1]["altjump"] = inputs2.UP
		inputs2.state[1]["run"] = inputs2.UP
		inputs2.state[1]["altrun"] = inputs2.UP
		inputs2.state[1]["dropitem"] = inputs2.UP
		inputs2.state[1]["pause"] = inputs2.UP
		
		inputs2.state[2] = {}
		inputs2.state[2]["up"] = inputs2.UP
		inputs2.state[2]["down"] = inputs2.UP
		inputs2.state[2]["left"] = inputs2.UP
		inputs2.state[2]["right"] = inputs2.UP
		inputs2.state[2]["jump"] = inputs2.UP
		inputs2.state[2]["altjump"] = inputs2.UP
		inputs2.state[2]["run"] = inputs2.UP
		inputs2.state[2]["altrun"] = inputs2.UP
		inputs2.state[2]["dropitem"] = inputs2.UP
		inputs2.state[2]["pause"] = inputs2.UP
	end

	

	--***********************************************************************************
	--                                                                                  *
	--  Lock table                                                                      *
	--                                                                                  *
	--***********************************************************************************
	
	do
		inputs2.locked = {}
		
		inputs2.locked[1] = {}
		inputs2.locked[1]["up"] = false
		inputs2.locked[1]["down"] = false
		inputs2.locked[1]["left"] = false
		inputs2.locked[1]["right"] = false
		inputs2.locked[1]["jump"] = false
		inputs2.locked[1]["altjump"] = false
		inputs2.locked[1]["run"] = false
		inputs2.locked[1]["altrun"] = false
		inputs2.locked[1]["dropitem"] = false
		inputs2.locked[1]["pause"] = false
		inputs2.locked[1]["all"] = false
		
		inputs2.locked[2] = {}
		inputs2.locked[2]["up"] = false
		inputs2.locked[2]["down"] = false
		inputs2.locked[2]["left"] = false
		inputs2.locked[2]["right"] = false
		inputs2.locked[2]["jump"] = false
		inputs2.locked[2]["altjump"] = false
		inputs2.locked[2]["run"] = false
		inputs2.locked[2]["altrun"] = false
		inputs2.locked[2]["dropitem"] = false
		inputs2.locked[2]["pause"] = false
		inputs2.locked[2]["all"] = false
	end
	
	
	inputs2.key = {}
	inputs2.key[1] = {}
	inputs2.key[2] = {}
	
	
	inputs2.debug = false
	
	
	--***********************************************************************************
	--                                                                                  *
	--  Update input                                                                    *
	--                                                                                  *
	--***********************************************************************************
	
	
	function inputs2.onLoop ()	
		
		local i = 0

		for k,v in pairs(inputs2.state[1]) do
			
			-- Debug
			if  inputs2.debug == true  then
				local debugStr = tostring(k)..": "..tostring(inputs2.state[1][k])
				
				if  inputs2.locked[1][k] == true  then
					debugStr = debugStr.." (L)"
				end

				Text.print (debugStr, 20, 80 + 20*i)
			end
			i = i+1
		end
		
		if(player2) then
			local i = 0

			for k,v in pairs(inputs2.state[2]) do
				
				-- Debug
				if  inputs2.debug == true  then
					local debugStr = tostring(k)..": "..tostring(inputs2.state[2][k])
					
					if  inputs2.locked[2][k] == true  then
						debugStr = debugStr.." (L)"
					end

					Text.print (debugStr, 420, 80 + 20*i)
				end
				i = i+1
			end
		end
	end

		

		function inputs2.onInputUpdate ()	
			for playerIndex, thePlayer in pairs(Player.get()) do
			
			inputs2.key[playerIndex]["up"] = thePlayer.upKeyPressing
			inputs2.key[playerIndex]["down"] = thePlayer.downKeyPressing
			inputs2.key[playerIndex]["left"] = thePlayer.leftKeyPressing
			inputs2.key[playerIndex]["right"] = thePlayer.rightKeyPressing
			inputs2.key[playerIndex]["jump"] = thePlayer.jumpKeyPressing
			inputs2.key[playerIndex]["altjump"] = thePlayer.altJumpKeyPressing
			inputs2.key[playerIndex]["run"] = thePlayer.runKeyPressing
			inputs2.key[playerIndex]["altrun"] = thePlayer.altRunKeyPressing
			inputs2.key[playerIndex]["dropitem"] = thePlayer.dropItemKeyPressing
			inputs2.key[playerIndex]["pause"] = thePlayer.pauseKeyPressing
		
		
			-- STORE INPUT STATE FOR EACH KEY
			local i = 0
			
			for k,v in pairs(inputs2.state[playerIndex]) do
				if  inputs2.state[playerIndex][k] == inputs2.UP			then
					if 	inputs2.key[playerIndex][k] == true 	then
						inputs2.state[playerIndex][k] = inputs2.PRESS
					end
				
				elseif inputs2.state[playerIndex][k] == inputs2.PRESS		then
					inputs2.state[playerIndex][k] = inputs2.HOLD
				
				elseif inputs2.state[playerIndex][k] == inputs2.HOLD		then
					if 	inputs2.key[playerIndex][k] == false 	then
						inputs2.state[playerIndex][k] = inputs2.RELEASE
					end
				
				elseif inputs2.state[playerIndex][k] == inputs2.RELEASE	then
					inputs2.state[playerIndex][k] = inputs2.UP
				
				end
			end
			
			
			-- Disable locked keys
			if  inputs2.locked[playerIndex]["up"] == true         then  thePlayer.upKeyPressing = false;             end
			if  inputs2.locked[playerIndex]["down"] == true       then  thePlayer.downKeyPressing = false;           end
			if  inputs2.locked[playerIndex]["left"] == true       then  thePlayer.leftKeyPressing = false;           end
			if  inputs2.locked[playerIndex]["right"] == true      then  thePlayer.rightKeyPressing = false;          end
			if  inputs2.locked[playerIndex]["jump"] == true       then  thePlayer.jumpKeyPressing = false;           end
			if  inputs2.locked[playerIndex]["altjump"] == true    then  thePlayer.altJumpKeyPressing = false;        end
			if  inputs2.locked[playerIndex]["run"] == true        then  thePlayer.runKeyPressing = false;            end
			if  inputs2.locked[playerIndex]["altrun"] == true     then  thePlayer.altRunKeyPressing = false;         end
			if  inputs2.locked[playerIndex]["dropitem"] == true   then  thePlayer.dropItemKeyPressing = false;       end
			if  inputs2.locked[playerIndex]["pause"] == true      then  thePlayer.pauseKeyPressing = false;          end
			
			if  inputs2.locked[playerIndex]["all"] == true         then  thePlayer.upKeyPressing = false;
			thePlayer.downKeyPressing = false;						thePlayer.leftKeyPressing = false;
			thePlayer.rightKeyPressing = false;						thePlayer.jumpKeyPressing = false;
			thePlayer.altJumpKeyPressing = false;					thePlayer.runKeyPressing = false;
			thePlayer.altRunKeyPressing = false;					thePlayer.pauseKeyPressing = false; end
		end
	end

return inputs2