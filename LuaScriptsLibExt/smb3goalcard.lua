local __title = "SMB3 Goal Cards";
local __version = "1.0.1.1";
local __description = "Make the SMB3 Goal Card act as it did in SMB3.";
local __author = "XNBlank";
local __url = "https://github.com/XNBlank";

--[[
HOW TO USE

1 . Make a new file in your worlds folder called LunaWorld.lua.
2 . Add " smb3card = loadAPI("smb3goalcard"); " to the file.
3 . In your onLoad function, you can toggle the card function on/off with " smb3card.usesCard(TRUE/FALSE); "
4 . If you want to have a certain level not use the card function, or if it doesn't have a card, add the above line, set it to false, and put it into a LunaDll.lua file in that levels graphic folder.

]]

local smbGoalCard_API = {} --instance

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
local resPath = getSMBXPath() .. "\\LuaScriptsLib\\smb3goalcard"; --res path
local uicard = Graphics.loadImage(resPath .. "\\smb3card.png");
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

smbGoalCard_API.GUIPosition1 = {x = 650, y = 550}
smbGoalCard_API.GUIPosition2 = {x = 698, y = 550}
smbGoalCard_API.GUIPosition3 = {x = 746, y = 550}

function smbGoalCard_API.onInitAPI()
    gotcard = false;
    addCard = false;
    registerEvent(smbGoalCard_API, "onLoop", "onLoopOverride");
    registerEvent(smbGoalCard_API, "onLoad", "onLoadOverride");
    cards = tonumber(dataInstance:get("cards"));
    card1 = tonumber(dataInstance:get("card1"));
    card2 = tonumber(dataInstance:get("card2"));
    card3 = tonumber(dataInstance:get("card3"));

    --Defines.smb3RouletteScoreValueStar = 4;
    --Defines.smb3RouletteScoreValueMushroom = 2;
    --Defines.smb3RouletteScoreValueFlower = 3;

end

function smbGoalCard_API.onLoadOverride()
    
end



function smbGoalCard_API.onLoopOverride()
        if(firstRun)then

            Graphics.placeSprite(1,uicard,smbGoalCard_API.GUIPosition1.x,smbGoalCard_API.GUIPosition1.y);
            Graphics.placeSprite(1,uicard,smbGoalCard_API.GUIPosition2.x,smbGoalCard_API.GUIPosition2.y);
            Graphics.placeSprite(1,uicard,smbGoalCard_API.GUIPosition3.x,smbGoalCard_API.GUIPosition3.y);

            if(card1 == 0) then
                Graphics.placeSprite(1, starcard, smbGoalCard_API.GUIPosition1.x, smbGoalCard_API.GUIPosition1.y);
            elseif(card1 == 1) then
                Graphics.placeSprite(1, mushroomcard, smbGoalCard_API.GUIPosition1.x, smbGoalCard_API.GUIPosition1.y);
            elseif(card1 == 2) then
                Graphics.placeSprite(1, flowercard, smbGoalCard_API.GUIPosition1.x, smbGoalCard_API.GUIPosition1.y);
            end

            if(card2 == 0) then
                Graphics.placeSprite(1, starcard, smbGoalCard_API.GUIPosition2.x, smbGoalCard_API.GUIPosition2.y);
            elseif(card2 == 1) then
                Graphics.placeSprite(1, mushroomcard, smbGoalCard_API.GUIPosition2.x, smbGoalCard_API.GUIPosition2.y);
            elseif(card2 == 2) then
                Graphics.placeSprite(1, flowercard, smbGoalCard_API.GUIPosition2.x, smbGoalCard_API.GUIPosition2.y);
            end

            if(card3 == 0) then
                Graphics.placeSprite(1, starcard, smbGoalCard_API.GUIPosition3.x, smbGoalCard_API.GUIPosition3.y);
            elseif(card3 == 1) then
                Graphics.placeSprite(1, mushroomcard, smbGoalCard_API.GUIPosition3.x, smbGoalCard_API.GUIPosition3.y);
            elseif(card3 == 2) then
                Graphics.placeSprite(1, flowercard, smbGoalCard_API.GUIPosition3.x, smbGoalCard_API.GUIPosition3.y);
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



        smbGoalCard_API.debugDraw();
        
        if(Level.winState() > 0) then
            smbGoalCard_API.endLevel();

        end


end

function smbGoalCard_API.debugDraw()
    --[[
    Text.print("Card 1 = " .. dataInstance:get("card1"), 0, 100);
    Text.print("Card 2 = " .. dataInstance:get("card2"), 0, 115);
    Text.print("Card 3 = " .. dataInstance:get("card3"), 0, 130);
    Text.print("Timer = " .. tostring(endLevelTimer), 0, 160);
    
    Text.print("Current Card = " .. tostring(thiscard), 0, 160);
    Text.print("Cards = " .. tostring(cards), 0, 75);
    Text.print("Win state: " .. Level.winState(), 0, 200)
]]
end

function smbGoalCard_API.endLevel()

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
                        Graphics.placeSprite(1, starcard, smbGoalCard_API.GUIPosition1.x, smbGoalCard_API.GUIPosition1.y);
                    elseif(card1 == 1) then
                        Graphics.placeSprite(1, mushroomcard, smbGoalCard_API.GUIPosition1.x, smbGoalCard_API.GUIPosition1.y);
                    elseif(card1 == 2) then
                        Graphics.placeSprite(1, flowercard, smbGoalCard_API.GUIPosition1.x, smbGoalCard_API.GUIPosition1.y);
                    end
                end

                if(cards == 2) then
                    if(card2 < 0) then
                        card2 = tonumber(thiscard);
                        dCard2 = card2;
                        dataInstance:set("card2", tostring(card2));
                    end

                    if(card2 == 0) then
                        Graphics.placeSprite(1, starcard, smbGoalCard_API.GUIPosition2.x, smbGoalCard_API.GUIPosition2.y);
                    elseif(card2 == 1) then
                        Graphics.placeSprite(1, mushroomcard, smbGoalCard_API.GUIPosition2.x, smbGoalCard_API.GUIPosition2.y);
                    elseif(card2 == 2) then
                        Graphics.placeSprite(1, flowercard, smbGoalCard_API.GUIPosition2.x, smbGoalCard_API.GUIPosition2.y);
                    end
                end

                if(cards == 3) then
                    if(card3 < 0) then
                        card3 = tonumber(thiscard);
                        dCard3 = card3;
                        dataInstance:set("card3", tostring(card3));
                    end

                    if(card3 == 0) then
                        Graphics.placeSprite(1, starcard, smbGoalCard_API.GUIPosition3.x, smbGoalCard_API.GUIPosition3.y);
                    elseif(card3 == 1) then
                        Graphics.placeSprite(1, mushroomcard, smbGoalCard_API.GUIPosition3.x, smbGoalCard_API.GUIPosition3.y);
                    elseif(card3 == 2) then
                        Graphics.placeSprite(1, flowercard, smbGoalCard_API.GUIPosition3.x, smbGoalCard_API.GUIPosition3.y);
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


function smbGoalCard_API.usesCard(me)
    doesUseCard = me;
end

return smbGoalCard_API;