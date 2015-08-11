local __title = "Animation set class";
local __version = "1.0";
local __description = "Provides a class to manage a sequence of images to an animation set. This can then be used for creating simple animations.";
local __author = "Kevsoft";
local __url = "https://github.com/Wohlhabend-Networks/LunaDLL/tree/master/LuaScriptsLibExt";

-- util:
local function scanNextPart(resourceImage, pixData, width, height, probeX, probeY, probeWidth, probeHeight, scanSkipColorData)
    if(probeX + probeWidth > width) then return nil end
    if(probeY + probeHeight > height) then return nil end
    
    --
    local isEmptyFrame = true
    for y = probeY, probeY + probeHeight do
        for x = probeX, probeX + probeWidth do
            local b = pixData[(y * width + x) * 4 + 1]
            local g = pixData[(y * width + x) * 4 + 2]
            local r = pixData[(y * width + x) * 4 + 3]
            local a = pixData[(y * width + x) * 4 + 4]
            
            if(scanSkipColorData[1] ~= -1) then isEmptyFrame = isEmptyFrame and b == scanSkipColorData[1] end
            if(scanSkipColorData[2] ~= -1) then isEmptyFrame = isEmptyFrame and g == scanSkipColorData[2] end
            if(scanSkipColorData[3] ~= -1) then isEmptyFrame = isEmptyFrame and r == scanSkipColorData[3] end
            if(scanSkipColorData[4] ~= -1) then isEmptyFrame = isEmptyFrame and a == scanSkipColorData[4] end
            
            if(not isEmptyFrame) then break end
        end
    end
    
    
    if(isEmptyFrame) then return nil end
    
    
    return {img = resourceImage, sx = probeX, sy = probeY, sw = probeWidth, sh = probeHeight}
end



-- SimpleAnimationSet-Class
local SimpleAnimationSet_mt = {}
SimpleAnimationSet_mt.__index = SimpleAnimationSet_mt

function SimpleAnimationSet_mt.__concat(lhs, rhs)
    local newAnimationSet = {}
    setmetatable(newAnimationSet, SimpleAnimationSet_mt)
    
    for _, v in pairs(lhs) do
        table.insert(newAnimationSet, {img = v.img, sx = v.sx, sy = v.sy, sw = v.sw, sh = v.sh})
    end
    
    for _, v in pairs(rhs) do
        table.insert(newAnimationSet, {img = v.img, sx = v.sx, sy = v.sy, sw = v.sw, sh = v.sh})
    end
    
    return newAnimationSet 
end

function SimpleAnimationSet_mt:drawImage(x, y, frame)
    local typeOfX = type(x)
    local typeOfY = type(y)
    local typeOfFrame = type(frame)
    if(typeOfX ~= "number")then
        error("Arg #1 x expected number (got " .. typeOfX .. ")", 2)
    end
    if(typeOfY ~= "number")then
        error("Arg #1 y expected number (got " .. typeOfY .. ")", 2)
    end
    if(typeOfFrame ~= "number")then
        error("Arg #3 frame expected number (got " .. typeOfFrame .. ")", 2)
    end
    if(#self < frame)then
        error("Arg #3 frame is out of bounds! Having " .. #self .. " frames (got frame index " .. frame .. ")", 2)
    end
    
    local frameObject = self[frame]
    Graphics.drawImage(frameObject.img, x, y, frameObject.sx, frameObject.sy, frameObject.sw, frameObject.sh)
end

-- Factory/Main API-Class
local AnimationSetFactory = {}
function AnimationSetFactory.readSpritesheet(imgResource, scanWidth, scanHeight, backgroundColorData)
    local output = {}
    
    local typeOfImgResource = type(imgResource)
    local typeOfScanWidth = type(scanWidth)
    local typeOfScanHeight = type(scanHeight)
    local typeOfBackgroundColor = type(backgroundColorData)
    if(typeOfImgResource ~= "userdata")then
        error("Arg #1 imgResource expected LuaResourceImage (got " .. typeOfImgResource .. ")", 2)
    end
    if(typeOfScanWidth ~= "number")then
        error("Arg #2 imgResource expected number (got " .. typeOfScanWidth .. ")", 2)
    end
    if(typeOfScanHeight ~= "number")then
        error("Arg #3 imgResource expected number (got " .. typeOfScanHeight .. ")", 2)
    end
    if(typeOfBackgroundColor ~= "table" and typeOfBackgroundColor ~= "nil")then
        error("Arg #4 backgroundColorData expected table (got " .. typeOfBackgroundColor .. ")", 2)
    end
    
    local pixData, width, height = Graphics.getPixelData(imgResource)
    
    if(scanWidth > width)then
        error("scanWidth is bigger than the actual image width!", 2)
    end
    if(scanHeight > height)then
        error("scanHeight is bigger than the actual image height!", 2)
    end
    
    local scanSkipColorData = backgroundColorData or {-1, -1, -1, 0}
    local colsToScan = math.floor(width / scanWidth)
    local rowsToScan = math.floor(height / scanHeight)
    
    for y = 0, rowsToScan - 1 do 
        local nextAnimationSet = {}
        setmetatable(nextAnimationSet, SimpleAnimationSet_mt)
        
        for x = 0, colsToScan - 1 do
            local nextImgPart = scanNextPart(imgResource, pixData, width, height, x * scanWidth, y * scanHeight, scanWidth, scanHeight, scanSkipColorData)
            if(nextImgPart)then
                table.insert(nextAnimationSet, nextImgPart)
            else
                break
            end
        end
        
        if(#nextAnimationSet > 0)then
            table.insert(output, nextAnimationSet)
        end
    end
    
    return output
end

function AnimationSetFactory.fromImages(imgs)
    local typeOfImgs = type(imgs)
    if(typeOfImgs ~= "table")then
        error("Arg #1 imgResource expected table (got " .. typeOfImgs .. ")", 2)
    end
    
    local animationSet = {}
    setmetatable(animationSet, SimpleAnimationSet_mt)
    
    for _, v in pairs(imgs)do
        local __, width, height = Graphics.getPixelData(v)
        table.insert(animationSet, {img = v, sx = 0, sy = 0, sw = width, sh = height})
    end
    
    return animationSet
end

function AnimationSetFactory.fromSubImages(imgs)
    local typeOfImgs = type(imgs)
    if(typeOfImgs ~= "table")then
        error("Arg #1 imgResource expected table (got " .. typeOfImgs .. ")", 2)
    end
    
    local animationSet = {}
    setmetatable(animationSet, SimpleAnimationSet_mt)
    
    for _, v in pairs(imgs)do
        table.insert(animationSet, {img = v.img, sx = v.sx, sy = v.sy, sw = v.sw, sh = v.sh})
    end
    
    return animationSet
end

return AnimationSetFactory
