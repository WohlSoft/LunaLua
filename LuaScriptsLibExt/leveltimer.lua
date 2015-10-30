local __title = "Level Timer";local __version = "1.1.1.1";local __description = "A simple script to provide a level timer, a la Super Mario World and Super Mario Bros. 3.";local __author = "Mike Santiago, edited by XNBlank";local __url = "http://mrmiketheripper.x10.mx/leveltimer"
local lf_levelTimerApi = {} --instance

local secondsleft = 300; --The amount of seconds left.local framecounter = 0; --Counts the amount of frames
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
local resPath = getSMBXPath() .. "\\LuaScriptsLib\\leveltimer"; --res path
local uiImage = Graphics.loadImage(resPath .. "\\ui.png"); --ui thing


lf_levelTimerApi.GUIPosition_NoStars = {x = 252, y = 50}lf_levelTimerApi.GUIPosition_Stars = {x = 252, y = 66}


function lf_levelTimerApi.onInitAPI()  timerEnabled = false; --user must start it
  registerEvent(lf_levelTimerApi, "onLoop", "onLoopOverride");
end


function lf_levelTimerApi.onLoopOverride()  if(timerEnabled == true) then    timeelapsed = round(framecounter / 60, 0);
    if(tonumber(mem(0x00B251E0, FIELD_DWORD)) > 0) and (beatLevel == false) then
      Graphics.placeSprite(1, uiImage, lf_levelTimerApi.GUIPosition_Stars.x, lf_levelTimerApi.GUIPosition_Stars.y, "", 2);
      Text.print(tostring(secondsleft - timeelapsed), 1, lf_levelTimerApi.GUIPosition_Stars.x + 44, lf_levelTimerApi.GUIPosition_Stars.y + 1);
    elseif (tonumber(mem(0x00B251E0, FIELD_DWORD)) == 0) and (beatLevel == false) then
      Graphics.placeSprite(1, uiImage, lf_levelTimerApi.GUIPosition_NoStars.x, lf_levelTimerApi.GUIPosition_NoStars.y, "", 2);
      Text.print(tostring(secondsleft - timeelapsed), 1, lf_levelTimerApi.GUIPosition_NoStars.x + 44, lf_levelTimerApi.GUIPosition_NoStars.y + 1);
    end

  --Text.print("Time to take..." .. tostring(takeTime), 0, 0);
  --Text.print("Seconds left..." .. tostring(secondsleft - timeelapsed), 0, 15);    if (passTime == true) then
      framecounter = framecounter + 1;
    end

    if(timeelapsed >= secondsleft) then      passTime = false;
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
      lf_levelTimerApi.doEndingStuffs();
    end
  end
end


local added = false;local showTimeLeft = false;
local showPointCalc = false;
local showAddPoints = false;
local countPoints = false;


function lf_levelTimerApi.doEndingStuffs()  local timeLeftDrawPoint = {x = 288, y = 150}
  local pointCalculationDrawPoint = {x = 247, y = 170}
  local addPointsDrawPoint = {x = 461, y = 70};
  local timeleft = secondsleft - timeelapsed;

  if(getSeconds == false) then
    takeTime = secondsleft - timeelapsed;
    getSeconds = true;
  end

  local newPoints = tonumber(mem(0x00B2C8E4, FIELD_DWORD)) + (timeleft * 50);
  Text.print("COURSE CLEAR", 288, 150);
  if(tonumber(mem(0x00B251E0, FIELD_DWORD)) > 0) and (beatLevel == true) then
    Graphics.placeSprite(1, uiImage, lf_levelTimerApi.GUIPosition_Stars.x, lf_levelTimerApi.GUIPosition_Stars.y, "", 2);
    Text.print(tostring(takeTime), 1, lf_levelTimerApi.GUIPosition_Stars.x + 44, lf_levelTimerApi.GUIPosition_Stars.y + 1);
  elseif (tonumber(mem(0x00B251E0, FIELD_DWORD)) == 0) and (beatLevel == true) then  Graphics.placeSprite(1, uiImage, lf_levelTimerApi.GUIPosition_NoStars.x, lf_levelTimerApi.GUIPosition_NoStars.y, "", 2);  Text.print(tostring(takeTime), 1, lf_levelTimerApi.GUIPosition_NoStars.x + 44, lf_levelTimerApi.GUIPosition_NoStars.y + 1);
end


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

  if(timeelapsed == 4) and (takeTime <= 0) then
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

function lf_levelTimerApi.getSecondsLeft()
  return secondsleft;
end

function lf_levelTimerApi.setSecondsLeft(to_set)  if (to_set > 999) then    secondsleft = 999;    framecounter = 0;    timeelapsed = 0;  elseif (to_set < 0) then    secondsleft = 0;    timeelapsed = 0;
    framecounter = 0;
  else    secondsleft = to_set;    timeelapsed = 0;
    framecounter = 0;
  endend

function lf_levelTimerApi.pauseTimer()
  passTime = false;
end

function lf_levelTimerApi.setTimerState(b_tf)
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

return lf_levelTimerApi;