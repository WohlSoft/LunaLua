local classicEvents = {}
local playerData = {}


function classicEvents.onInitAPI()
    if(not isOverworld)then
        playerData[1] = {}
        if(player2)then
            if(player2.isValid)then
                playerData[2] = {}
            end
        end
    end
    
    for plIndex, plData in pairs(playerData) do
        plData.upKeyPressing = false
        plData.downKeyPressing = false
        plData.leftKeyPressing = false
        plData.rightKeyPressing = false
        plData.jumpKeyPressing = false
        plData.altJumpKeyPressing = false
        plData.runKeyPressing = false
        plData.altRunKeyPressing = false
        plData.dropItemKeyPressing = false
        plData.pauseKeyPressing = false
        
        plData.playerJumping = false
        
        plData.currentSection = -1
    end
end


local function checkKeyboardEvent(plObject, plIndex, plData, plFieldName, plFieldID)
    if(plData[plFieldName] == false and plObject[plFieldName] == true)then
        EventManager.callEvent("onKeyDown", plFieldID, plIndex)
        return
    end
    if(plData[plFieldName] == true and plObject[plFieldName] == false)then
        EventManager.callEvent("onKeyUp", plFieldID, plIndex)
        return
    end
end


-- FIXME: MusicManager::setCurrentSection((int)player->CurrentSection);
function classicEvents.doEvents()
    for plIndex, plData in pairs(playerData) do
        local plObject = Player.get()[plIndex]
        checkKeyboardEvent(plObject, plIndex, plData, "upKeyPressing", KEY_UP)
        checkKeyboardEvent(plObject, plIndex, plData, "downKeyPressing", KEY_DOWN)
        checkKeyboardEvent(plObject, plIndex, plData, "leftKeyPressing", KEY_LEFT)
        checkKeyboardEvent(plObject, plIndex, plData, "rightKeyPressing", KEY_RIGHT)
        checkKeyboardEvent(plObject, plIndex, plData, "jumpKeyPressing", KEY_JUMP)
        checkKeyboardEvent(plObject, plIndex, plData, "altJumpKeyPressing", KEY_SPINJUMP)
        checkKeyboardEvent(plObject, plIndex, plData, "runPressing", KEY_X) -- Maybe use a better name?
        checkKeyboardEvent(plObject, plIndex, plData, "altRunPressing", KEY_RUN)  -- Maybe use a better name?
        checkKeyboardEvent(plObject, plIndex, plData, "dropItemKeyPressing", KEY_SEL)
        checkKeyboardEvent(plObject, plIndex, plData, "pauseKeyPressing", KEY_STR)
        
        if(plObject:mem(0x60, FIELD_WORD) == -1 and plData.playerJumping == false)then
            EventManager.callEvent("onJump", plIndex)
        elseif(plObject:mem(0x60, FIELD_WORD) == 0 and plData.playerJumping == true)then
            EventManager.callEvent("onJumpEnd", plIndex)
        end
        
        if(plObject.section ~= plData.currentSection)then
            local evLoadSecitionName = "onLoadSection"
            EventManager.callEvent(evLoadSecitionName, plIndex)
            EventManager.callEvent(evLoadSecitionName .. plObject.section, plIndex)
        end
        EventManager.callEvent("onLoopSection" .. plObject.section, plIndex)
        
        -- Copy new data here to plData
        plData.upKeyPressing = plObject.upKeyPressing
        plData.downKeyPressing = plObject.downKeyPressing
        plData.leftKeyPressing = plObject.leftKeyPressing
        plData.rightKeyPressing = plObject.rightKeyPressing
        plData.jumpKeyPressing = plObject.jumpKeyPressing
        plData.altJumpKeyPressing = plObject.upKeyPrealtJumpKeyPressingssing
        plData.runPressing = plObject.runPressing
        plData.altRunPressing = plObject.altRunPressing
        plData.dropItemKeyPressing = plObject.dropItemKeyPressing
        plData.pauseKeyPressing = plObject.pauseKeyPressing
        
        plData.playerJumping = plObject:mem(0x60, FIELD_WORD) == -1
        
        plData.currentSection = plObject.section
    end
end

return classicEvents
