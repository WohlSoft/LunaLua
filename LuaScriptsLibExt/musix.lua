local musix = {};

musix.counter = 400;

local count = 0;

musix.musicCodes = {};

musix.musicPositions = {};

function musix.onInitAPI() 
	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
	
	for s = 0,20,1 do
		musix.musicCodes[s] = "";
		musix.musicPositions[s] = {x = 20, y = 580};
	end
	
	registerEvent("musix", "onLoop", "update", true) --Register the loop event
end

function musix.update()
	if count > 0 then
		printText(tostring(musix.musicCodes[player.section]),musix.musicPositions[player.section].x,musix.musicPositions[player.section].y);
		count = count - 1;
	end
	
	if getInput().str:find("music") then
		count = musix.counter;
		getInput():clear();
	end
end

return musix;