local classicEvents = {}
local playerData = {}
local playerKeymapProperties = {}
playerKeymapProperties[KEY_UP] = "upKeyPressing"
playerKeymapProperties[KEY_DOWN] = "downKeyPressing"
playerKeymapProperties[KEY_LEFT] = "leftKeyPressing"
playerKeymapProperties[KEY_RIGHT] = "rightKeyPressing"
playerKeymapProperties[KEY_JUMP] = "jumpKeyPressing"
playerKeymapProperties[KEY_SPINJUMP] = "altJumpKeyPressing"
playerKeymapProperties[KEY_X] = "runKeyPressing" -- Maybe use a better name?
playerKeymapProperties[KEY_RUN] = "altRunKeyPressing" -- Maybe use a better name?
playerKeymapProperties[KEY_SEL] = "dropItemKeyPressing"
playerKeymapProperties[KEY_STR] = "pauseKeyPressing"

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
        for _,keymapPropertyName in pairs(playerKeymapProperties) do
            plData[keymapPropertyName] = false
        end
        
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
        for keymapEnumValue,keymapPropertyName in pairs(playerKeymapProperties) do
            checkKeyboardEvent(plObject, plIndex, plData, keymapPropertyName, keymapEnumValue)
        end
        
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
        for _,keymapPropertyName in pairs(playerKeymapProperties) do
            plData[keymapPropertyName] = plObject[keymapPropertyName]
        end
        
        plData.playerJumping = plObject:mem(0x60, FIELD_WORD) == -1
        
        plData.currentSection = plObject.section
    end
end

return classicEvents
