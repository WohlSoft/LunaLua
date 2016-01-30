--**************************************
--                                     *
--  pSwitchTicking.lua                 *
--  v1.0                               *
--                                     *
--  pSwitch Ticking Behaviour for A2XT *
--                                     *
--**************************************

local pSwitchTicking = {}

local tickingChn = nil
local tickingSfx = nil

function pSwitchTicking.onInitAPI()
    Defines.pswitch_music = false
    tickingSfx = Audio.SfxOpen(Misc.resolveFile("pSwitchTicking/pSwitchTicking.ogg"))
    registerEvent(pSwitchTicking, "onEventDirect", "onEventDirect", true)
end

function pSwitchTicking.onEventDirect(eventObj, eventName)
    if (eventName == "P Switch - Start") then
        if (tickingChn == nil) then
            tickingChn = Audio.SfxPlayCh(-1, tickingSfx, -1)
        end
    elseif (eventName == "P Switch - End") then
        if (tickingChn ~= nil) then
            Audio.SfxFadeOut(tickingChn, 50)
            tickingChn = nil
        end
    end
end

return pSwitchTicking
