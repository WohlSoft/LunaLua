--********************************--
--** _    __          __  ____  **--
--**| |  / /__  _____/ /_/ __ \ **--
--**| | / / _ \/ ___/ __/ /_/ / **--
--**| |/ /  __/ /__/ /_/ _, _/  **--
--**|___/\___/\___/\__/_/ |_|   **--
--**                            **--
--********************************--
------Created by Hoeloe - 2015------
-----Open-Source Vector Library-----
-------For Super Mario Bros X-------
----------------v1.0a---------------

local vectr = {};
local version = "1.0";

local function mthide(a)
	return {};
end

do --VECTOR

local function proj(a,b)
	local n = b:normalise();
	return (a..n)*n;
end

do --Vector 2

--CLASS DEF
local vect2 = {};
local v2mt = {};

function vect2.normalise(a)
	if(a.sqrlength == 0) then 
		return vectr.v2(0,0)
	elseif(a.sqrlength == 1) then
		return a;
	else
		return vectr.v2(a.x/a.length,a.y/a.length);
	end
end

function vect2.rotate(a,d)
	local r = d*0.0174534; --deg2rad
	local sr = math.sin(r);
	local cr = math.cos(r);
	return vectr.v2(a.x*cr - a.y*sr, a.x*sr + a.y*cr);
end

function vect2.lookat(a,x1,y1)
	local v;
	local t = v2mt_typecheck(x1);
	if(t == "number") then
		v = vectr.v2(x1,y1):normalise();
	else
		v = x1:normalise();
	end
	return v*a.length;
end

function vect2.tov3(a)
	return vectr.v3(a.x,a.y,0);
end

function vect2.tov4(a)
	return vectr.v3(a.x,a.y,0,0);
end

function vect2.dot(a,b) 
	return a.x*b.x + a.y*b.y 
end

vect2.normalize = vect2.normalise;
vect2.project = proj;

--METATABLE

local function v2mt_typecheck(a)
	if(type(a) == "number") then
		return "number";
	elseif(a._type ~= nil and a._type == "vector2") then
		return "vector2";
	else
		error("Calculation cannot be performed on an object of this type.", 2);
	end
end

function v2mt.__index(obj,key)
		if(key == "x") then return rawget(obj, "x")
	elseif(key == "y") then return rawget(obj, "y")
	elseif(key == "sqrlength") then
		return obj.x*obj.x + obj.y*obj.y
	elseif(key == "length") then
		return math.sqrt(obj.x*obj.x + obj.y*obj.y)
	elseif(key == "_type") then
		return "vector2";
	else
		return vect2[key]
	end
end

function v2mt.__newindex(obj,key,val)
		    if(key == "x") then rawset(obj, "x", val);
		elseif(key == "y") then rawset(obj, "y", val);
		elseif(key == "length" or key == "sqrlength") then
			error("Cannot set the length of a vector directly. Try changing the component values.",2)
		elseif(key == "_type") then
			error("Cannot set the type of an object.",2)
		else
			error("Field "..key.." does not exist in the vector2 data structure.",2);
		end
end
	
function v2mt.__tostring(obj) 
	return "("..tostring(obj.x)..", "..tostring(obj.y)..")" 
end

function v2mt.__add(a,b) 
	local ta = v2mt_typecheck(a);
	local tb = v2mt_typecheck(b);
	if(ta == "number") then
		if(tb == "number") then
			return a+b;
		else
			return vectr.v2(a+b.x,a+b.y)
		end
	elseif(type(b) == "number") then
			return vectr.v2(a.x+b,a.y+b)
	else
		return vectr.v2(a.x+b.x,a.y+b.y)
	end
end

function v2mt.__sub(a,b) 
	local ta = v2mt_typecheck(a);
	local tb = v2mt_typecheck(b);
	if(ta == "number") then
		if(tb == "number") then
			return a-b;
		else
			return vectr.v2(a-b.x,a-b.y)
		end
	elseif(type(b) == "number") then
			return vectr.v2(a.x-b,a.y-b)
	else
		return vectr.v2(a.x-b.x,a.y-b.y)
	end
end

function v2mt.__unm(a)
	return vectr.v2(-a.x,-a.y) 
end

function v2mt.__mul(a,b) 
	local ta = v2mt_typecheck(a);
	local tb = v2mt_typecheck(b);
	if(ta == "number") then
		if(tb == "number") then
			return a*b;
		else
			return vectr.v2(a*b.x,a*b.y)
		end
	elseif(tb == "number") then
		return vectr.v2(a.x*b,a.y*b)
	else
		return vectr.v2(a.x*b.x,a.y*b.y) 
	end
end
			   
function v2mt.__div(a,b) 
	local ta = v2mt_typecheck(a);
	local tb = v2mt_typecheck(b);
	if(ta == "number") then
		if(tb == "number") then
			return a/b;
		else
			return vectr.v2(a/b.x,a/b.y)
		end
	elseif(tb == "number") then
			return vectr.v2(a.x/b,a.y/b)
	else
		return vectr.v2(a.x/b.x,a.y/b.y) 
	end
end
			   
function v2mt.__eq(a,b)
	if(a == nil or b == nil or typecheck(a) ~= "vector2" or typecheck(b) ~= "vector2") then 
		return false; 
	else
		return a.x==b.x and a.y==b.y
	end
end

function v2mt.__len(a)
	return 2;
end

v2mt.__concat = vect2.dot;
v2mt.__mod = proj;

--CONSTRUCTOR
function vectr.v2(x,y)
	if(type(x) == "number") then
		y = y or x;
	elseif(x ~= nil and x._type ~= nil and x._type == "vector2") then
		y = x.y;
		x = x.x;
	else
		error("Invalid vector definition.",2);
	end
	
	local v = {x=x, y=y};
	setmetatable(v, v2mt);
	return v;
end

end

do --Vector 3

--METATABLE
local createmt3;

local v3mt = {}
function v3mt.dot(a,b)
	return a.x*b.x + a.y*b.y + a.z*b.z
end

function v3mt.cross(a,b)
	return vectr.v3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)
end

function v3mt.typecheck(a)
	if(type(a) == "number") then
		return "number";
	elseif(a._type ~= nil and a._type == "vector3") then
		return "vector3";
	else
		error("Calculation cannot be performed on an object of this type.", 2);
	end
end

function v3mt.newindex(obj,key,val)
	if(key == "x") then setmetatable(obj,createmt3(val,obj.y,obj.z))
	elseif(key == "y") then setmetatable(obj,createmt3(obj.x,val,obj.z))
	elseif(key == "z") then setmetatable(obj,createmt3(obj.x,obj.y,val))
	elseif(key == "length" or key == "sqrlength") then
		error("Cannot set the length of a vector directly. Try changing the component values.",2)
	elseif(key == "_type") then
		error("Cannot set the type of an object.",2)
		else
			error("Field "..key.." does not exist in the vector3 data structure.",2);
	end
end

function v3mt.tostring(obj)
	return "("..tostring(obj.x)..", "..tostring(obj.y)..", "..tostring(obj.z)..")" 
end

function v3mt.add(a,b)
	local ta = v3mt.typecheck(a);
	local tb = v3mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a+b;
		else
			return vectr.v3(a+b.x,a+b.y,a+b.z)
		end
	elseif(tb == "number") then
			return vectr.v3(a.x+b,a.y+b,a.z+b)
	else
		return vectr.v3(a.x+b.x,a.y+b.y,a.z+b.z) 
	end
end

function v3mt.sub(a,b)
	local ta = v3mt.typecheck(a);
	local tb = v3mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a-b;
		else
			return vectr.v3(a-b.x,a-b.y,a-b.z)
		end
	elseif(tb == "number") then
			return vectr.v3(a.x-b,a.y-b,a.z-b)
	else
		return vectr.v3(a.x-b.x,a.y-b.y,a.z-b.z) 
	end
end

function v3mt.neg(a) 
	return vectr.v3(-a.x,-a.y,-a.z)
end


function v3mt.mul(a,b)
	local ta = v3mt.typecheck(a);
	local tb = v3mt.typecheck(b);
	if(ta == "number") then
		if(tb == "number") then
			return a*b;
		else
			return vectr.v3(a*b.x,a*b.y,a*b.z)
		end
	elseif(tb == "number") then
			return vectr.v3(a.x*b,a.y*b,a.z*b)
	else
		return vectr.v3(a.x*b.x,a.y*b.y,a.z*b.z) 
	end
end

function v3mt.div(a,b)
	local ta = v3mt.typecheck(a);
	local tb = v3mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a/b;
		else
			return vectr.v3(a/b.x,a/b.y,a/b.z)
		end
	elseif(tb == "number") then
			return vectr.v3(a.x/b,a.y/b,a.z/b)
	else
		return vectr.v3(a.x/b.x,a.y/b.y,a.z/b.z) 
	end
end

function v3mt.equals(a,b)
	return (a.x==b.x) and (a.y==b.y) and (a.z==b.z) 
end

function v3mt.len(a)
	return 3;
end

createmt3 = function(x,y,z,sl,l)
	local mt = {}
	mt.__index = 
	function(obj,key)
		    if(key == "x") then return x
		elseif(key == "y") then return y
		elseif(key == "z") then return z
		elseif(key == "sqrlength") then
			if(sl == nil) then
				local sm = x*x + y*y + z*z;
				setmetatable(obj,createmt3(x,y,z,sm));
				return sm;
			else
				return sl;
			end
		elseif(key == "length") then
			if(l == nil) then
				local sm =  sl or (x*x + y*y + z*z);
				local mag = math.sqrt(sm);
				setmetatable(obj,createmt3(x,y,z,sm,mag));
				return mag;
			else
				return l;
			end
		elseif(key == "_type") then
			return "vector3";
		else
			return nil;
		end
	end
	
	mt.__newindex = v3mt.newindex;
	mt.__tostring = v3mt.tostring;
	mt.__add = v3mt.add;
	mt.__sub = v3mt.sub;
	mt.__unm = v3mt.neg;
	mt.__mul = v3mt.mul;
	mt.__div = v3mt.div;
	mt.__concat = v3mt.dot;
	mt.__pow = v3mt.cross;
	mt.__mod = proj;
	mt.__eq = v3mt.equals;
	mt.__len = v3mt.len;
	--mt.__metatable = mthide;
	
	return mt;
end

--CLASS DEF
local vect3 = {}
function vect3.normalise(a)
	if(a.sqrlength == 0) then 
		return vectr.v3(0,0,0)
	elseif(a.sqrlength == 1) then
		return a;
	else
		return vectr.v3(a.x/a.length,a.y/a.length,a.z/a.length);
	end
end

function vect3.rotate(a,roll,pitch,yaw)
	if(type(pitch) == "number") then --euler
		local r_r = roll*0.0174534; --deg2rad
		local p_r = pitch*0.0174534; --deg2rad
		local y_r = yaw*0.0174534; --deg2rad
		
		local cosx = math.cos(r_r);
		local sinx = math.sin(r_r);
		local cosy = math.cos(p_r);
		local siny = math.sin(p_r);
		local cosz = math.cos(y_r);
		local sinz = math.sin(y_r);
							 
		local x = a.x;
		local y = a.y;
		local z = a.z;			
			
		--rotz
		x = x*cosz - y*sinz;
		y = x*sinz + y*cosz;
		
		--rot y
		x = x*cosy + z*siny;
		z = -x*siny + z*cosy;
		
		--rot x
		y = y*cosx - z*sinx
		z = y*sinx + z*cosx;
		
		return vectr.v3(x,y,z);			
	elseif(pitch ~= nil and pitch._type ~= nil and pitch._type == "vector3") then --angleaxis
		local p = pitch:normalise();
		local x = roll*0.0174534; --deg2rad
		local cosx = math.cos(x);
		local sinx = math.sin(x);
		return cosx*a + sinx*(p^a) + (1-cosx)*(p..a)*p;
	elseif(roll ~= nil and roll._type ~= nil and roll._type == "mat3") then --matrix
		return roll*a;
	else
		error("Invalid rotation format specified.",2)
	end
end

function vect3.lookat(a,x1,y1,z1)
	local v3;
	local t = v3mt.typecheck(x1);
	if(t == "number") then
		v3 = vectr.v3(x1,y1,z1):normalise();
	else
		v3 = x1:normalise();
	end
	return v3*a.length;
end

function vect3.planeproject(a,b)
	b = b:normalise();
	local n = a%b;
	return a-n;
end

function vect3.tov2(a)
	return vectr.v2(a.x,a.y);
end

function vect3.tov4(a)
	return vectr.v4(a.x,a.y,a.z,0);
end

vect3.normalize = vect3.normalise;
vect3.dot = v3mt.dot;
vect3.cross = v3mt.cross;
vect3.project = proj;

--CONSTRUCTOR
function vectr.v3(x,y,z)
	local v = {};
	
	for m,n in pairs(vect3) do
		v[m] = n;
	end
	
	if(type(x) == "number") then
		y = y or x;
		z = z or y;
	elseif(x ~= nil and x._type ~= nil and x._type == "vector2") then
		y = x.y;
		z = 0;
		x = x.x;
	elseif(x ~= nil and x._type ~= nil and x._type == "vector3") then
		y = x.y;
		z = x.z;
		x = x.x;
	else
		error("Invalid vector definition.",2)
	end
	
	setmetatable(v,createmt3(x,y,z));
	return v;
end

end

do --Vector 4

--METATABLE
local createmt4;
local v4mt = {}

function v4mt.dot(a,b)
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
end

function v4mt.typecheck(a)
	if(type(a) == "number") then
		return "number";
	elseif(a._type ~= nil and a._type == "vector4") then
		return "vector4";
	else
		error("Calculation cannot be performed on an object of this type.", 2);
	end
end

function v4mt.newindex(obj,key,value)
		    if(key == "x") then setmetatable(obj,createmt4(val,obj.y,obj.z,obj.w))
		elseif(key == "y") then setmetatable(obj,createmt4(obj.x,val,obj.z,obj.w))
		elseif(key == "z") then setmetatable(obj,createmt4(obj.x,obj.y,val,obj.w))
		elseif(key == "w") then setmetatable(obj,createmt4(obj.x,obj.y,obj.z,val))
		elseif(key == "length" or key == "sqrlength") then
			error("Cannot set the length of a vector directly. Try changing the component values.",2)
		elseif(key == "_type") then
			error("Cannot set the type of an object.",2)
		else
			error("Field "..key.." does not exist in the vector4 data structure.",2);
		end
end

function v4mt.tostring(obj)
	return "("..tostring(obj.x)..", "..tostring(obj.y)..", "..tostring(obj.z)..", "..tostring(obj.w)..")" 
end

function v4mt.add(a,b)
	local ta = v4mt.typecheck(a);
	local tb = v4mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a+b;
		else
			return vectr.v4(a+b.x,a+b.y,a+b.z,a+b.w)
		end
	elseif(tb == "number") then
			return vectr.v4(a.x+b,a.y+b,a.z+b,a.w+b)
	else
		return vectr.v4(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w) 
	end
end

function v4mt.sub(a,b)
	local ta = v4mt.typecheck(a);
	local tb = v4mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a-b;
		else
			return vectr.v4(a-b.x,a-b.y,a-b.z,a-b.w)
		end
	elseif(tb == "number") then
			return vectr.v4(a.x-b,a.y-b,a.z-b,a.w-b)
	else
		return vectr.v4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w) 
	end
end

function v4mt.neg(a) 
	return vectr.v4(-a.x,-a.y,-a.z,-a.w)
end

function v4mt.mul(a,b)
	local ta = v3mt.typecheck(a);
	local tb = v3mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a*b;
		else
			return vectr.v4(a*b.x,a*b.y,a*b.z,a*b.w)
		end
	elseif(tb == "number") then
			return vectr.v4(a.x*b,a.y*b,a.z*b,a.w*b)
	else
		return vectr.v4(a.x*b.x,a.y*b.y,a.z*b.z,a.w*b.w) 
	end
end

function v4mt.div(a,b)
	local ta = v4mt.typecheck(a);
	local tb = v4mt.typecheck(b);
	if(ta == "number") then
		if(ta == "number") then
			return a/b;
		else
			return vectr.v4(a/b.x,a/b.y,a/b.z,a/b.w)
		end
	elseif(tb == "number") then
			return vectr.v4(a.x/b,a.y/b,a.z/b,a.w/b)
	else
		return vectr.v4(a.x/b.x,a.y/b.y,a.z/b.z,a.w/b.w) 
	end
end

function v4mt.equals(a,b)
	return (a.x==b.x) and (a.y==b.y) and (a.z==b.z) and (a.w==b.w)
end

function v4mt.len(a)
	return 4;
end

createmt4 = function(x,y,z,w,sl,l)
	local mt = {}
	mt.__index = 
	function(obj,key)
		    if(key == "x") then return x
		elseif(key == "y") then return y
		elseif(key == "z") then return z
		elseif(key == "w") then return w
		elseif(key == "sqrlength") then
			if(sl == nil) then
				local sm = x*x + y*y + z*z + w*w;
				setmetatable(obj,createmt4(x,y,z,w,sm));
				return sm;
			else
				return sl;
			end
		elseif(key == "length") then
			if(l == nil) then
				local sm = sl or (x*x + y*y + z*z + w*w);
				local mag = math.sqrt(sm);
				setmetatable(obj,createmt4(x,y,z,w,sm,mag));
				return mag;
			else
				return l;
			end
		elseif(key == "_type") then
			return "vector4";
		else
			return nil;
		end
	end
	
	mt.__newindex = v4mt.newindex;
	mt.__tostring = v4mt.tostring;
	mt.__add = v4mt.add;
	mt.__sub = v4mt.sub;
	mt.__unm = v4mt.neg;
	mt.__mul = v4mt.mul;
	mt.__div = v4mt.div;
	mt.__concat = v4mt.dot;
	mt.__mod = proj;
	mt.__eq = v4mt.equals;
	mt.__len = v4mt.len;
	--mt.__metatable = mthide;
	
	return mt;
end

--CLASS DEF
local vect4 = {}
function vect4.normalise(a)
	if(a.sqrlength == 0) then 
		return vectr.v4(0,0,0,0)
	elseif(a.sqrlength == 1) then
		return a;
	else
		return vectr.v4(a.x/a.length,a.y/a.length,a.z/a.length,a.w/a.length);
	end
end

function vect4.rotate(a,roll,pitch,yaw)
	if(type(pitch) == "number") then --euler
		local r_r = roll*0.0174534; --deg2rad
		local p_r = pitch*0.0174534; --deg2rad
		local y_r = yaw*0.0174534; --deg2rad
		
		local cosx = math.cos(r_r);
		local sinx = math.sin(r_r);
		local cosy = math.cos(p_r);
		local siny = math.sin(p_r);
		local cosz = math.cos(y_r);
		local sinz = math.sin(y_r);
							 
		local x = a.x;
		local y = a.y;
		local z = a.z;			
			
		--rotz
		x = x*cosz - y*sinz;
		y = x*sinz + y*cosz;
		
		--rot y
		x = x*cosy + z*siny;
		z = -x*siny + z*cosy;
		
		--rot x
		y = y*cosx - z*sinx
		z = y*sinx + z*cosx;
		
		return vectr.v4(x,y,z,a.w);			
	elseif(pitch ~= nil and pitch._type ~= nil and pitch._type == "vector3") then --angleaxis
		local p = pitch:normalise();
		local x = roll*0.0174534; --deg2rad
		local cosx = math.cos(x);
		local sinx = math.sin(x);
		local a3 = a:tov3();
		a3 = cosx*a3 + sinx*(p^a3) + (1-cosx)*(p..a3)*p;
		return vectr.v4(a3.x,a3.y,a3.z,a.w);
	elseif(roll ~= nil and roll._type ~= nil and roll._type == "mat3") then --3matrix
		local a3 = a:tov3();
		a3 = roll*a3;
		return vectr.v4(a3.x,a3.y,a3.z,a.w);
	elseif(roll ~= nil and roll._type ~= nil and roll._type == "mat4") then --4matrix
		return roll*a;
	else
		error("Invalid rotation format specified.",2)
	end
end

function vect4.lookat(a,x1,y1,z1)
	local v3;
	if(type(x1) == "number") then
		v3 = vectr.v4(x1,y1,z1):normalise();
	elseif(x1 ~= nil and x1._type ~= nil and x1._type == "vector3") then
		v3 = x1:normalise();
	else
		error("Invalid lookat vector.",2)
	end
	v3 = v3*(a:tov3()).length;
	return vectr.v4(v3.x,v3.y,v3.z,a.w)
end

function vect4.planeproject(a,b)
	b = b:normalise();
	local n = a%b;
	return a-n;
end

function vect4.tov2(a)
	return vectr.v2(a.x,a.y);
end

function vect4.tov3(a)
	return vectr.v3(a.x,a.y,a.z);
end

vect4.normalize = vect4.normalise;
vect4.dot = v4mt.dot;
vect4.project = proj;

--CONSTRUCTOR
function vectr.v4(x,y,z,w)
	local v = {};
	
	for m,n in pairs(vect4) do
		v[m] = n;
	end
	
	if(type(x) == "number") then
		y = y or x;
		z = z or y;
		w = w or 1;
	elseif(x ~= nil and x._type ~= nil and x._type == "vector2") then
		y = x.y;
		z = 0;
		w = 1;
		x = x.x;
	elseif(x ~= nil and x._type ~= nil and x._type == "vector3") then
		y = x.y;
		z = x.z;
		w = 1;
		x = x.x;
	elseif(x ~= nil and x._type ~= nil and x._type == "vector4") then
		y = x.y;
		z = x.z;
		w = x.w;
		x = x.x;
	else
		error("Invalid vector definition.",2)
	end
	setmetatable(v,createmt4(x,y,z,w));
	return v;
end

end

end

do --MATRIX

local function trace(a,c)
	local t = 0;
	for i=1,c,1 do
		t = t+a[i][i];
	end
	return t;
end

do --Matrix 2x2

--METATABLE
local mat2_mt = {}
mat2_mt.__index = function(obj,key)
	if(key == "det") then
		return obj[1][1]*obj[2][2] - obj[1][2]*obj[2][1];
	elseif(key == "trace") then
		return trace(obj,2);
	elseif(key == "inverse") then
		local d = obj.det;
		if(d == 0) then
			return nil; --Matrix is singular and has no inverse.
		end
		return vectr.mat2({obj[2][2], -obj[1][2]}, {-obj[2][1],obj[1][1]})/d;
	elseif(key == "transpose") then
		return vectr.mat2({obj[1][1],obj[2][1]},{obj[1][2],obj[2][2]});
	elseif(key == "_type") then
		return "mat2";
	else
		return nil;
	end
end

function mat2_mt.typecheck(a,b)
	if(type(a) == "number") then
		return "number";
	elseif(b and a._type ~= nil and a._type == "vector2") then
		return "vector2";
	elseif(a._type ~= nil and a._type == "mat2") then
		return "mat2";
	else
		error("Calculation cannot be performed on an object of this type.", 2);
	end
end

mat2_mt.__newindex = function(obj,key,val)
	if(key == "det") then
		error("Cannot set the determinant of a matrix.",2)
	elseif(key == "trace") then
		error("Cannot set the trace of a matrix.",2)
	elseif(key == "inverse") then
		error("Cannot set the inverse of a matrix.",2)
	elseif(key == "transpose") then
		error("Cannot set the transpose of a matrix.",2)
	elseif(key == "_type") then
		error("Cannot set the type of an object.",2)
	else
		error("Field "..key.." does not exist in the mat2 data structure.",2);
	end
end

mat2_mt.__add = function(a,b)
	local ta = mat2_mt.typecheck(a,false);
	local tb = mat2_mt.typecheck(b,false);
	if(ta == "number") then
		if(tb == "number") then
			return a+b;
		else
			local t = vectr.mat2(b);
			for i=1,2,1 do
				for j=1,2,1 do
					t[i][j] = t[i][j]+a;
				end
			end
			return t;
		end
	elseif(tb == "number") then
		local t = vectr.mat2(a);
		for i=1,2,1 do
			for j=1,2,1 do
				t[i][j] = t[i][j]+b;
			end
		end
		return t;
	else
		local t = vectr.mat2(a);
		for i=1,2,1 do
			for j=1,2,1 do
				t[i][j] = t[i][j]+b[i][j];
			end
		end
		return t;
	end
end
mat2_mt.__sub = function(a,b)
	local ta = mat2_mt.typecheck(a,false);
	local tb = mat2_mt.typecheck(b,false);
	if(ta == "number") then
		if(tb == "number") then
			return a-b;
		else
			local t = vectr.mat2(b);
			for i=1,2,1 do
				for j=1,2,1 do
					t[i][j] = a-t[i][j];
				end
			end
			return t;
		end
	elseif(tb == "number") then
		local t = vectr.mat2(a);
		for i=1,2,1 do
			for j=1,2,1 do
				t[i][j] = t[i][j]-b;
			end
		end
			return t;
	else
		local t = vectr.mat2(a);
		for i=1,2,1 do
			for j=1,2,1 do
				t[i][j] = t[i][j]-b[i][j];
			end
		end
		return t;
	end
end
mat2_mt.__unm = function(a) 
		local t = vectr.mat2(a);
		for i=1,2,1 do
			for j=1,2,1 do
				t[i][j] = -t[i][j];
			end
		end
		return t;
end
mat2_mt.__mul = 
function(a,b)
	local ta = mat2_mt.typecheck(a,true);
	local tb = mat2_mt.typecheck(b,true);
	if(ta == "number") then
		if(tb == "number" or tb == "vector2") then
			return a*b;
		else
			local t = vectr.mat2(b);
			for i=1,2,1 do
				for j=1,2,1 do
					t[i][j] = t[i][j]*a;
				end
			end
			return t;
		end
	elseif(ta == "vector2") then
		if(tb == "mat2") then
			error("Invalid matrix multiplication.",2);
		else
			return a*b;
		end
	elseif(ta == "mat2") then
		if(tb == "number") then
			local t = vectr.mat2(a);
			for i=1,2,1 do
				for j=1,2,1 do
					t[i][j] = t[i][j]*b;
				end
			end
			return t;
		elseif(tb ==  "vector2") then
							local t = {0,0};
							local v = {b.x,b.y};
							for i=1,2,1 do
								for j=1,2,1 do
									t[i] = t[i] + v[j]*a[i][j];
								end
							end
							return vectr.v2(t[1],t[2])
		elseif(b._type ==  "mat2") then
							local t = vectr.mat2({0,0},{0,0})
							for i=1,2,1 do
								for j=1,2,1 do
									for k=1,2,1 do
										t[i][j] = t[i][j] + a[i][k]*b[k][j];
									end
								end
							end
							return t;
		else
			error("Invalid matrix multiplication.",2);
		end
	else
		error("Invalid matrix multiplication.",2);
	end
end
mat2_mt.__div =
function(a,b)
	local ta = mat2_mt.typecheck(a,true);
	local tb = mat2_mt.typecheck(b,true);
	if(tb == "mat2") then
		if(ta ~= "vector2") then
			return a*b.inverse;
		else
			error("Invalid matrix operation.",2);
		end
	elseif(tb == "vector2") then
		if(ta == "vector2" or ta == "number") then
			return a/b;
		else
			error("Invalid matrix operation.",2);
		end
	else
		if(ta == "vector2" or ta == "number") then
			return a/b;
		else
			local t = vectr.mat2(a);
			for i=1,2,1 do
				for j=1,2,1 do
					t[i][j] = t[i][j]/b;
				end
			end
			return t;
		end
	end
end
mat2_mt.__eq = function(a,b)
	for i=1,2,1 do
		for j=1,2,1 do
			if(a[i][j] ~= b[i][j]) then
				return false;
			end
		end
	end
	return true;
end
mat2_mt.__tostring = function(obj) return "(("..tostring(obj[1][1])..", "..tostring(obj[1][2]).."), ("..tostring(obj[2][1])..", "..tostring(obj[2][2]).."))"; end
mat2_mt.__metatable = mthide;
mat2_mt.__len = function(a) return 2; end

--CONSTRUCTOR
function vectr.mat2(row1,row2)
	if(row1._type ~= nil and row1._type == "mat2") then
		row2 = row1[2];
		row1 = row1[1];
	elseif(row1._type ~= nil and row1._type ~= "mat2") then
		error("Invalid matrix definition - invalid arguments.",2)
	elseif(#row1 ~= 2 or row2 == nil or #row2 ~= 2) then
		error("Invalid matrix definition - wrong number of matrix elements.",2)
	end
	local m = {row1,row2}
	m.tomat3 = function(a)
		return vectr.mat3({a[1][1],a[1][2],0},{a[2][1],a[2][2],0},{0,0,1});
	end
	
	m.tomat4 = function(a)
		return vectr.mat4({a[1][1],a[1][2],0,0},{a[2][1],a[2][2],0,0},{0,0,1,0},{0,0,0,1});
	end
	
	setmetatable(m,mat2_mt)
	return m;
end

end

do --Matrix 3x3

local mat3_mt = {}
mat3_mt.__index = function(obj,key)
	if(key == "det") then
		return obj[1][1]*(obj[2][2]*obj[3][3] - obj[2][3]*obj[3][2]) - obj[1][2]*(obj[2][1]*obj[3][3] - obj[2][3]*obj[3][1]) + obj[1][3]*(obj[2][1]*obj[3][2] - obj[2][2]*obj[3][1])
	elseif(key == "trace") then
		return trace(obj,3)
	elseif(key == "inverse") then
		local d = obj.det;
		if(d == 0) then
			return nil; --Matrix is singular and has no inverse.
		end
		local t = vectr.mat3({0,0,0},{0,0,0},{0,0,0});
		for i=1,3,1 do
			local i2 = (i%3)+1;
			local i3 = (i2%3)+1
			for j=1,3,1 do
				local j2 = (j%3)+1;
				local j3 = (j2%3)+1
				t[i][j] = obj[j2][i2]*obj[j3][i3] - obj[j3][i2]*obj[j2][i3];
			end
		end
		return t/d;
	elseif(key == "transpose") then
		return vectr.mat3({obj[1][1],obj[2][1],obj[3][1]},{obj[1][2],obj[2][2],obj[3][2]},{obj[1][3],obj[2][3],obj[3][3]});
	elseif(key == "_type") then
		return "mat3";
	else
		return nil;
	end
end

function mat3_mt.typecheck(a,b)
	if(type(a) == "number") then
		return "number";
	elseif(b and a._type ~= nil and a._type == "vector3") then
		return "vector3";
	elseif(a._type ~= nil and a._type == "mat3") then
		return "mat3";
	else
		error("Calculation cannot be performed on an object of this type.", 2);
	end
end

mat3_mt.__newindex = function(obj,key,val)
	if(key == "det") then
		error("Cannot set the determinant of a matrix.",2)
	elseif(key == "trace") then
		error("Cannot set the trace of a matrix.",2)
	elseif(key == "inverse") then
		error("Cannot set the inverse of a matrix.",2)
	elseif(key == "transpose") then
		error("Cannot set the transpose of a matrix.",2)
	elseif(key == "_type") then
		error("Cannot set the type of an object.",2)
	else
		error("Field "..key.." does not exist in the mat3 data structure.",2);
	end
end
mat3_mt.__add = function(a,b)
	local ta = mat3_mt.typecheck(a,false);
	local tb = mat3_mt.typecheck(b,false);
	if(ta == "number") then
		if(tb == "number") then
			return a+b;
		else
			local t = vectr.mat3(b);
			for i=1,3,1 do
				for j=1,3,1 do
					t[i][j] = t[i][j]+a;
				end
			end
			return t;
		end
	elseif(tb == "number") then
		local t = vectr.mat3(a);
		for i=1,3,1 do
			for j=1,3,1 do
				t[i][j] = t[i][j]+b;
			end
		end
		return t;
	else
		local t = vectr.mat3(a);
		for i=1,3,1 do
			for j=1,3,1 do
				t[i][j] = t[i][j]+b[i][j];
			end
		end
		return t;
	end
end
mat3_mt.__sub = function(a,b)
	local ta = mat3_mt.typecheck(a,false);
	local tb = mat3_mt.typecheck(b,false);
	if(ta == "number") then
		if(tb == "number") then
			return a-b;
		else
			local t = vectr.mat3(b);
			for i=1,3,1 do
				for j=1,3,1 do
					t[i][j] = a-t[i][j];
				end
			end
			return t;
		end
	elseif(tb == "number") then
		local t = vectr.mat3(a);
		for i=1,3,1 do
			for j=1,3,1 do
				t[i][j] = t[i][j]-b;
			end
		end
		return t;
	else
		local t = vectr.mat3(a);
		for i=1,3,1 do
			for j=1,3,1 do
				t[i][j] = t[i][j]-b[i][j];
			end
		end
		return t;
	end
end
mat3_mt.__unm = 
function(a) 
		local t = vectr.mat3(a);
		for i=1,3,1 do
			for j=1,3,1 do
				t[i][j] = -t[i][j];
			end
		end
		return t;
end

mat3_mt.__mul = function(a,b)
	local ta = mat3_mt.typecheck(a,true);
	local tb = mat3_mt.typecheck(b,true);
	if(ta == "number") then
		if(tb == "number" or tb == "vector3") then
			return a*b;
		else
			local t = vectr.mat3(b);
			for i=1,3,1 do
				for j=1,3,1 do
					t[i][j] = t[i][j]*a;
				end
			end
			return t;
		end
	elseif(ta == "vector3") then
		if(tb == "mat3") then
			error("Invalid matrix multiplication.",2);
		else
			return a*b;
		end
	elseif(ta == "mat3") then
		if(tb == "number") then
			local t = vectr.mat3(a);
			for i=1,3,1 do
				for j=1,3,1 do
					t[i][j] = t[i][j]*b;
				end
			end
			return t;
		elseif(tb ==  "vector3") then
							local t = {0,0,0};
							local v = {b.x,b.y,b.z};
							for i=1,3,1 do
								for j=1,3,1 do
									t[i] = t[i] + v[j]*a[i][j];
								end
							end
							return vectr.v3(t[1],t[2],t[3])
		elseif(b._type ==  "mat3") then
							local t = vectr.mat3({0,0,0},{0,0,0},{0,0,0})
							for i=1,3,1 do
								for j=1,3,1 do
									for k=1,3,1 do
										t[i][j] = t[i][j] + a[i][k]*b[k][j];
									end
								end
							end
							return t;
		else
			error("Invalid matrix multiplication.",2);
		end
	else
		error("Invalid matrix multiplication.",2);
	end
end

mat3_mt.__div =
function(a,b)
	local ta = mat3_mt.typecheck(a,true);
	local tb = mat3_mt.typecheck(b,true);
	if(tb == "mat3") then
		if(ta ~= "vector3") then
			return a*b.inverse;
		else
			error("Invalid matrix operation.",2);
		end
	elseif(tb == "vector3") then
		if(ta == "vector3" or ta == "number") then
			return a/b;
		else
			error("Invalid matrix operation.",2);
		end
	else
		if(ta == "vector3" or ta == "number") then
			return a/b;
		else
			local t = vectr.mat3(a);
			for i=1,3,1 do
				for j=1,3,1 do
					t[i][j] = t[i][j]/b;
				end
			end
			return t;
		end
	end
end

mat3_mt.__eq = function(a,b)
	for i=1,3,1 do
		for j=1,3,1 do
			if(a[i][j] ~= b[i][j]) then
				return false;
			end
		end
	end
	return true;
end
mat3_mt.__tostring = function(obj) return "(("..tostring(obj[1][1])..", "..tostring(obj[1][2])..", "..tostring(obj[1][3]).."), ("..tostring(obj[2][1])..", "..tostring(obj[2][2])..", "..tostring(obj[2][3]).."), ("..tostring(obj[3][1])..", "..tostring(obj[3][2])..", "..tostring(obj[3][3]).."))"; end
mat3_mt.__metatable = mthide;
mat3_mt.__len = function(a) return 3; end

--CONSTRUCTOR
function vectr.mat3(row1,row2,row3)
	if(row1._type ~= nil and row1._type == "mat3") then
		row3 = row1[3];
		row2 = row1[2];
		row1 = row1[1];
	elseif(row1._type ~= nil and row1._type ~= "mat3") then
		error("Invalid matrix definition - invalid arguments.",2)
	elseif(#row1 ~= 3 or row2 == nil or #row2 ~= 3 or row3 == nil or #row3 ~= 3) then
		error("Invalid matrix definition - wrong number of matrix elements.",2)
	end
	local m = {row1,row2,row3}
	m.tomat2 = function(a)
		return vectr.mat2({a[1][1],a[1][2]},{a[2][1],a[2][2]});
	end
	
	m.tomat4 = function(a)
		return vectr.mat4({a[1][1],a[1][2],a[1][3],0},{a[2][1],a[2][2],a[2][3],0},{a[3][1],a[3][2],a[3][3],0},{0,0,0,1});
	end
	
	setmetatable(m,mat3_mt)
	return m;
end

end

do --Matrix 4x4

--METATABLE
local mat4_mt = {}
mat4_mt.__index = function(obj,key)
	if(key == "det") then
		return obj[1][4]*(obj[2][3]*(obj[3][2]*obj[4][1] - obj[3][1]*obj[4][2]) + obj[2][2]*(obj[3][1]*obj[4][3] - obj[3][3]*obj[4][1]) + obj[2][1]*(obj[3][3]*obj[4][2] - obj[3][2]*obj[4][3])) +
			   obj[1][2]*obj[2][1]*(obj[3][4]*obj[4][3] - obj[3][3]*obj[4][4]) + obj[1][2]*(obj[2][4]*(obj[3][3]*obj[4][1] - obj[3][1]*obj[4][3]) + obj[2][3]*(obj[3][1]*obj[4][4] - obj[3][4]*obj[4][1])) +
			   obj[1][3]*(obj[2][4]*(obj[3][1]*obj[4][2]-obj[3][2]*obj[4][1]) + obj[2][2]*(obj[3][4]*obj[4][1]-obj[3][1]*obj[4][4]) + obj[2][1]*(obj[3][2]*obj[4][4]-obj[3][4]*obj[4][2])) +
			   obj[1][1]*(obj[2][4]*(obj[3][2]*obj[4][3]-obj[3][3]*obj[4][2]) + obj[2][3]*(obj[3][4]*obj[4][2]-obj[3][2]*obj[4][4]) + obj[2][2]*(obj[3][3]*obj[4][4]-obj[3][4]*obj[4][3]))
	elseif(key == "trace") then
		return trace(obj,4);
	elseif(key == "inverse") then
		return (1/obj.det)*vectr.mat4({-obj[4][2]*(obj[2][4]*obj[3][3]-obj[2][3]*obj[3][4]) + obj[4][3]*(obj[2][4]*obj[3][2] - obj[2][2]*obj[3][4]) - obj[4][4]*(obj[2][3]*obj[3][2] - obj[2][2]*obj[3][3]),
									    obj[4][2]*(obj[1][4]*obj[3][3]-obj[1][3]*obj[3][4]) - obj[4][3]*(obj[1][4]*obj[3][2] - obj[1][2]*obj[3][4]) + obj[4][4]*(obj[1][3]*obj[3][2] - obj[1][2]*obj[3][3]),
									   -obj[4][2]*(obj[1][4]*obj[2][3]-obj[1][3]*obj[2][4]) + obj[4][3]*(obj[1][4]*obj[2][2] - obj[1][2]*obj[2][4]) - obj[4][4]*(obj[1][3]*obj[2][2] - obj[1][2]*obj[2][3]),
									    obj[3][2]*(obj[1][4]*obj[2][3]-obj[1][3]*obj[2][4]) - obj[3][3]*(obj[1][4]*obj[2][2] - obj[1][2]*obj[2][4]) + obj[3][4]*(obj[1][3]*obj[2][2] - obj[1][2]*obj[2][3])},
										
									  { obj[4][1]*(obj[2][4]*obj[3][3]-obj[2][3]*obj[3][4]) - obj[4][3]*(obj[2][4]*obj[3][1] - obj[2][1]*obj[3][4]) + obj[4][4]*(obj[2][3]*obj[3][1] - obj[2][1]*obj[3][3]),
									   -obj[4][1]*(obj[1][4]*obj[3][3]-obj[1][3]*obj[3][4]) + obj[4][3]*(obj[1][4]*obj[3][1] - obj[1][1]*obj[3][4]) - obj[4][4]*(obj[1][3]*obj[3][1] - obj[1][1]*obj[3][3]),
									    obj[4][1]*(obj[1][4]*obj[2][3]-obj[1][3]*obj[2][4]) - obj[4][3]*(obj[1][4]*obj[2][1] - obj[1][1]*obj[2][4]) + obj[4][4]*(obj[1][3]*obj[2][1] - obj[1][1]*obj[2][3]),
									   -obj[3][1]*(obj[1][4]*obj[2][3]-obj[1][3]*obj[2][4]) + obj[3][3]*(obj[1][4]*obj[2][1] - obj[1][1]*obj[2][4]) - obj[3][4]*(obj[1][3]*obj[2][1] - obj[1][1]*obj[2][3])},
									   
									  {-obj[4][1]*(obj[2][4]*obj[3][2]-obj[2][2]*obj[3][4]) + obj[4][2]*(obj[2][4]*obj[3][1] - obj[2][1]*obj[3][4]) - obj[4][4]*(obj[2][2]*obj[3][1] - obj[2][1]*obj[3][2]),
									    obj[4][1]*(obj[1][4]*obj[3][2]-obj[1][2]*obj[3][4]) - obj[4][2]*(obj[1][4]*obj[3][1] - obj[1][1]*obj[3][4]) + obj[4][4]*(obj[1][2]*obj[3][1] - obj[1][1]*obj[3][2]),
									   -obj[4][1]*(obj[1][4]*obj[2][2]-obj[1][2]*obj[2][4]) + obj[4][2]*(obj[1][4]*obj[2][1] - obj[1][1]*obj[2][4]) - obj[4][4]*(obj[1][2]*obj[2][1] - obj[1][1]*obj[2][2]),
									    obj[3][1]*(obj[1][4]*obj[2][2]-obj[1][2]*obj[2][4]) - obj[3][2]*(obj[1][4]*obj[2][1] - obj[1][1]*obj[2][4]) + obj[3][4]*(obj[1][2]*obj[2][1] - obj[1][1]*obj[2][2])},
										
									  { obj[4][1]*(obj[2][3]*obj[3][2]-obj[2][2]*obj[3][3]) - obj[4][2]*(obj[2][3]*obj[3][1] - obj[2][1]*obj[3][3]) + obj[4][3]*(obj[2][2]*obj[3][1] - obj[2][1]*obj[3][2]),
									   -obj[4][1]*(obj[1][3]*obj[3][2]-obj[1][2]*obj[3][3]) + obj[4][2]*(obj[1][3]*obj[3][1] - obj[1][1]*obj[3][3]) - obj[4][3]*(obj[1][2]*obj[3][1] - obj[1][1]*obj[3][2]),
									    obj[4][1]*(obj[1][3]*obj[2][2]-obj[1][2]*obj[2][3]) - obj[4][2]*(obj[1][3]*obj[2][1] - obj[1][1]*obj[2][3]) + obj[4][3]*(obj[1][2]*obj[2][1] - obj[1][1]*obj[2][2]),
									   -obj[3][1]*(obj[1][3]*obj[2][2]-obj[1][2]*obj[2][3]) + obj[3][2]*(obj[1][3]*obj[2][1] - obj[1][1]*obj[2][3]) - obj[3][3]*(obj[1][2]*obj[2][1] - obj[1][1]*obj[2][2])});
	elseif(key == "transpose") then
		return vectr.mat4({obj[1][1],obj[2][1],obj[3][1],obj[4][1]},{obj[1][2],obj[2][2],obj[3][2],obj[4][2]},{obj[1][3],obj[2][3],obj[3][3],obj[4][3]},{obj[1][4],obj[2][4],obj[3][4],obj[4][4]});
	elseif(key == "_type") then
		return "mat4";
	else 
		return nil;
	end
end

function mat4_mt.typecheck(a,b)
	if(type(a) == "number") then
		return "number";
	elseif(b and a._type ~= nil and a._type == "vector4") then
		return "vector4";
	elseif(a._type ~= nil and a._type == "mat4") then
		return "mat4";
	else
		error("Calculation cannot be performed on an object of this type.", 2);
	end
end

mat4_mt.__newindex = function(obj,key,val)
	if(key == "det") then
		error("Cannot set the determinant of a matrix.",2)
	elseif(key == "trace") then
		error("Cannot set the trace of a matrix.",2)
	elseif(key == "inverse") then
		error("Cannot set the inverse of a matrix.",2)
	elseif(key == "transpose") then
		error("Cannot set the transpose of a matrix.",2)
	elseif(key == "_type") then
		error("Cannot set the type of an object.",2)
	else
		error("Field "..key.." does not exist in the mat4 data structure.",2);
	end
end
mat4_mt.__add = function(a,b)
	local ta = mat4_mt.typecheck(a,false);
	local tb = mat4_mt.typecheck(b,false);
	if(ta == "number") then
		if(tb == "number") then
			return a+b;
		else
			local t = vectr.mat4(b);
			for i=1,4,1 do
				for j=1,4,1 do
					t[i][j] = t[i][j]+a;
				end
			end
			return t;
		end
	elseif(tb == "number") then
		local t = vectr.mat4(a);
		for i=1,4,1 do
			for j=1,4,1 do
				t[i][j] = t[i][j]+b;
			end
		end
		return t;
	else
		local t = vectr.mat4(a);
		for i=1,4,1 do
			for j=1,4,1 do
				t[i][j] = t[i][j]+b[i][j];
			end
		end
		return t;
	end
end
												
mat4_mt.__sub = function(a,b)
	local ta = mat4_mt.typecheck(a,false);
	local tb = mat4_mt.typecheck(b,false);
	if(ta == "number") then
		if(tb == "number") then
			return a-b;
		else
			local t = vectr.mat4(b);
			for i=1,4,1 do
				for j=1,4,1 do
					t[i][j] = a-t[i][j];
				end
			end
			return t;
		end
	elseif(tb == "number") then
		local t = vectr.mat4(a);
		for i=1,4,1 do
			for j=1,4,1 do
				t[i][j] = t[i][j]-b;
			end
		end
		return t;
	else
		local t = vectr.mat4(a);
		for i=1,4,1 do
			for j=1,4,1 do
				t[i][j] = t[i][j]-b[i][j];
			end
		end
		return t;
	end
end
												
mat4_mt.__unm = function(a)
		local t = vectr.mat4(a);
		for i=1,4,1 do
			for j=1,4,1 do
				t[i][j] = -t[i][j];
			end
		end
		return t;
end

mat4_mt.__mul = 
function(a,b)
	local ta = mat4_mt.typecheck(a,true);
	local tb = mat4_mt.typecheck(b,true);
	if(ta == "number") then
		if(tb == "number" or tb == "vector4") then
			return a*b;
		else
			local t = vectr.mat4(b);
			for i=1,4,1 do
				for j=1,4,1 do
					t[i][j] = t[i][j]*a;
				end
			end
			return t;
		end
	elseif(ta == "vector4") then
		if(tb == "mat4") then
			error("Invalid matrix multiplication.",2);
		else
			return a*b;
		end
	elseif(ta == "mat4") then
		if(tb == "number") then
			local t = vectr.mat4(a);
			for i=1,4,1 do
				for j=1,4,1 do
					t[i][j] = t[i][j]*b;
				end
			end
			return t;
		elseif(tb ==  "vector4") then
							local t = {0,0,0,0};
							local v = {b.x,b.y,b.z,b.w};
							for i=1,4,1 do
								for j=1,4,1 do
									t[i] = t[i] + v[j]*a[i][j];
								end
							end
							return vectr.v4(t[1],t[2],t[3],t[4])
		elseif(b._type ==  "mat4") then
							local t = vectr.mat4({0,0,0,0},{0,0,0,0},{0,0,0,0})
							for i=1,4,1 do
								for j=1,4,1 do
									for k=1,4,1 do
										t[i][j] = t[i][j] + a[i][k]*b[k][j];
									end
								end
							end
							return t;
		else
			error("Invalid matrix multiplication.",2);
		end
	else
		error("Invalid matrix multiplication.",2);
	end
end
	
mat4_mt.__div =
function(a,b)
	local ta = mat4_mt.typecheck(a,true);
	local tb = mat4_mt.typecheck(b,true);
	if(tb == "mat4") then
		if(ta ~= "vector4") then
			return a*b.inverse;
		else
			error("Invalid matrix operation.",2);
		end
	elseif(tb == "vector4") then
		if(ta == "vector4" or ta == "number") then
			return a/b;
		else
			error("Invalid matrix operation.",2);
		end
	else
		if(ta == "vector4" or ta == "number") then
			return a/b;
		else
			local t = vectr.mat4(a);
			for i=1,4,1 do
				for j=1,4,1 do
					t[i][j] = t[i][j]/b;
				end
			end
			return t;
		end
	end
end

mat4_mt.__eq = function(a,b)
	for i=1,4,1 do
		for j=1,4,1 do
			if(a[i][j] ~= b[i][j]) then
				return false;
			end
		end
	end
	return true;
end
mat4_mt.__tostring = function(obj) return "(("..tostring(obj[1][1])..", "..tostring(obj[1][2])..", "..tostring(obj[1][3])..", "..tostring(obj[1][4]).."), ("..
												tostring(obj[2][1])..", "..tostring(obj[2][2])..", "..tostring(obj[2][3])..", "..tostring(obj[2][4]).."), ("..
												tostring(obj[3][1])..", "..tostring(obj[3][2])..", "..tostring(obj[3][3])..", "..tostring(obj[3][4]).."), ("..
												tostring(obj[4][1])..", "..tostring(obj[4][2])..", "..tostring(obj[4][3])..", "..tostring(obj[4][4]).."))"; end
mat4_mt.__metatable = mthide;
mat4_mt.__len = function(a) return 3; end

--CONSTRUCTOR
function vectr.mat4(row1,row2,row3, row4)
	if(row1._type ~= nil and row1._type == "mat4") then
		row4 = row1[4]
		row3 = row1[3];
		row2 = row1[2];
		row1 = row1[1];
	elseif(row1._type ~= nil and row1._type ~= "mat4") then
		error("Invalid matrix definition - invalid arguments.",2)
	elseif(#row1 ~= 4 or row2 == nil or #row2 ~= 4 or row3 == nil or #row3 ~= 4 or row4 == nil or #row4 ~= 4) then
		error("Invalid matrix definition - wrong number of matrix elements.",2)
	end
	local m = {row1,row2,row3,row4}
	m.tomat2 = function(a)
		return vectr.mat2({a[1][1],a[1][2]},{a[2][1],a[2][2]});
	end
	
	m.tomat3 = function(a)
		return vectr.mat3({a[1][1],a[1][2],a[1][3]},{a[2][1],a[2][2],a[2][3]},{a[3][1],a[3][2],a[3][3]});
	end
	
	setmetatable(m,mat4_mt)
	return m;
end

end

end

function vectr.lerp(a,b,t)
		return b*t + a*(1-t);
end

local function viter(state,n)
	if(string.sub(state._type,1,6) == "vector") then
		if(n == 0) then
			n = "x"	
		elseif(n == "x") then
			n = "y";
		elseif(n == "y") then
			n = "z"
		elseif(n == "z") then
			n = "w"
		elseif(n == "w") then
			n = nil;
		end
		if(state[n] ~= nil) then
			return n,state[n];
		end
	elseif(string.sub(state._type,1,3) == "mat") then
		if(n == 0) then
			n = {1,1};
		elseif(n[2] < #state) then
			n[2] = n[2] + 1;
		elseif(n[1] < #state) then
			n[1] = n[1] + 1;
			n[2] = 1;
		else
			n = nil;
		end
		if(n ~= nil) then
			return n,state[n[1]][n[2]];
		end
	end
end

function vectr.pairs(a)
	return viter,a,0;
end

local g_mt = {};
function g_mt.__index(obj,key)
	if(key == "zero2") then
		return vectr.v2(0,0);
	elseif(key == "up2") then
		return vectr.v2(0,1);
	elseif(key == "right2") then
		return vectr.v2(1,0);
	elseif(key == "zero3") then
		return vectr.v3(0,0,0);
	elseif(key == "forward3") then
		return vectr.v3(0,0,1);
	elseif(key == "up3") then
		return vectr.v3(0,1,0);
	elseif(key == "right3") then
		return vectr.v3(1,0,0);
	elseif(key == "zero4") then
		return vectr.v4(0,0,0,0);
	elseif(key == "w4") then
		return vectr.v4(0,0,0,1);
	elseif(key == "forward4") then
		return vectr.v4(0,0,1,0);
	elseif(key == "up4") then
		return vectr.v4(0,1,0,0);
	elseif(key == "right4") then
		return vectr.v4(1,0,0,0);
	elseif(key == "empty2") then
		return vectr.mat2({0,0},{0,0});
	elseif(key == "id2") then
		return vectr.mat2({1,0},{0,1});
	elseif(key == "empty3") then
		return vectr.mat3({0,0,0},{0,0,0},{0,0,0});
	elseif(key == "id3") then
		return vectr.mat3({1,0,0},{0,1,0},{0,0,1});
	elseif(key == "empty4") then
		return vectr.mat4({0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0});
	elseif(key == "id4") then
		return vectr.mat4({1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1});
	elseif(key == "_ver") then
		return version;
	end
end

function g_mt.__newindex(obj,key,val)
	error("Cannot access a read-only object.",2)
end

setmetatable(vectr,g_mt);

return vectr;