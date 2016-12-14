--booBuddies.lua 
--v1.0.2
--Created by S1eth, 2016

local colliders = API.load("colliders");

local booBuddies = {};

function booBuddies.onInitAPI()
  registerEvent(booBuddies, "onTick", "onTick", true); --Register the loop event
	registerEvent(booBuddies, "onDraw", "onDraw", true); --Register the draw event
end


--***************************************************************************************************
--                                                                                                  *
--              UTILITY FUNCTIONS                                                                   *
--                                                                                                  *
--***************************************************************************************************

local function round(x)
	return x>=0 and math.floor(x+0.5) or math.ceil(x-0.5);
end

-- Finds the corresponding section for a point (x,y). (If the point is within 1000 pixel of the section boundary) 
local function getSection(x, y)
	for index,section in pairs(Section.get()) do
		local boundary = section.boundary;
		local sectionBox = colliders.Box( 
			boundary.left - 1000,
			boundary.top - 1000,
			boundary.right - boundary.left + 1000,  
			boundary.bottom - boundary.top + 1000
		)
	
		if colliders.collide(sectionBox, colliders.Point(x, y)) then
			return index-1; -- one-indexed --> zero-indexed
		end 
	end
	
	return nil;
end 


--***************************************************************************************************
--                                                                                                  *
--              PRIVATE CONSTANTS                                                                   *
--                                                                                                  *
--***************************************************************************************************

-- boo types used in SMW's boo buddies
local BOO_TYPES_SMW = {0, 1, 2, 0}; --repeats when more than 4 boos are used

-- load spritesheet for the boos 
local BOO_IMAGE_RESOURCE = Graphics.loadImage(Misc.resolveFile("booBuddies/boos.png"));

local MOUNT_TYPES = {shoe = 1, yoshi = 3};

local style = {SMW = 0, SMB3 = 1, SMB1 = 2, SMM_SMW = 3};

local direction = {right = 0, left = 1};

local angularDirection = {clockwise = 1, CW = 1, counterclockwise = -1, CCW = -1};

local _preset = {};
_preset.SMW = {};
_preset.SMW.style            = "SMW";
_preset.SMW.numberOfBoos     = 10;
_preset.SMW.booSpacing       = 0.483321947;
_preset.SMW.circleRadius     = 5 * 32;
_preset.SMW.angularSpeed     = math.pi / 275;
_preset.SMW.angularDirection = angularDirection.clockwise;

_preset.SMM = {};
_preset.SMM.style            = "SMM_SMW";
_preset.SMM.numberOfBoos     = 8;
_preset.SMM.booSpacing       = math.pi / 4.5;
_preset.SMM.circleRadius     = 3 * 32;
_preset.SMM.angularSpeed     = math.pi / 240;
_preset.SMM.angularDirection = angularDirection.clockwise;


--***************************************************************************************************
--                                                                                                  *
--              PUBLIC MEMBERS                                                                      *
--                                                                                                  *
--***************************************************************************************************

booBuddies.diesTo = {shoe = false, starman = false};

booBuddies.booNPCID = {SMW = 43, SMB3 = 38, SMB1 = 38, SMM_SMW = 43}

booBuddies.canSpinjump = false;  

--***************************************************************************************************
--                                                                                                  *
--              CLASS: Boo                                                                          *
--                                                                                                  *
--***************************************************************************************************

local Boo = {};
Boo.__index = Boo;
Boo.isAlive = true;

function Boo.create()
	local newInstance = {};
	setmetatable(newInstance, Boo);
	
	return newInstance;
end 

-- get the graphic rectangle for a specific boo animation frame 
function getBooRectangle(styleIndex, direction, booIndex, frameIndex)
	local rectangle = { width = 32, height = 32};  
	rectangle.left = style[styleIndex] * 96 + ((styleIndex == "SMW") and booIndex * 32 or 0); -- only SMW has multiple boo types (for now?) 
  rectangle.top  = direction * 64         + ((styleIndex == "SMW") and frameIndex * 32 or 0); -- only SMW has animation frames (for now?)
	return rectangle;
end

--***************************************************************************************************
--                                                                                                  *
--              CLASS: BooCircle                                                                    *
--                                                                                                  *
--***************************************************************************************************

local booCircleTable = {};

local BooCircle = {}
BooCircle.__index = BooCircle;
BooCircle.isActive = false;
BooCircle.toBeDeleted = false;
BooCircle.toBeKilled = false;
BooCircle.initial = {};

function BooCircle.create(args)
	local newInstance = {} -- our new object
	setmetatable (newInstance, BooCircle) -- make BooCircle handle lookup
	
	local preset;
	if(args.preset == nil) then 
		preset = _preset.SMW;
	else
		assert(_preset[args.preset] ~= nil, "preset "..args.preset.." does not exist");
		preset = _preset[args.preset];
	end 
	
  newInstance.style             = args.style                              or preset.style;
	
	assert(args.x ~= nil and type(args.x) == "number", "x coordinate must be a number");
	assert(args.y ~= nil and type(args.y) == "number", "y coordinate must be a number");
  newInstance.x                 = args.x                                  
	newInstance.y                 = args.y                                   

	newInstance.section           = args.section                            or getSection(newInstance.x,newInstance.y);
	newInstance.layer             = args.layer                              or "Default";

	newInstance.circleRadius      = args.circleRadius                       or preset.circleRadius;
	newInstance.angularSpeed      = args.angularSpeed                       or preset.angularSpeed;
	newInstance.angularDirection  = angularDirection[args.angularDirection] or preset.angularDirection;
	newInstance.numberOfBoos      = args.numberOfBoos                       or preset.numberOfBoos;
	newInstance.booRadius         = args.booRadius                          or 10;
	newInstance.frameTime         = args.frameTime                          or 8;
	newInstance.frame             = args.frame                              or 0;
	newInstance.booSpacing        = args.booSpacing                         or preset.booSpacing; 
  newInstance.angle             = args.angle                              or math.pi*1.5 - (newInstance.booSpacing * (newInstance.numberOfBoos-1)/2); 
	
	-- reset to these initial values when entering/leaving a section
	newInstance.initial.angle = newInstance.angle;  
	newInstance.initial.frame = newInstance.frame; 
	
	newInstance.boos = {};
	for i=1,newInstance.numberOfBoos,1 do
	  table.insert(newInstance.boos, Boo.create(booBuddies.diesTo)); 
	end 
	
	table.insert (booCircleTable, newInstance)
	return newInstance;
end

function BooCircle:update()

	-- Deactivate if not on a visible layer or current section
	-- reset the cycle
	if Layer.get(self.layer).isHidden 
	or not(player.section == self.section 
	       or (player2 ~= nil and player2.section == self.section)) 
	then 
		if (self.isActive) then 
			self.isActive = false;
			self.angle = self.initial.angle;
			self.frame = self.initial.frame;
		end 
		return; 
  end
	
	-- Reactivate 
	if not self.isActive then 
		self.isActive = true;
	end

	--update position
	self.angle = self.angle + self.angularSpeed * self.angularDirection;
  self.frame = (self.frame + 1) % (self.frameTime * 2);
	
	--check collision with player
	for i,boo in pairs(self.boos) do
		
	  if(boo.isAlive) then
			local booX = round(self.x + self.circleRadius * math.cos(self.angle + (i-1) * self.booSpacing));
			local booY = round(self.y + self.circleRadius * math.sin(self.angle + (i-1) * self.booSpacing));
		
			local hitbox = colliders.Circle(booX, booY, self.booRadius);
			for _,p in pairs({player, player2}) do
			
				local bounce, spinjump = colliders.bounce(p,hitbox);
				local downSlash = colliders.downSlash(p,hitbox);
				
				if(bounce and p.DeathTimer == 0 and not inStar) then
					if(booBuddies.canSpinjump and (spinjump or downSlash or p.MountType == MOUNT_TYPES.yoshi or p.MountType == MOUNT_TYPES.shoe) ) then
						colliders.bounceResponse(p);
						if(p.MountType == MOUNT_TYPES.shoe and booBuddies.diesTo["shoe"]) then
							self:killBoo(boo, booX, booY); 
						end
					else
						p:harm();
					end
				elseif(colliders.collide(p,hitbox)) then
					--check for starman 
					if(booBuddies.diesTo["starman"] and inStar) then 
						self:killBoo(boo, booX, booY);
					else
						p:harm();
					end
				end
				
			end
		end
	end -- for i,boo in pairs(self.boos)
	
end -- BooCircle:update()

function BooCircle:draw()

	if not self.isActive  then return end 

	for i,boo in pairs(self.boos) do
	  if(boo.isAlive) then
			local booX = round(self.x + self.circleRadius * math.cos(self.angle + (i-1) * self.booSpacing));
			local booY = round(self.y + self.circleRadius * math.sin(self.angle + (i-1) * self.booSpacing));
		
			local dir = direction.right;
			if(self.style ~= "SMW") and (player.x + player.width/2 < booX) then  
				dir = direction.left;
			end
			
			local booRect;
			if (self.frame < self.frameTime) then
				booRect = getBooRectangle(self.style, dir, BOO_TYPES_SMW[1+((i-1) % 4)], 0);
			else
				booRect = getBooRectangle(self.style, dir, BOO_TYPES_SMW[1+((i-1) % 4)], 1);
			end
			
			Graphics.drawImageToSceneWP(BOO_IMAGE_RESOURCE, booX-16, booY-16, booRect.left, booRect.top, booRect.width, booRect.height, -45.0);
		end
	end -- for i,boo in pairs(self.boos)
	
end -- BooCircle:draw()

function BooCircle:delete()
	self.toBeDeleted = true; -- delete during next onTick()  
end

function BooCircle:kill()
	self.toBeKilled = true; -- kill during next onTick()  
end

function BooCircle:killBoos()
	for i,boo in pairs(self.boos) do
		if(boo.isAlive) then
			local booX = round(self.x + self.circleRadius * math.cos(self.angle + (i-1) * self.booSpacing));
			local booY = round(self.y + self.circleRadius * math.sin(self.angle + (i-1) * self.booSpacing));
		
			self:killBoo(boo, booX, booY); 
		end
	end -- for i,boo in pairs(self.boos)
end

function BooCircle:killBoo(boo, x, y)
	boo.isAlive = false;
	
	NPC.spawn(booBuddies.booNPCID[self.style], x-16, y-16, self.section):kill();
end

function booBuddies.create(args)
	return BooCircle.create(args);
end

function booBuddies.onTick()

  --delete circles with the toBeKilled flag 
	for i=#booCircleTable,1,-1 do
    if(booCircleTable[i].toBeKilled == true) then
      booCircleTable[i]:killBoos();
    end
  end
	
	--delete instances with the toBeDeleted flag 
	for i=#booCircleTable,1,-1 do
    if(booCircleTable[i].toBeDeleted == true) then
      table.remove(booCircleTable, i);
    end
  end

	for _,booCircle in pairs (booCircleTable) do
		booCircle:update();
	end
end

function booBuddies.onDraw() 
	for _,booCircle in pairs (booCircleTable) do
		booCircle:draw();
	end
end

return booBuddies;