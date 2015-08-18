colliders = loadSharedAPI("colliders");

local point = colliders.Point(-199824, -200208);
point:Debug(true);

local box = colliders.Box(-199740, -200224, 32, 32);
box:Debug(true);

local circle = colliders.Circle(-199624, -200208, 16);
circle:Debug(true);

local tri = colliders.Tri(-199524, -200208, {-16,16}, {0,-16}, {16, 16});
tri:Debug(true);

local poly = colliders.Poly(-199424, -200208, {-16,16}, {0,-16}, {16, 16}, {0,0});
poly:Debug(true);

function onLoop()
	local s = {};
	
	if colliders.collide(player, point) then
		table.insert(s, "point");
	end
	if colliders.collide(player, box) then
		table.insert(s, "box");
	end
	if colliders.collide(player, circle) then
		table.insert(s, "circle");
	end
	if colliders.collide(player, tri) then
		table.insert(s, "tri");
	end
	if colliders.collide(player, poly) then
		table.insert(s, "poly");
	end
	
	
	local y = 0;
	for _,v in ipairs(s) do
		printText(v,0,y);
		y = y + 20;
	end
end