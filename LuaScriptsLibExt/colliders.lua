--colliders.lua 
--v1.0.3
--Created by Hoeloe, 2015
local colliders = {}

local TYPE_PLAYER = 1;
local TYPE_NPC = 2;
local TYPE_BLOCK = 3;
local TYPE_ANIM = 4;
local TYPE_BOX = 5;
local TYPE_CIRCLE = 6;

local Vector = {}
Vector.__index = Vector;
Vector.__eq = function(a,b) return a.x == b.x and a.y == b.y; end
Vector.__add = Vector.add;
Vector.__sub = Vector.subtract;
Vector.__mul = Vector.mul;
Vector.__unm = function(a) return colliders.Vector(-a.x, -a.y) end

function colliders.Vector(x,y)
	local v = {}
	setmetatable(v,Vector);
	v.x = x;
	v.y = y;
	v.magnitude = math.sqrt(x*x + y*y);
	return v;
end

function Vector.dot(v1, v2)
	return (v1.x*v2.x) + (v1.y*v2.y);
end

function Vector:dot(v)
	return Vector.dot(self, v);
end

function Vector:normalise()
	return colliders.Vector(self.x/self.magnitude, self.y/self.magnitude);
end

function Vector:normalize()
	return self:normalise();
end

function Vector.add(a,b)
	return colliders.Vector(a.x+b.x,a.y+b.y);
end

function Vector:add(v)
	return Vector.add(self,v);
end

function Vector.subtract(a,b)
	return colliders.Vector(a.x-b.x,a.y-b.y);
end

function Vector:subtract(v)
	return Vector.subtract(self,v);
end

function Vector.mul(v,c)
	return colliders.Vector(v.x*c,v.y*c);
end

function Vector:mul(c)
	return Vector.mul(self, c);
end

function colliders.Box(x,y,width,height)
	return { x = x, y = y, width = width, height = height };
end

function colliders.Circle(x,y,radius)
	return { x = x, y = y, radius = radius };
end

local function getType(obj)
	if(obj.powerup ~= nil) then
		return TYPE_PLAYER;
	elseif(obj.slippery ~= nil) then
		return TYPE_BLOCK;
	elseif(obj.timer ~= nil) then
		return TYPE_ANIM;
	elseif(obj.id ~= nil) then
		return TYPE_NPC;
	elseif(obj.width ~= nil and obj.height ~= nil) then
		return TYPE_BOX;
	elseif(obj.radius ~= nil) then
		return TYPE_CIRCLE;
	else
		error("Unknown collider type.", 2);
	end
end

local function testBoxCircle(ta,tb)
			local vs = { colliders.Vector(ta.x-tb.x,ta.y-tb.y), 
						 colliders.Vector(ta.x+ta.width-tb.x,ta.y-tb.y),
						 colliders.Vector(ta.x-tb.x,ta.y+ta.height-tb.y),
						 colliders.Vector(ta.x+ta.width-tb.x,ta.y+ta.height-tb.y) }
			for _,v in pairs(vs) do
				if(v.magnitude < tb.radius) then
					return true;
				end
			end
			return false;
end

function colliders.getHitbox(a)
	local ta = getType(a);
	
	if(ta == TYPE_BOX or ta == TYPE_CIRCLE) then
		return a;
	end
	
	if(ta == TYPE_BLOCK) then
			error("Collision with blocks not yet supported.", 2);
	end
	
	if(ta == TYPE_PLAYER) then
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
	
	if(ta == TYPE_BOX or ta == TYPE_CIRCLE) then
		return a;
	end
	
	if(ta == TYPE_BLOCK) then
		error("Collision with blocks not yet supported.", 2);
	end
		
	local ca = colliders.getHitbox(a);
	
	if(ca == nil) then return false; end;
	
	if(ta == TYPE_PLAYER or ta == TYPE_NPC or ta == TYPE_ANIM) then
		ca.x = ca.x + a.speedX;
		ca.y = ca.y + a.speedY;
	end
	
	return ca;
end

function colliders.bounce(a,b)
	local ta = getType(a);
	
	if(ta == TYPE_BOX or ta == TYPE_CIRCLE) then
		error("Cannot check if an unmoving collider type (Box or Circle) is bouncing!",2);
	end
	
	local c = colliders.speedCollide(a,b);
	local ba = colliders.getHitbox(a);
	local bb = colliders.getHitbox(b);
	
	if(ba == nil or bb == nil) then return false, ta == TYPE_PLAYER and a:mem(0x50, FIELD_WORD) == -1; end
	
	return (c and a.speedY > 0 and ba.y+ba.height/2 < bb.y), ta == TYPE_PLAYER and a:mem(0x50, FIELD_WORD) == -1;
end

function colliders.slash(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is slashing!",2);
	end
	
	if(a:mem(0xF0, FIELD_WORD) ~= 5) then
		return false;
	end
	
	local slash = a:mem(0x14,FIELD_WORD) > 0;
	local hb = colliders.getHitbox(a);
	if(hb == nil) then return false; end;
	hb.x = hb.x + hb.width*a:mem(0x106,FIELD_WORD);
	hb.height = 16;
	hb.y = hb.y + 4;
	
	return slash and colliders.collide(hb,b);
end

function colliders.tail(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is using the tanooki tail!",2);
	end
	
	local tail = player:mem(0x164,FIELD_WORD) > 0
	local hb = colliders.getHitbox(a);
	if(hb == nil) then return false; end;
	if(a:mem(0x50, FIELD_WORD) == -1) then --Spinjumping
		hb.x = hb.x - hb.width*a:mem(0x106,FIELD_WORD);
		hb.width = 96;
	else
		hb.x = hb.x + hb.width*a:mem(0x106,FIELD_WORD);
		hb.width = 32;
	end
	hb.y = hb.y+hb.height-32;
	hb.height = 32;
	
	return tail and colliders.collide(hb,b);
end


function colliders.tongue(a,b)
	local ta = getType(a);
	
	if(ta ~= TYPE_PLAYER) then
		error("Cannot check if a non-player is using the tanooki tail!",2);
	end
	
	if(a:mem(0x108, FIELD_WORD) ~= 3) then
		return false;
	end
	
	local tongue = player:mem(0x10C,FIELD_WORD) == 1
	local tOut = player:mem(0xB6,FIELD_WORD) ~= -1
	local hb = colliders.getHitbox(a);
	if(hb == nil) then return false; end;
	hb.x = player:mem(0xB0,FIELD_FLOAT) + (player:mem(0xB4,FIELD_WORD) * player:mem(0x106,FIELD_WORD));
	hb.width = 32;
	hb.height = 48;
	hb.y = hb.y + 28
	return tongue and colliders.collide(hb,b);
end

--[[
function colliders.bounceResponseAndKill(a, b, height)
	colliders.bounceResponse(a,height);
	local t = getType(b);
	if(t == TYPE_PLAYER) then
		b:harm();
	elseif(t == TYPE_NPC) then
		--Insert kill NPC here
	end
end ]]--

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

	if(ta == TYPE_BOX and (tb == TYPE_BOX or tb == TYPE_CIRCLE)) then
		if(tb == TYPE_BOX) then --Check each side of both boxes
			return (a.x < b.x+b.width and a.x+a.width > b.x and a.y < b.y+b.height and a.y + a.height > b.y)
		elseif(tb == TYPE_CIRCLE) then --Check each corner of the box with the circle
			testBoxCircle(a,b);
		end
	elseif(ta == TYPE_CIRCLE and (tb == TYPE_BOX or tb == TYPE_CIRCLE)) then
		if(tb == TYPE_BOX) then --Check each side of both boxes
			testBoxCircle(b,a);
		elseif(tb == TYPE_CIRCLE) then --Check each corner of the box with the circle
			return (colliders.Vector(a.x-b.x,a.y-b.y).magnitude < (a.radius + b.radius));
		end
	else
		local ca;
		local cb;
	
		if(ta == TYPE_BLOCK or tb == TYPE_BLOCK) then
			error("Collision with blocks not yet supported.", 2);
		end
		
		if(((ta == TYPE_NPC or ta == TYPE_PLAYER) and not a.isValid) or ((tb == TYPE_NPC or tb == TYPE_PLAYER) and not b.isValid)) then
			return false;
		end
		
		ca = colliders.getHitbox(a);
		cb = colliders.getHitbox(b);
		if(ca == nil or cb == nil) then return false; end;
		return colliders.collide(ca,cb);
	end

end

function colliders.speedCollide(a,b)
	local ta = getType(a);
	local tb = getType(b);
	
	if((ta == TYPE_BOX or ta == TYPE_CIRCLE) and (tb == TYPE_BOX or tb == TYPE_CIRCLE)) then
		return colliders.collide(a,b);
	end
	
	if(ta == TYPE_BLOCK or tb == TYPE_BLOCK) then
		error("Collision with blocks not yet supported.", 2);
	end
		
	if(((ta == TYPE_NPC or ta == TYPE_PLAYER) and not a.isValid) or ((tb == TYPE_NPC or tb == TYPE_PLAYER) and not b.isValid)) then
		return false;
	end
		
	ca = colliders.getSpeedHitbox(a);
	cb = colliders.getSpeedHitbox(b);
	if(ca == nil or cb == nil) then return false; end;
	return colliders.collide(ca,cb);
end

return colliders;