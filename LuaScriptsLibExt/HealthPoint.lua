-- HealthPoint
-- Made by WasabiJellyfish
-- v1.0.3
 
local HealthPoint = {}
local pNPC = loadSharedAPI("pnpc")
local graphX = loadSharedAPI("graphX")
local colliders = loadSharedAPI("colliders")
 
function HealthPoint.onInitAPI() --Is called when the api is loaded by loadAPI.
        registerEvent(HealthPoint, "onLoop", "assign", true) --Register the loop event
        registerEvent(HealthPoint, "onNPCKill", "onNPCKill", true) --Register the damage event      
end
 ----------------------------------------------------------------------------------------------
 ---                       DOCUMENTATION AND FUNCTIONS                                      ---
 ----------------------------------------------------------------------------------------------
 --[[
    Damage types
    1 = jump
    2 = spinjump
    3 = fireball
    4 = hammer
    5 = shell
    6 = tail
    7 = link
    8 = thrown
    9 = boomerang
    10 = peachbomb
    11 = swordbeam
    12 = linkfire
    13 = iceblock
    14 = yoshifire
    15 = yoshiice
   
    HealthPoint.setNPCHealth(id, healthamount)
 
 
    HealthPoint.setNPCDamage(id, damagetype, damageamount)
 
 
    HealthPoint.setGlobalDamage(damagetype, damageamount)
 
 
    HealthPoint.makeNPCInvincible(id)
 
 
    HealthPoint.makeNPCNormal(id)
 --]]
 
 ----------------------------------------------------------------------------------------------
 ---                            THE ACTUAL CODE STUFF                                       ---
 ----------------------------------------------------------------------------------------------
 
 
local bingo = {1, 2, 3, 4, 5, 6, 7, 8, 12, 15, 17, 18, 19, 20, 23, 24, 25, 27, 28, 29, 36, 37, 38, 39, 42, 43, 44, 47, 48, 49, 50, 51, 52, 53, 54, 55, 59, 61, 63, 65, 71, 72, 73, 74, 76, 77, 86, 89, 93, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 135, 136, 137, 161, 162, 163, 164, 165, 166, 167, 168, 172, 173, 174, 175, 176, 177, 180, 189, 194, 195, 199, 200, 201, 203, 204, 205, 206, 207, 208, 209, 210, 229, 230, 231, 232, 233, 234, 235, 236, 242, 243, 244, 245, 247, 256, 257, 261, 262, 263, 267, 268, 270, 271, 272, 275, 280, 281, 284, 285, 286}
 -- Health --
 -- Change these values corresponding to the id inside the [] to change the enemy's health --
  HealthPoint.npcHealth = {}
  for _,v in pairs(bingo) do
  table.insert(HealthPoint.npcHealth, v, 3)
  end
 
 
 -- Jump Damage --
  HealthPoint.jumpDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.jumpDamage, v, 1)
  end
 -- Spinjump Damage --
  HealthPoint.spinjumpDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.spinjumpDamage, v, 1)
  end
 
  -- Fireball Damage --
  HealthPoint.fireballDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.fireballDamage, v, 1)
  end
 
 -- Hammer Damage --
  HealthPoint.hammerDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.hammerDamage, v, 1)
  end
 
 -- Projectile Damage --
  HealthPoint.projectileDamage = {}
  for _,v in pairs(bingo) do
  table.insert(HealthPoint.projectileDamage, v, 1)
  end
 
 -- Tail Damage --
  HealthPoint.tailDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.tailDamage, v, 1)
  end
 
 -- Link Attack Damage --
  HealthPoint.linkDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.linkDamage, v, 1)
  end
 
  -- Thrown Damage --
  HealthPoint.thrownDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.thrownDamage, v, 1)
  end
 
  -- Boomerang Damage--
  HealthPoint.boomerangDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.boomerangDamage, v, 1)
  end
 
  HealthPoint.peachbombDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.peachbombDamage, v, 1)
  end
 
  HealthPoint.swordbeamDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.swordbeamDamage, v, 1)
  end
 
  HealthPoint.yoshifireDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.yoshifireDamage, v, 1)
  end
 
 HealthPoint.iceblockDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.iceblockDamage, v, 1)
  end
  HealthPoint.linkfireDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.linkfireDamage, v, 1)
  end
 HealthPoint.yoshiiceDamage = {}
   for _,v in pairs(bingo) do
  table.insert(HealthPoint.yoshiiceDamage, v, 1)
  end
 ----------------------------------------------------------------------------------------------
 ---                            THE ACTUAL CODE STUFF                                       ---
 ----------------------------------------------------------------------------------------------
 damevaeg = {}
 damevaeg[1] = HealthPoint.jumpDamage
 damevaeg[2] = HealthPoint.spinjumpDamage
 damevaeg[3] = HealthPoint.fireballDamage
 damevaeg[4] = HealthPoint.hammerDamage
 damevaeg[5] = HealthPoint.projectileDamage
 damevaeg[6] = HealthPoint.tailDamage
 damevaeg[7] = HealthPoint.linkDamage
 damevaeg[8] = HealthPoint.thrownDamage
 damevaeg[9] = HealthPoint.boomerangDamage
 damevaeg[10] = HealthPoint.peachbombDamage
 damevaeg[11] = HealthPoint.swordbeamDamage
 damevaeg[12] = HealthPoint.linkfireDamage
 damevaeg[13] = HealthPoint.iceblockDamage
 damevaeg[14] = HealthPoint.yoshifireDamage
 damevaeg[15] = HealthPoint.yoshiiceDamage
 
 function HealthPoint.setNPCHealth(id, number)
 HealthPoint.npcHealth[id] = number
 end
 function HealthPoint.setNPCDamage(id, cause, number)
damevaeg[cause][id] = number
end
 function HealthPoint.setGlobalDamage(cause, number)
for l,_ in pairs(HealthPoint.npcHealth) do
 damevaeg[cause][l] = number
 end
 end
 
 function HealthPoint.makeNPCInvincible(id)
 for r,_ in pairs(damevaeg) do
damevaeg[r][id] = 0
end
 end
 function HealthPoint.makeNPCNormal(NPCid)
 table.remove(HealthPoint.npcHealth, NPCid)
 for r,_ in pairs(damevaeg) do
 table.remove(damevaeg[r], NPCid)
 end
 end
 
 ----------------------------------------------------------------------------------------------
 ---                                 DAMAGE INPUT                                           ---
 ----------------------------------------------------------------------------------------------
 
 function HealthPoint.onNPCKill(eventObj, killedNPC, killReason)
    if killedNPC.id == 13 and killReason == 4 then
    fire = true
    hammer = false
    shell = false
    end
        if HealthPoint.npcHealth[killedNPC.id] ~= nil  then
            local wrappedNPC = pNPC.wrap(killedNPC)
           
 
        if colliders.collideNPC(wrappedNPC, 171) then
            hammer = true
            fire = false
            shell = false
            elseif colliders.collideNPC(wrappedNPC, 292) then
            boomerang = true
            hammer = false
            fire = false
            shell = false
            elseif colliders.collideNPC(wrappedNPC, 108) then
            yoshifire = true
            hammer = false
            fire = false
            shell = false
           
            elseif colliders.collideNPC (wrappedNPC, 263) then
            Text.print ("ICE", 100, 100)
            ice = true
            shell = false
            hammer = false
            fire = false
           
            elseif colliders.collideNPC (wrappedNPC, 291) then
            peachbomb = true
            shell = false
           
            elseif colliders.collideNPC (wrappedNPC, 266) then
            laser = true
            shell = false
            elseif colliders.collideNPC(wrappedNPC, 237) then
            yoshiice = true
            shell = false
            else
            shell = true
           
            end
 
           
            if wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.jumpDamage[wrappedNPC.id] and killReason == 1 then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.jumpDamage[wrappedNPC.id]
                playSFX(2)
                wrappedNPC.speedX = 0
               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.fireballDamage[wrappedNPC.id] and killReason == 3 and fire then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.fireballDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
                fire = false
               
                elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.iceblockDamage[wrappedNPC.id] and killReason == 3 and ice then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.iceblockDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.hammerDamage[wrappedNPC.id] and killReason == 3 and hammer then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.hammerDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
                elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.boomerangDamage[wrappedNPC.id] and killReason == 3 and boomerang then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.boomerangDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.yoshifireDamage[wrappedNPC.id] and killReason == 3 and yoshifire then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.yoshifireDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
                    elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.yoshifireDamage[wrappedNPC.id] and killReason == 3 and yoshiice then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.yoshiiceDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
                elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.peachbombDamage[wrappedNPC.id] and killReason == 3 and peachbomb then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.peachbombDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
                        elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.swordbeamDamage[wrappedNPC.id] and killReason == 10 and laser then
                 eventObj.cancelled = true
                 wrappedNPC:mem(0x156, FIELD_WORD, 20)
                 wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.swordbeamDamage[wrappedNPC.id]
                 playSFX(89)   
                 wrappedNPC.speedX = 0
                 
       
           
                elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.projectileDamage[wrappedNPC.id] and killReason == 3 and shell and not fire and not ice then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.projectileDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
                shell = false
               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.thrownDamage[wrappedNPC.id] and   (killReason == 4 or killReason == 5) then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.thrownDamage[wrappedNPC.id]
         
         
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.tailDamage[wrappedNPC.id] and killReason == 7 then
                eventObj.cancelled = true
                wrappedNPC:mem(0x156, FIELD_WORD, 20)
                wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.tailDamage[wrappedNPC.id]
                playSFX(3)
                wrappedNPC.speedX = 0
               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.spinjumpDamage[wrappedNPC.id] and killReason == 8 then
                 eventObj.cancelled = true
                 wrappedNPC:mem(0x156, FIELD_WORD, 20)
                 wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.spinjumpDamage[wrappedNPC.id]
                 playSFX(2)
                wrappedNPC.speedX = 0
               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.linkDamage[wrappedNPC.id] and killReason == 10 and not fire then
                 eventObj.cancelled = true
                 wrappedNPC:mem(0x156, FIELD_WORD, 20)
                 wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.linkDamage[wrappedNPC.id]
                 playSFX(89)   
                 wrappedNPC.speedX = 0
                 
                               
            elseif wrappedNPC.data.hit > 1 and wrappedNPC.data.hit > HealthPoint.linkfireDamage[wrappedNPC.id] and killReason == 10 and fire then
                 eventObj.cancelled = true
                 wrappedNPC:mem(0x156, FIELD_WORD, 20)
                 wrappedNPC.data.hit = wrappedNPC.data.hit - HealthPoint.linkfireDamage[wrappedNPC.id]
                 playSFX(89)   
                 wrappedNPC.speedX = 0
                 
            end
    end
 end
 
 
 
 ----------------------------------------------------------------------------------------------
 ---                              HEALTH ASSIGNMENT                                         ---
 ----------------------------------------------------------------------------------------------
 
 function HealthPoint.assign()
    for k,v in pairs(NPC.get()) do
   
    local n = pNPC.wrap(v);
   
        if n.data.hit == nil then
            if HealthPoint.npcHealth[n.id] ~= nil  then
                n.data.hit = HealthPoint.npcHealth[n.id]
 
            end
        end
       
        if n.data.hp == nil then
            if HealthPoint.npcHealth[n.id] ~= nil then
                n.data.hp = HealthPoint.npcHealth[n.id]
 
            end
        end
       
        if n.data.hpmax == nil then
            if HealthPoint.npcHealth[n.id] ~= nil then
                n.data.hpmax = HealthPoint.npcHealth[n.id]
            end
        end
       
        if n ~= nil and HealthPoint.healthbar then
            if n:mem(0x12A, FIELD_WORD) ~= 0 and n.data.hp ~= nil then
                graphX.boxLevel (n.x-8,n.y-6, 32, 8,  0xFFFFFFF55)
                graphX.boxLevel (n.x-7,n.y-5, 30, 6,  0x00000055)
                graphX.boxLevel (n.x-7,n.y-5, n.data.hp/n.data.hpmax*30, 6,  0xFFF05BFF)
                graphX.boxLevel (n.x-7,n.y-5, n.data.hit/n.data.hpmax*30, 6,  0xF61818FF)
 
            end
        end
 
        if n.data.hp ~= nil then
            if n.data.hp > n.data.hit then
                n.data.hp = n.data.hp -0.02
            end
            if n.data.hit > n.data.hpmax then
                n.data.hit = n.data.hpmax
            end
        end
    end
 end
 return HealthPoint
