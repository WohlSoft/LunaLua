--debug.lua
--By PixelPest
--1.2


local debug = {};
local debugTable = {};


function debug.onInitAPI()
   registerEvent(debug, "onTickEnd", "onTickEnd", false);
end


function debug.type(obj, heading)
  local h = heading or "";
  table.insert(debugTable, {h, type(obj)});
end


function debug.simple(obj, heading)
  local h = heading or "";

   if (type(obj) ~= "string") and (type(obj) ~= "number") and (type(obj) ~= "boolean") and (type(obj) ~= "nil") then
    error("Variable defined by "..h.." is not considered simple (string nor boolean nor number nor nil).");
  end

  table.insert(debugTable, {h, obj});
end

function debug.table(obj, heading)
  local h = heading or "";

  if obj == nil then
    table.insert(debugTable, {h, "nil"});
  elseif type(obj) ~= "table" then
    error("Variable defined by "..h.." is not a table.");
  else
    for k, v in pairs(obj) do
      table.insert(debugTable, {h.."["..k.."]", v})
    end
  end
end

function debug.onTickEnd()
  if #debugTable > 30 then
    error("Too many lines (>30).");
  end
 
  local yCounter = 0;
 
  for _, v in pairs(debugTable) do
    if v[1] == "" then
      Text.printWP(tostring(v[2]), 0, yCounter, 10);
    else
      Text.printWP(v[1]..": "..tostring(v[2]), 0, yCounter, 10);
    end
    
    yCounter = yCounter + 20;
  end

  debugTable = {};
end


return debug


