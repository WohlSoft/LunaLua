--*****************
--* metaluna.lua  *
--* v1.0          *
--*               *
--*****************

local metaluna = {}
local sectionIds = {}


if Animation.__tostring == nil then
function Animation.__tostring(obj)
	return "Animation (id="..obj.id..", x="..obj.x..", y="..obj.y..")" 
end; end;

if BGO.__tostring == nil then
function BGO.__tostring(obj)
	return "BGO (id="..obj.id..", x="..obj.x..", y="..obj.y..")"
end; end;

if Block.__tostring == nil then
function Block.__tostring(obj)
	return "Block (id="..obj.id..", x="..obj.x..", y="..obj.y..")"
end; end;

if Camera.__tostring == nil then
function Camera.__tostring(obj)
	return "Camera (x="..obj.x..", y="..obj.y..", w="..obj.width..", h="..obj.height..")" 
end; end;

if Layer.__tostring == nil then
function Layer.__tostring(obj)
	return "Layer ("..obj.layerName..")" 
end; end;

if Level.__tostring == nil then
function Level.__tostring(obj)
	return "Level (title="..obj.title..")" 
end; end;

if NPC.__tostring == nil then
function NPC.__tostring(obj)
	return "NPC (id="..obj.id..")"
end; end;

if Player.__tostring == nil then
function Player.__tostring(obj)
	return "Player (char="..obj.character..")"
end; end;

if RECT.__tostring == nil then
function RECT.__tostring(obj)
	return "RECT (l="..tostring(obj.left)..",r="..tostring(obj.right)..",".."t="..tostring(obj.top)..",b="..tostring(obj.bottom)..")"
end; end;

if RECTd.__tostring == nil then
function RECTd.__tostring(obj)
	return "RECTd (l="..tostring(obj.left)..",r="..tostring(obj.right)..",".."t="..tostring(obj.top)..",b="..tostring(obj.bottom)..")"
end; end;

if Section.__tonumber == nil then
function Section.__tonumber(obj)
	local secId = 1
	for  k,v in pairs (Section.get()) do
		if  v == obj  then  
			secId = k
			break;
		end
	end
	
	return secId
end; end;

if Section.__tostring == nil then
function Section.__tostring(obj)
	return "Section ("..tostring(tonumber(obj))..")"
end; end;

if Warp.__tostring == nil then
function Warp.__tostring(obj)
	return "Warp ("..tostring(obj:mem (0x5A, FIELD_WORD))..")"
end; end;

return metaluna;