--***************************************************************************--
--*██████╗  ██████╗  ██████╗ ███████╗██╗██╗  ██╗   ██╗     ██╗   ██╗ █████╗ *--
--*██╔══██╗██╔════╝ ██╔═══██╗██╔════╝██║╚██╗██╔╝   ██║     ██║   ██║██╔══██╗*--
--*██████╔╝██║  ███╗██║   ██║█████╗  ██║ ╚███╔╝    ██║     ██║   ██║███████║*--
--*██╔══██╗██║   ██║██║   ██║██╔══╝  ██║ ██╔██╗    ██║     ██║   ██║██╔══██║*--
--*██████╔╝╚██████╔╝╚██████╔╝██║     ██║██╔╝ ██╗██╗███████╗╚██████╔╝██║  ██║*--
--*╚═════╝  ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝*--
--***************************************************************************--
--* A small library for rendering BGOs of custom sizes and changing render order
--* Coded by Sambo, Oct. 2016.
--* NOTE: You must use PNG CGFX for this to work. LunaLua currently doesn't support drawing of masked GIFs.
--***************************************************************************--

local bgofix = {}

local defaults = {}
defaults.foreground = {23,24,25,36,45,46,49,50,51,68,69,187,188,189,190}
defaults.veryBackBgos = {11,12,75,76,77,78,79}
defaults.specialBGOs = {98,160}

local bgos = {} -- Table of unique BGO IDs and their custom properties
local blank = Graphics.loadImage(Misc.resolveFile("bgofix/blank.png"))
local animData = {}

function bgofix.onInitAPI()
	registerEvent(bgofix, "onStart", "initialize")
	registerEvent(bgofix, "onDraw", "draw")
end

--[[********************************************
  * Load the custom image for each BGO if it exists in .PNG format
  * This will rather redundantly reload images that are already in the memory of SMBX; however, there is no workaround for this issue right now.
  ********************************************]]
local function loadImg(id)
	local imageName = Misc.resolveFile("background-" .. id .. ".png") -- Search level and episode directorties
	local image;
	if imageName then
		image = Graphics.loadImage(imageName)
	end
	if image then return image;
	else return nil; end;
end

--[[********************************************
  * Set the given BGO to be drawn over if a valid custom image exists
  * and it is larger than the original in either dimension.
  ********************************************]]
local function setShouldDraw(bgo, image)
	if image then
		if (image.width ~= bgo.width) or (image.height ~= image.height) then
			return true;
		else
			return false;
		end
	end
end

--[[********************************************
  * Get the default render priority for the BGO with the given ID
  ********************************************]]
local function getDefault(id) -- Test sets from most likely to least likely, except for the largest set, to avoid as many tests as possible.
	for _,v in pairs(defaults.foreground) do
		if id == v then
			return -20;
		end
	end
	for _,v in pairs(defaults.veryBackBgos) do
		if id == v then
			return -95;
		end
	end
	for _,v in pairs(defaults.specialBGOs) do
		if id == v then
			return -80;
		end
	end
	return -85;
end

--[[********************************************
  * Set the render priority to the default value or to a custom value inputted by the user
  * Note: This uses LunaLua render priorities, which are not equivalent to SMBX sort-order priorities!
  * This could change the render order from how it is normally. If this occurs, use setPriority to fix it.
  * This will only work if called in onStart!
  ********************************************]]
function bgofix.setPriority(id, priority)
	assert(bgos[id].image, "Cannot change the render priority of BGO-" .. id .. " because it doesn't have a .PNG custom image.") -- Make sure the user didn't break the rules.
	if (priority) and not (bgos[id].shouldDraw) then
		bgos[id].shouldDraw = true -- Handling for the user changing the render priority of BGOs which are the same size as the original
	end

	bgos[id].priority = priority or getDefault(id) -- fetch default setting if custom setting is not given.
	Graphics.sprites.background[id].img = blank -- Make the actual BGO invisible. This is to allow the user to make the BGO's render priority lower.
end

--[[********************************************
  * Resize the given BGO to have the same dimensions as its image
  ********************************************]]
local function resize(bgo, image)
	if ((image) and (bgo.width ~= image.width or bgo.height ~= image.height)) then
		bgo.width  = image.width
		bgo.height = image.height
	end
end

--[[********************************************
  * Initialization
  ********************************************]]
function bgofix.initialize()

	for _,bgo in ipairs(BGO.get()) do
		if not bgos[bgo.id] then -- Do this only once for each BGO with a unique ID.
			bgos[bgo.id] = {}
			bgos[bgo.id].image = loadImg(bgo.id) -- custom image (nil if no valid image)
			bgos[bgo.id].shouldDraw = setShouldDraw(bgo, bgos[bgo.id].image); -- Does the BGO need to be rendered over?
			
			
			if bgos[bgo.id].shouldDraw then
				bgofix.setPriority(bgo.id) -- LunaLua render priority
			end
			
		end
		
		if bgos[bgo.id] then
			resize(bgo, bgos[bgo.id].image);
		end
		
	end
end

--[[********************************************
  * Get all BGOs on the screen
  * This is why I bothered to resize any BGOs so they were bigger.
  * If I hadn't done this, BGOs that still appeared to be onscreen would disappear because the object isn't on the screen.
  ********************************************]]
local function getBgosOnScreen()
	local bgosOnScreen = {}
	for _,cam in pairs(Camera.get()) do -- Check this way so it works in 2P mode
		for __,bgo in pairs(BGO.getIntersecting(cam.x, cam.y, cam.x + cam.width, cam.y + cam.height)) do
			if not bgo.isHidden then -- Don't insert BGOs that are on hidden layers!
				table.insert(bgosOnScreen, bgo)
			end
		end
	end
	return bgosOnScreen;
end

--[[********************************************
  * Set a custom animation
  * Animation image is a separate image from the BGO image with the naming style "bgo-anim-*.png," where '*' is the ID of the BGO.
  * This may seem like a weird way to do it, but it is so they will display better in the PGE editor.
  ********************************************]]
function bgofix.setAnimation(id, numFrames, frameSpeed)

	if not bgos[id].shouldDraw then
		bgos[id].shouldDraw = true
		bgos[id].priority = getDefault(id)
		Graphics.sprites.background[id].img = blank
	end
	
	animData[id] = {}
	
	local imageName = Misc.resolveFile("bgo-anim-" .. id .. ".png")
	if imageName then
		animData[id].image = Graphics.loadImage(imageName)
	else
		error("Animation image for background-" .. id .. " not found.")
	end
	
	animData[id].frames = numFrames or 4
	animData[id].frameSpeed = frameSpeed or 8 -- The number of ticks per frame for the BGO. Default = 8
	animData[id].frame = 0 -- Current frame. Reset to 0 when this == numFrames - 1
	animData[id].tick = 0 -- Tick counter. Reset to 0 and move to the next frame when this == frameSpeed
	
	assert(animData[id].frames > 0, "Invalid number of frames for BGO-" .. id .. ". Value must be greater than 0.")
	assert(animData[id].frameSpeed > 0, "Invalid frame speed value for BGO-" .. id .. ". Value must be greater than 0.")
end

--[[********************************************
  * Update the frame numbers of the BGOs with custom animations
  ********************************************]]
local function updateAnimations()
	for id,_ in pairs(animData) do
		-- Compact frame controller
		-- Why doesn't anyone use the modulus operator??
		animData[id].frame = math.floor(animData[id].tick / animData[id].frameSpeed)
		animData[id].tick = (animData[id].tick + 1) % (animData[id].frames * animData[id].frameSpeed)
	end
end

--[[********************************************
  * Draw over the BGOs that are on the screen
  ********************************************]]
function bgofix.draw()	
	-- Draw all BGOs on the screen that are listed for drawing
	for _,bgo in pairs(getBgosOnScreen()) do
		if bgos[bgo.id].shouldDraw then
			if animData[bgo.id] then -- Draw animations for BGOs that have them
				Graphics.drawImageToSceneWP(animData[bgo.id].image, bgo.x, bgo.y, 0, bgo.height * animData[bgo.id].frame, bgo.width, bgo.height, bgos[bgo.id].priority)
				--							image					position		source image position					dimensions			priority
			else
				Graphics.drawImageToSceneWP(bgos[bgo.id].image, bgo.x, bgo.y, bgos[bgo.id].priority)
			end
		end
	end
	updateAnimations();
end

return bgofix