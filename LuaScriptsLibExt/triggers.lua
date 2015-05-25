--triggers.lua 
--v1.0.0
--Created by Hoeloe, 2015
--DEPRECATED IN LUNALUA v0.6 : Use onEvent instead.
local triggers = {}

function triggers.Trigger(X,Y,Actions,Event)
	local t = {x = X, y = Y, actions = Actions, event = Event};
	return t;
end

--Test if the given coordinates are within one square of the second coordinates
function triggers.withinSquare(x,y,sqx,sqy)
	if math.abs(x-sqx) < 16 and math.abs(y-sqy) < 16 then
		return true;
	else
		return false;
	end
end

--Test if the trigger is visible or not
function triggers.getTrigger(npc,tx,ty)
	if(triggers.withinSquare(npc.x, npc.y, tx, ty) and npc:mem(0x74,FIELD_WORD) ~= 0) then
		return true;
	else
		return false;
	end
end

--Reset the trigger so that it can be used again
function triggers.resetTrigger(npc,eventName)
	npc:mem(0x74,FIELD_WORD,0);
	if(eventName ~= nil) then
		triggerEvent(eventName);
	end
end

--Test the given trigger ID and trigger list, to process the triggers
function triggers.testTriggers(triggerID, trigs)
  local ts = findnpcs(triggerID,-1);
  for key,value in pairs(ts) do
	--printText("X:"..value.x.." Y:"..value.y.." W:"..value:mem(0x74,FIELD_WORD),0,key*18);
	for k,v in pairs(trigs) do
		if triggers.getTrigger(value,v.x,v.y) then
			v.actions();
			if(v.event ~= nil) then
				triggers.resetTrigger(value,v.event);
			else
				triggers.resetTrigger(value);
			end
		end
	end
  end
end

return triggers;