--bitwise.lua 
--v1.0.1
--Created by Hoeloe, 2015
local bitwise = {}

local XOR_t =
{ 
   {0,1},
   {1,0}
}

local AND_t =
{ 
   {0,0},
   {0,1}
}

local OR_t =
{ 
   {0,1},
   {1,1}
}

local NOT_t =
{ 
   1,
   0
}

function bitwise.invert(t)
	local t2 = {}
	for k,v in t do
		t2[k] = {}
		for l,w in v do
			t2[k][l] = 1-w;
		end
	end
	return t2;
end

function bitwise.bitunary(a,t)
    local p = 1
	local c = 0;
	while(a > 0) do
	  c = c + (t[(a % 2)+1]*p)
      a = math.floor(a/2)
      p = p * 2
	end
    return c
end

function bitwise.bitbinary(a,b,t)
    local p = 1
	local c = 0;
	while(a > 0 or b > 0) do
	  c = c + (t[(a % 2)+1][(b % 2)+1]*p)
      a = math.floor(a/2)
      b = math.floor(b/2)
      p = p * 2
	end
    return c
end

function bitwise.leftshift(a,s)
	if(s <= 0) then return a end;
	local p = 0;
    while(s > 0) do
		p = p + 2;
		s = s - 1;
	end
	return a * p;
end

function bitwise.cleftshift(a,s)
	local s2 = bitwise.leftshift(a,s);
	local b = bitwise.rightshift(s2,s);
	if(b == a) then return s2 else
		local d = a-b;
		d = bitwise.rightshift(d,64-s);
		return d+s2;
	end
end

function bitwise.rightshift(a,s)
	if(s <= 0) then return a end;
	local p = 0;
    while(s > 0) do
		p = p + 2;
		s = s - 1;
	end
	return a / p;
end

function bitwise.crightshift(a,s)
	local s2 = bitwise.rightshift(a,s);
	local b = bitwise.leftshift(s2,s);
	if(b == a) then return s2 else
		local d = a-b;
		d = bitwise.leftshift(d,64-s);
		return d+s2;
	end
end

function bitwise.bitnot(a)
    return bitwise.bitunary(a,NOT_t);
end

function bitwise.bitand(a,b)
    return bitwise.bitbinary(a,b,AND_t);
end

function bitwise.bitnand(a,b)
    return bitwise.bitnot(bitwise.bitand(a,b));
end

function bitwise.bitor(a,b)
    return bitwise.bitbinary(a,b,OR_t);
end

function bitwise.bitnor(a,b)
    return bitwise.bitnot(bitwise.bitnor(a,b));
end

function bitwise.bitxor(a,b)
    return bitwise.bitbinary(a,b,XOR_t);
end

return bitwise;