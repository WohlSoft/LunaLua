-----------------------------------------------------------------------------------------------------
-- *********************************************************************************************** --
-- ** '########:::::'###::::'########::'########:'####::'######::'##:::::::'########::'######:: ** --
-- **  ##.... ##:::'## ##::: ##.... ##:... ##..::. ##::'##... ##: ##::::::: ##.....::'##... ##: ** --
-- **  ##:::: ##::'##:. ##:: ##:::: ##:::: ##::::: ##:: ##:::..:: ##::::::: ##::::::: ##:::..:: ** --
-- **  ########::'##:::. ##: ########::::: ##::::: ##:: ##::::::: ##::::::: ######:::. ######:: ** --
-- **  ##.....::: #########: ##.. ##:::::: ##::::: ##:: ##::::::: ##::::::: ##...:::::..... ##: ** --
-- **  ##:::::::: ##.... ##: ##::. ##::::: ##::::: ##:: ##::: ##: ##::::::: ##:::::::'##::: ##: ** --
-- **  ##:::::::: ##:::: ##: ##:::. ##:::: ##::::'####:. ######:: ########: ########:. ######:: ** --
-- ** ..:::::::::..:::::..::..:::::..:::::..:::::....:::......:::........::........:::......::: ** --
-- *********************************************************************************************** --
---------------------------------------Created by Hoeloe - 2015--------------------------------------
---------------------------------Open-Source Particle Effect Library---------------------------------
----------------------------------------For Super Mario Bros X---------------------------------------
-----------------------------------------------v1.2.5------------------------------------------------
------------------------------------------REQUIRES RNG.lua-------------------------------------------
----------------------------------------REQUIRES EVENTU.lua------------------------------------------
--------------------------------------REQUIRES COLLIDERS.lua-----------------------------------------


local particles = {}
local rng = loadSharedAPI("rng")
local eventu = loadSharedAPI("eventu");
local colliders = loadSharedAPI("colliders");

local function clamp(x,mi,ma)
	return math.min(ma,math.max(mi,x));
end

--GRADIENTS

local function findPrevIndex(t,i)
	while i > 0 and t[i] == nil do
		i = i-1;
	end
	return i;
end

local function findNextIndex(t,i)
	while i < 100 and t[i] == nil do
		i = i+1;
	end
	return i;
end

local colour = {};
colour.__index = colour;
colour.__mul = function(a,b)
				if(type(a) == "number") then
					return particles.Col(a*b.r*255,a*b.g*255,a*b.b*255,a*b.a*255);
				elseif(type(b) == "number") then
					return particles.Col(a.r*b*255,a.g*b*255,a.b*b*255,a.a*b*255);
				else
					return particles.Col(a.r*b.r*255,a.g*b.g*255,a.b*b.b*255,a.a*b.a*255);
				end
			end
colour.__add = function(a,b)
				if(type(a) == "number") then
					return particles.Col((a+b.r)*255,(a+b.g)*255,(a+b.b)*255,(a+b.a)*255);
				elseif(type(b) == "number") then
					return particles.Col((a.r+b)*255,(a.g+b)*255,(a.b+b)*255,(a.a+b)*255);
				else
					return particles.Col((a.r+b.r)*255,(a.g+b.g)*255,(a.b+b.b)*255,(a.a+b.a)*255);
				end
			end

function particles.Col(r,g,b,a)
	r = math.min(r,255);
	g = math.min(g,255);
	b = math.min(b,255);
	if(a ~= nil) then
		a = math.min(a,255);
	end
	local c = {r=r/255,g=g/255,b=b/255,a=a/255 or 1};
	setmetatable(c,colour);
	return c;
end

function particles.ColFromHexRGB(h)
	return particles.Col(math.floor(h/(256*256)),math.floor(h/256)%256,h%256,255);
end
function particles.ColFromHexRGBA(h)
	return particles.Col(math.floor(h/(256*256*256)),math.floor(h/(256*256))%256,math.floor(h/256)%256,h%256);
end

local gradient = {};
gradient.__index = gradient;

function particles.Grad(points, vals)
	local g = {};
	for k,v in ipairs(points) do
		g[math.floor(v*100)] = vals[k];
	end
	setmetatable(g,gradient);
	g.rget = function(x) return g:get(x); end;
	
	return g;
end

function gradient:get(x)
	x = clamp(math.floor(x*100),0,100);
	if(self[x] == nil) then
		local i1 = findPrevIndex(self,x);
		local i2 = findNextIndex(self,x);
		local t1 = self[i1];
		local t2 = self[i2];
		local a;
		if(t1 == nil) then t1 = t2; end
		if(t2 == nil) then t2 = t1; end
		if(t1 == nil) then t1 = 0; t2 = 0; a = 0 end
		
		if(i2 == i1) then 
			a = 0;
		else
			a = (x-i1)/(i2-i1);
		end
		self[x] = t2*a + t1*(1-a);
	end
	return self[x];
end

--FORCEFIELDS

local pointField = {};
pointField.__index = pointField;

local function nullFalloff(x)
	return x*x;
end

function pointField:get(x,y)
	
	local dx = x-self.x;
	local dy = y-self.y;
	local m = math.sqrt(dx*dx + dy*dy);
	local d = m/self.radius;
	if(d > 1) then
		return 0,0;
	end
	local mag = self.falloff(1-d)*self.strength;
	return -dx*mag/m,-dy*mag/m;
end

function pointField:addEmitter(emitter,attract)
	attract = (attract == nil) or attract;
	emitter.forcefields[self] = attract;
end

function pointField:removeEmitter(emitter)
	self.emitters[emitter] = nil;
	emitter.forcefields[self] = nil;
end

function particles.PointField(x,y,radius,strength,falloff)
	local f = {x=x,y=y,radius=radius,strength=strength or 1,falloff=falloff or nullFalloff}
	setmetatable(f,pointField);
	return f;
end

local lineField = {};
lineField.__index = lineField;

function lineField:get(x,y)
	
	local dx = self.x2-self.x1;
	local dy = self.y2-self.y1;
	local l = dx*dx + dy*dy;
	local xd = 0;
	local yd = 0;
	if(l == 0) then
		xd = x-self.x1;
		yd = y-self.y1;
	else
		local t = ((x-self.x1)*(self.x2-self.x1) + (y-self.y1)*(self.y2-self.y1))/l;
		if(t < 0) then
			xd = x-self.x1;
			yd = y-self.y1;
		elseif(t > 1) then
			xd = x-self.x2;
			yd = y-self.y2;
		else
			local px = self.x1 + t*(self.x2-self.x1);
			local py = self.y1 + t*(self.y2-self.y1);
			xd = x-px;
			yd = y-py;
		end
	end
	local m = math.sqrt(xd*xd + yd*yd);
	local d = m/self.radius;
	if(d > 1) then
		return 0,0;
	end
	local mag = self.falloff(1-d)*self.strength;
	return -xd*mag/m,-yd*mag/m;
end


function lineField:addEmitter(emitter,attract)
	attract = (attract == nil) or attract;
	emitter.forcefields[self] = attract;
end

function lineField:removeEmitter(emitter)
	self.emitters[emitter] = nil;
	emitter.forcefields[self] = nil;
end

function particles.LineField(x1,y1,x2,y2,radius,strength,falloff)
	local f = {x1=x1,y1=y1,x2=x2,y2=y2,radius=radius,strength=strength or 1,falloff=falloff or nullFalloff}
	setmetatable(f,lineField);
	return f;
end

--EMITTERS

local emitter = {};
emitter.__index = emitter;


local parseValue;

local function parseRange(val,stack)
	if(val == nil) then
		return nil;
	elseif(tonumber(val) ~= nil) then
		return tonumber(val),tonumber(val);
	elseif(stack[val] ~= nil) then
		return parseRange(stack[val],stack);
	else
		local m1,m2 = string.match(val,"^%s*(.-)%s*:%s*(.-)%s*$");
		m1 = parseValue(m1,stack);
		m2 = parseValue(m2,stack);
		local low = math.min(m1,m2);
		local high = math.max(m1,m2);
		return low,high;
	end
end

local function parseGrad(val)
	if(val == nil) then return default;
	elseif(val.get ~= nil) then return val.rget;
	elseif (type(val) == "function") then return val;
	else return nil end
end

local function nullScale()
	return 1;
end

local function nullColour()
	return particles.Col(255,255,255,255);
end

local function linScale(x)
	return x;
end

local keywords = {"rate","width","height","limit","xOffset","yOffset","texture","col","rotation","scale","lifetime","speedX","speedY","accelX","accelY","speedTime","speedXTime","speedYTime","rotSpeed","scaleTime","framesX","framesY","frameSpeed","rotSpeedTime","boundleft","boundright","boundtop","boundbottom","despawnTime","collision","collisionType","bounceEnergy","speedScale","rotSpeedScale","speedXScale","speedYScale","colTime","blend","space","updateType","targetX","targetY","targetTime","radOffset","maxAngle","colLength","scaleType","ribbonType","scaleLength"}

local function contains(t,val)
	for _,v in ipairs(t) do
		if(v == val) then
			return true;
		end
	end
	return false;
end

local function split(s, x)
	local r = {};
	i = 1;
	for v in string.gmatch(s,"([^"..x.."]+)") do
		r[i] = v;
		i = i + 1;
	end
	return r;
end

local function cloneTable(tbl)
	local c = {}
	for k,v in pairs(tbl) do
		c[k] = v;
	end
	return c;
end

local emitterDefs = {}
local emitterStacks = {}

local function parseList(list,stack)
	local r = split(string.gsub(list, "%s+", ""),",");
	for k,v in ipairs(r) do
		r[k] = parseValue(v,stack);
	end
	return r;
end

local collisionEnum = {none=0,stop=1,bounce=2,kill=3}
local collisionType = {coarse=0,fine=1}
local blendEnum = {alpha=0,additive=1}
local spaceEnum = {world=0}
spaceEnum["local"] = 1;
local updateEnum = {seconds=0,frames=1}

--Ribbon only
local scaleEnum = {center=0,centre=0,top=1,bottom=2}
local ribbonEnum = {disjoint=0,continuous=1}

local function isEnum(s,elist)
	for k,_ in pairs(elist) do
		if(string.match(s,"^%s*"..k.."%s*$")) then
			return true;
		end
	end
	return false;
end

local function trim(s)
	return s:match("^%s*(.-)%s*$");
end

local function parseCol(v,stack)
	if(v ~= nil and v.r ~= nil) then
		return v;
	end
	if(stack[v] ~= nil) then
		return parseCol(stack[v],stack);
	end
	if(type(v) == "number") then
		return particles.ColFromHexRGBA(v);
	end
	local r,g,b,a = string.match(v,"^%s*0x([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])%s*$");
	if(a == nil) then
		r,g,b = string.match(v,"^%s*0x([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])([0-9a-fA-F][0-9a-fA-F])%s*$");
		a = "FF";
	end
	if(r == nil and tonumber(v) ~= nil) then
		return particles.ColFromHexRGBA(v);
	elseif(r ~= nil) then
		return particles.Col(tonumber("0x"..r),tonumber("0x"..g),tonumber("0x"..b),tonumber("0x"..a))
	end
end

function parseValue(l,stack)
	if(type(l) ~= "string") then --Already parsed
		return l;
	elseif(string.match(l,"^%s*0x.-%s*$")) then --Colour
		return parseCol(l,stack);
	elseif(tonumber(l) ~= nil) then --Basic number
		return tonumber(l);
	elseif(stack[l] ~= nil) then --Variable
		return parseValue(stack[l],stack);
	elseif(string.match(l,"^%s*0x.-%s*:%s*0x.-%s*$")) then --Colour Range
			local frst,scnd = string.match(l, "^%s*(0x[0-9a-fA-F]+):(0x[0-9a-fA-F]+)%s*$");
			if(frst == nil) then
				frst = l;
			end
			local a = parseCol(frst,stack);
			if(scnd ~= nil) then
				lst = {a,parseCol(scnd,stack)};
			else
				return a;
			end
	elseif(string.match(l, "^%s*.-%s*:%s*.-%s*$")) then --Numerical Range
		return {parseRange(l,stack)};
	elseif(string.match(l,"^%s*{[^}]*}%s*$")) then --List
		local lst = string.match(l, "^%s*{(.-)}%s*$")
		if(lst ~= nil) then
			local vs = parseList(lst,stack);
			for k,v in ipairs(vs) do
				vs[k] = parseValue(v,stack)
			end
			vs[0] = true; --Is a discrete list
			return vs;
		else
			error("Syntax Error: Invalid list definition: "..tostring(l));
		end
	elseif(string.match(l,"^%s*{.+}%s*,%s*{.+}%s*$")) then --Gradient
		local pts,vals = string.match(l,"^%s*{(.+)}%s*,%s*{(.+)}%s*$");
		pts = parseList(pts,stack);
		for k,v in ipairs(pts) do
			pts[k] = tonumber(v);
			if(pts[k] == nil) then
				error("Invalid gradient definition: "..l,2);
			end
		end
		
		vals = parseList(vals,stack);
		
		return parseGrad(particles.Grad(pts,vals));
	elseif(string.match(l, "^[%w%p%s%c]+%.[%w%p%s%c]+$")) then --Image
		local f = Misc.resolveFile(l);
		if(f == nil) then
			f = Misc.resolveFile("particles/"..l);
		end
		if(f == nil) then
			f = Misc.resolveFile("graphics/particles/"..l);
		end
		return Graphics.loadImage(f);
	elseif(isEnum(l,collisionEnum) or isEnum(l,collisionType) or isEnum(l,blendEnum) or isEnum(l,spaceEnum) or isEnum(l,updateEnum) or --[[Ribbon only]] isEnum(l,ribbonEnum) or isEnum(l,scaleEnum)) then --Enum
		return l;
	else
		error("Syntax Error: Invalid statement: "..tostring(l),2);
	end
end

local function parseLine(l,stack,r)
	l = string.match(l, "^(.-)%-%-.*$") or l;
	if(l == nil) then return end;
	local k,v = string.match(l, "^%s*(.-)%s*=%s*(.-)%s*$");
	if(v == nil) then return end;
	if(contains(keywords,k)) then
		r[k] = parseValue(v,stack);
		stack[k] = r[k];
	else
		stack[k] = parseValue(v,stack);
	end
end

function particles.loadEmitterProperties(path)	
	local f;
	if(not pcall(function() f = io.open(path,"r") end)) then
		error("Could not open particle descriptor file "..tostring(path),3);
	else
		local stack = {}
		local result = {};
		for l in f:lines() do
			parseLine(l,stack,result);
		end
		f:close();
		return result,stack;
	end
end

local function parseFile(p)
		if(emitterDefs[p] == nil) then
			emitterDefs[p],emitterStacks[p] = particles.loadEmitterProperties(p);
		end
		return cloneTable(emitterDefs[p]),emitterStacks[p];
end

local paramDefaultMaps = {}
do --define Default maps
paramDefaultMaps["rate"] = 1;
paramDefaultMaps["xOffset"] = 0;
paramDefaultMaps["yOffset"] = 0;
paramDefaultMaps["framesX"] = 1;
paramDefaultMaps["framesY"] = 1;
paramDefaultMaps["frameSpeed"] = 8;
paramDefaultMaps["collision"] = collisionEnum.none;
paramDefaultMaps["collisionType"] = collisionType.fine;
paramDefaultMaps["bounceEnergy"] = 0.5;
paramDefaultMaps["texture"] = nil;
paramDefaultMaps["col"] = particles.Col(255,255,255,255);
paramDefaultMaps["scale"] = 1;
paramDefaultMaps["lifetime"] = 1;
paramDefaultMaps["speedX"] = 0;
paramDefaultMaps["speedY"] = 0;
paramDefaultMaps["accelX"] = 0;
paramDefaultMaps["accelY"] = 0;
paramDefaultMaps["rotation"] = 0;
paramDefaultMaps["rotSpeed"] = 0;
paramDefaultMaps["limit"] = 500;
paramDefaultMaps["despawnTime"] = 180;
paramDefaultMaps["targetX"] = nil;
paramDefaultMaps["targetY"] = nil;
paramDefaultMaps["radOffset"] = 0;
paramDefaultMaps["blend"] = blendEnum.alpha;
paramDefaultMaps["space"] = spaceEnum.world;
paramDefaultMaps["updateType"] = updateEnum.seconds;
paramDefaultMaps["width"] = 
	function(p)
		if(p.texture == nil) then
			return 32;
		else
			return p.texture.width/p.framesX;
		end
	end;
paramDefaultMaps["height"] = 
	function(p)
			if(p.texture == nil) then
				return 32;
			else
				return p.texture.height/p.framesY;
			end
	end;
paramDefaultMaps["boundleft"] = function(p) return (math.min(p.minX,math.min(p.minX+(p.minSpeedX*p.maxLife), p.minX+(p.minSpeedX+(p.minAccelX*p.maxLife))*p.maxLife))-p.maxWidth*p.maxScale)-p.maxRadius; end;
paramDefaultMaps["boundright"] = function(p) return (math.max(p.maxX,math.max(p.maxX+(p.maxSpeedX*p.maxLife), p.maxX+(p.maxSpeedX+(p.maxAccelX*p.maxLife))*p.maxLife))+p.maxWidth*p.maxScale)+p.maxRadius; end;
paramDefaultMaps["boundtop"] = function(p) return (math.min(p.minY,math.min(p.minY+(p.minSpeedY*p.maxLife), p.minY+(p.minSpeedY+(p.minAccelY*p.maxLife))*p.maxLife))-p.maxHeight*p.maxScale)-p.maxRadius; end;
paramDefaultMaps["boundbottom"] = function(p) return (math.max(p.maxY,math.max(p.maxY+(p.maxSpeedY*p.maxLife), p.maxY+(p.maxSpeedY+(p.maxAccelY*p.maxLife))*p.maxLife))+p.maxHeight*p.maxScale)+p.maxRadius; end;

--Ribbon only

paramDefaultMaps["maxAngle"] = 60;
paramDefaultMaps["scaleType"] = scaleEnum.centre;
paramDefaultMaps["ribbonType"] = ribbonEnum.continuous;
end
	
local paramRangeMaps = {}
do --define Range maps
paramRangeMaps["rate"] = {"minRate","maxRate"}
paramRangeMaps["bounceEnergy"] = {"minBounce", "maxBounce"}
paramRangeMaps["width"] = {"minWidth", "maxWidth"}
paramRangeMaps["height"] = {"minHeight", "maxHeight"}
paramRangeMaps["scale"] = {"minScale", "maxScale"}
paramRangeMaps["lifetime"] = {"minLife", "maxLife"}
paramRangeMaps["speedX"] = {"minSpeedX", "maxSpeedX"}
paramRangeMaps["speedY"] = {"minSpeedY", "maxSpeedY"}
paramRangeMaps["accelX"] = {"minAccelX", "maxAccelX"}
paramRangeMaps["accelY"] = {"minAccelY", "maxAccelY"}
paramRangeMaps["rotation"] = {"minRot", "maxRot"}
paramRangeMaps["rotSpeed"] = {"minRotSpd", "maxRotSpd"}
paramRangeMaps["frameSpeed"] = {"minFrameSpd", "maxFrameSpd"}
paramRangeMaps["xOffset"] = {"minX", "maxX"}
paramRangeMaps["yOffset"] = {"minY", "maxY"}
paramRangeMaps["targetX"] = {"minTargetX","maxTargetX"}
paramRangeMaps["targetY"] = {"minTargetY","maxTargetY"}
paramRangeMaps["radOffset"] = {"minRadius","maxRadius"}
end
local paramGradMaps = {}
do --define Gradient maps
paramGradMaps["scaleTime"] = nullScale;
paramGradMaps["speedXTime"] = nullScale;
paramGradMaps["speedYTime"] = nullScale;
paramGradMaps["speedTime"] = nullScale;
paramGradMaps["rotSpeedTime"] = nullScale;
paramGradMaps["targetTime"] = linScale;
paramGradMaps["speedScale"] = nullScale;
paramGradMaps["speedXScale"] = nullScale;
paramGradMaps["speedYScale"] = nullScale;
paramGradMaps["rotSpeedScale"] = nullScale;
paramGradMaps["colTime"] = nullColour;
paramGradMaps["colScale"] = nullColour;

--Ribbon only
paramGradMaps["colLength"] = nullColour;
paramGradMaps["scaleLength"] = nullScale;
end

local paramColMaps = {}
do --define Colour maps
paramColMaps["col"] = {"minCol","maxCol"};
end

local paramEnumMaps = {}
do --define Enum maps
paramEnumMaps["collision"] = collisionEnum;
paramEnumMaps["collisionType"] = collisionType;
paramEnumMaps["blend"] = blendEnum;
paramEnumMaps["space"] = spaceEnum;
paramEnumMaps["updateType"] = updateEnum;

--Ribbon only
paramEnumMaps["scaleType"] = scaleEnum;
paramEnumMaps["ribbonType"] = ribbonEnum;
end



local nullMeta = {}
nullMeta.__index = function(tb,k) error("Cannot access a destroyed particle system.",2) end
nullMeta.__newindex = function(tb,k,v) error("Cannot access a destroyed particle system.",2) end

do --Emitter Functions

function particles.Emitter(x,y,source,prewarm)
	local p;
	local s;
	
	if(type(source) == "table") then
		p = source;
		s = {};
	else
		p,s = parseFile(source);
	end
	setmetatable(p,emitter);
	
	p.x = x;
	p.y = y;
	p.prewarm = prewarm or 0;
	p.prewarm = tonumber(p.prewarm)
	p.initPrewarm = p.prewarm;
	p.hasPrewarmed = false;
	
	p.forcefields = {};
	
	p.enabled = true;
	p:setParam("despawnTime",p.despawnTime,s)
	p.despawnCount = p.despawnTime;
	
	p:setParam("col",p.col,s)
	p:setParam("limit",p.limit,s)
	p:setParam("bounceEnergy",p.bounceEnergy,s);
	p:setParam("rate",p.rate,s);
	p:setParam("xOffset",p.xOffset,s);
	p:setParam("yOffset",p.yOffset,s);
	p:setParam("framesX",p.framesX,s);
	p:setParam("framesY",p.framesY,s);
	p:setParam("radOffset",p.radOffset,s);
	
	p:setParam("width",p.width,s);
	p:setParam("height",p.height,s);
	p:setParam("scale",p.scale,s);
	
	p:setParam("lifetime",p.lifetime,s);
	
	p:setParam("speedX",p.speedX,s);
	p:setParam("speedY",p.speedY,s);
	
	p:setParam("accelX",p.accelX,s);
	p:setParam("accelY",p.accelY,s);
	
	p:setParam("targetX",p.targetX,s);
	p:setParam("targetY",p.targetY,s);
	
	p:setParam("rotation",p.rotation,s);
	
	p:setParam("rotSpeed",p.rotSpeed,s);
	
	p:setParam("frameSpeed",p.frameSpeed,s);
	
	p:setParam("scaleTime",p.scaleTime,s);
	
	p:setParam("speedXTime",p.speedXTime,s);
	p:setParam("speedYTime",p.speedYTime,s);
	p:setParam("speedTime",p.speedTime,s);
	
	if(p.targetX ~= nil) then
		p:setParam("speedTime",nil);
		p:setParam("speedXTime",nil);
		p:setParam("speedX",nil);
		p:setParam("accelX",nil);
	end
	
	if(p.targetY ~= nil) then
		p:setParam("speedTime",nil);
		p:setParam("speedYTime",nil);
		p:setParam("speedY",nil);
		p:setParam("accelY",nil);
	end
	
	
	p:setParam("targetTime",p.targetTime,s);
	
	p:setParam("rotSpeedTime",p.rotSpeedTime,s);
	
	p:setParam("colTime",p.colTime,s);
	p:setParam("colScale",p.colScale,s);
	
	p:setParam("speedScale",p.speedScale,s);
	p:setParam("speedXScale",p.speedXScale,s);
	p:setParam("speedYScale",p.speedYScale,s);
	p:setParam("rotSpeedScale",p.rotSpeedScale,s);
	
	p:setParam("blend",p.blend,s);
	p:setParam("space",p.space,s);
	p:setParam("updateType",p.updateType,s);
	
	p:setParam("boundleft",p.boundleft,s);
	p:setParam("boundright",p.boundright,s);
	p:setParam("boundtop",p.boundtop,s);
	p:setParam("boundbottom",p.boundbottom,s);
	
	p:setParam("collision",p.collision,s);
	p:setParam("collisionType",p.collisionType,s);
	p.particles = {};
	return p;
end

function emitter:getParamDefault(name)
	if(paramDefaultMaps[name] ~= nil) then
		if(type(paramDefaultMaps[name]) == "function") then
			return paramDefaultMaps[name](self);
		else
			return paramDefaultMaps[name];
		end
	elseif(paramGradMaps[name] ~= nil) then
		return paramGradMaps[name];
	else
		return nil;
	end
end

function emitter:setParam(name,value,stack)
	stack = stack or {};
	if(paramRangeMaps[name] ~= nil) then
		local vals = parseValue(value,stack);
		if(vals == nil) then
			self[name] = self:getParamDefault(name);
			self[paramRangeMaps[name][1]] = self[name];
			self[paramRangeMaps[name][2]] = self[name];
		else
			if(type(vals) == "number") then
				self[name] = vals;
				self[paramRangeMaps[name][1]] = vals;
				self[paramRangeMaps[name][2]] = vals;
			elseif(type(vals) == "table") then
				self[name] = vals[1];
				self[paramRangeMaps[name][1]] = vals[1];
				self[paramRangeMaps[name][2]] = vals[2];
			else
				error("Syntax Error: Invalid numerical range specified: "..tostring(value),2)
			end
			if(name == "frameSpeed") then
				self[name] = math.floor(self[name]);
				self[paramRangeMaps[name][1]] = self[name];
				self[paramRangeMaps[name][2]] = math.ceil(self[paramRangeMaps[name][2]]);
			end
		end
	elseif(paramGradMaps[name] ~= nil) then
		self[name] = parseValue(value,stack);
		if(self[name] == nil) then
			self[name] = self:getParamDefault(name);
		end
	elseif(paramColMaps[name] ~= nil) then
		self[name] = parseValue(value,stack);
		self[paramColMaps[name][1]] = self[name];
		self[paramColMaps[name][2]] = self[name];
		if(self[name] == nil) then
			self[name] = self:getParamDefault(name);
			self[paramColMaps[name][1]] = self[name];
			self[paramColMaps[name][2]] = self[name];
		end
		if(self[name][0] == nil) then --is not a discrete list
			if(self[name].r == nil) then --is a range list
				self[paramColMaps[name][1]] = self[name][1];
				self[paramColMaps[name][2]] = self[name][2];
				self[name] = self[name][1];
			end
		else
			self[name][0] = nil;
		end
	elseif(paramEnumMaps[name] ~= nil) then
		self[name] = paramEnumMaps[name][value] or self:getParamDefault(name);
	else
		self[name] = tonumber(value) or self:getParamDefault(name);
	end
end

function emitter:FlipX()
	self:setParam("speedX",tostring(-self.maxSpeedX)..":"..tostring(-self.minSpeedX));
	self:setParam("accelX",tostring(-self.maxAccelX)..":"..tostring(-self.minAccelX));

	local r = self.boundright;
	
	self:setParam("boundright",-self.boundleft);
	self:setParam("boundleft",-r);
end

function emitter:FlipY()
	self:setParam("speedY",tostring(-self.maxSpeedY)..":"..tostring(-self.minSpeedY));
	self:setParam("accelY",tostring(-self.maxAccelY)..":"..tostring(-self.minAccelY));
	
	local t = self.boundtop;
	self:setParam("boundtop",-self.boundbottom);
	self:setParam("boundbottom",-t);
end

function emitter:AttachToCamera(camera, snap)
	if(snap == nil) then snap = true; end
	local ox = 0;
	local oy = 0;
	if(not snap) then
		ox = self.x-(camera.x+camera.width/2)
		oy = self.y-(camera.y+camera.height/2)
	end
	self.parent = {obj = camera, __type = "Camera", offsetX = ox, offsetY = oy};
end

function emitter:Attach(object, snap, flipOnDirectionChange, startDirection)
	local typ;
	if(object.slippery ~= nil) then
		typ = "Block";
	elseif(object.subTimer ~= nil) then
		typ = "Animation";
	elseif(object.powerup ~= nil) then
		typ = "Player";
	elseif(object.direction ~= nil) then
		typ = "NPC";
	else
		error("Emitter could not be attached to given object: "..tostring(object),2);
	end
	
	if(snap == nil) then snap = true; end
	local ox = 0;
	local oy = 0;
	if(not snap) then
		ox = self.x-(object.x+object.width/2)
		oy = self.y-(object.y+object.height/2)
	end
	
	if(flipOnDirectionChange == nil) then flipOnDirectionChange = false; end
	if(startDirection == nil) then 
		startDirection = 1; 
		if(typ == "Player") then
			startDirection = object:mem(0x106,FIELD_WORD);
		elseif(typ == "NPC") then
			startDirection = object.direction;	
		end
	end
	
	local lastDir;
	if(flipOnDirectionChange) then
		if(typ == "Player") then
			if(object:mem(0x106,FIELD_WORD) ~= startDirection) then
				self:FlipX();
			end
			lastDir = object:mem(0x106,FIELD_WORD);
		elseif(typ == "NPC") then
			if(object.direction ~= startDirection) then
				self:FlipX();
				ox = -ox;
			end
			lastDir = object.direction;
		end
	end
	
	self.parent = {obj = object, __type = typ, offsetX = ox, offsetY = oy, lastDirection = lastDir, shouldFlip = flipOnDirectionChange}
end

function emitter:GetOffset()
	if(self.parent == nil) then
		return 0,0;
	else
		return self.parent.offsetX, self.parent.offsetY;
	end
end

function emitter:SetOffset(x,y)
	if(self.parent == nil) then
		return;
	else
		self.parent.offsetX = x;
		self.parent.offsetY = y;
	end
end

function emitter:Detach()
	self.parent = nil;
end

function emitter:KillParticles()
	for k,_ in ipairs(self.particles) do
		self.particles[k] = nil;
	end
end

function emitter:Destroy()
	for k,_ in pairs(self) do
		self[k] = nil;
	end
	setmetatable(self,nullMeta);
end

function emitter:setPrewarm(value)
	self.initPrewarm = value;
	self.prewarm = self.initPrewarm;
end

function emitter:Count()
	return #self.particles;
end

function emitter:Scale(n)
	self:setParam("scale",tostring(self.minScale*n)..":"..tostring(self.maxScale*n));
	self:setParam("speedX",tostring(self.minSpeedX*n)..":"..tostring(self.maxSpeedX*n));
	self:setParam("speedY",tostring(self.minSpeedY*n)..":"..tostring(self.maxSpeedY*n));
	self:setParam("accelX",tostring(self.minAccelX*n)..":"..tostring(self.maxAccelX*n));
	self:setParam("accelY",tostring(self.minAccelY*n)..":"..tostring(self.maxAccelY*n));
	self:setParam("boundleft",self.boundleft*n);
	self:setParam("boundright",self.boundright*n);
	self:setParam("boundtop",self.boundtop*n);
	self:setParam("boundbottom",self.boundbottom*n);
end

function emitter:Emit(n)
	n = n or 1;
		
	local xoff = 0;
	local yoff = 0;
	if(self.space == spaceEnum.world) then
		xoff = self.x;
		yoff = self.y;
	end
	for i = 1,n do
		if(self.limit > 0 and self:Count() >= self.limit) then break end;
		local p = {};
		
		local radoffx = 0;
		local radoffy = 0;
		if(self.minRadius ~= 0 or self.maxRadius ~= 0) then
			local angle = rng.random(-math.pi,math.pi);
			local m = rng.random(self.minRadius,self.maxRadius);
			radoffx = m*math.cos(angle);
			radoffy = m*math.sin(angle);
		end
		
		p.x = radoffx + xoff + rng.random(self.minX,self.maxX);
		
		p.y = radoffy + yoff + rng.random(self.minY,self.maxY);
		
		if(type(self.col) == "table" and self.col.r == nil) then
			p.col = rng.irandomEntry(self.col);
		else
			local cr = rng.random();
			p.col = particles.Col((self.minCol.r*(1-cr) + self.maxCol.r*cr)*255,(self.minCol.g*(1-cr) + self.maxCol.g*cr)*255,(self.minCol.b*(1-cr) + self.maxCol.b*cr)*255,(self.minCol.a*(1-cr) + self.maxCol.a*cr)*255);
		end
		
		p.ttl = rng.random(self.minLife,self.maxLife);
		p.initTtl = p.ttl;
		
		p.scale = rng.random(self.minScale,self.maxScale);
		p.initScale = p.scale;
		p.width = rng.random(self.minWidth,self.maxWidth);
		p.height = rng.random(self.minHeight,self.maxHeight);
		
		local st = (p.scale-self.minScale)/(self.maxScale-self.minScale);
		
		p.speedX = rng.random(self.minSpeedX,self.maxSpeedX)*self.speedXScale(st)*self.speedScale(st);
		p.speedY = rng.random(self.minSpeedY,self.maxSpeedY)*self.speedYScale(st)*self.speedScale(st);
		
		p.scaleCol = self.colScale(st);
		
		p.initSpeedX = p.speedX;
		p.initSpeedY = p.speedY;
		
		p.accelX = rng.random(self.minAccelX,self.maxAccelX);
		p.accelY = rng.random(self.minAccelY,self.maxAccelY);
		
		p.forceX = 0;
		p.forceY = 0;
		
		if(self.minTargetX ~= nil) then
			p.targetX = xoff + rng.random(self.minTargetX, self.maxTargetX);
			p.initX = p.x;
		end
		if(self.minTargetY ~= nil) then
			p.targetY = yoff + rng.random(self.minTargetY, self.maxTargetY);
			p.initY = p.y;
		end
		
		p.rotation = rng.random(self.minRot,self.maxRot);
		
		p.rotSpeed = rng.random(self.minRotSpd,self.maxRotSpd)*self.rotSpeedScale(st);
		
		p.frame = 0;
		p.maxframes = self.framesX*self.framesY;
		p.frametimer = rng.randomInt(self.minFrameSpd,self.maxFrameSpd);
		p.initFrametimer = p.frametimer;
		
		p.collider = nil;
		if(self.collision ~= collisionEnum.none) then
			p.collider = colliders.Point(p.x,p.y);
			p.bounceEnergy = rng.random(self.minBounce,self.maxBounce);
		end
		
		table.insert(self.particles,p);
	end
end

function emitter:Draw(priority)
	priority = priority or 0.5;
	if(self.tte == nil) then 
		self.tte = 1/rng.random(self.minRate,self.maxRate);
	end
	
	if(self.parent ~= nil) then
		if(self.parent.__type == "Camera" or self.parent.__type == "Animation" or self.parent.__type == "Block" or self.parent.__type == "Player" or self.parent.__type == "NPC") then
			if(self.parent.obj == nil or (self.parent.obj.isValid ~= nil and not self.parent.obj.isValid)) then
				self:Detach();
				self.enabled = false;
			else
				self.x = self.parent.obj.x + self.parent.obj.width/2 + self.parent.offsetX;
				self.y = self.parent.obj.y + self.parent.obj.height/2 + self.parent.offsetY;
				if(self.parent.shouldFlip) then
					if(self.parent.__type == "Player") then
						if(self.parent.obj:mem(0x106,FIELD_WORD) ~= self.parent.lastDirection) then
							self:FlipX();
							self.parent.offsetX = -self.parent.offsetX;
						end
						self.parent.lastDirection = self.parent.obj:mem(0x106,FIELD_WORD);
					elseif(self.parent.__type == "NPC") then
						if(self.parent.obj.direction ~= self.parent.lastDirection) then
							self:FlipX();
							self.parent.offsetX = -self.parent.offsetX;
						end
						self.parent.lastDirection = self.parent.obj.direction;
					end
				end
			end
		end
	end
	
	local cam = Camera.get()[1];
	
	local despawning = cam.x > self.x+self.boundright or cam.x+cam.width < self.x+self.boundleft or cam.y > self.y+self.boundbottom or cam.y+cam.height < self.y+self.boundtop;
	if(despawning) then
		self.despawnCount = self.despawnCount-1;
		if(self.despawnCount <= 0) then
			while #self.particles > 0 do
				table.remove(self.particles,1);
			end
			self.hasPrewarmed = false;
			
			if(self.prewarm <= 0) then 
				self.prewarm = self.initPrewarm;
			end
		end
		return;
	end
	
	self.despawnCount=self.despawnTime;
	local delta = 1;
	if(self.updateType == updateEnum.seconds) then
		delta = eventu.deltaTime;
		if(delta <= 0) then delta = 0.01538461538; end -- Cap deltaTime so it never gets stuck in an infinite loop. Cap is SMBX native framerate (1/65)
	end
		
	
	local tris = {};
	local txs = {};
	local cols = {};
	local i = 0;
	repeat
		local prewarming = not self.hasPrewarmed and self.prewarm > 0;
		if(self.enabled) then
			if(self.tte <= 0) then
				while self.tte <= 0 do
					self:Emit();
					self.tte = self.tte + 1/rng.random(self.minRate,self.maxRate);
				end
				self.tte = 1/rng.random(self.minRate,self.maxRate);
			end
			
			self.tte = self.tte-delta;
		end
		
		local removeQueue = {};
		local ox,oy = -cam.x,-cam.y;
		local fw = 1/self.framesX;
		local fh = 1/self.framesY;
		local hit = self.collision ~= collisionEnum.none and not despawning and #Block.getIntersecting(self.x+self.boundleft,self.y+self.boundtop,self.x+self.boundright,self.y+self.boundbottom) > 0;
		local islocal = self.space == spaceEnum["local"];
		local k = 1;
		while(k <= #self.particles) do
			local v = self.particles[k];
			local removed = false;
			local t = 1 - (v.ttl/v.initTtl);
			v.scale = v.initScale*self.scaleTime(t);
			v.speedX = v.initSpeedX*self.speedXTime(t)*self.speedTime(t);
			v.speedY = v.initSpeedY*self.speedYTime(t)*self.speedTime(t);
			
			
			local w = v.width*0.5*v.scale;
			local h = v.height*0.5*v.scale;
			
			if(islocal) then
				v.x = v.x+self.x;
				v.y = v.y+self.y;
			end
			
			v.forceX = 0;
			v.forceY = 0;
			for l,w in pairs(self.forcefields) do
				local fx,fy = l:get(v.x,v.y);
				local mod = 1;
				if(not w) then
					mod = -1;
				end
				v.forceX = v.forceX+fx*mod;
				v.forceY = v.forceY+fy*mod;
			end
			
			

			local ct = math.cos(v.rotation*0.01745);
			local st = math.sin(v.rotation*0.01745);
			v.rotation = (v.rotation+v.rotSpeed*delta)%360;
			
			local p1x = (-w*ct) + (h*st);
			local p1y = (-w*st) - (h*ct);
			
			local p2x = (w*ct) + (h*st);
			local p2y = (w*st) - (h*ct);
			
			local p3x = (-w*ct) - (h*st);
			local p3y = (h*ct) - (w*st);
			
			local p4x = (w*ct) - (h*st);
			local p4y = (w*st) + (h*ct);
			
			local fx = ((v.frame % self.framesX)/self.framesX);
			local fy = (math.floor(v.frame/self.framesX)/self.framesY);
			
			local colour = v.col*self.colTime(t)*v.scaleCol;
			
			if(self.blend == blendEnum.alpha) then
				colour.r = colour.r*colour.a;
				colour.g = colour.g*colour.a;
				colour.b = colour.b*colour.a;
			end
			
			if(not prewarming) then
				tris[i] = v.x + p1x + ox;
				tris[i+1] = v.y + p1y + oy;
				
				tris[i+2] = v.x + p2x + ox;
				tris[i+3] = v.y + p2y + oy;
				
				tris[i+4] = v.x + p3x + ox;
				tris[i+5] = v.y + p3y + oy;
				
				tris[i+6] = v.x + p3x + ox;
				tris[i+7] = v.y + p3y + oy;
				
				tris[i+8] = v.x + p2x + ox;
				tris[i+9] = v.y + p2y + oy;
				
				tris[i+10] = v.x + p4x + ox;
				tris[i+11] = v.y + p4y + oy;
				
				
				txs[i] = fx;
				txs[i+1] = fy;
				
				txs[i+2] = fx+fw;
				txs[i+3] = fy;
				
				txs[i+4] = fx;
				txs[i+5] = fy+fh;
				
				txs[i+6] = fx;
				txs[i+7] = fy+fh;
				
				txs[i+8] = fx+fw;
				txs[i+9] = fy;
				
				txs[i+10] = fx+fw;
				txs[i+11] = fy+fh;
				
				
				for j=0,23,4 do
					cols[(2*i)+j] = colour.r;
					cols[(2*i)+j+1] = colour.g;
					cols[(2*i)+j+2] = colour.b;
					cols[(2*i)+j+3] = colour.a;
				end
				
				i = i+12;
			end
			
			if(v.collider ~= nil and hit) then
				v.collider.x = v.x;
				v.collider.y = v.y;
				if((self.collisionType == collisionType.coarse and #Block.getIntersecting(v.x,v.y,v.x+1,v.y+1) > 0) or
					(self.collisionType == collisionType.fine and colliders.collideBlock(v.collider,colliders.BLOCK_SOLID..colliders.BLOCK_LAVA..colliders.BLOCK_HURT..colliders.BLOCK_PLAYER))) then
					if(self.collision == collisionEnum.kill) then
						table.remove(self.particles,k);
						removed = true;
					elseif(self.collision == collisionEnum.stop) then
						v.speedX = 0;
						v.speedY = 0;
						v.initSpeedX = 0;
						v.initSpeedY = 0;
					elseif(self.collision == collisionEnum.bounce) then
						if(math.abs(v.speedX) >= math.abs(v.speedY)) then
							v.speedX = -v.speedX*v.bounceEnergy;
							v.initSpeedX = -v.initSpeedX*v.bounceEnergy;
						end
						if(math.abs(v.speedY) >= math.abs(v.speedX)) then
							v.speedY = -v.speedY*v.bounceEnergy;
							v.initSpeedY = -v.initSpeedY*v.bounceEnergy;
						end
					end
				end
			end
			
			v.ttl = v.ttl-delta;
			if(v.ttl <= 0) then	
				table.remove(self.particles,k);
				removed = true;
			else			
				if(islocal) then
					v.x = v.x-self.x;
					v.y = v.y-self.y;
				end
				local tau = 0;
				if(v.targetX ~= nil or v.targetY ~= nil) then
					tau = self.targetTime(t);
				end
				if(v.targetX ~= nil) then
					v.x = v.targetX*tau + v.initX*(1-tau);
				else
					v.x = v.x + v.speedX*delta;
				end
				if(v.targetY ~= nil) then
					local tau = self.targetTime(t);
					v.y = v.targetY*tau + v.initY*(1-tau);
				else
					v.y = v.y + v.speedY*delta;
				end
				v.speedX = v.speedX+(v.accelX+v.forceX)*delta;
				v.initSpeedX = v.initSpeedX + (v.accelX+v.forceX)*delta;
				v.speedY = v.speedY+(v.accelY+v.forceY)*delta;
				v.initSpeedY = v.initSpeedY + (v.accelY+v.forceY)*delta;
				v.frametimer = v.frametimer-1;
				if(v.frametimer == 0) then
					v.frame = (v.frame+1)%v.maxframes;
					v.frametimer = v.initFrametimer;
				end
			end
			if(not removed) then
				k = k+1;
			end
		end
		
		if(prewarming) then
			self.prewarm = self.prewarm-delta;
		end
	until(self.prewarm <= 0)
	
	self.hasPrewarmed = true;
	
	Graphics.glDraw{texture=self.texture, vertexCoords=tris, textureCoords=txs, vertexColors=cols, priority=priority}
end

end

local ribbon = {};
ribbon.__index = ribbon;

do --Ribbon Functions

function particles.Ribbon(x,y,source)
	local p;
	local s;
	
	if(type(source) == "table") then
		p = source;
		s = {};
	else
		p,s = parseFile(source);
	end
	setmetatable(p,ribbon);
	
	p.x = x;
	p.y = y;
	
	p.enabled = true;
	
	p:setParam("col",p.col,s)
	p:setParam("rate",p.rate,s);
	p:setParam("xOffset",p.xOffset,s);
	p:setParam("yOffset",p.yOffset,s);
	p:setParam("framesX",p.framesX,s);
	p:setParam("framesY",p.framesY,s);
	
	p:setParam("maxAngle",p.maxAngle);
	p:setParam("scaleType",p.scaleType);
	p:setParam("ribbonType",p.ribbonType);
	p:setParam("colLength",p.colLength);
	
	p:setParam("width",p.width,s);
	
	p:setParam("lifetime",p.lifetime,s);
	
	p:setParam("speedX",p.speedX,s);
	p:setParam("speedY",p.speedY,s);
	
	p:setParam("accelX",p.accelX,s);
	p:setParam("accelY",p.accelY,s);
	
	p:setParam("targetX",p.targetX,s);
	p:setParam("targetY",p.targetY,s);
	
	p:setParam("frameSpeed",p.frameSpeed,s);
	
	p:setParam("scaleTime",p.scaleTime,s);
	p:setParam("scaleLength",p.scaleLength,s);
	
	p:setParam("speedXTime",p.speedXTime,s);
	p:setParam("speedYTime",p.speedYTime,s);
	p:setParam("speedTime",p.speedTime,s);
	
	if(p.targetX ~= nil) then
		p:setParam("speedTime",nil);
		p:setParam("speedXTime",nil);
		p:setParam("speedX",nil);
		p:setParam("accelX",nil);
	end
	
	if(p.targetY ~= nil) then
		p:setParam("speedTime",nil);
		p:setParam("speedYTime",nil);
		p:setParam("speedY",nil);
		p:setParam("accelY",nil);
	end
	
	p:setParam("targetTime",p.targetTime,s);
	
	p:setParam("rotSpeedTime",p.rotSpeedTime,s);
	
	p:setParam("colTime",p.colTime,s);
	
	p:setParam("blend",p.blend,s);
	p:setParam("updateType",p.updateType,s);
	
	p.segments = {};
	return p;
end

ribbon.getParamDefault = emitter.getParamDefault;
ribbon.setParam = emitter.setParam;

function ribbon:FlipX()
	self:setParam("speedX",tostring(-self.maxSpeedX)..":"..tostring(-self.minSpeedX));
	self:setParam("accelX",tostring(-self.maxAccelX)..":"..tostring(-self.minAccelX));
end

function ribbon:FlipY()
	self:setParam("speedY",tostring(-self.maxSpeedY)..":"..tostring(-self.minSpeedY));
	self:setParam("accelY",tostring(-self.maxAccelY)..":"..tostring(-self.minAccelY));
end

ribbon.AttachToCamera = emitter.AttachToCamera;
ribbon.Attach = emitter.Attach;
ribbon.GetOffset = emitter.GetOffset;
ribbon.SetOffset = emitter.SetOffset;
ribbon.Detach = emitter.Detach;

function ribbon:KillTrail()
	for k,_ in ipairs(self.segments) do
		self.segments[k] = nil;
	end
end

function ribbon:Destroy()
	for k,_ in pairs(self) do
		self[k] = nil;
	end
	setmetatable(self,nullMeta);
end

function ribbon:Count()
	return #self.segments;
end

function ribbon:Scale(n)
	self:setParam("width",tostring(self.minWidth*n)..":"..tostring(self.maxWidth*n));
	self:setParam("speedX",tostring(self.minSpeedX*n)..":"..tostring(self.maxSpeedX*n));
	self:setParam("speedY",tostring(self.minSpeedY*n)..":"..tostring(self.maxSpeedY*n));
	self:setParam("accelX",tostring(self.minAccelX*n)..":"..tostring(self.maxAccelX*n));
	self:setParam("accelY",tostring(self.minAccelY*n)..":"..tostring(self.maxAccelY*n));
end

function ribbon:Emit(n)
	n = n or 1;
		
	for i = 1,n do
		local p = {};
		
		p.xoff = rng.random(self.minX,self.maxX);
		p.yoff = rng.random(self.minY,self.maxY);
		p.x = self.x + p.xoff;
		p.y = self.y + p.yoff;
		
		if(type(self.col) == "table" and self.col.r == nil) then
			p.col = rng.irandomEntry(self.col);
		else
			local cr = rng.random();
			p.col = particles.Col((self.minCol.r*(1-cr) + self.maxCol.r*cr)*255,(self.minCol.g*(1-cr) + self.maxCol.g*cr)*255,(self.minCol.b*(1-cr) + self.maxCol.b*cr)*255,(self.minCol.a*(1-cr) + self.maxCol.a*cr)*255);
		end
		
		p.ttl = rng.random(self.minLife,self.maxLife);
		p.initTtl = p.ttl;
		
		p.width = rng.random(self.minWidth,self.maxWidth);
		p.initWidth = p.width;
		
		p.speedX = rng.random(self.minSpeedX,self.maxSpeedX);
		p.speedY = rng.random(self.minSpeedY,self.maxSpeedY);
		
		p.initSpeedX = p.speedX;
		p.initSpeedY = p.speedY;
		
		p.accelX = rng.random(self.minAccelX,self.maxAccelX);
		p.accelY = rng.random(self.minAccelY,self.maxAccelY);
		
		if(self.minTargetX ~= nil) then
			p.targetX = xoff + rng.random(self.minTargetX, self.maxTargetX);
			p.initX = p.x;
		end
		if(self.minTargetY ~= nil) then
			p.targetY = yoff + rng.random(self.minTargetY, self.maxTargetY);
			p.initY = p.y;
		end
		
		p.frame = 0;
		p.maxframes = self.framesX*self.framesY;
		p.frametimer = rng.randomInt(self.minFrameSpd,self.maxFrameSpd);
		p.initFrametimer = p.frametimer;
		
		p.recent = true;
		
		table.insert(self.segments,p);
	end
end

local function ccw(x1,y1,x2,y2,px,py)
	return (py-y1)*(x2-x1) > (y2-y1)*(px-x1);
end

local function intersect(x1,y1,x2,y2,x3,y3,x4,y4)
	return ccw(x1,y1,x3,y3,x4,y4) ~= ccw(x2,y2,x3,y3,x4,y4) and ccw(x1,y1,x2,y2,x3,y3) ~= ccw(x1,y1,x2,y2,x4,y4)
end

function ribbon:Draw(priority)
	priority = priority or 0.5;
	if(self.tte == nil) then 
		self.tte = 1/rng.random(self.minRate,self.maxRate);
	end
	
	if(self.parent ~= nil) then
		if(self.parent.__type == "Camera" or self.parent.__type == "Animation" or self.parent.__type == "Block" or self.parent.__type == "Player" or self.parent.__type == "NPC") then
			if(self.parent.obj == nil or (self.parent.obj.isValid ~= nil and not self.parent.obj.isValid)) then
				self:Detach();
				self.enabled = false;
			else
				self.x = self.parent.obj.x + self.parent.obj.width/2 + self.parent.offsetX;
				self.y = self.parent.obj.y + self.parent.obj.height/2 + self.parent.offsetY;
				if(self.parent.shouldFlip) then
					if(self.parent.__type == "Player") then
						if(self.parent.obj:mem(0x106,FIELD_WORD) ~= self.parent.lastDirection) then
							self:FlipX();
							self.parent.offsetX = -self.parent.offsetX;
						end
						self.parent.lastDirection = self.parent.obj:mem(0x106,FIELD_WORD);
					elseif(self.parent.__type == "NPC") then
						if(self.parent.obj.direction ~= self.parent.lastDirection) then
							self:FlipX();
							self.parent.offsetX = -self.parent.offsetX;
						end
						self.parent.lastDirection = self.parent.obj.direction;
					end
				end
			end
		end
	end
	
	local cam = Camera.get()[1];
	
	local delta = 1;
	if(self.updateType == updateEnum.seconds) then
		delta = eventu.deltaTime;
		if(delta <= 0) then delta = 0.01538461538; end -- Cap deltaTime so it never gets stuck in an infinite loop. Cap is SMBX native framerate (1/65)
	end
		
	
	local tris = {};
	local txs = {};
	local cols = {};
	local i = 0;
	
		if(self.enabled) then
			if(self.tte <= 0) then
					self:Emit();
					self.tte = 1/rng.random(self.minRate,self.maxRate);
			end
			
			self.tte = self.tte-delta;
		end
		
		local ox,oy = -cam.x,-cam.y;
		local fw = 1/self.framesX;
		local fh = 1/self.framesY;
		
		local k = 1;
		while(k <= #self.segments) do
			local v = self.segments[k];
			local last = self.segments[k-1];
			if(last == nil) then
				last = v;
				v.nolast = true;
			end
			local removed = false;
			local t = 1 - (v.ttl/v.initTtl);
			v.speedX = v.initSpeedX*self.speedXTime(t)*self.speedTime(t);
			v.speedY = v.initSpeedY*self.speedYTime(t)*self.speedTime(t);
			
			
			if(not self.enabled) then
				v.recent = false;
			end
			
			if((v.x == last.x and v.y == last.y) or (v.recent and not last.recent)) then
				v.nolast = true;
			end
			
			local trailend = 1;
			local trailstart = #self.segments;
			for lst = k,1,-1 do
				if(self.segments[lst].nolast or self.segments[lst].skipped) then
					trailend = lst+1;
					break;
				end
			end
			for lst = k,#self.segments,1 do
				if(self.segments[lst].nolast or self.segments[lst].skipped) then
					trailstart = lst-1;
					break;
				end
			end
			local leng
			if(trailstart == trailend) then 
				leng = 1 
			else
				leng = math.max(0,math.min(1,1-((k-trailend)/(trailstart-trailend))));
			end
			local ws = self.scaleTime(t)*self.scaleLength(leng);
			v.width = v.initWidth*ws;
			local wt,wb;
			
			if(self.scaleType==scaleEnum.centre) then
				wt = v.width*0.5;
				wb = v.width*0.5;
			elseif((self.scaleType==scaleEnum.top and v.x >= last.x) or (self.scaleType==scaleEnum.bottom and v.x < last.x)) then
				wb = v.initWidth*0.5;
				wt = v.width-v.initWidth*0.5;
			elseif((self.scaleType==scaleEnum.bottom and v.x >= last.x) or (self.scaleType==scaleEnum.top and v.x < last.x)) then				
				wb = v.width-v.initWidth*0.5;
				wt = v.initWidth*0.5;
			end
			
			
			local fx = ((v.frame % self.framesX)/self.framesX);
			local fy = (math.floor(v.frame/self.framesX)/self.framesY);
			
			v.colour = v.col*self.colTime(t)*self.colLength(leng);
			
			if(self.blend == blendEnum.alpha) then
				v.colour.r = v.colour.r*v.colour.a;
				v.colour.g = v.colour.g*v.colour.a;
				v.colour.b = v.colour.b*v.colour.a;
			end
			
			v.rot = 1.570796-math.atan2(v.x-last.x,v.y-last.y);
			local st = math.sin(v.rot);
			local ct = math.cos(v.rot);
			
			local p1x = wb*st;
			local p1y = -wb*ct;
			
			local p2x = -wt*st;
			local p2y = wt*ct;
			
			--[[if(v.x > last.x) then
				p1x = -p1x;
				p1y = -p1y;
				p2x = -p2x;
				p2y = -p2y;
			end]]
			
			if(self.enabled and v.recent and k == #self.segments) then
				v.x = self.x + v.xoff;
				v.y = self.y + v.yoff;
			end
			
			if(math.abs(v.rot-last.rot) < math.rad(self.maxAngle) and not v.nolast) then
				v.x1 = v.x+p1x;
				v.y1 = v.y+p1y;
				v.x2 = v.x+p2x;
				v.y2 = v.y+p2y;
				
				if(last.x1 == nil or (last.nolast and (last.recent or not v.recent))) then
					last.x1 = last.x+p1x;
					last.y1 = last.y+p1y;
					last.x2 = last.x+p2x;
					last.y2 = last.y+p2y;
				else
					if(intersect(last.x,last.y,last.x1,last.y1,v.x,v.y,v.x1,v.y1)) then
						v.x1 = last.x1;
						v.y1 = last.y1;
					end
					
					if(intersect(last.x,last.y,last.x2,last.y2,v.x,v.y,v.x2,v.y2)) then
						v.x2 = last.x2;
						v.y2 = last.y2;
					end
				end
				
				tris[i] = last.x1+ox;
				tris[i+1] = last.y1+oy;
					
				tris[i+2] = v.x1+ox;
				tris[i+3] = v.y1+oy;
					
				tris[i+4] = last.x2+ox;
				tris[i+5] = last.y2+oy;
					
				tris[i+6] = last.x2+ox;
				tris[i+7] = last.y2+oy;
					
				tris[i+8] = v.x1+ox;
				tris[i+9] = v.y1+oy;
					
				tris[i+10] = v.x2+ox;
				tris[i+11] = v.y2+oy;
				
					
				txs[i] = fx;
				txs[i+1] = fy;
					
				txs[i+2] = fx+fw;
				txs[i+3] = fy;
					
				txs[i+4] = fx;
				txs[i+5] = fy+fh;
					
				txs[i+6] = fx;
				txs[i+7] = fy+fh;
					
				txs[i+8] = fx+fw;
				txs[i+9] = fy;
					
				txs[i+10] = fx+fw;
				txs[i+11] = fy+fh;
					
				for j=0,23,4 do
					if(j == 0 or j==8 or j==12) then
						cols[(2*i)+j] = last.colour.r;
						cols[(2*i)+j+1] = last.colour.g;
						cols[(2*i)+j+2] = last.colour.b;
						cols[(2*i)+j+3] = last.colour.a;
					else
						cols[(2*i)+j] = v.colour.r;
						cols[(2*i)+j+1] = v.colour.g;
						cols[(2*i)+j+2] = v.colour.b;
						cols[(2*i)+j+3] = v.colour.a;
					end
				end
					
				i = i+12;
			elseif(k>2 and self.ribbonType==ribbonEnum.continuous and (last.recent or not v.recent)) then
				v.x1 = last.x1;
				v.x2 = last.x2;
				v.y1 = last.y1;
				v.y2 = last.y2;
			else
				v.skipped = true;
			end
			
			v.ttl = v.ttl-delta;
			if(v.ttl <= 0) then	
				table.remove(self.segments,k);
				removed = true;
			else
				local tau = 0;
				if(v.targetX ~= nil or v.targetY ~= nil) then
					tau = self.targetTime(t);
				end
				if(v.targetX ~= nil) then
					v.x = v.targetX*tau + v.initX*(1-tau);
				else
					v.x = v.x + v.speedX*delta;
				end
				if(v.targetY ~= nil) then
					local tau = self.targetTime(t);
					v.y = v.targetY*tau + v.initY*(1-tau);
				else
					v.y = v.y + v.speedY*delta;
				end
				v.speedX = v.speedX+(v.accelX)*delta;
				v.initSpeedX = v.initSpeedX + (v.accelX)*delta;
				v.speedY = v.speedY+(v.accelY)*delta;
				v.initSpeedY = v.initSpeedY + (v.accelY)*delta;
				v.frametimer = v.frametimer-1;
				if(v.frametimer == 0) then
					v.frame = (v.frame+1)%v.maxframes;
					v.frametimer = v.initFrametimer;
				end
			end
			if(not removed) then
				k = k+1;
			end
		end
	
	Graphics.glDraw{texture=self.texture, vertexCoords=tris, textureCoords=txs, vertexColors=cols, priority=priority}
end

end

	
return particles;