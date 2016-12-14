--smoovement.lua
--by Spinda
--v1.1

local vectr = API.load("vectr");

local smoovement = {};

smoovement.customDraw = false;

smoovement.customTick = false;

smoovement.world = {transform = vectr.id3, children = {}, actualPriority = 0};

function smoovement.translateM(x, y) -- creates a matrix for translation
	return vectr.mat3(	{1, 0, x},
						{0, 1, y},
						{0, 0, 1});
end;

function smoovement.rotateM(s1,c1) -- creates a matrix for rotation
	return vectr.mat3(	{c1, -s1, 0},
						{s1, c1,  0},
						{0,  0,   1});
end;

function smoovement.scaleM(x,y) -- creates a matrix for scaling
	return vectr.mat3(	{x, 0, 0},
						{0, y, 0},
						{0, 0, 1});
end;

function smoovement.vectorTransToXY(vector, trans) -- turns a vector2 and a transformation into an x and y value
	local v3 = trans * vectr.v3(vector.x,vector.y,1);
	return v3.x, v3.y;
end;

local function drawImageTransformed(t)
	local img = t.img; -- setting values
	local trans = t.transform or vectr.id3;
	local p = t.actualPriority;
	local w = img.width;
	local h = img.height;
	local pivX = t.pivotX or .5;
	local pivY = t.pivotY or .5;
	local o = t.opacity or 1;
	local sceneC = t.usesSceneCoords;
	local vertCoords = {};
	local w1 = - pivX * w; -- setting values so that the pivot is the center
	local w2 = w + w1;
	local h1 = -pivY * h;
	local h2 = h + h1;
	vertCoords[0], vertCoords[1] = smoovement.vectorTransToXY(vectr.v2(w1,h1), trans); -- setting vertex coordinates
	vertCoords[2], vertCoords[3] = smoovement.vectorTransToXY(vectr.v2(w1,h2), trans);
	vertCoords[4], vertCoords[5] = smoovement.vectorTransToXY(vectr.v2(w2,h1), trans);
	vertCoords[6], vertCoords[7] = smoovement.vectorTransToXY(vectr.v2(w2,h2), trans);
	vertCoords[8], vertCoords[9] = smoovement.vectorTransToXY(vectr.v2(w1,h2), trans);
	vertCoords[10], vertCoords[11] = smoovement.vectorTransToXY(vectr.v2(w2,h1), trans);
	local textCoords = {};
	textCoords[0]  = 0; textCoords[1]  = 0; -- setting texture coordinates
	textCoords[2]  = 0; textCoords[3]  = 1;
	textCoords[4]  = 1; textCoords[5]  = 0;
	textCoords[6]  = 1; textCoords[7]  = 1;
	textCoords[8]  = 0; textCoords[9]  = 1;
	textCoords[10] = 1; textCoords[11] = 0;
	Graphics.glDraw{texture = img,
					vertexCoords = vertCoords,
					textureCoords = textCoords,
					color = {1, 1, 1, o},
					priority = p,
					sceneCoords = sceneC}; -- drawing the image
end

local function deleteChildren(sprite, delete) -- deleting sprites recursively
	if deleted then sprite.deleted = true; end;
	for i = #sprite.children, 1, -1 do
		deleteChildren(sprite.children[i],delete or sprite.deleted);
		if delete or sprite.deleted then
			table.remove(sprite.children,i);
		end;
	end;
end;

local function transformChildren(sprite) -- transforming sprites recursively so the transform of its parents get taken over for their children
	for _,v in pairs(sprite.children) do
		v.transform = sprite.transform * smoovement.translateM(v.x, v.y) * smoovement.scaleM(v.scaleX, v.scaleY) * smoovement.rotateM(math.sin(v.rotation * math.pi/180), math.cos(v.rotation * math.pi/180));
		v.actualPriority = v.priority + sprite.actualPriority;
		transformChildren(v);
	end;
end;

local function drawChildren(sprite) -- drawing the sprites
	for _,v in pairs(sprite.children) do
		if v.hasAnimation then
			v.counter = (v.counter + 1) % v.dTime;
			if v.counter == 0 then
				v.frame = v.frame % #v.frames + 1;
				v.img = v.frames[v.frame];
				v.width = v.img.width;
				v.height = v.img.height;
			end;
		end;
		if v.img ~= nil and v.visible then
			drawImageTransformed(v);
		end;
		drawChildren(v);
	end;
end;

local function contains(t,val)
	for _, v in pairs(t) do
		if v == val then return true; end;
	end;
	return false;
end;

function smoovement.tick()
	deleteChildren(smoovement.world,false);
	transformChildren(smoovement.world);
end;

function smoovement.draw()
	drawChildren(smoovement.world);
end;

function smoovement.onInitAPI()
	registerEvent(smoovement,"onDraw","onDraw",false);
	registerEvent(smoovement,"onTick","onTick",false);
end

function smoovement.onDraw()
	if not smoovement.customDraw then
		smoovement.draw();
	end;
end;

function smoovement.onTick()
	if not smoovement.customTick then
		smoovement.tick();
	end;
end;

function smoovement.createSprite(t)
	local newSpr = {img = t.img,
					x = t.x or 0,
					y = t.y or 0,
					rotation = t.rotation or 0,
					pivotX = t.pivotX or .5,
					pivotY = t.pivotY or .5,
					scaleX = t.scaleX or 1,
					scaleY = t.scaleY or 1,
					visible = t.visible,
					usesSceneCoords = usesSceneCoords,
					hasParent = false,
					hasAnimation = false,
					dTime = 0,
					counter = 0,
					frame = 1,
					priority = t.priority or 0,
					opacity = t.opacity,
					parent = smoovement.world,
					frames = {},
					children = {},
					actualPriority = 0,
					transform = vectr.id3,
					deleted = false,
					_isSprite = true};
	if newSpr.visible == nil then newSpr.visible = true; end;
	if newSpr.usesSceneCoords == nil then newSpr.usesSceneCoords = false; end;
	if newSpr.img ~= nil then
		newSpr.height = t.height or newSpr.img.height;
		newSpr.width = t.width or newSpr.img.width;
	else
		newSpr.height = t.height or 0;
		newSpr.width = t.width or 0;
	end;
	
	newSpr.setScale = function(self, scaleX, scaleY) -- defining methods to make setting certain values faster
		self.scaleX = scaleX; self.scaleY = scaleY;
	end;
	newSpr.setPivot = function(self, pivotX, pivotY)
		self.pivotX = pivotX; self.pivotY = pivotY;
	end;
	newSpr.moveTo = function(self, x, y)
		self.x = x; self.y = y;
	end;
	newSpr.setParent = function(self, parent) -- method to set a parent
		for i = #self.parent.children, 1, -1 do -- removing sprite from children table of the previous parent
			if self == self.parent.children[i] then
				table.remove(self.parent.children,i);
				break;
			end;
		end;
		self.parent = parent;
		table.insert(self.parent.children,self);
		self.hasParent = true;
	end;
	newSpr.removeParent = function(self)
		self.hasParent = false;
		for i = #self.parent.children, 1, -1 do -- removing sprite from childen table of the previous paeent
			if self == self.parent.children[i] then
				table.remove(self.parent.children,i);
				break;
			end;
		end;
	end;
	newSpr.setAnimation = function(self, tab, dTime)
		self.hasAnimation = true;
		self.frames = tab;
		self.dTime = dTime;
		self.counter = 0;
		self.frame = 1;
		self.width = self.frames[1].width;
		self.height = self.frames[1].height;
	end;
	newSpr.setSprite = function(self, sprite)
		self.hasAnimation = false;
		self.img = sprite;
		self.height = self.img.height;
		self.width = self.img.width;
	end;
	newSpr.delete = function(self)
		self.deleted = true;
	end;
	table.insert(smoovement.world.children,newSpr);
	return newSpr;
end;
	
return smoovement;