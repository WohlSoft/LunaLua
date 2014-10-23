--[[
This is a example code for the new API-usage.
You are free to use whatever you wanna do with it.
]]
local framecounter = {} --Package table
local myCounter = 0 --Counter for passed frames


function framecounter.onInitAPI() --Is called when the api is loaded by loadAPI.
	--init vars
	myCounter = 0
	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
	registerEvent("framecounter", "onLoop") --Register the loop event
end

function framecounter.onLoop() --The hooked onLoop-Event
	myCounter = myCounter + 1 --Increment the passed frames variable
end

function framecounter.getCurrentFrame() --Returns the number of passed frames.
	return myCounter --Return the value of the variable
end

return framecounter --Return the package when the api is getting loaded.