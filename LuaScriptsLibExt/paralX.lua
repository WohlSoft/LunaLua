--***************************************************************************************
--                                                                                      *
--  paralX.lua                                                                          *
--  v1.1                                                                                *
--  Documentation: http://engine.wohlnet.ru/pgewiki/CinematX.lua                        *
--  Discussion thread: http://talkhaus.raocow.com/viewtopic.php?f=36&t=15516            *
--                                                                                      *
--***************************************************************************************

local paralX = {}

function paralX.onInitAPI() --Is called when the api is loaded by loadAPI.
	--register event handler
	--registerEvent(string apiName, string internalEventName, string functionToCall, boolean callBeforeMain)
   
	registerEvent(paralX, "onStart", "onStart", true) --Register the init event
	registerEvent(paralX, "onLoop", "onLoop", true) --Register the init event
	registerEvent(paralX, "onCameraUpdate", "onCameraUpdate", true) --Register the init event
end



local indexedParallaxes = {}

local Parallax = {}
Parallax.__index = Parallax

function Parallax.create (args)
	local thisParallaxObj = {}                                      -- our new object
	setmetatable (thisParallaxObj, Parallax)              			-- make Parallax handle lookup
	
	-- Read parameters
	thisParallaxObj.image = args.image  			 				-- initialize our object
	thisParallaxObj.frames = args.frames  or  1
	thisParallaxObj.animSpeed = args.animSpeed  or  0.05
	thisParallaxObj.alpha = args.alpha  or  1
	
	thisParallaxObj.sections = args.sections  or  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21}
	thisParallaxObj.priority = args.priority  or  -96 				
	
	thisParallaxObj.speedX = args.speedX  or  0
	thisParallaxObj.speedY = args.speedY  or  0
	
	thisParallaxObj.x = args.x  or  0
	thisParallaxObj.y = args.y  or  0
	
	thisParallaxObj.wobbleX = args.wobbleX  or  0
	thisParallaxObj.wobbleY = args.wobbleY  or  0
	
	thisParallaxObj.parallaxX = args.parallaxX  or  0.75
	thisParallaxObj.parallaxY = args.parallaxY  or  0.75

	thisParallaxObj.repeatX = args.repeatX
	thisParallaxObj.repeatY = args.repeatY
	if  thisParallaxObj.repeatX == nil  then
		thisParallaxObj.repeatX = true
	end
	if  thisParallaxObj.repeatY == nil  then
		thisParallaxObj.repeatY = true
	end

	thisParallaxObj.gapX = args.gapX  or  0
	thisParallaxObj.gapY = args.gapY  or  0

	
	-- Control vars
	thisParallaxObj.animFrame = 0	
	thisParallaxObj.visible = true
	
	thisParallaxObj.speedx = 0
	thisParallaxObj.speedy = 0
	thisParallaxObj.speedx = 0
	thisParallaxObj.speedy = 0
	
	table.insert (indexedParallaxes, thisParallaxObj)
	return thisParallaxObj;
end


function paralX.create (args)
	return Parallax.create (args)
end


local function tableContains (myTable, element)
	for _, value in pairs (myTable) do
		if value == element then
			return true
		end
	end
	return false
end


function Parallax:update ()
	-- Update animation
	self.animFrame = (self.animFrame + self.animSpeed) % self.frames;
	
	local w = self.image.width
	local h = self.image.height / self.frames
	
	self.speedx = (self.speedx + self.speedX) % (w + self.gapX)
	self.speedy = (self.speedy + self.speedY) % (h + self.gapY)

end


function Parallax:getMidCoords ()
	local w = self.image.width
	local h = self.image.height / self.frames

	local mX = -self.parallaxX * (Camera.get()[1].x + 200000) + w + self.x + self.speedx + self.wobbleX
	local mY = -self.parallaxY * (Camera.get()[1].y + 200000) + self.y + self.speedy + self.wobbleY
	
	if  self.repeatX == true  then
		mX = mX % (w + self.gapX)
	end
	if  self.repeatY == true  then
		mY = mY % (h + self.gapY)
	end
	
	return mX,mY
end


function Parallax:draw ()
	if  self.image == nil  then
		Text.print ("Nil image", 4, 400,300)
		return
	end

	
	local w = self.image.width
	local h = self.image.height / self.frames
	
	local numTilesX = 1
	local numTilesY = 1
	
	-- Only draw in given section
	if  --[[tableContains (self.sections, player.section) == true  and]]  self.visible == true  then
		--windowDebug ("test")
	
		local mX, mY = self:getMidCoords()
		
		if  self.repeatX == true  then
			numTilesX = math.ceil(Camera.get()[1].width/w)+1
		end
		if  self.repeatY == true  then
			numTilesY = math.ceil(Camera.get()[1].height/h)+1
		end

		-- Draw
		local drawProps = {x=mX, y=mY, type=RTYPE_IMAGE,
						   image=self.image,
						   --isSceneCoordinates=true,
						   priority=self.priority,
						   sourceX=0, sourceY=h*math.floor(self.animFrame),
						   sourceHeight=h, opacity=self.alpha}
		
		for k=1,numTilesX do
			for l=1,numTilesY do
				drawProps.x = mX + (k-2)*(w+self.gapX)
				drawProps.y = mY + (l-1)*(h+self.gapY)
				Graphics.draw (drawProps)
			end
		end
	end	
end


function paralX.onLoop ()
	for k,v in pairs(indexedParallaxes) do
		v:update ()
	end
end

function paralX.onCameraUpdate ()
	for k,v in pairs(indexedParallaxes) do
		v:draw ()
	end
end


return paralX;