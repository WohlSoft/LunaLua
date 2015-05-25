--raocoin.lua 
--v1.1.0
--Created by Hoeloe, 2015
local raocoin = {}

local checkBought = false;
local nearestPrice = -1;

local raocoins = 0;
local lastRaocoins = -1;
local lastMPs = -1;
local levelFinished = false;

raocoin.imageID = 999999;
raocoin.GUIPosition = {x = 472, y = 66}

raocoin.items = {}

local path = "../../../LuaScriptsLib/raocoin/";

function raocoin.currentRaocoins()
	return UserData.getValue("WorldRaocoins") + raocoins;
end

--Allows you to re-route the path of the HUD image. By default, this is  "../../../LuaScriptsLib/raocoin/" (inside a subfolder of LuaScriptsLib). Use this to locate the folder containing "raocoins.bmp".
function raocoin.setImagePath(p)
	path = p;
	refreshImage();
end

function raocoin.getImagePath()
	return path;
end

local function refreshImage()
  loadImage(path.."raocoins.bmp", raocoin.imageID, 0xFF00FF);
end

function raocoin.onInitAPI()

	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
	
	registerEvent("raocoin", "onLoad", "init", true) --Register the init event
	registerEvent("raocoin", "onLoop", "update", true) --Register the loop event
end

function raocoin.init()
  refreshImage();
  placeSprite(1,raocoin.imageID,raocoin.GUIPosition.x,raocoin.GUIPosition.y);
end

--Create a new shop item. 
--The first 4 arguments define a rectangle, which, when the player is inside, the price of this item is displayed.
--itemprice is the price in raocoins of the item.
--varname is the variable name to save the purchase under in the save file.
--actions is a function that defines what should happen when this item is bought (or when loading the level if the item is already bought).
function raocoin.shopItem(left, right, top, bottom, itemprice, varname, actions)
	return { l = left, r = right, u = top, b = bottom, price = itemprice, savename = varname, acts = actions}
end

--Checks available raocoins and, if the player has enough, buy the item.
function raocoin.buyItem(item)
		if raocoin.canAfford(item) then
				UserData.setValue(item.savename,1);
				UserData.setValue("WorldRaocoins", UserData.getValue("WorldRaocoins")-item.price);
				UserData.save();
				item.acts();
		end
end

--Checks if the player can afford a given item.
function raocoin.canAfford(item)
	return UserData.getValue("WorldRaocoins") >= item.price;
end

--Updates the price indicator and ensures correct loading of previously bought items.
function raocoin.update()

	--If an item has already been bought, perform the "buy" action, free of charge (runs only once at the start of the level).
	if checkBought == false then
		for k1,v1 in pairs(raocoin.items) do
			if UserData.getValue(v1.savename) == 1 then
				v1.acts();
			end
		end
		checkBought = true;
	end

	--Finds the first display rectangle that the player is inside, and displays the price of the related object.
	local setPrice = false;
	for k,v in pairs(raocoin.items) do
		if (UserData.getValue(v.savename) == nil or UserData.getValue(v.savename) == 0) and player.x > v.l and player.x < v.r and player.y > v.u and player.y < v.b then
			nearestPrice = v.price
			setPrice = true
			break
		end
	end
	
	--If the player is not inside any display rectangle, stop displaying prices.
	if setPrice == false then
		nearestPrice = -1
	end

	--Draw the price to the screen.
	if nearestPrice > 0 then
			local width = 34 + 9*string.len(tostring(nearestPrice));
			placeSprite(1, 999999, 400-width, 100, "", 2);
		    printText (tostring(nearestPrice), 456-width, 100)
    end
	
	
		local rcs = findnpcs(274, -1);
	  local mps = findnpcs(192, -1);
	  
	  local raocounter = 0;
	  
	  for key,value in pairs(rcs) do
			raocounter = raocounter + 1;
	  end
	  
	  local mpcounter = 0;
	  
	  for key,value in pairs(mps) do
			mpcounter = mpcounter + 1;
	  end
	  
	  if UserData.isValueSet("WorldRaocoins") == false then
			UserData.setValue("WorldRaocoins", 0)
			UserData.save()
	  end
	  
	  if(mpcounter < lastMPs) then
			UserData.setValue("WorldRaocoins", UserData.getValue("WorldRaocoins") + raocoins);
			raocoins = 0;
			UserData.save();
	  end
	  
	  lastMPs = mpcounter;
		
	  if(mem(0x00B2C59E,FIELD_WORD) ~= 0) then --level is ending
		if(levelFinished == false) then
			UserData.setValue("WorldRaocoins", UserData.getValue("WorldRaocoins") + raocoins);
			UserData.save();
			levelFinished = true;
		end
		printText(tostring(UserData.getValue("WorldRaocoins")),1,raocoin.GUIPosition.x+98-18*string.len(tostring(UserData.getValue("WorldRaocoins"))),raocoin.GUIPosition.y+1);
	  else
		printText(tostring(UserData.getValue("WorldRaocoins")+raocoins),1,raocoin.GUIPosition.x+98-18*string.len(tostring(UserData.getValue("WorldRaocoins")+raocoins)),raocoin.GUIPosition.y+1);
		if(raocounter < lastRaocoins) then
			raocoins = raocoins+lastRaocoins-raocounter;
		end
		lastRaocoins = raocounter;
	  end
end

return raocoin;