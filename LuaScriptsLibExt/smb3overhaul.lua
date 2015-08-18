local __title = "SMB3 Overhaul Mod";
local __version = "1.0.2";
local __description = "Makes SMBX more like SMB3. Credit to Mike Santiago for the timer api.";
local __author = "XNBlank";
local __url = "https://github.com/XNBlank";

local smb_threeHud_API = {} --instance


--Let's load all of the graphics first.
local resPath = getSMBXPath() .. "\\LuaScriptsLib\\smb3overhaul"; --res path
local back = Graphics.loadImage(resPath .. "\\back.png"); --ui thing
local lifecounter = Graphics.loadImage(resPath .. "\\lifecounter.png"); --ui thing
local coincounter = Graphics.loadImage(resPath .. "\\coincounter.png"); --ui thing
local starcounter = Graphics.loadImage(resPath .. "\\starcounter.png"); --ui thing
local pbarGfx = Graphics.loadImage(resPath .. "\\pbar.png"); --ui thing
local pbarGfx_arrow = Graphics.loadImage(resPath .. "\\pbar_arrow.png"); --ui thing
local pbarGfx_active = Graphics.loadImage(resPath .. "\\pbar_active.png"); --ui thing
local useReserve = false;
local toggleEasyMode = true;
local usePBar = true;
local pbarCount = 0;
local reduceTimer = 6;
local lastpowerup = 0;
local disableSpinJump = true;

--Load leveltimer vars (by LuigiFan2010)
local secondsleft = 300; --The amount of seconds left.
local framecounter = 0; --Counts the amount of frames
local postWinFrameCounter = 0; --Counts frams since after the win
local player1 = Player(); --The player variable
local player2 = Player(2);
local beatLevel = false;
local getSeconds = false;
local takeTime = 0;
local playerkilled = false; --If the player was killed already so we don't kill him 21390840239 times
local passTime = true; --Whether or not to pass time, if false, time will stop.
local timerEnabled = true; --Whether or not the timer itself is actually enabled or not
local warnedPlayer = false; --Whether or not we've warned the player that time is running out
local uiImage = Graphics.loadImage(resPath .. "\\ui.png"); --ui thing

smb_threeHud_API.GUIPosition_NoStars = {x = 252, y = 50}
smb_threeHud_API.GUIPosition_Stars = {x = 252, y = 66}

--Load Goal Card vars 
local card1 = -1; -- basically a bool. Checks if card is used
local card2 = -1;
local card3 = -1;
local cards = 0;
local goalcard = NPC(11);
local getframe = 0;
local postWinFrameCounter = 0;
local endLevelTimer = 0;
local gotcard = false;
local addCard = false;
local doesUseCard = true;
local playOnce = false;
local timer = 1;
local mushroomcard = Graphics.loadImage(resPath .. "\\mushroom.png");
local flowercard = Graphics.loadImage(resPath .. "\\flower.png");
local starcard = Graphics.loadImage(resPath .. "\\star.png");

local oneup = Graphics.loadImage(resPath .. "\\1up.png");
local twoup = Graphics.loadImage(resPath .. "\\2up.png");
local threeup = Graphics.loadImage(resPath .. "\\3up.png");
local fiveup = Graphics.loadImage(resPath .. "\\5up.png");

local curLivesCount = mem(0x00B2C5AC, FIELD_FLOAT);

local dataInstance = Data(Data.DATA_WORLD,"SMB3 Cards", true);

local levelFinished = false;
local firstRun = true

smb_threeHud_API.GUIPosition1 = {x = 594, y = 542}
smb_threeHud_API.GUIPosition2 = {x = 642, y = 542}
smb_threeHud_API.GUIPosition3 = {x = 690, y = 542}


function smb_threeHud_API.onInitAPI()
	timerEnabled = false;
	hud(false);
	Graphics.placeSprite(1,back,0,0);
	Graphics.placeSprite(1,lifecounter,85, 568);
	Graphics.placeSprite(1,coincounter,496, 548);
	Graphics.placeSprite(1,starcounter,158, 568);

	if(usePBar == true) then
		Graphics.placeSprite(1,pbarGfx,300, 570);
	end

    gotcard = false;
    addCard = false;
    cards = tonumber(dataInstance:get("cards"));
    card1 = tonumber(dataInstance:get("card1"));
    card2 = tonumber(dataInstance:get("card2"));
    card3 = tonumber(dataInstance:get("card3"));

	Defines.smb3RouletteScoreValueStar = 4;
	Defines.smb3RouletteScoreValueMushroom = 2;
	Defines.smb3RouletteScoreValueFlower = 3;

	registerEvent(smb_threeHud_API, "onLoop", "onLoopOverride");
	registerEvent(smb_threeHud_API, "onInputUpdate", "onInputUpdateOverride");

end

function smb_threeHud_API.onLoad()


end

function smb_threeHud_API.onLoopOverride()
	if(useReserve == false) then
		player.reservePowerup = 0;
	end

	--[[
	--This was used to test for collisions for disabling spin jump

	Text.print("If the player is touching the ground or ", 0, 0);
	Text.print("a slope, disable Spin Jump.", 0, 16);
	Text.print("Sprite standing on " .. tostring(player:mem(0x176, FIELD_WORD)), 0, 48);
	Text.print("Climbing " .. tostring(player:mem(0x40, FIELD_WORD)), 0, 64);
	if(player.altJumpKeyPressing == true) then
		Text.print("Player IS pressing Spin Jump", 0, 80);
	elseif(player.altJumpKeyPressing == false) then
		Text.print("Player IS NOT pressing Spin Jump", 0, 80);
	end
	]]


  if(toggleEasyMode == true) then
    local isHurt = player:mem(0x140, FIELD_WORD);

    if(player.powerup > 2) then
      lastpowerup = player.powerup;
    end

    if(lastpowerup > 2) and (isHurt > 50) then
      player.powerup = 2;
      lastpowerup = player.powerup;
    end

    --Text.print(tostring(isHurt), 0 ,0);
    --Text.print(tostring(player.powerup), 0 ,16);
  end


	--Load internal functions and variables
	local score = mem(0x00B2C8E4, FIELD_DWORD);
	local lives = mem(0x00B2C5AC, FIELD_FLOAT);
	local coins = mem(0x00B2C5A8, FIELD_DWORD);
	local stars = mem(0x00B251E0, FIELD_DWORD);

	local isFlying = player:mem(0x16E, FIELD_WORD);


	local levelname = Level.name();
	local level_length = string.len(levelname);
	local speed = player.speedX;
	local pwingSound = Audio.SfxOpen(resPath .. "\\pmeter.wav");


	if(levelname == "") or (level_length > 11) then
		levelname = "Level";
	end

	--Draw HUD Elements.
	Text.print(string.format("%02d",coins), 1,542, 550);
	Text.print(tostring(stars), 1,204, 570);
	--Text.print("Score ", 250, 549);
	Text.print(string.format("%08d", score),1,300, 550);
	Text.print(tostring(lives), 1,140, 570);

	Text.print(levelname,85, 549);
	--Text.print(tostring(canFly), 0, 0);
	--Text.print(tostring(pbarCount), 0, 16);
	--Text.print(tostring(isFlying), 0, 32);

	--Start PWing Events
	if(usePBar == true) then


    if(reduceTimer <= 0) then
      reduceTimer = 0;
    end

    if(player.runKeyPressing == true) or (player.altRunKeyPressing == true) then
      if(pbarCount >= 10) or (isFlying == -1) then
        Graphics.placeSprite(1,pbarGfx_arrow,300, 570);   
        reduceTimer = reduceTimer - 1;   
      end

      if(pbarCount >= 15) or (isFlying == -1) then
        Graphics.placeSprite(1,pbarGfx_arrow,316, 570);
        reduceTimer = reduceTimer - 1;
      end

      if(pbarCount >= 20) or (isFlying == -1) then
        Graphics.placeSprite(1,pbarGfx_arrow,332, 570);
        reduceTimer = reduceTimer - 1;
      end

      if(pbarCount >= 25) or (isFlying == -1) then
        Graphics.placeSprite(1,pbarGfx_arrow,348, 570);
        reduceTimer = reduceTimer - 1;  
      end

      if(pbarCount >= 30) or (isFlying == -1) then
        Graphics.placeSprite(1,pbarGfx_arrow,364, 570);
        reduceTimer = reduceTimer - 1;
      end

      if(pbarCount >= 35) or (isFlying == -1) then
        Graphics.placeSprite(1,pbarGfx_arrow,380, 570);
        reduceTimer = reduceTimer - 1;
      end 

    end

		if(speed >= 6) or (speed <= -6) or (isFlying == -1) then

			pbarCount = pbarCount + 1;

			if(pbarCount >= 40) then
				pbarCount = 40;
				if(Audio.SfxIsPlaying(1) ~= 1) then
					Audio.SfxPlayCh(1, pwingSound, 1);
					Graphics.placeSprite(1,pbarGfx_active,398, 570);
				end
			end
		else
			pbarCount = pbarCount - 1;
			Graphics.unplaceSprites(pbarGfx_active);
		end

		if(pbarCount <= 0) then
			pbarCount = 0;
		end

   if(pbarCount <= 9) and (reduceTimer == 0) then
      Graphics.unplaceSprites(pbarGfx_arrow, 300, 570);
    end
    if(pbarCount <= 14) and (reduceTimer == 0) then
      Graphics.unplaceSprites(pbarGfx_arrow, 316, 570);
    end
    if(pbarCount <= 19) and (reduceTimer == 0) then
      Graphics.unplaceSprites(pbarGfx_arrow, 332, 570);
    end
    if(pbarCount <= 24) and (reduceTimer == 0) then
      Graphics.unplaceSprites(pbarGfx_arrow, 348, 570);
    end
    if(pbarCount <= 29) and (reduceTimer == 0) then
      Graphics.unplaceSprites(pbarGfx_arrow, 364, 570);
    end
    if(pbarCount <= 34) and (reduceTimer == 0) then
      Graphics.unplaceSprites(pbarGfx_arrow, 380, 570);
    end


    --end
  end






	--Start Timer Events

 if(timerEnabled == true) then
    timeelapsed = round(framecounter / 60, 0);
    if(beatLevel == false) then
		Graphics.placeSprite(1, uiImage, 486, 569, "", 2);
		Text.print(string.format("%03d",secondsleft - timeelapsed), 1, 524, 570);
	end


  --Text.print("Time to take..." .. tostring(takeTime), 0, 0);
  --Text.print("Seconds left..." .. tostring(secondsleft - timeelapsed), 0, 15);


    if (passTime == true) then
      framecounter = framecounter + 1;
    end
    if(timeelapsed >= secondsleft) then
      passTime = false;
      if(beatLevel == false) then
        if(playerkilled == false) then
          player1:kill();
          if(player2 ~= nil) then
            if(player2.isValid) then
              player2:kill();
            end
          end
          playerkilled = true;
        end
      end
    end
    if((secondsleft - timeelapsed) <= 100) then
      if(passTime == true) then
        if(warnedPlayer == false) then
          warnedPlayer = true;
          playSFXSDL(resPath .. "\\warning.wav");
          --test = "-----WARNING!-----\n\nYou're running out of time!";
          --Text.showMessageBox(type(test));
        end
      end
    end

    if(Level.winState() > 0) then

      beatLevel = true;
      passTime = false;
      postWinFrameCounter = postWinFrameCounter + 1;
      smb_threeHud_API.doEndingStuffs();
    end
  end


	local added = false;
	local showTimeLeft = false;
	local showPointCalc = false;
	local showAddPoints = false;
	local countPoints = false;



-- Start SMB3 Cards events

        if(firstRun)then

            if(card1 == 0) then
                Graphics.placeSprite(1, starcard, smb_threeHud_API.GUIPosition1.x, smb_threeHud_API.GUIPosition1.y);
            elseif(card1 == 1) then
                Graphics.placeSprite(1, mushroomcard, smb_threeHud_API.GUIPosition1.x, smb_threeHud_API.GUIPosition1.y);
            elseif(card1 == 2) then
                Graphics.placeSprite(1, flowercard, smb_threeHud_API.GUIPosition1.x, smb_threeHud_API.GUIPosition1.y);
            end

            if(card2 == 0) then
                Graphics.placeSprite(1, starcard, smb_threeHud_API.GUIPosition2.x, smb_threeHud_API.GUIPosition2.y);
            elseif(card2 == 1) then
                Graphics.placeSprite(1, mushroomcard, smb_threeHud_API.GUIPosition2.x, smb_threeHud_API.GUIPosition2.y);
            elseif(card2 == 2) then
                Graphics.placeSprite(1, flowercard, smb_threeHud_API.GUIPosition2.x, smb_threeHud_API.GUIPosition2.y);
            end

            if(card3 == 0) then
                Graphics.placeSprite(1, starcard, smb_threeHud_API.GUIPosition3.x, smb_threeHud_API.GUIPosition3.y);
            elseif(card3 == 1) then
                Graphics.placeSprite(1, mushroomcard, smb_threeHud_API.GUIPosition3.x, smb_threeHud_API.GUIPosition3.y);
            elseif(card3 == 2) then
                Graphics.placeSprite(1, flowercard, smb_threeHud_API.GUIPosition3.x, smb_threeHud_API.GUIPosition3.y);
            end
      
            
            firstRun = false
        end

        temp = NPC.get(11, -1);

        if(temp == nil) then

        elseif(temp[1] ~= nil) then
        thiscard = (temp[1]:mem(0xE4, FIELD_WORD));
        -- Text.print(tostring(temp[1]:mem(0xE4, FIELD_WORD)), 0, 0);
        end

        --temp[0]:mem(0xE4, FIELD_WORD);


        
        if(Level.winState() > 0) then
            smb_threeHud_API.endLevel();

        end




end

function smb_threeHud_API.doEndingStuffs()
  local timeLeftDrawPoint = {x = 288, y = 150}
  local pointCalculationDrawPoint = {x = 247, y = 170}
  local addPointsDrawPoint = {x = 365, y = 515};
  local timeleft = secondsleft - timeelapsed;

  if(getSeconds == false) then
    takeTime = secondsleft - timeelapsed;
    getSeconds = true;
  end

  local newPoints = tonumber(mem(0x00B2C8E4, FIELD_DWORD)) + (timeleft * 50);

  Text.print("COURSE CLEAR", 288, 150);

	Graphics.placeSprite(1, uiImage, 486, 569, "", 2);
	Text.print(string.format("%03d",takeTime), 1, 524, 570);


  timeelapsed = round(postWinFrameCounter / 60, 0);
  if(timeelapsed > 0) then
    showPointCalc = true;
  end

  if(timeelapsed > 1) then
    if(added ~= true) then
      showAddPoints = true;
      countPoints = true;
    end
  end

  if(timeelapsed == 4) or (takeTime <= 0) then
    if(added ~= true) then
      mem(0x00B2C8E4, FIELD_DWORD, newPoints);
        added = true;
        showAddPoints = false;  
    end
  end

  if(countPoints == true) then
          if(takeTime > 100) then
        takeTime = takeTime - 10;
        playSFXSDL(resPath .. "\\drumroll.wav");
      elseif(takeTime >= 1) then
        takeTime = takeTime - 1;
        playSFXSDL(resPath .. "\\drumroll.wav");
      elseif(takeTime <= 0) then
        takeTime = 0;
        countPoints = false;
      end
    end




  if(showPointCalc) then
    Text.print(tostring(timeleft) .. " x 50 = " .. tostring(timeleft * 50), pointCalculationDrawPoint.x, pointCalculationDrawPoint.y); 
  end
  if(showAddPoints == true) then
    Text.print("+" .. tostring(timeleft * 50), 3, addPointsDrawPoint.x, addPointsDrawPoint.y);

  end
end


function smb_threeHud_API.getSecondsLeft()
  return secondsleft;
end

function smb_threeHud_API.setSecondsLeft(to_set)
  if (to_set > 999) then
    secondsleft = 999;
  elseif (to_set < 0) then
    setSecondsLeft = 0;
  else
    secondsleft = to_set;
  end
end

function smb_threeHud_API.pauseTimer()
  passTime = false;
end

function smb_threeHud_API.setTimerState(b_tf)
  timerEnabled = b_tf;
end

local clock = os.clock;
function sleep(n)  -- seconds
  local t0 = clock();
  while clock() - t0 <= n do end
end

function round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end

function smb_threeHud_API.endLevel()

        levelFinished = true;

	if(doesUseCard == true) then

        if(Level.winState() == 1) then

        	postWinFrameCounter = postWinFrameCounter + 1;
        	local endLevelTimer = round(postWinFrameCounter / 60, 0);

	        if (addCard == false) then
	        	if(cards == nil) then
	        		cards = 0;
	        	end
	        	
	        	if(cards >= 4) then
	        		cards = 0;
	        		addCard = true;
	        	elseif(cards < 3) then
	        		local newcard = 1;
	            	cards = cards + newcard;
	            	addCard = true;
	            end
	        end

                gotcard = true;

				if (card1 == nil) then
					card1 = -1;
				end
				if (card2 == nil) then
					card2 = -1;
				end
				if (card3 == nil) then
					card3 = -1;
				end


				if(cards == 1) then
					if(card1 < 0) then
						card1 = tonumber(thiscard);
						dCard1 = card1;
						dataInstance:set("card1", tostring(card1));
					end

					if(card1 == 0) then
						Graphics.placeSprite(1, starcard, smb_threeHud_API.GUIPosition1.x, smb_threeHud_API.GUIPosition1.y);
					elseif(card1 == 1) then
						Graphics.placeSprite(1, mushroomcard, smb_threeHud_API.GUIPosition1.x, smb_threeHud_API.GUIPosition1.y);
					elseif(card1 == 2) then
						Graphics.placeSprite(1, flowercard, smb_threeHud_API.GUIPosition1.x, smb_threeHud_API.GUIPosition1.y);
					end
				end

				if(cards == 2) then
					if(card2 < 0) then
						card2 = tonumber(thiscard);
						dCard2 = card2;
						dataInstance:set("card2", tostring(card2));
					end

					if(card2 == 0) then
						Graphics.placeSprite(1, starcard, smb_threeHud_API.GUIPosition2.x, smb_threeHud_API.GUIPosition2.y);
					elseif(card2 == 1) then
						Graphics.placeSprite(1, mushroomcard, smb_threeHud_API.GUIPosition2.x, smb_threeHud_API.GUIPosition2.y);
					elseif(card2 == 2) then
						Graphics.placeSprite(1, flowercard, smb_threeHud_API.GUIPosition2.x, smb_threeHud_API.GUIPosition2.y);
					end
				end

				if(cards == 3) then
					if(card3 < 0) then
						card3 = tonumber(thiscard);
						dCard3 = card3;
						dataInstance:set("card3", tostring(card3));
					end

					if(card3 == 0) then
						Graphics.placeSprite(1, starcard, smb_threeHud_API.GUIPosition3.x, smb_threeHud_API.GUIPosition3.y);
					elseif(card3 == 1) then
						Graphics.placeSprite(1, mushroomcard, smb_threeHud_API.GUIPosition3.x, smb_threeHud_API.GUIPosition3.y);
					elseif(card3 == 2) then
						Graphics.placeSprite(1, flowercard, smb_threeHud_API.GUIPosition3.x, smb_threeHud_API.GUIPosition3.y);
					end
				end

                Text.print("Got Card!", 280, 115)
                if(thiscard == 1) then
                        Graphics.placeSprite(1,mushroomcard,450,96, "", 2);
                        local dCard1 = tonumber(dataInstance:get("card1 "));
                elseif(thiscard == 2) then
                        Graphics.placeSprite(1,flowercard,450,96, "", 2);
                        local dCard2 = tonumber(dataInstance:get("card2 "));
                elseif(thiscard == 0) then
                        Graphics.placeSprite(1,starcard,450,96, "", 2);
                        local dCard3 = tonumber(dataInstance:get("card3 "));
                end

               
                
                
                if(cards == 1) then
                	--Text.print("set card1 to " .. tostring(card1), 0, 45);
        			
            	elseif(cards == 2) then
            		--Text.print("set card2 to " .. tostring(card2), 0, 45);
        			
        		elseif(cards == 3) then
        			--Text.print("set card3 to " .. tostring(card3), 0, 45);

        			if(card1 == 0) and (card2 == 0) and (card3 == 0) then
        				mem(0x00B2C5AC, FIELD_FLOAT, (curLivesCount + 5));
        				if(playOnce == false) then
        					playSFXSDL(resPath .. "\\1up.wav");
        					playOnce = true;
        				end
        				Graphics.placeSprite(1,fiveup,500,110);
        			elseif(card1 == 1) and (card2 == 1) and (card3 == 1) then
        				mem(0x00B2C5AC, FIELD_FLOAT, (curLivesCount + 2));
        				if(playOnce == false) then
        					playSFXSDL(resPath .. "\\1up.wav");
        					playOnce = true;
        				end
        				Graphics.placeSprite(1,twoup,500,110);
        			elseif(card1 == 2) and (card2 == 2) and (card3 == 2) then
        				mem(0x00B2C5AC, FIELD_FLOAT, (curLivesCount + 3));
        				if(playOnce == false) then
        					playSFXSDL(resPath .. "\\1up.wav");
        					playOnce = true;
        				end
        				Graphics.placeSprite(1,threeup,500,110);
        			else
        				mem(0x00B2C5AC, FIELD_FLOAT, (curLivesCount + 1));
        				if(playOnce == false) then
        					playSFXSDL(resPath .. "\\1up.wav");
        					playOnce = true;
        				end
        				Graphics.placeSprite(1,oneup,500,110);
        			end

        			if(endLevelTimer >= 1) then
	        			card1 = -1;
	        			card2 = -1;
	        			card3 = -1;
	        			cards = 0;
	        			dataInstance:set("card1", tostring(card1));
	        			dataInstance:set("card2", tostring(card2));
	        			dataInstance:set("card3", tostring(card3));
        			end
        		end

				dataInstance:set("cards", tostring(cards));

				dataInstance:save();
        end
    end

end


function smb_threeHud_API.usesCard(me)
	doesUseCard = me;
end

function smb_threeHud_API.usesPBar(me)
  usePBar = me;
end

function smb_threeHud_API.useEasyMode(me)
  toggleEasyMode = me;
end

function smb_threeHud_API.disableSpinJump(me)
	disableSpinJump = me;
end

function smb_threeHud_API.onInputUpdateOverride()


	if(disableSpinJump == true) then
		if(player.altJumpKeyPressing == true) and (timer == 1) and (player:mem(0x146, FIELD_WORD) == 2) or (player.altJumpKeyPressing == true) and (timer == 1) and  (player:mem(0x48, FIELD_WORD) > 0) or (player.altJumpKeyPressing == true) and (timer == 1) and  (player:mem(0x176, FIELD_WORD) > 0) or (player.altJumpKeyPressing == true) and (timer == 1) and  (player:mem(0x40, FIELD_WORD) > 0) then
			player.altJumpKeyPressing = false;
			player:mem(0x50, FIELD_WORD, 0);
			player:mem(0x11C, FIELD_WORD, 15);
			player.jumpKeyPressing = true;
			timer = 0;
		elseif(player.altJumpKeyPressing == false) and (timer == 0) then
			timer = 1;
		end
	end

end


return smb_threeHud_API;