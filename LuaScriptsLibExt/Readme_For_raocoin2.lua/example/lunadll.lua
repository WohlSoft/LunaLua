raocoin = loadSharedAPI("raocoin2");
rc = raocoin.registerCurrency(274, true, 472, 66);
coins = raocoin.registerMemCurrency(0x00B2C5A8,FIELD_WORD,true,false,0,0,"coins.png");

local shop = {}

shop["mushroom"] = { npc = 9, item = rc:addItem(2,function() triggerEvent("BuyMushroom"); end,true), x = -199904, y=-200164};
shop["fire"] = { npc = 183, item = rc:addItem(4,function() triggerEvent("BuyFire"); end,true), x = -199776, y=-200164};
shop["ice"] = { npc = 277, item = rc:addItem(6,function() triggerEvent("BuyIce"); end,true), x = -199648, y=-200164};
shop["leaf"] = { npc = 34, item = rc:addItem(10,function() triggerEvent("BuyLeaf"); end,true), x = -199520, y=-200164};
shop["tanuki"] = { npc = 169, item = rc:addItem(20,function() triggerEvent("BuyTanuki"); end,true), x = -199392, y=-200164};
shop["hammer"] = { npc = 170, item = rc:addItem(20,function() triggerEvent("BuyHammer"); end,true), x = -199264, y=-200164};
shop["1up"] = { npc = 187, item = rc:addItem(1,function() spawnNPC(187,player.x,player.y,player.section); end,false), x = -199072, y=-200164};
shop["3up"] = { npc = 188, item = rc:addItem(3,function() spawnNPC(188,player.x,player.y,player.section); end,false), x = -198944, y=-200164};
shop["random"] = { npc = 287, item = rc:addItem(3,function() spawnNPC(287,player.x,player.y,player.section); end,false), x = -198816, y=-200164};
shop["coins"] = { npc = 10, item = rc:addItem(1,function() for i = 1,25,1 do spawnNPC(10,player.x,player.y,player.section); end end,false), x = -198698, y=-200164};
shop["random2"] = { npc = 287, item = coins:addItem(50,function() spawnNPC(287,player.x,player.y,player.section); end,false), x = -198442, y=-200164};

function onLoad()
	for k,v in pairs(shop) do
		v.item:placeToken(v.x,v.y,0,v.npc);
	end
end

function raocoin.onCollect(currency)
	if(currency.id == 0x00B2C5A8) then
		currency:save();
	end
end

function onEvent(name)
	if(name == "UndoBuys") then
		for k,v in pairs(shop) do
			v.item:undoBuy();
		end
	end
end