--These are redefines for the wiki
if __wikiGEN then
    FIELD_BYTE = 0
    FIELD_WORD = 1
    FIELD_DWORD = 2
    FIELD_FLOAT = 3
    FIELD_DFLOAT = 4
    FIELD_BOOLEAN = 5
end

--This is a helper function used for the wikiGEN.
local function getKeyOfValue(theTable, theValue)
    for key, value in pairs(theTable) do
        if value == theValue then
            return key
        end
    end
    error("No key for \""..tostring(theValue).."\" found!")
end

local sortedCategories = {
    "Movement",
    "Effect Modification",
    "Sound Modification",
    "Game Exit",
    "Coin Value",
    "Level",
    "Cheats",
    "Player Settings: Link",
    "Block Modification",
    "Kill Modification"
}

local function getBoolean(self)
    return mem(self.address, FIELD_WORD) == -1
end

local function setBoolean(self, value)
    local shortVal = 0
    if(value)then
        shortVal = -1
    end
    mem(self.address, FIELD_WORD, shortVal)
end


-- The define Table
-- Group and desc are only for the wiki generator
local defines = {
    --[[ Movement Defines ]]--
    gravity                         = {defValue = 12 , minVal = 0, maxVal = nil, address = 0x00B2C6F4, size = FIELD_WORD,
                                        n = 1, group = "Movement", desc = "The gravity of the player."},
    earthquake                      = {defValue = 0  , minVal = 0, maxVal = nil, address = 0x00B250AC, size = FIELD_WORD,
                                        n = 2, group = "Movement", desc = "The earthquake factor of the Level. It resets to 0 after time."},
    jumpheight                      = {defValue = 20 , minVal = 0, maxVal = nil, address = 0x00B2C6DC, size = FIELD_WORD,
                                        n = 3, group = "Movement", desc = "The jumpheight of the player."},
    jumpheight_bounce               = {defValue = 20 , minVal = 0, maxVal = nil, address = 0x00B2C6E2, size = FIELD_WORD,
                                        n = 4, group = "Movement", desc = "The jumpheight of the player when bouncing of a enemy."},
    
    --[[ Effect Modification Defines ]]--
    effect_NpcToCoin                = {defValue = 11 , minVal = 0, maxVal = nil, address = 0x00A3C86E, size = FIELD_BYTE,
                                        n = 1, group = "Effect Modification", desc = "The effect ID of the npc-to-coins function (default is the coinflip effect). "..
                                                                                     "The npc-to-coins function is executed when the player hits an exit, which transforms "..
                                                                                     "the npcs into coins."},
    
    --[[ Sound Modification Defines ]]--
    sound_NpcToCoin                 = {defValue = 14 , minVal = 0, maxVal = nil, address = 0x00A3C87F, size = FIELD_BYTE,
                                        n = 1, group = "Sound Modification", desc = "The sound ID of the npc-to-coins function (default is the coin sound). "..
                                                                                    "The npc-to-coins function is executed when the player hits an exit, which transforms "..
                                                                                    "the npcs into coins."},
    
    --[[ Game Exit Defines ]]--
    npcToCoinValue                  = {defValue = 1  , minVal = 0, maxVal = 99 , address = 0x00A3C891, size = FIELD_BYTE,
                                        n = 1, group = "Game Exit", desc = "The coin-value for every destroyed npc in the npc-to-coins function. "..
                                                                                    "The npc-to-coins function is executed when the player hits an exit, which transforms "..
                                                                                    "the npcs into coins."},
    npcToCoinValueReset             = {defValue = 100, minVal = 1, maxVal = 100, address = 0x00A3C8EE, size = FIELD_BYTE,
                                        n = 2, group = "Game Exit", desc = "How many coins get subtracted from the coin-value when the coin value hits 100 coins. "..
                                                                                    "The npc-to-coins function is executed when the player hits an exit, which transforms "..
                                                                                    "the npcs into coins."},
    smb3RouletteScoreValueStar      = {defValue = 10,  minVal = 1, maxVal = 12,  address = 0x00A266AC, size = FIELD_DWORD,
                                        n = 3, group = "Game Exit", desc = "The score value of smb3 roulette star"},
    smb3RouletteScoreValueMushroom  = {defValue = 6,   minVal = 1, maxVal = 12,  address = 0x00A26719, size = FIELD_DWORD,
                                        n = 4, group = "Game Exit", desc = "The score value of smb3 roulette mushroom"},
    smb3RouletteScoreValueFlower    = {defValue = 8,   minVal = 1, maxVal = 12,  address = 0x00A2677E, size = FIELD_DWORD,
                                        n = 5, group = "Game Exit", desc = "The score value of smb3 roulette flower"},    
                                        
    --[[ Coin Value Defines ]]--
    coinValue                       = {defValue = 1  , minVal = 0, maxVal = 99 , address = 0x00A262BD, size = FIELD_BYTE,
                                        n = 1, group = "Coin Value", desc = "How much a coin npc is worth as coins. It will affect every coin-npc which has a 1-coin value "..
                                                                                    "as default (except the npc-to-coins function)."},
    coin5Value                      = {defValue = 5  , minVal = 0, maxVal = 99 , address = 0x00A262C9, size = FIELD_BYTE,
                                        n = 2, group = "Coin Value", desc = "How much a 5-coin npc is worth as coins."},
    coin20Value                     = {defValue = 20 , minVal = 0, maxVal = 99 , address = 0x00A262B7, size = FIELD_BYTE,
                                        n = 3, group = "Coin Value", desc = "How much a 20-coin npc is worth as coins."},
    
    --[[ Level Defines ]]--
    levelFreeze                     = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8B4, size = FIELD_BOOLEAN,
                                        n = 1, group = "Level", desc = "If the level is freezed. (Only you can move!)",
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    
    --[[ Cheat Defines ]]--
    cheat_shadowmario               = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8AA, size = FIELD_BOOLEAN,
                                        n = 1, group = "Cheats", desc = "If the cheat shadowmario/ghostytime is active. If this cheat is active, then the player can go through blocks.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_ahippinandahoppin         = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8AC, size = FIELD_BOOLEAN,
                                        n = 2, group = "Cheats", desc = "If the cheat ahippinandahoppin/jumplikesomething is active. This cheat grants infinite double jumps.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_sonictooslow              = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8AE, size = FIELD_BOOLEAN,
                                        n = 3, group = "Cheats", desc = "If the cheat sonictooslow/chipmunktimes is active. This cheat makes the player run very fast.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_illparkwhereiwant         = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8B0, size = FIELD_BOOLEAN,
                                        n = 4, group = "Cheats", desc = "If the cheat illparkwhereiwant is active. If this cheat is active, then you can move freely on the world map.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_wingman                   = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8B2, size = FIELD_BOOLEAN,
                                        n = 5, group = "Cheats", desc = "If the cheat wingman/peawing is active. This cheat makes the player/yoshi fly infinitly.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_captainn                  = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8B6, size = FIELD_BOOLEAN,
                                        n = 6, group = "Cheats", desc = "If the cheat captainn/juststop is active. If this cheat is active, then you can freeze time with the PAUSE-Button.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_flamerthrower             = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8B8, size = FIELD_BOOLEAN,
                                        n = 7, group = "Cheats", desc = "If the cheat flamerthrower/mchammerdude is active. If this cheat is active, then you can spam projectiles very fast (i.e Fireball, Iceball, Hammer).", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_moneytree                 = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8BA, size = FIELD_BOOLEAN,
                                        n = 8, group = "Cheats", desc = "If the cheat moneytree/havemoney is active. If this cheat is active, then you can spam projectiles very fast (i.e Fireball, Iceball, Hammer).", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_speeddemon                = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8BE, size = FIELD_BOOLEAN,
                                        n = 9, group = "Cheats", desc = "If the cheat speeddemon/maytoofast is active. This values is the same as the \"Max FPS\" setting. If this is active, then there is no frame limit.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_donthurtme                = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8C0, size = FIELD_BOOLEAN,
                                        n = 10, group = "Cheats", desc = "If the cheat donthurtme/strategyyo is active. This values is the same as the \"God Mode\" setting. If this is active, then the player has god mode active and cannot be killed.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    cheat_stickyfingers             = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8C2, size = FIELD_BOOLEAN,
                                        n = 11, group = "Cheats", desc = "If the cheat stickyfingers/tenacioustoes is active. This values is the same as the \"Grab All\" setting. If this is active, then player can grab every npc.", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    player_hasCheated               = {defValue = false, minVal = nil, maxVal = nil, address = 0x00B2C8C4, size = FIELD_BOOLEAN,
                                        n = 12, group = "Cheats", desc = "This is the indicator if the player has cheated (used on of the cheat codes). If this is the case then saving is deactivated. However with the cheat redigitiscool/raocowisswell you can activate cheating again. How lucky that you can deactivate it again :)", 
                                        customFuncGet = getBoolean, customFuncSet = setBoolean},
    
    
    
    --[[ Player Defines ]]--
    player_link_shieldEnabled       = {defValue = true, minVal = nil, maxVal = nil, address = nil, size = FIELD_BOOLEAN,
                                        n = 1, group = "Player Settings: Link", desc = "If the shield of link is enabled.", 
                                        customFuncGet = function(self)
                                            return mem(0x00A53042, FIELD_BYTE) ~= 0x90
                                        end,
                                        customFuncSet = function(self, value)
                                            if(value)then
                                                mem(0x00A53042, FIELD_BYTE, 0x52)
                                                mem(0x00A53043, FIELD_BYTE, 0x50)
                                                mem(0x00A53044, FIELD_BYTE, 0xE8)
                                                mem(0x00A53045, FIELD_BYTE, 0x07)
                                                mem(0x00A53046, FIELD_BYTE, 0xFF)
                                                mem(0x00A53047, FIELD_BYTE, 0xF3)
                                                mem(0x00A53048, FIELD_BYTE, 0xFF)
                                            else
                                                mem(0x00A53042, FIELD_BYTE, 0x90)
                                                mem(0x00A53043, FIELD_BYTE, 0x90)
                                                mem(0x00A53044, FIELD_BYTE, 0x90)
                                                mem(0x00A53045, FIELD_BYTE, 0x90)
                                                mem(0x00A53046, FIELD_BYTE, 0x90)
                                                mem(0x00A53047, FIELD_BYTE, 0x90)
                                                mem(0x00A53048, FIELD_BYTE, 0x90)
                                            end
                                        end},
    player_link_fairyVineEnabled    = {defValue = true, minVal = nil, maxVal = nil, address = nil, size = FIELD_BOOLEAN,
                                        n = 2, group = "Player Settings: Link", desc = "If the vine fairy is enabled",
                                        customFuncGet = function(self)
                                            return mem(0x009AAF93, FIELD_BYTE) == 0x5
                                        end,
                                        customFuncSet = function(self, value)
                                            if(value)then
                                                mem(0x009AAF93, FIELD_BYTE, 0x5)
                                            else
                                                mem(0x009AAF93, FIELD_BYTE, 0xFF)
                                            end
                                        end},
    
    
    block_hit_link_rupeeID1         = {defValue = 251, minVal = 0, maxVal = 300, address = 0x009DBD9A, size = FIELD_WORD,
                                        n = 1, group = "Block Modification", desc = "The npc id for Link's main coin when hitting the block. (Default would be the green rupee.)"},
    block_hit_link_rupeeID2         = {defValue = 252, minVal = 0, maxVal = 300, address = 0x009DBDFF, size = FIELD_WORD,
                                        n = 2, group = "Block Modification", desc = "The npc id for Link's secondary coin when hitting the block (chance 20:3). (Default would be the blue rupee.)"},
    block_hit_link_rupeeID3         = {defValue = 253, minVal = 0, maxVal = 300, address = 0x009DBE64, size = FIELD_WORD,
                                        n = 3, group = "Block Modification", desc = "The npc id for Link's tertiary coin when hitting the block (chance 60:3). (Default would be the red rupee.)"},

    kill_drop_link_rupeeID1         = {defValue = 251, minVal = 0, maxVal = 300, address = 0x00A32943, size = FIELD_WORD,
                                        n = 1, group = "Kill Modification", desc = "The npc id for Link's main coin when killing an enemy. (Default would be the green rupee.)"},
    kill_drop_link_rupeeID2         = {defValue = 252, minVal = 0, maxVal = 300, address = 0x00A329D5, size = FIELD_WORD,
                                        n = 2, group = "Kill Modification", desc = "The npc id for Link's secondary coin when killing an enemy. (chance 15:3). (Default would be the blue rupee.)"},
    kill_drop_link_rupeeID3         = {defValue = 253, minVal = 0, maxVal = 300, address = 0x00A32A6F, size = FIELD_WORD,
                                        n = 3, group = "Kill Modification", desc = "The npc id for Link's tertiary coin when killing an enemy. (chance 40:3). (Default would be the red rupee.)"},
}

--(Re)sets a define
local function setDefine(defTable, value)
    local theValue = nil
    if(value ~= nil)then
        theValue = value
    else
        theValue = defTable.defValue
    end
    if(defTable.customFuncSet)then
        defTable:customFuncSet(theValue)
    else
        mem(defTable.address, defTable.size, theValue)
    end
    
end

local function getDefine(defTable)
    if(defTable.customFuncGet)then
        return defTable:customFuncGet()
    else
        return mem(defTable.address, defTable.size)    
    end
end

--The actual host code
local definesLib  = setmetatable({
    -- On Level startup reset all defines
    onInitAPI = function()
        for _,defineTable in pairs(defines) do
            setDefine(defineTable)
        end
    end

}, {
    --Neat function to modify a define
    __newindex = function (tarTable, key, value)
        --A bunch of error checking
        assert(key)
        local theDefine = defines[key]
        if not theDefine then
            error("Field \""..tostring(key).."\" does not exist!", 2)
        end
        if type(value) ~= "number" and type(value) ~= "boolean" and type(value) ~= "nil" then
            error("Value is not a number: Need number or boolean, got "..type(value).."!", 2)
        end
        if theDefine.minVal then
            if theDefine.minVal > value then
                error("Value "..value.." is smaller than the minimum value of "..theDefine.minVal.."!", 2)
            end
        end
        if theDefine.maxVal then
            if theDefine.maxVal < value then
                error("Value "..value.." is bigger than the maximum value of "..theDefine.maxVal.."!", 2)
            end
        end
        --Set the actual define
        setDefine(theDefine, value)
    end,

    __index = function (tarTable, key)
        assert(key)
        local theDefine = defines[key]
        if not theDefine then
            error("Field \""..tostring(key).."\" does not exist!", 2)
        end
        
        return getDefine(theDefine)
    end
})

if not __wikiGEN then
    return definesLib
end

--[[

To generate the Wiki:
1. Set up any base lua intepreter.
2. Define __wikiGEN to true
3. Loadup this file
]]


local function generateWiki()
    print("Starting to generate wiki...")

    local wfile = io.open("GenWiki.txt", "w+")
    print("Generate Header Template...")

    wfile:write("Following code for \"Template:Defines table\":\n")

    wfile:write("<table style=\"width: 100%; border: solid 1px black; margin: 2px; border-spacing: 0px;\">\n")
    wfile:write("<tr><th colspan=\"5\" style=\"font-weight: bold; font-size: large; padding-bottom: .3em; border-bottom: solid #C9C9C9 1px; background: #D3FFC2; line-height:28px; text-align: left\">\n")
    wfile:write("{{{1}}}</th></tr>\n")
    wfile:write("<tr>\n")
    wfile:write("<td style=\"width: 350px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Name of Define</td>\n")
    wfile:write("<td style=\"width: 150px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Type</td>\n")
    wfile:write("<td style=\"width: 150px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Default value</td>\n")
    wfile:write("<td style=\"width: 150px; background: #E0E0E0; padding: .4em; font-weight:bold;\">Min/Max Size</td>\n")
    wfile:write("<td style=\"background: #E0E0E0; padding: .4em; font-weight:bold;\">Description</td></tr>\n")
    wfile:write("{{{2}}}\n")
    wfile:write("</table>")

    wfile:write("\n=============================\n\n")
    io.flush(wfile)


    print("Generate Row Template...")
    wfile:write("Following code for \"Template:Defines table/row\":\n")

    wfile:write("<tr style=\"background-color: {{#ifeq:{{{4|}}}|odd|#FFFFFF|#E8E8E8}};\">\n")
    wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{1}}}</td>\n")
    wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{2}}}</td>\n")
    wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{3}}}</td>\n")
    wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{4}}}</td>\n")
    wfile:write("<td style=\"border-top: solid #C9C9C9 1px; padding: .4em;\">{{{5|}}}</td></tr>\n")
    wfile:write("\n=============================\n\n")
    io.flush(wfile)


    print("Generate SUPER AMAZING CONTENT...")
    wfile:write("Following code for the main page: \n")

    wfile:write("[[Category:LunaLua API]]\n")
    wfile:write("Since LunaLua v0.6.1 you can use these powerful defines to modify SMBX values. They are safe to use and will reset on every level.\n")


    local sortedTable = {}

    for defineName, defineTable in pairs(defines) do
        local defineGroup = defineTable.group or "Misc"
        if not sortedTable[defineGroup] then
            sortedTable[defineGroup] = {}
        end
        if not sortedTable[getKeyOfValue(sortedCategories,defineGroup)] then
            sortedTable[getKeyOfValue(sortedCategories,defineGroup)] = {name = defineGroup, theTable = {}}
        end
        sortedTable[getKeyOfValue(sortedCategories,defineGroup)].theTable[defineTable["n"]] = {name = defineName, theTable = defineTable}
    end

    for unused1, defineGroupTable in pairs(sortedTable) do
        if defineGroupTable.theTable and #defineGroupTable.theTable > 0 then
            local defineGroup = defineGroupTable.name
            wfile:write("{{Defines table|"..defineGroup.."||2=\n\n")
            local oddness = true
            for unused2, defineTablePack in pairs(defineGroupTable.theTable) do
                local defineName = defineTablePack.name
                local defineTable = defineTablePack.theTable
                wfile:write("{{Defines table/row|")
                wfile:write("Defines."..defineName.."|")
                if defineTable.size == FIELD_BYTE then
                    wfile:write("BYTE|")
                elseif defineTable.size == FIELD_WORD then
                    wfile:write("WORD|")
                elseif defineTable.size == FIELD_DWORD then
                    wfile:write("DWORD|")
                elseif defineTable.size == FIELD_FLOAT then
                    wfile:write("FLOAT|")
                elseif defineTable.size == FIELD_DFLOAT then
                    wfile:write("DFLOAT (double)|")
                elseif defineTable.size == FIELD_BOOLEAN then
                    wfile:write("BOOLEAN|")
                end
                wfile:write(tostring(defineTable.defValue).."|")
                if defineTable.minVal and defineTable.maxVal then
                    wfile:write("Min. Value: "..defineTable.minVal.."<br>")
                    wfile:write("Max. Value: "..defineTable.maxVal.."|")
                elseif defineTable.minVal then
                    wfile:write("Min. Value: "..defineTable.minVal.."|")
                elseif defineTable.maxVal then
                    wfile:write("Max. Value: "..defineTable.maxVal.."|")
                else
                    wfile:write("---|")
                end
                wfile:write(defineTable.desc)
                if oddness then
                    wfile:write("|odd")
                end
                wfile:write("}}\n")
                oddness = not oddness
            end
            wfile:write("\n}}\n\n\n")
        end
    end
    io.close(wfile)
    print("Done writing and closed file")
end

if __wikiGEN then generateWiki() end
