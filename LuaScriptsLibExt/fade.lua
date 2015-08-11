local __title = "Fade Class";
local __version = "1.0";
local __description = "Provides simple fading effects";
local __author = "Kevsoft";
local __url = "https://github.com/Wohlhabend-Networks/LunaDLL/tree/master/LuaScriptsLibExt";

local FadeFactory = {}

local function hexToRGBA(hexVal)
    local r = bit.rshift(bit.band(hexVal, 0xFF000000), 24)
    local g = bit.rshift(bit.band(hexVal, 0x00FF0000), 16)
    local b = bit.rshift(bit.band(hexVal, 0x0000FF00), 8)
    local a = bit.band(hexVal, 0x000000FF)
    return r, g, b, a
end


local FadeClass_mt = {}
FadeClass_mt.__index = FadeClass_mt

function FadeClass_mt:onHUDDraw()
    if(not self.isActive)then return end
    
    local r = bit.lshift(math.floor(self.transitionCurrentRed + 0.5), 24)
    local g = bit.lshift(math.floor(self.transitionCurrentGreen + 0.5), 16)
    local b = bit.lshift(math.floor(self.transitionCurrentBlue + 0.5), 8)
    local a = math.floor(self.transitionCurrentAlpha + 0.5)
    
    Graphics.glSetTextureRGBA(nil, bit.bor(r, g, b, a))
    local glTriangles = {
        self.y,
        self.x,              self.y + self.height,
        self.x + self.width, self.y,
        self.x + self.width, self.y + self.height,
        self.x,              self.y + self.height,
        self.x + self.width, self.y
    }
    glTriangles[0] = self.x
    
    Graphics.glDrawTriangles(glTriangles, {}, 6);
    Graphics.glSetTextureRGBA(nil, 0xFFFFFFFF);
    
    if(self.isPause)then return end
    
    local transitionStepRed = (self.transitionStartRed - self.transitionEndRed) / self.transitionTime
    local transitionStepGreen = (self.transitionStartGreen - self.transitionEndGreen) / self.transitionTime
    local transitionStepBlue = (self.transitionStartBlue - self.transitionEndBlue) / self.transitionTime
    local transitionStepAlpha = (self.transitionStartAlpha - self.transitionEndAlpha) / self.transitionTime
    
    
    
    if(not self:isTransitionRedFinished())then 
        self.transitionCurrentRed = self.transitionCurrentRed - transitionStepRed 
    end
    if(not self:isTransitionGreenFinished())then 
        self.transitionCurrentGreen = self.transitionCurrentGreen - transitionStepGreen
    end
    if(not self:isTransitionBlueFinished())then
        self.transitionCurrentBlue = self.transitionCurrentBlue - transitionStepBlue
    end
    if(not self:isTransitionAlphaFinished())then
        self.transitionCurrentAlpha = self.transitionCurrentAlpha - transitionStepAlpha
    end
    
    if(self:isTransitionRedFinished())then
        self.transitionCurrentRed = self.transitionEndRed
    end
    if(self:isTransitionGreenFinished())then
        self.transitionCurrentGreen = self.transitionEndGreen
    end
    if(self:isTransitionBlueFinished())then
        self.transitionCurrentBlue = self.transitionEndBlue
    end
    if(self:isTransitionAlphaFinished())then
        self.transitionCurrentAlpha = self.transitionEndAlpha
    end
    
end

function FadeClass_mt:isTransitionRedFinished()
    if(self.transitionStartRed > self.transitionEndRed)then
        return self.transitionEndRed >= self.transitionCurrentRed
    else
        return self.transitionEndRed <= self.transitionCurrentRed
    end
end

function FadeClass_mt:isTransitionGreenFinished()
    if(self.transitionStartGreen > self.transitionEndGreen)then
        return self.transitionEndGreen >= self.transitionCurrentGreen
    else
        return self.transitionEndGreen <= self.transitionCurrentGreen
    end
end

function FadeClass_mt:isTransitionBlueFinished()
    if(self.transitionStartBlue > self.transitionEndBlue)then
        return self.transitionEndBlue >= self.transitionCurrentBlue
    else
        return self.transitionEndBlue <= self.transitionCurrentBlue
    end
end

function FadeClass_mt:isTransitionAlphaFinished()
    if(self.transitionStartAlpha > self.transitionEndAlpha)then
        return self.transitionEndAlpha >= self.transitionCurrentAlpha
    else
        return self.transitionEndAlpha <= self.transitionCurrentAlpha
    end
end

function FadeClass_mt:reset()
    self.transitionCurrentRed = self.transitionStartRed
    self.transitionCurrentGreen = self.transitionStartGreen
    self.transitionCurrentBlue = self.transitionStartBlue
    self.transitionCurrentAlpha = self.transitionStartAlpha
end

function FadeClass_mt:start()
    self:reset()
    self.isActive = true
    self.isPause = false
end

function FadeClass_mt:pause()
    self.isPause = true
end

function FadeClass_mt:continue()
    self.isActive = true
    self.isPause = false
end

function FadeClass_mt:stop()
    self.isActive = false
end



function FadeClass_mt:setStart(r, g, b, a)
    local typeOfR = type(r)
    if(g == nil)then
        if(typeOfR ~= "number")then
            error("Arg #1 colorVal is not a number (got " .. typrOfR .. ")", 2)
        end
        
        local rVal, gVal, bVal, aVal = hexToRGBA(r)
        self.transitionStartRed = rVal
        self.transitionStartGreen = gVal
        self.transitionStartBlue = bVal
        self.transitionStartAlpha = aVal
    else
        local typeOfG = type(g)
        local typeOfB = type(b)
        local typeOfA = type(a)
        if(typeOfR ~= "number")then
            error("Arg #1 r is not a number (got " .. typrOfR .. ")", 2)
        end
        if(typeOfG ~= "number")then
            error("Arg #2 g is not a number (got " .. typeOfG .. ")", 2)
        end
        if(typeOfB ~= "number")then
            error("Arg #3 b is not a number (got " .. typeOfB .. ")", 2)
        end
        if(typeOfA ~= "number")then
            error("Arg #4 a is not a number (got " .. typeOfA .. ")", 2)
        end
        
        if r < 0 or r > 255 then error("Arg #1 must be between 0 and 255 (got " .. r .. ")") end
        if g < 0 or b > 255 then error("Arg #2 must be between 0 and 255 (got " .. g .. ")") end
        if b < 0 or b > 255 then error("Arg #3 must be between 0 and 255 (got " .. b .. ")") end
        if a < 0 or a > 255 then error("Arg #4 must be between 0 and 255 (got " .. a .. ")") end
        
        
        self.transitionStartRed = r
        self.transitionStartGreen = g
        self.transitionStartBlue = b
        self.transitionStartAlpha = a
    end
    
end

function FadeClass_mt:setEnd(r, g, b, a)
    local typeOfR = type(r)
    if(g == nil)then
        if(typeOfR ~= "number")then
            error("Arg #1 colorVal is not a number (got " .. typrOfR .. ")", 2)
        end
        
        local rVal, gVal, bVal, aVal = hexToRGBA(r)
        self.transitionEndRed = rVal
        self.transitionEndGreen = gVal
        self.transitionEndBlue = bVal
        self.transitionEndAlpha = aVal
    else
        local typeOfG = type(g)
        local typeOfB = type(b)
        local typeOfA = type(a)
        if(typeOfR ~= "number")then
            error("Arg #1 r is not a number (got " .. typrOfR .. ")", 2)
        end
        if(typeOfG ~= "number")then
            error("Arg #2 g is not a number (got " .. typeOfG .. ")", 2)
        end
        if(typeOfB ~= "number")then
            error("Arg #3 b is not a number (got " .. typeOfB .. ")", 2)
        end
        if(typeOfA ~= "number")then
            error("Arg #4 a is not a number (got " .. typeOfA .. ")", 2)
        end
        
        if r < 0 or r > 255 then error("Arg #1 must be between 0 and 255 (got " .. r .. ")") end
        if g < 0 or b > 255 then error("Arg #2 must be between 0 and 255 (got " .. g .. ")") end
        if b < 0 or b > 255 then error("Arg #3 must be between 0 and 255 (got " .. b .. ")") end
        if a < 0 or a > 255 then error("Arg #4 must be between 0 and 255 (got " .. a .. ")") end
        
        self.transitionEndRed = r
        self.transitionEndGreen = g
        self.transitionEndBlue = b
        self.transitionEndAlpha = a
    end
end

function FadeClass_mt:isFinished()
    return self:isTransitionRedFinished() and self:isTransitionGreenFinished() and self:isTransitionBlueFinished() and self:isTransitionAlphaFinished()
end

function FadeClass_mt:setTransitionTime(transitionTime)
    self.transitionTime = transitionTime
end

local createdFadeRegister = {}

function FadeFactory.onInitAPI() 
    registerEvent(FadeFactory, "onHUDDraw", nil, false)
end

function FadeFactory.onHUDDraw()
    for _, v in pairs(createdFadeRegister) do
        v:onHUDDraw()
    end
end

setmetatable(FadeFactory, {
    __call = function(_t)
        local newFade = {}
        setmetatable(newFade, FadeClass_mt)
        
        newFade.x = 0
        newFade.y = 0
        newFade.width = 800
        newFade.height = 600
        newFade.isActive = false
        newFade.isPause = false
        
        newFade.transitionStartRed = 0xFF
        newFade.transitionStartGreen = 0xFF
        newFade.transitionStartBlue = 0xFF
        newFade.transitionStartAlpha = 0xFF
        
        newFade.transitionEndRed = 0
        newFade.transitionEndGreen = 0
        newFade.transitionEndBlue = 0
        newFade.transitionEndAlpha = 0
        
        newFade.transitionCurrentRed = 0xFF
        newFade.transitionCurrentGreen = 0xFF
        newFade.transitionCurrentBlue = 0xFF
        newFade.transitionCurrentAlpha = 0xFF
        
        newFade.transitionTime = 65
        
        
        table.insert(createdFadeRegister, newFade)
        return newFade
    end
})

return FadeFactory