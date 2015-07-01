--colliders.lua 
--v2.0.1
--Created by Hoeloe, 2015
local colliders = {}

local TYPE_PLAYER = 1;
local TYPE_NPC = 2;
local TYPE_BLOCK = 3;
local TYPE_ANIM = 4;
local TYPE_BOX = 5;
local TYPE_CIRCLE = 6;
local TYPE_POINT = 7;
local TYPE_POLY = 8;
local TYPE_TRI = 9;

function colliders.onInitAPI()
	registerEvent(colliders, "onLoop", "update", false) --Register the loop event
end

local function getScreenBounds()
	local h = (player:mem(0xD0, FIELD_DFLOAT));
	local b = { left = player.x-400+player.speedX, right = player.x+400+player.speedX, top = player.y-260+player.speedY, bottom = player.y+340+player.speedY };
	
    local sect = Section(player.section);
    local bounds = sect.boundary;

	if(b.left < bounds.left - 10) then
		b.left = bounds.left - 10;
		b.right = b.left + 800;
	end
	
	if(b.right > bounds.right - 10) then
		b.right = bounds.right - 10;
		b.left = b.right - 800;
	end
	
	if(b.top < bounds.top+40-h) then
		b.top = bounds.top+40-h;
		b.bottom = b.top + 600;
	end
	
	if(b.bottom > bounds.bottom+40-h) then
		b.bottom = bounds.bottom+40-h;
		b.top = b.bottom - 600;
	end
	
	return b;
	
end

local function worldToScreen(x,y)
			local b = getScreenBounds();
			local x1 = x-b.left;
			local y1 = y-b.top-(player:mem(0xD0, FIELD_DFLOAT))+30;
			return x1,y1;
end

local colliderList = {};

local Vector = {}

function Vector.dot(v1, v2)
	return (v1.x*v2.x) + (v1.y*v2.y);
end

function Vector:normalise()
	return colliders.Vector(self.x/self.magnitude, self.y/self.magnitude);
end

function Vector:normalize()
	return self:normalise();
end

function Vector.add(a,b)
	if(type(a) == 'number') then return Vector.add(colliders.Vector(a,a),b);
	elseif(type(b) == 'number') then return Vector.add(a,colliders.Vector(b,b));
	else return colliders.Vector(a.x+b.x,a.y+b.y);
	end
end

function Vector.subtract(a,b)
	if(type(a) == 'number') then return Vector.subtract(colliders.Vector(a,a),b);
	elseif(type(b) == 'number') then return Vector.subtract(a,colliders.Vector(b,b));
	else return colliders.Vector(a.x-b.x,a.y-b.y);
	end
end

function Vector.mul(v,c)
	if(type(v) == 'number') then return Vector.mul(c,v); 
	else return colliders.Vector(v.x*c,v.y*c);
	end
end

function Vector.div(v,c)
	return colliders.Vector(v.x/c, v.y/c);
end

local Vectormt = {}

function colliders.Vector(x,y)
	local v = {}
	setmetatable(v,Vectormt);
	v.x = x;
	v.y = y;
	v.sqrMagnitude = x*x + y*y;
	v.magnitude = math.sqrt(v.sqrMagnitude);
	return v;
end

Vectormt.__index = Vector;
Vectormt.__eq = function(a,b) return a.x == b.x and a.y == b.y; end
Vectormt.__add = Vector.add;
Vectormt.__sub = Vector.subtract;
Vectormt.__mul = Vector.mul;
Vectormt.__div = Vector.div;
Vectormt.__unm = function(a) return colliders.Vector(-a.x, -a.y) end
Vectormt.__tostring = function(a) return "("..tostring(a.x)..", "..tostring(a.y)..")"; end

local debugList = {}

local function createMeta(t)
	local mt = {}
	mt.__index = function(obj, key)
					if(key=="TYPE") then return t; end;
					if(key=="Debug") then
					return function(object, bool)
								if(debugList[object] ~= nil and not bool) then
									debugList[object] = nil;
								elseif(debugList[object] == nil and bool) then
									debugList[object] = object;
								end
							end;
					end;
				end;
	return mt;
end

function colliders.Box(x,y,width,height)
	local b = { x = x, y = y, width = width, height = height };
	b.Draw = function(obj, c)
		c = c or 0xFF000099;
		Graphics.glSetTextureRGBA(nil, c);
		local pts = {};
		local x1,y1 = worldToScreen(obj.x - 10, obj.y + 10);
		pts[0] = x1; pts[1] = y1;
		pts[2] = x1+obj.width; pts[3] = y1;
		pts[4] = x1; pts[5] = y1+obj.height;
		pts[6] = x1; pts[7] = y1+obj.height;
		pts[8] = x1+obj.width; pts[9] = y1+obj.height;
		pts[10] = x1+obj.width; pts[11] = y1;
		
		Graphics.glDrawTriangles(pts, {}, 6);
		Graphics.glSetTextureRGBA(nil, 0xFFFFFFFF);
	end
	setmetatable(b,createMeta(TYPE_BOX));
	return b;
end

local function circleToTris(obj)
		local x1 = obj.x;
		local y1 = obj.y;
		local pts = {};
		local m = math.ceil(math.sqrt(obj.radius));
		if(m < 1) then m = 1; end
		local s = (math.pi/2)/m;
		local ind = 0;
		local xmult = 1;
		local ymult = -1;
		for n=1,4 do
			local lx = 0;
			local ly = 1;
			for i=1,m do
				local xs = math.cos((math.pi/2)-s*i);
				local ys = math.sin((math.pi/2)-s*i);
				pts[ind] = x1;
				pts[ind+1] = y1;
				pts[ind+2] = x1+xmult*obj.radius*lx;
				pts[ind+3] = y1+ymult*obj.radius*ly;
				pts[ind+4] = x1+xmult*obj.radius*xs;
				pts[ind+5] = y1+ymult*obj.radius*ys;
				ind = ind+6;
				lx = xs;
				ly = ys;
			end
			if xmult == 1 then
				if ymult == -1 then
					ymult = 1;
				elseif ymult == 1 then
					xmult = -1;
				end
			elseif xmult == -1 then
				if ymult == -1 then
					xmult = 1;
				elseif ymult == 1 then
					ymult = -1;
				end
			end
		end
		return pts;
end

function colliders.Circle(x,y,radius)
	local c = { x = x, y = y, radius = radius };
	c.Draw = function(obj, co)
		co = co or 0xFF00FF99;
		Graphics.glSetTextureRGBA(nil, co);
		local x1,y1 = worldToScreen(obj.x - 10, obj.y + 10);
		local x2 = obj.x;
		local y2 = obj.y;
		
		obj.x = x1;
		obj.y = y1;
		
		local pts = circleToTris(obj);
		
		obj.x = x2;
		obj.y = y2;
		
		Graphics.glDrawTriangles(pts, {}, (#pts + 1)/2);
		Graphics.glSetTexture(nil, 0xFFFFFFFF);
	end
	setmetatable(c,createMeta(TYPE_CIRCLE));
	return c;
end

function colliders.Point(x,y)
	local p = { x = x, y = y };
	p.Draw = function(obj, c)
		c = c or 0x0099FF99;
		Graphics.glSetTextureRGBA(nil, c);
		local pts = {};
		local x1,y1 = worldToScreen(obj.x - 10, obj.y + 10);
		pts[0] = x1; pts[1] = y1;
		pts[2] = x1+1; pts[3] = y1;
		pts[4] = x1; pts[5] = y1+1;
		pts[6] = x1; pts[7] = y1+1;
		pts[8] = x1+1; pts[9] = y1+1;
		pts[10] = x1+1; pts[11] = y1;
		
		Graphics.glDrawTriangles(pts, {}, 6);
		Graphics.glSetTextureRGBA(nil, 0xFFFFFFFF);
	end
	setmetatable(p,createMeta(TYPE_POINT));
	return p;
end

function colliders.Tri(x,y,p1,p2,p3)
	local p = { x=x, y=y, v={p1,p2,p3} };
	
	local winding = 0;
	for k,v in ipairs(p.v) do
		if(v[1] == nil or v[2] == nil) then
			error("Invalid polygon definition.", 2);
		end
		
		--Calculate winding order.
		local n = k+1;
		local pr = k-1;
		if(n > 3) then n = 1; end
		if(pr <= 0) then pr = 3; end
		winding = winding + (v[1]+p.v[n][1])*(v[2]-p.v[n][2]);
		
		if(p.minX == nil or v[1] < p.minX) then
			p.minX = v[1];
		end
		if(p.maxX == nil or v[1] > p.maxX) then
			p.maxX = v[1];
		end
		if(p.minY == nil or v[2] < p.minY) then
			p.minY = v[2];
		end
		if(p.maxY == nil or v[2] > p.maxY) then
			p.maxY = v[2];
		end
	end
	
	--If winding order is anticlockwise, triangulation will fail, so reverse vertex list in that case.
	if(winding > 0) then
		local pv = p.v[1];
		p.v[1] = p.v[3];
		p.v[3] = pv;
	end
	
	p.Get = function(obj, index)
		if(index < 1 or index >= 4) then
			error("Invalid triangle index.", 2);
		end
		return { obj.v[index][1]+obj.x, obj.v[index][2]+obj.y };
	end
	
	p.Rotate = function(obj, angle)
		local s = math.sin(math.pi*angle/180);
		local c = math.cos(math.pi*angle/180);
		
		local t = colliders.Tri(obj.x, obj.y, 
		{obj.v[1][1]*c - obj.v[1][2]*s, obj.v[1][1]*s + obj.v[1][2]*c},
		{obj.v[2][1]*c - obj.v[2][2]*s, obj.v[2][1]*s + obj.v[2][2]*c},
		{obj.v[3][1]*c - obj.v[3][2]*s, obj.v[3][1]*s + obj.v[3][2]*c});
		
		obj.v = t.v;
		obj.minX = t.minX;
		obj.maxX = t.maxX;
		obj.minY = t.minY;
		obj.maxY = t.maxY;
	end
	
	p.Translate = function(obj, x, y)
		for i=1,3 do
			obj.v[i] = {obj.v[i][1]+x, obj.v[i][2]+y};
		end
		obj.minX = obj.minX + x;
		obj.maxX = obj.maxX + x;
		obj.minY = obj.minY + y;
		obj.maxY = obj.maxY + y;
	end
	
	p.Scale = function(obj, x, y)
		y = y or x;
		for i=1,3 do
			obj.v[i] = {obj.v[i][1]*x, obj.v[i][2]*y};
		end
		obj.minX = obj.minX*x;
		obj.maxX = obj.maxX*x;
		obj.minY = obj.minY*y;
		obj.maxY = obj.maxY*y;
	end
	
	p.Draw = function(obj, c)
		c = c or 0x00FF0099;
		Graphics.glSetTextureRGBA(nil, c);
		local pts = {};
		local x1,y1 = worldToScreen(obj.x - 10, obj.y + 10);
		pts[0] = x1+obj.v[1][1]; pts[1] = y1+obj.v[1][2];
		pts[2] = x1+obj.v[2][1]; pts[3] = y1+obj.v[2][2];
		pts[4] = x1+obj.v[3][1]; pts[5] = y1+obj.v[3][2];
		
		Graphics.glDrawTriangles(pts, {}, 3);
		Graphics.glSetTextureRGBA(nil, 0xFFFFFFFF);
	end
	
	setmetatable(p,createMeta(TYPE_TRI));
	return p;
end

local function getType(obj)
	if(obj.TYPE ~= nil) then
		return obj.TYPE;
	elseif(obj.powerup ~= nil) then
		return TYPE_PLAYER;
	elseif(obj.slippery ~= nil) then
		return TYPE_BLOCK;
	elseif(obj.timer ~= nil) then
		return TYPE_ANIM;
	elseif(obj.id ~= nil) then
		return TYPE_NPC;
	else
		error("Unknown collider type.", 2);
	end
end

local function convertPoints(p)
	local a;
	local b;
	local c;
	if(p.x == nil) then
		a = p;
		b = colliders.Point(p[1],p[2]);
		c = colliders.Vector(p[1],p[2]);
	elseif(p.magnitude == nil) then
		a = {p.x, p.y};
		b = p;
		c = colliders.Vector(p.x,p.y);
	else
		a = {p.x, p.y};
		b = colliders.Point(p.x,p.y);
		c = p;
	end
	return a,b,c;
end

local function sign(a)
	if(a == 0) then return 0;
	elseif(a > 0) then return 1;
	else return -1 end;
end


local function testBoxCircle(ta,tb)
			local aabb = colliders.Box(tb.x-tb.radius, tb.y-tb.radius, tb.radius*2, tb.radius*2);
			
			if(not colliders.collide(ta,aabb)) then
				return false;
			end
			
			if(tb.x > ta.x and tb.x < ta.x+ta.width and tb.y > ta.y and tb.y < ta.y+ta.height) then return true; end;
			
			local vs = { colliders.Vector(ta.x - tb.x,ta.y - tb.y), 
						 colliders.Vector(ta.x+ta.width - tb.x,ta.y - tb.y),
						 colliders.Vector(ta.x - tb.x,ta.y+ta.height - tb.y),
						 colliders.Vector(ta.x+ta.width - tb.x,ta.y+ta.height - tb.y) };
						 
						 
			for _,v in pairs(vs) do
				if(v.magnitude < tb.radius) then
					return true;
				end
			end
			
			
			return (tb.y > ta.y and tb.y < ta.y+ta.height and (math.abs(tb.x - (ta.x+ta.width)) < tb.radius or math.abs(tb.x - ta.x) < tb.radius)) or 
					(tb.x > ta.x and tb.x < ta.x+ta.width and (math.abs(tb.y - (ta.y+ta.height)) < tb.radius or math.abs(tb.y - ta.y) < tb.radius));
end

local function testBoxPoint(a,b)
			local _,p = convertPoints(b);
			return (p.x > a.x and p.x < a.x+a.width and p.y > a.y and p.y < a.y+a.height);
end

local function testCirclePoint(a,b)
			local _,p = convertPoints(b);
			return (colliders.Vector(a.x-p.x,a.y-p.y).magnitude < a.radius);
end

local function isLeft(a, p0, p1)
	return ((p0[1] - a[1]) * (p1[2] - a[2]) - (p1[1] - a[1]) * (p0[2] - a[2]));
end

local function testTriPoint(a,b)
	local aabb = colliders.Box(a.minX+a.x, a.minY+a.y, a.maxX-a.minX, a.maxY-a.minY);
	
	local p,c = convertPoints(b);
	
	if(not colliders.collide(aabb,c)) then
		return false;
	end
	
	local b1 = (isLeft(a:Get(1), a:Get(2), p) > 0);
	local b2 = (isLeft(a:Get(2), a:Get(3), p) > 0);
	local b3 = (isLeft(a:Get(3), a:Get(1), p) > 0);
	
	return b1 and b2 and b3;
end

local function testTriBox(a,b)
	local aabb = colliders.Box(a.minX+a.x, a.minY+a.y, a.maxX-a.minX, a.maxY-a.minY);
	
	if(not colliders.collide(aabb,b)) then
		return false;
	end
		
	if(testBoxPoint(b,a:Get(1)) or testBoxPoint(b,a:Get(2)) or testBoxPoint(b,a:Get(3))) then
		return true;
	end
		
	if(colliders.raycast(a:Get(1),a:Get(2), b) or colliders.raycast(a:Get(2), a:Get(3), b) or colliders.raycast(a:Get(3), a:Get(1), b)) then
		return true;
	else
		return false;
	end
end

local function testTriTri(a,b)
	local aabb1 = colliders.Box(a.minX+a.x, a.minY+a.y, a.maxX-a.minX, a.maxY-a.minY);
	local aabb2 = colliders.Box(b.minX+b.x, b.minY+b.y, b.maxX-b.minX, b.maxY-b.minY);
	
	if(not colliders.collide(aabb1,aabb2)) then
		return false;
	end
		
	if(testTriPoint(b,a:Get(1)) or testTriPoint(b,a:Get(2)) or testTriPoint(b,a:Get(3))) then
		return true;
	end
		
	if(colliders.raycast(a:Get(1),a:Get(2), b) or colliders.raycast(a:Get(2), a:Get(3), b) or colliders.raycast(a:Get(3), a:Get(1), b)) then
		return true;
	else
		return false;
	end
end

local function testTriCircle(a,b)

	local vs = circleToTris(b);
	
	for i=0,#vs,6 do
		if(testTriTri(colliders.Tri(0,0,{vs[i], vs[i+1]}, {vs[i+2], vs[i+3]}, {vs[i+4],vs[i+5]}), a)) then return true; end;
	end
	return false;
end

local function testTriPoly(a,b)
	local aabb1 = colliders.Box(a.minX+a.x, a.minY+a.y, a.maxX-a.minX, a.maxY-a.minY);
	local aabb2 = colliders.Box(b.minX+b.x, b.minY+b.y, b.maxX-b.minX, b.maxY-b.minY);
	
	if(not colliders.collide(aabb1,aabb2)) then
		return false;
	end
	
	for k,v in ipairs(b.tris) do
		if(testTriTri(a,v)) then
			return true;
		end
	end
	return false;
end

function colliders.Poly(x,y,...)
	local arg = {...};
	
	local p = {x=x, y=y};
	local ts = {};
	
	for _,v in ipairs(arg) do
		if(v[1] == nil or v[2] == nil) then
			error("Invalid polygon definition.", 2);
		end
		if(p.minX == nil or v[1] < p.minX) then
			p.minX = v[1];
		end
		if(p.maxX == nil or v[1] > p.maxX) then
			p.maxX = v[1];
		end
		if(p.minY == nil or v[2] < p.minY) then
			p.minY = v[2];
		end
		if(p.maxY == nil or v[2] > p.maxY) then
			p.maxY = v[2];
		end
	end
	
	local vlist;
	local winding = 0;
	
	--Calculate winding order.
	for k,v in ipairs(arg) do
		local n = k+1;
		local pr = k-1;
		if(n > table.getn(arg)) then n = 1; end
		if(pr <= 0) then pr = table.getn(arg); end
		winding = winding + (v[1]+arg[n][1])*(v[2]-arg[n][2]);
	end
	
	--If winding order is anticlockwise, triangulation will fail, so reverse vertex list in that case.
	if(winding > 0) then
		vlist = {};
		local argn = #arg;
		for k,v in ipairs(arg) do
			vlist[argn - k + 1] = v;
		end
	else 
		vlist = arg;
	end
	
	local trilist = {};
	
	--Repeatedly search for and remove convex triangles (ears) from the polygon (as long as they have no other vertices inside them). When the polygon has only 3 vertices left, stop.
	while(table.getn(vlist) > 3) do
		local count = table.getn(vlist);
		for k,v  in ipairs(vlist) do
			local n = k+1;
			local pr = k-1;
			if(n > table.getn(vlist)) then n = 1; end
			if(pr <= 0) then pr = table.getn(vlist); end
			local lr = v[1] > vlist[pr][1] or v[2] > vlist[pr][2];
			if lr then
				lr = 1;
			else
				lr = -1;
			end
			local left = isLeft(vlist[n], vlist[pr], v);
			if(left > 0) then
				local t = colliders.Tri(0,0,vlist[pr],v,vlist[n]);
				local pointin = false;
				for k2,v2 in ipairs(vlist) do
					if(k2 ~= k and k2 ~= n and k2 ~= pr and testTriPoint(t,v2)) then
						pointin = true;
						break;
					end
				end
				if(not pointin) then
					table.insert(trilist, t);
					table.remove(vlist,k);
					break;
				end
			elseif(left == 0) then
				table.remove(vlist,k);
				break;
			end
		end
		if(table.getn(vlist) == count) then
			error("Polygon is not simple. Please remove any edges that cross over.",2);
		end
	end
	
	--Insert the final triangle to the triangle list.
	table.insert(trilist, colliders.Tri(0,0,vlist[1],vlist[2],vlist[3]));
	
	for k,v in ipairs(trilist) do
		v.x = p.x;
		v.y = p.y;
	end
	
	p.tris = trilist;
	
	p.Rotate = function(obj, angle)
		for k,v in ipairs(obj.tris) do
			v:Rotate(angle);
			if(v.minX < obj.minX) then obj.minX = v.minX; end
			if(v.maxX > obj.maxX) then obj.maxX = v.maxX; end
			if(v.minY < obj.minY) then obj.minY = v.minY; end
			if(v.maxY > obj.maxY) then obj.maxY = v.maxY; end
		end
	end
	
	p.Translate = function(obj, x, y)
		for k,v in ipairs(obj.tris) do
			v:Translate(x,y);
		end
		obj.minX = obj.minX + x;
		obj.maxX = obj.maxX + x;
		obj.minY = obj.minY + y;
		obj.maxY = obj.maxY + y;
	end
	
	p.Scale = function(obj, x, y)
		y = y or x;
		for k,v in ipairs(obj.tris) do
			v:Scale(x,y);
		end
		obj.minX = obj.minX*x;
		obj.maxX = obj.maxX*x;
		obj.minY = obj.minY*y;
		obj.maxY = obj.maxY*y;
	end
	
	p.Draw = function(obj, c)
		c = c or 0x0000FF99;
		for _,v in ipairs(obj.tris) do
			v.x = obj.x;
			v.y = obj.y;
			Graphics.glSetTextureRGBA(nil, c);
			v:Draw(c);
		end
	end
	
	setmetatable(p,createMeta(TYPE_POLY))
	
	return p;
end


local function testPolyPoint(a,b)
	local _,p = convertPoints(b);
	if(p.x < a.x+a.minX or p.x > a.x+a.maxX or p.y < a.y+a.minY or p.y > a.y+a.maxY) then return false end;
	
	for k,v in ipairs(a.tris) do
		v.x = a.x;
		v.y = a.y;
		if(testTriPoint(v,b)) then
			return true;
		end
	end
	return false;
end

local function intersect(a1, a2, b1, b2)
	local l1 = isLeft(b1,b2,a1);
	local l2 = isLeft(b1,b2,a2);
	local l3 = isLeft(a1,a2,b1);
	local l4 = isLeft(a1,a2,b2);
	
	return (sign(l1) ~= sign(l2) and sign(l3) ~= sign(l4));
	
end

local function testPolyPoly(a,b)
	local bba = colliders.Box(a.minX+a.x, a.minY+a.y, a.maxX-a.minX, a.maxY-a.minY);
	local bbb = colliders.Box(b.minX+b.x, b.minY+b.y, b.maxX-b.minX, b.maxY-b.minY);
	if(not colliders.collide(bba,bbb)) then return false; end;
	
	for k,v in ipairs(a.tris) do
		v.x = a.x;
		v.y = a.y;
		for k2,v2 in ipairs(b.tris) do
			v2.x = b.x;
			v2.y = b.y;
			if(testTriTri(v,v2)) then
				return true;
			end
		end
	end
	return false;
	
end

local function testPolyBox(a,b)
	local pb = colliders.Poly(0,0,{b.x,b.y},{b.x+b.width,b.y},{b.x+b.width,b.y+b.height},{b.x,b.y+b.height});
	return testPolyPoly(a,pb);
end

local function testPolyCircle(a,b)

	local vs = circleToTris(b);
	
	for i=0,#vs,6 do
		if(testTriPoly(colliders.Tri(0,0,{vs[i], vs[i+1]}, {vs[i+2], vs[i+3]}, {vs[i+4],vs[i+5]}), a)) then return true; end;
	end
	
	return false;
end

function colliders.raycast(startPoint,endPoint,collider)
	local c = colliders.getHitbox(collider);
	
	local a,sp,v1 = convertPoints(startPoint);
	local b,ep,v2 = convertPoints(endPoint);
	
	if(sp.x == ep.x and sp.y == ep.y) then
		return colliders.collide(sp,c);
	end
	
	if(colliders.collide(sp,c) or colliders.collide(ep,c)) then
		return true;
	end
	
	local t = getType(c);
	--NOTE: t CANNOT be a non-primitive collider here, because getHitbox always returns a primitive collider.
	
	if(t == TYPE_BOX) then
		return (intersect(a,b,{c.x,c.y},{c.x+c.width,c.y})
			or intersect(a,b,{c.x+c.width,c.y},{c.x+c.width,c.y+c.height})
			or intersect(a,b,{c.x+c.width,c.y+c.height},{c.x,c.y+c.height})
			or intersect(a,b,{c.x,c.y+c.height},{c.x,c.y}));
	elseif(t == TYPE_CIRCLE) then
		local l2 = (ep.x-sp.x)*(ep.x-sp.x) + (ep.y-sp.y)*(ep.y-sp.y);
		local p = colliders.Vector(c.x,c.y);
		local t = ((p:subtract(v1)):dot(v2:subtract(v1))):mul(1/l2);
		if(t < 0) then return colliders.collide(c,sp);
		elseif(t > 1) then return colliders.collide(c,ep);
		else
			local pr = v1:add((v2:subtract(v1)):mul(t));
			return (p:subtract(pr)).magnitude <= c.radius;
		end
	elseif(t == TYPE_POINT) then
		local p = colliders.Vector(c.x,c.y);
		return math.abs(v2:subtract(p).magnitude + v1:subtract(p).magnitude  - v2:subtract(v1).magnitude) < 0.001;
	elseif(t == TYPE_POLY) then
			local bb = colliders.Box(c.minX+c.x, c.minY+c.y, c.maxX-c.minX, c.maxY-c.minY);
			if(not colliders.raycast(startPoint, endPoint, bb)) then return false; end
			
			for k,v in ipairs(c.v) do
			local n = c.v[k+1];
			if(n == nil) then
				n = c.v[1];
			end
			n = {n[1]+c.x,n[2]+c.y};
			local m = {v[1]+c.x,v[2]+c.y};
			
			if(intersect(m,n,a,b)) then return true; end
		end
		return false;
	elseif(t == TYPE_TRI) then
			local bb = colliders.Box(c.minX+c.x, c.minY+c.y, c.maxX-c.minX, c.maxY-c.minY);
			if(not colliders.raycast(startPoint, endPoint, bb)) then return false; end
			
			if(intersect(a,b,c:Get(1),c:Get(2)) or intersect(a,b,c:Get(2), c:Get(3)) or intersect(a,b,c:Get(3),c:Get(1))) then return true;
			else return false end;
	end
end

local function getBlockHitbox(id, x, y, wid, hei)
	
	--	 /|
	--	/_|
	--Slope bottomleft to topright floor
	local lrslope_floor = {452,321,365,316,357,358,306,305,302,616,299,340,341,472,480,636,635,326,324,604,600,332}; 
	
	--	|\
	--	|_\
	--Slope topleft to bottomright floor
	local rlslope_floor = {451,319,366,315,359,360,308,307,301,617,300,343,342,474,482,638,637,327,325,601,605,333}; 
	
	--	|-/
	--	|/
	--Slope bottomleft to topright ceil	
	local lrslope_ceil = {318,367,363,364,314,313,310,479,485,328,614,613,334};
		
	--	\-|
	--	 \|
	--Slope topleft to bottomright ceil			
	local rlslope_ceil = {317,368,361,362,312,311,309,476,486,329,77,78,335}; 

	--	[][]
	--
	--Vertical half block
	local uphalf = {289,168,69};

	local isAny = function(tbl, val) 
		for _,v in ipairs(tbl) do 
			if v == val then
				return true;
			end
		end
		return false;
	end

	if isAny(lrslope_floor,id) then --Slope bottomleft to topright floor
		return colliders.Tri(x,y,{0,hei},{wid,0},{wid,hei});
	elseif isAny(rlslope_floor,id) then --Slope topleft to bottomright floor
		return colliders.Tri(x,y,{0,0},{wid,hei},{0,hei});
	elseif isAny(lrslope_ceil,id) then --Slope bottomleft to topright ceil
		return colliders.Tri(x,y,{0,0},{wid,0},{0,hei});
	elseif isAny(rlslope_ceil,id) then --Slope topleft to bottomright ceil
		return colliders.Tri(x,y,{0,0},{wid,0},{wid,hei});
	elseif isAny(uphalf,id) then --Vertical half block
		return colliders.Box(x,y,wid,hei/2);
	else
		return colliders.Box(x,y,wid,hei);
	end
end

function colliders.getAABB(a)
	local ta = getType(a);
	
	if(a.TYPE == TYPE_BOX) then
		return a;
	end
	
	if(ta == TYPE_BLOCK or ta == TYPE_PLAYER or ta == TYPE_NPC or ta == TYPE_ANIM) then
		return colliders.getHitbox(a);
	end
	
	if(ta == TYPE_CIRCLE) then return colliders.Box(a.x - a.radius, a.y - a.radius, 2*a.radius, 2*a.radius) end;
	if(ta == TYPE_POINT) then return colliders.Box(a.x, a.y, 1, 1) end;
	if(ta == TYPE_TRI or ta == TYPE_POLY) then return colliders.Box(a.minX+a.x, a.minY+a.y, a.maxX-a.minX, a.maxY-a.minY) end;
	
	return nil;
end

function colliders.getHitbox(a)
	local ta = getType(a);
	
	if(a.TYPE ~= nil) then
		return a;
	end
	
	if(ta == TYPE_BLOCK) then
			return getBlockHitbox(a.id, a.x, a.y, a.width, a.height);
	elseif(ta == TYPE_PLAYER) then
		return colliders.Box(a.x, a.y, a:mem(0xD8, FIELD_DFLOAT), a:mem(0xD0, FIELD_DFLOAT));
	elseif(ta == TYPE_NPC) then
		if(a:mem (0x12A, FIELD_WORD) <= 0) then
			return nil;
		else
			return colliders.Box(a.x, a.y, a:mem(0x90, FIELD_DFLOAT), a:mem(0x88, FIELD_DFLOAT));
		end
	elseif(ta == TYPE_ANIM) then
		return colliders.Box(a.x, a.y, a.width, a.height);
	end
end

function colliders.getSpeedHitbox(a)
	local ta = getType(a);
	
	if(a.TYPE ~= nil) then
		return a;
	end
		
	local ca = colliders.getHitbox(a);
	
	if(ca == nil) then return ca; end;
	
	if(ta == TYPE_PLAYER or ta == TYPE_NPC or ta == TYPE_ANIM or ta == TYPE_BLOCK) then
		ca.x = ca.x + a.speedX;
		ca.y = ca.y + a.speedY;
	end
	
	return ca;
end

function colliders.bounce(a,b)
	local ta = getType(a);
	
	if(a.TYPE ~= nil) then
		error("Cannot check if an unmoving collider type (Point, Box, Circle, Tri or Poly) is bouncing!",2);
	end
	
	local c = colliders.speedCollide(a,b);
	local ba = colliders.getHitbox(a);
	local bb = colliders.getHitbox(b);
	
	if(ba == nil or bb == nil) then return false, ta == TYPE_PLAYER and a:mem(0x50, FIELD_WORD) == -1; end
	
	return (c and a.speedY >= 0 and ba.y+ba.height/2 < bb.y), ta == TYPE_PLAYER and a:mem(0x50, FIELD_WORD) == -1;
end

function colliders.slash(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is slashing!",2);
	end
	
	if(a:mem(0xF0, FIELD_WORD) ~= 5) then --player must be Link
		return false;
	end
	
	local slash = a:mem(0x14,FIELD_WORD) > 0;
	local hb = colliders.getHitbox(a);
	if(not slash or hb == nil) then return false; end;
	hb.x = hb.x + hb.width*2*a:mem(0x106,FIELD_WORD);
	hb.height = 16;
	hb.width = hb.width - 4;
	hb.y = hb.y + 8;
	
	if(a:mem(0x12E, FIELD_WORD) == -1) then
		hb.y = hb.y + 8;
	end
	
	return colliders.collide(hb,b);
end

function colliders.downSlash(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is down slashing!",2);
	end
	
	if(a:mem(0xF0, FIELD_WORD) ~= 5) then --player must be Link
		return false;
	end
	
	local downslash = a:mem(0x114, FIELD_WORD) == 9; --player sprite index is down slash
	local hb = colliders.getHitbox(a);
	if(not downslash or hb == nil) then return false; end;
	hb.x = hb.x + 4
	hb.height = 20;
	hb.width = hb.width - 4;
	hb.y = hb.y + 48;
	
	return colliders.collide(hb,b);
end

function colliders.tail(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is using the tanooki tail!",2);
	end
	
	local tail = a:mem(0x164,FIELD_WORD) > 0
	local hb = colliders.getHitbox(a);
	if(not tail or hb == nil) then return false; end;
	hb.x = hb.x + (hb.width-4)*a:mem(0x106,FIELD_WORD)+4;
	hb.width = 16;
	hb.y = hb.y+hb.height-20;
	hb.height = 16;
	
	return colliders.collide(hb,b);
end

function colliders.tongue(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is using the tanooki tail!",2);
	end
	
	if(a:mem(0x108, FIELD_WORD) ~= 3) then
		return false;
	end
	
	local tongue = a:mem(0x10C,FIELD_WORD) == 1
	local tOut = a:mem(0xB6,FIELD_WORD) ~= -1
	local hb = colliders.getHitbox(a);
	if(not tongue or hb == nil) then return false; end;
	hb.x = a:mem(0xB0,FIELD_FLOAT) + ((a:mem(0xB4,FIELD_WORD) + 8) * a:mem(0x106,FIELD_WORD)) - 8;
	hb.width = 16;
	hb.height = 16;
	hb.y = hb.y + 34;
	
	if(a:mem(0x12E, FIELD_WORD) == -1) then
		hb.y = hb.y - 26;
	end
	
	return tongue and colliders.collide(hb,b);
end

function colliders.bounceResponse(a, height)
	height = height or jumpheightBounce();
	height = height/1.001;
	local t = getType(a);
	local s = a.speedY;
	if(t == TYPE_PLAYER) then
		if(a:mem(0x50, FIELD_WORD) == -1) then --Spinjumping
			height = height / 1.2;
		end
		if(a:mem(0x50, FIELD_WORD) ~= -1 and a:mem(0x11E, FIELD_WORD) == 0) then --Not spinjumping and holding JUMP
			height = height * 1.2;
			a:mem(0x11C, FIELD_WORD, height);
		elseif(a:mem(0x50, FIELD_WORD) == -1 and a:mem(0x120, FIELD_WORD) == 0) then --Spinjumping and holding SPINJUMP
			height = height
			a:mem(0x11C, FIELD_WORD, height);
		else
			a.speedY = -20/3;
		end
	else
		a.speedY = -height;
	end
	runAnimation(75, a.x, a.y+s*2, 1);
	playSFX(2);
end



function colliders.collide(a,b)
	local ta = getType(a);
	local tb = getType(b);

	if(ta == TYPE_BOX and (b.TYPE ~= nil)) then
		if(tb == TYPE_BOX) then --Check each side of both boxes
			return (a.x < b.x+b.width and a.x+a.width > b.x and a.y < b.y+b.height and a.y + a.height > b.y)
		elseif(tb == TYPE_CIRCLE) then --Check each corner of the box with the circle
			return testBoxCircle(a,b);
		elseif(tb == TYPE_POINT) then --Check if the point is inside the box
			return testBoxPoint(a,b);
		elseif(tb == TYPE_POLY) then --Check poly with box
			return testPolyBox(b,a);
		elseif(tb == TYPE_TRI) then
			return testTriBox(b,a);
		end
	elseif(ta == TYPE_CIRCLE and (b.TYPE ~= nil)) then
		if(tb == TYPE_BOX) then --Check each side of both boxes
			return testBoxCircle(b,a);
		elseif(tb == TYPE_CIRCLE) then --Check each corner of the box with the circle
			return (colliders.Vector(a.x-b.x,a.y-b.y).magnitude < (a.radius + b.radius));
		elseif(tb == TYPE_POINT) then --Check if the point is inside the circle
			return testCirclePoint(a,b);
		elseif(tb == TYPE_POLY) then --Check poly with sampled circle
			return testPolyCircle(b,a);
		elseif(tb == TYPE_TRI) then
			return testTriCircle(b,a);
		end
	elseif(ta == TYPE_POINT and (b.TYPE ~= nil)) then
		if(tb == TYPE_BOX) then --Check each side of the box with the point
			return testBoxPoint(b,a);
		elseif(tb == TYPE_CIRCLE) then --Check the point with the circle
			return testCirclePoint(b,a);
		elseif(tb == TYPE_POINT) then --Check if the points are the same
			return ta.x == tb.x and ta.y == tb.y;
		elseif(tb == TYPE_POLY) then --Check if the point is inside the poly
			return testPolyPoint(b,a);
		elseif(tb == TYPE_TRI) then
			return testTriPoint(b,a);
		end
	elseif(ta == TYPE_POLY and (b.TYPE ~= nil)) then
		if(tb == TYPE_BOX) then --Check poly with box
			return testPolyBox(a,b);
		elseif(tb == TYPE_CIRCLE) then --Check poly with sampled circle
			return testPolyCircle(a,b);
		elseif(tb == TYPE_POINT) then --Check if the point is inside the poly
			return testPolyPoint(a,b);
		elseif(tb == TYPE_POLY) then --Check if both polys are intersecting
			return testPolyPoly(a,b);
		elseif(tb == TYPE_TRI) then
			return testTriPoly(b,a);
		end
	elseif(ta == TYPE_TRI and (b.TYPE ~= nil)) then
		if(tb == TYPE_BOX) then
			return testTriBox(a,b);
		elseif(tb == TYPE_CIRCLE) then
			return testTriCircle(a,b);
		elseif(tb == TYPE_POINT) then
			return testTriPoint(a,b);
		elseif(tb == TYPE_POLY) then
			return testTriPoly(a,b);
		elseif(tb == TYPE_TRI) then
			return testTriTri(a,b);
		end
	end
		local ca;
		local cb;
		
		if(((ta == TYPE_NPC or ta == TYPE_PLAYER) and not a.isValid) or ((tb == TYPE_NPC or tb == TYPE_PLAYER) and not b.isValid)) then
			return false;
		end
		
		ca = colliders.getHitbox(a);
		cb = colliders.getHitbox(b);
		if(ca == nil or cb == nil) then return false; end;
		return colliders.collide(ca,cb);

end

function colliders.speedCollide(a,b)
	local ta = getType(a);
	local tb = getType(b);
	
	if((a.TYPE ~= nil) and (b.TYPE ~= nil)) then
		return colliders.collide(a,b);
	end
	
	if(((ta == TYPE_NPC or ta == TYPE_PLAYER) and not a.isValid) or ((tb == TYPE_NPC or tb == TYPE_PLAYER) and not b.isValid)) then
		return false;
	end
		
	ca = colliders.getSpeedHitbox(a);
	cb = colliders.getSpeedHitbox(b);
	if(ca == nil or cb == nil) then return false; end;
	
	return colliders.collide(ca,cb);
end

function colliders.collideNPCBlock(a,b)
	local idA = type(a) == 'number';
	local idB = type(b) == 'number';
	local c = 0;
	
	if(idA and idB) then
		for _,v in pairs(NPC.get(a,-1)) do
			local hb = colliders.getAABB(v);
			for _,v2 in pairs(Block.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
				if(v2.id == b and colliders.collide(v,v2)) then
					c = c + 1;
				end
			end
		end
	elseif(idA and not idB) then
		local _,count = colliders.collideNPC(b,a);
		c = count;
	elseif(not idA and idB) then
		local _,count = colliders.collideBlock(a,b);
		c = count;
	else
		if(colliders.collide(a,b)) then c = 1; else c = 0; end;
	end
	
	return c>0,c;
end

function colliders.collideBlock(a,b)
	local npcA = type(a) == 'number';
	local npcB = type(b) == 'number';
	local c = 0;
	
	if(npcA and npcB) then
		for _,v in pairs(Block.get(a,-1)) do
			local hb = colliders.getAABB(v);
			for _,v2 in pairs(Block.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
				if(v2.id == b and colliders.collide(v,v2)) then
					c = c + 1;
				end
			end
		end
	elseif(npcA and not npcB) then
		local hb = colliders.getAABB(b);
		for _,v in pairs(Block.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
			if(v.id == a and colliders.collide(v,b)) then
				c = c + 1;
			end
		end
	elseif(not npcA and npcB) then
		local hb = colliders.getAABB(a);
		for _,v in pairs(Block.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
			if(v.id == b and colliders.collide(v,a)) then
				c = c + 1;
			end
		end
	else
		if(colliders.collide(a,b)) then c = 1; else c = 0; end;
	end
	
	return c>0,c;
end

function colliders.collideNPC(a,b)
	local npcA = type(a) == 'number';
	local npcB = type(b) == 'number';
	local c = 0;
	
	if(npcA and npcB) then
		for _,v in pairs(NPC.get(a,-1)) do
			local hb = colliders.getAABB(v);
			for _,v2 in pairs(NPC.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
				if(v2.id == b and colliders.collide(v,v2)) then
					c = c + 1;
				end
			end
		end
	elseif(npcA and not npcB) then
		local hb = colliders.getAABB(b);
		for _,v in pairs(NPC.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
			if(v.id == a and colliders.collide(v,b)) then
				c = c + 1;
			end
		end
	elseif(not npcA and npcB) then
		local hb = colliders.getAABB(a);
		for _,v in pairs(NPC.getIntersecting(hb.x,hb.y,hb.x+hb.width,hb.y+hb.height)) do
			if(v.id == b and colliders.collide(v,a)) then
				c = c + 1;
			end
		end
	else
		if(colliders.collide(a,b)) then c = 1; else c = 0; end;
	end
	
	return c>0,c;
end

function colliders.update()
	for _,v in pairs(debugList) do
		v:Draw();
	end
end


return colliders;