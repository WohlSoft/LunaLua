---------------------------------------
--------***********************--------
--------**______ _   _ _____ **--------
--------**| ___ \ \ | |  __ \**--------
--------**| |_/ /  \| | |  \/**--------
--------**|    /| . ` | | __ **--------
--------**| |\ \| |\  | |_\ \**--------
--------**\_| \_\_| \_/\____/**--------
--------**                   **--------
--------***********************--------
---------------------------------------
--------Created by Hoeloe - 2015-------
--Open-Source Random Number Generator--
--------For Super Mario Bros X---------
---------------v1.0.2------------------

local rng = {};

local MAG_SEED = 817796519;
local quot = 977441620;
rng.seed = nil;

--Generates the metatable for the rng.
local function genMT()
	--Calculate max number (not constant across build platforms.)
	local x = 1.0
	pcall(function()
				while x * 4 > x * 2 do
					x = x * 2
				end
				if x * 4 < x * 2 then
					x = x * 2
				end
			end)
			
    local d = x / 2
	
	pcall(function()
			while x + d ~= x do
				if x + d > x and x + d + d ~= x + d then
					x = x + d
				end
				d = d / 2
			end
		end)
	x = math.floor(x);
	
	local mt = {};
	mt.__index = function(tbl,key)
		if(key == "MAX") then
			return x;
		else
			return rawget(tbl,key);
		end
	end
	mt.__newindex = function(tbl,key,val)
		if(key == "MAX") then
			error("Attempted to set a read-only field. This is not allowed.",2);
		elseif(key == "seed") then
			rawset(tbl,key,val);
		else
			error("Write access to this object's fields is prohibited.",2);
		end
	end
	
	return mt;
end

local function genRand()
	--Initialise RNG
	if(rng.seed == nil) then
		rng.seed = math.floor(math.fmod(os.clock()*958134896,MAG_SEED));
	end
	
	--Generate PRN and the quotient for the next iteration
	local s = math.abs((26147705 * rng.seed) + quot);
	rng.seed = math.fmod(s, MAG_SEED);
	quot = math.floor(s/MAG_SEED);
	
	--Normalise PRN
	return rng.seed/(MAG_SEED-1);
end

function rng.randomInt(a,b)
	if(a == nil and b == nil) then
		return math.floor(rng.random(1000000)+0.5);
	elseif(a == nil and b ~= nil) then
		return rng.randomInt(0,b);
	elseif(a ~= nil and b == nil) then
		return rng.randomInt(0,a);
	else
		if(a <= b) then
			return math.floor(rng.random(a-0.5,b+0.5)+0.5);
		else
			return rng.randomInt(b,a);
		end
	end
end

function rng.random(a,b)
	if(b == nil) then
		if(a == nil) then
			return genRand();
		else
			return a * genRand();
		end
	else
		if(a == b) then
			return a;
		elseif(a > b) then
			return rng.random(b,a);
		else
			local range = b-a;
			return range*genRand() + a;
		end
	end
end

function rng.randomEntry(tbl)
	local nt = {};
	for _,v in pairs(tbl) do
		table.insert(nt,v);
	end
	local n = rng.randomInt(#nt - 1);
	return nt[n+1];
end


function rng.irandomEntry(tbl)
	local n = rng.randomInt(table.getn(tbl) - 1);
	return tbl[n+1];
end

function rng.randomChar(a,b)
	if(b == nil and (a == nil or type(a) == 'boolean')) then
		if(a == nil) then
			local c = rng.randomChar("A","Z");
			local l = rng.randomChar("a","z");
			if(rng.randomInt(0,1) == 0) then return c else return l end;
		elseif(a) then --Upper case
			return rng.randomChar("A","Z");
		else --Lower case
			return rng.randomChar("a","z");
		end
	else
		local s;
		local f;
		if(a == nil) then s = string.byte("A",1) else s = string.byte(a,1); end
		if(b == nil) then f = s; s = string.byte("A",1); else f = string.byte(b,1); end
		local n = rng.randomInt(s,f);
		return string.char(n);
	end
end

setmetatable(rng,genMT());

return rng;