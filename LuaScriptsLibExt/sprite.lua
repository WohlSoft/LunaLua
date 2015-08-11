local __title = "Animated Sprite Utility Class";
local __version = "1.0";
local __description = "Provides functions for placing (animated) sprites with movement ability.";
local __author = "Kevsoft";
local __url = "https://github.com/Wohlhabend-Networks/LunaDLL/tree/master/LuaScriptsLibExt";

local SpriteFactory = {
    COOR_CAMERA = 0,
    COOR_SCENE = 1
}

local function validateAnimationSet(vTable)
    for k,v in pairs(vTable) do
        local typeOfImg = type(v.img)
        local typeOfSX = type(v.sx)
        local typeOfSY = type(v.sy)
        local typeOfSW = type(v.sw)
        local typeOfSH = type(v.sh)
        
        if(typeOfImg ~= "userdata")then error("AnimationSet #".. k .. "does not contain LuaResourceImage field .img! (got " .. typeOfImg .. ")") end
        if(typeOfSX ~= "number")then error("AnimationSet #".. k .. "does not contain number field .sx! (got " .. typeOfSX .. ")") end
        if(typeOfSY ~= "number")then error("AnimationSet #".. k .. "does not contain number field .sy! (got " .. typeOfSY .. ")") end
        if(typeOfSW ~= "number")then error("AnimationSet #".. k .. "does not contain number field .sw! (got " .. typeOfSW .. ")") end
        if(typeOfSH ~= "number")then error("AnimationSet #".. k .. "does not contain number field .sh! (got " .. typeOfSH .. ")") end  
    end
end


local function validateAnimationSetMap(vTable)
    for k,v in pairs(vTable) do
        local typeOfKey = type(k)
        local typeOfValue = type(v)
        if(typeOfKey ~= "string")then
            error("One of the elements in the animation map does not contain a string key!\nType of Key: " .. typeOfKey .. "\nType of Value: " .. typeOfValue)
        end
        if(typeOfValue ~= "table")then
            error("One of the elements in the animation map does not contain a animation array table value!\nType of Key: " .. typeOfKey .. "\nType of Value: " .. typeOfValue)
        end
        
        validateLuaResourceImageArray(v)
    end
end


local SpriteClass_mt = {}
SpriteClass_mt.__index = SpriteClass_mt

function SpriteClass_mt:onLoop()
    self.x = self.x + self.speedX
    self.y = self.y + self.speedY
    
    local currentAnimationSet = self:getCurrentAnimationSet()
    if(#currentAnimationSet == 0)then
        return
    end
    
    if(self.frameTimerCurrent == self.frameTimerSpeed)then
        if(self.currentAnimationSetFrame >= #currentAnimationSet)then
            self.currentAnimationSetFrame = 1
        else
            self.currentAnimationSetFrame = self.currentAnimationSetFrame + 1
        end
    end
    
    if(self.frameTimerCurrent == self.frameTimerSpeed)then
        self.frameTimerCurrent = 1
    else
        self.frameTimerCurrent = self.frameTimerCurrent + 1
    end
end

function SpriteClass_mt:onHUDDraw()
    local currentAnimationSet = self.animationSets[self.currentAnimationSet]
    if(type(currentAnimationSet) ~= "table")then
        error("Internal Error: Current animation set does not exist in the animation sets of the sprite")
    end
    if(#currentAnimationSet == 0)then
        return
    end
    
    if(self.isShown)then
        local nextFrameImage = currentAnimationSet[self.currentAnimationSetFrame]
        if(not nextFrameImage)then error("The current frame in the current animation set is empty!") end
        if(self.mode == SpriteFactory.COOR_CAMERA)then
            Graphics.drawImage(nextFrameImage.img, self.x, self.y, nextFrameImage.sx, nextFrameImage.sy, nextFrameImage.sw, nextFrameImage.sh)
        elseif(self.mode == SpriteFactory.COOR_SCENE)then
            Graphics.drawImageToScene(nextFrameImage.img, self.x, self.y, nextFrameImage.sx, nextFrameImage.sy, nextFrameImage.sw, nextFrameImage.sh)
        end
    end
end

function SpriteClass_mt:show()
    self.isShown = true
end

function SpriteClass_mt:hide()
    self.isShown = false
end

function SpriteClass_mt:toggle()
    self.isShown = not self.isShown
end

function SpriteClass_mt:isVisible()
    return self.isShown
end

function SpriteClass_mt:getCurrentFrame()
    return self.currentAnimationSetFrame
end

function SpriteClass_mt:getFrameSpeed()
    return self.frameTimerSpeed
end

function SpriteClass_mt:getCurrentAnimationSet()
    local currentAnimationSet = self.animationSets[self.currentAnimationSet]
    if(type(currentAnimationSet) ~= "table")then
        error("Internal Error: Current animation set does not exist in the animation sets of the sprite")
    end
    return currentAnimationSet
end

function SpriteClass_mt:getCurrentAnimationSetName()
    return self.currentAnimationSet
end

function SpriteClass_mt:addAnimationSet(name, animationSet)
    validateAnimationSet(animationSet)
    self.animationSets[name] = animationSet
end

function SpriteClass_mt:addAnimationSets(animationSetMap)
    validateAnimationSetMap(animationSetMap)
    for animSetName, animSet in pairs(animationSetMap) do
        self:addAnimationSet(animSetName, animSet)
    end
end

function SpriteClass_mt:removeAnimationSet(name)
    self.animationSets[name] = nil
end

function SpriteClass_mt:getAnimationSet(name)
    return self.animationSets[name]
end

function SpriteClass_mt:setCurrentFrame(currentFrame)
    local currentAnimationSet = self:getCurrentAnimationSet()
    local frames = #currentAnimationSet
    if(currentFrame > frames or currentFrame < 1)then
        error("Frame is not valid! Animation set \"" .. self:getCurrentAnimationSetName() .. "\" has frame index from 1 to " .. frames .. " (got " .. currentFrame .. ")")
    end
    self.currentAnimationSetFrame = currentFrame
end

function SpriteClass_mt:setFrameSpeed(frameSpeed)
    if(frameSpeed < 1)then
        error("Frame speed not valid! The frame speed must be higher than 1!")
    end
    self.frameTimerSpeed = frameSpeed
end




local createdSpriteRegister = {}
--setmetatable(createdSpriteRegister, { __mode = "v" })


    
function SpriteFactory.onInitAPI() 
    registerEvent(SpriteFactory, "onLoop", nil, false)
    registerEvent(SpriteFactory, "onHUDDraw", nil, false)
end

function SpriteFactory.onLoop()
    for _, v in pairs(createdSpriteRegister) do
        v:onLoop()
    end
end

function SpriteFactory.onHUDDraw()
    for _, v in pairs(createdSpriteRegister) do
        v:onHUDDraw()
    end
end

setmetatable(SpriteFactory, {

    --[[    Create new instance
            Sprite([mode, x, y, speedX, speedY])
            Sprite(LuaResourceImage, [mode, x, y, speedX, speedY])
            Sprite({LuaResourceImage...}, [mode, x, y, speedX, speedY])
            Sprite({ someAnim = {LuaResourceImage...}, someAnim2 = {LuaResourceImage...}, [mode, x, y, speedX, speedY]})
    ]]--
    __call = function(_t, ...)
        local newSprite = {}
        setmetatable(newSprite, SpriteClass_mt)
        
        newSprite.animationSets = {}
        newSprite.currentAnimationSet = "default"
        newSprite.currentAnimationSetFrame = 1
        
        newSprite.frameTimerSpeed = 1
        newSprite.frameTimerCurrent = 1
        
        newSprite.isShown = false
        newSprite.updateSpeed = true
        
        
        local args = {...}
        local firstArg = args[1]
        local firstArgTypeStr = type(firstArg)
        if(#args > 0 and firstArgTypeStr ~= "number")then --If we assum parameters has been passed and the first argument is not a number
            
            
            --[[ Detect argument signature
                1 - Animation set
                2 - Map of Animation set
            ]]
            local firstArgType = 0
            if(firstArgTypeStr ~= "table") then
                error("Arg #1 in Sprite constructor is not a table (got " .. firstArgTypeStr .. ")")
            end
            
            if(#firstArg <= 0)then
                error("Arg #1 in Sprite constructor is an empty table (use nil instead)")
            end
            
            if(type(firstArg[1]) == "table")then
                firstArgType = 1
            else
                firstArgType = 2
            end
            
            if(firstArgType == 1)then
                -- Validate
                validateAnimationSet(firstArg)
                newSprite.animationSets["default"] = firstArg
            elseif(firstArgType == 2)then
                -- Validate
                validateAnimationSetMap(firstArg)
                newSprite.animationSets = firstArg
                for k,v in pairs(newSprite.animationSets) do newSprite.currentAnimationSet = k break end -- Set the first animation set as the current one
            end
            
            table.remove(args, 1)
        else
            newSprite.animationSets["default"] = {}
        end
        
        if(args[1])then if(type(args[1]) ~= "number") then error("Arg #2 \"mode\" expected number, got " .. type(args[1])) end end
        if(args[2])then if(type(args[2]) ~= "number") then error("Arg #3 \"x\" expected number, got " .. type(args[2])) end end
        if(args[3])then if(type(args[3]) ~= "number") then error("Arg #4 \"y\" expected number, got " .. type(args[3])) end end
        if(args[4])then if(type(args[4]) ~= "number") then error("Arg #5 \"speedX\" expected number, got " .. type(args[4])) end end 
        if(args[5])then if(type(args[5]) ~= "number") then error("Arg #6 \"speedY\" expected number, got " .. type(args[5])) end end
        
        if(args[1])then
            if(args[1] ~= SpriteFactory.COOR_CAMERA and args[1] ~= SpriteFactory.COOR_SCENE)then
                error("Arg #2 \"mode\" is not Sprite.COOR_CAMERA or Sprite.COOR_SCENE!")
            end
        end
        
        newSprite.mode = args[1] or SpriteFactory.COOR_CAMERA
        newSprite.x = args[2] or 0
        newSprite.y = args[3] or 0
        newSprite.speedX = args[4] or 0
        newSprite.speedY = args[5] or 0
        
        
        
        table.insert(createdSpriteRegister, newSprite)
        return newSprite
    end

})
return SpriteFactory

