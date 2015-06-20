package.path = package.path .. ";./worlds/?/?.lua" .. ";./?.lua"
triggers = require("triggers");

trigs = {}
trigs[0] = triggers.Trigger(-200000, -200000, 
        function()
		player.powerup = 3; --Give player fire
		end, 
		"HideTriggerLayer");
		
function onLoop()
	--Test Axe objects for triggering
	triggers.testTriggers(178, trigs);
end