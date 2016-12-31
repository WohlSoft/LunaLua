-------------------------xxxxxxxxxxxx--------------------------
---------------------xxxxxx        xxxxxx----------------------
-------------------xxxx                xxxx--------------------
-----------------xxxx                    xxxx------------------
-----------------xx            oooooo      xx------------------
---------------xxxx          oo      oo    xxxx----------------
---------------xx            oooo    oo      xx----------------
---------------xx            oooo    oo      xx----------------
---------------xx      oooooooo      oo      xx----------------
---------------xx    oo        oooooo  oo    xx----------------
---------------xx  oo                    oo  xx----------------
---------------xx  oo            oo      oo  xx----------------
---------------xx  oo            oooo    oo  xx----------------
---------------xx    oo      oooo        oo  xx----------------
---------------xx      oooooo          oo    xx----------------
---------------xx            oooo    oo      xx----------------
---------------xx                oo  oo      xx----------------
---------------xx              oo    oo      xx----------------
---------------xx              oo            xx----------------
---------------xxxx          oo            xxxx----------------
-----------------xx          oo            xx------------------
-----------------xxxx          oo        xxxx------------------
-------------------xxxx                xxxx--------------------
---------------------xxxxxx        xxxxxx----------------------
-------------------------xxxxxxxxxxxx--------------------------
---------------------------------------------------------------
--__________                             __        ________  --
--\______   \_____    ____   ____  ____ |__| ____  \_____  \ --
-- |       _/\__  \  /  _ \_/ ___\/  _ \|  |/    \  /  ____/ --
-- |    |   \ / __ \(  <_> )  \__(  <_> )  |   |  \/       \ --
-- |____|_  /(____  /\____/ \___  >____/|__|___|  /\_______ \--
--        \/      \/            \/              \/         \/--         
---------------------------------------------------------------              
--------------------Created by Hoeloe - 2015-------------------
-------------Open-Source Currency and Shop Library-------------
---------------------For Super Mario Bros X--------------------
---------------------------v2.0.10-----------------------------
----------------------REQUIRES ENCRYPT.dll---------------------
---------------------REQUIRES COLLIDERS.lua--------------------
-----------------------REQUIRES PNPC.lua-----------------------

local raocoin = {}
local encrypt = loadSharedAPI("encrypt");
local colliders = loadSharedAPI("colliders");
local pnpc;

local currencies = {}
local currencyCache = {}
local memcurrencies = {}
local memcurrencyCache = {}

local data = encrypt.Data(Data.DATA_WORLD, "raocoin", true);

local currencyMT = {}

function currencyMT.createMT(id,memtype)
	local mt = {}
	mt.__index = 
		function(tbl, key)
			if(key == "refreshUI") then return currencyMT.refreshUI;
			elseif(key == "save") then return currencyMT.save;
			elseif(key == "get") then return currencyMT.get;
			elseif(key == "set") then return currencyMT.set;
			elseif(key == "addItem") then return raocoin.addItem;
			elseif(key == "id") then return id;
			elseif(key == "type") then return memtype; end
		end
	mt.__newindex = function(tbl, key, val) error("Attempted to write to a read-only field.",2); end
	return mt;
end

function currencyMT:refreshUI()
	self.img = Graphics.loadImage(self.path);
end

function currencyMT:save()
	if(self.type == -1) then --npc currency
		data:set("currency_"..self.id, data:get("currency_"..self.id)+currencyCache[self.id].value);
		currencyCache[self.id].value = 0;
	else --mem currency
		data:set("memcurrency_"..self.id, self:get());
	end
	data:save();
end

function currencyMT:get()
	if(self.type == -1) then --npc currency
		return data:get("currency_"..self.id) + currencyCache[self.id].value
	else --mem currency
		return mem(self.id,self.type);
	end
end

function currencyMT:set(value)
	if(value < 0) then value = 0; end;
	if(self.type == -1) then --npc currency
		--data:set("currency_"..self.id, value);
		currencyCache[self.id].value = value - data:get("currency_"..self.id);
	else --mem currency
		--data:set("memcurrency_"..self.id, value);
		mem(self.id,self.type,value);
	end
	--self:save();
end

function raocoin.registerCurrency(npcID, showUI, UIx, UIy, imgPath, ignoreHud)
	showUI = showUI or (showUI == nil and false);
	UIx = UIx or 0;
	UIy = UIy or 0;
	imgPath = imgPath or getSMBXPath().."\\LuaScriptsLib\\raocoin\\dragoncoin.png";
	ignoreHud = ignoreHud or (ignoreHud == nil and false);
	
	local c = { visible = showUI, ignoreHud = ignoreHud, img = Graphics.loadImage(imgPath), x = UIx, y = UIy, path = imgPath, onCollect = function(currency, item) end };
	setmetatable(c,currencyMT.createMT(npcID,-1));
	
	c:refreshUI();
	
	if(data:get("currency_"..npcID) == nil) then
		data:set("currency_"..npcID, 0);
		data:save();
	end
	currencyCache[npcID] = { value = 0, lastCount = -1, count = -1 };
	
	currencies[npcID] = c;
	
	return c;
end


function raocoin.registerMemCurrency(memaddress, memType, restore, showUI, UIx, UIy, imgPath, ignoreHud)
	restore = restore or (restore == nil and true);
	showUI = showUI or (showUI == nil and false);
	UIx = UIx or 0;
	UIy = UIy or 0;
	imgPath = imgPath or getSMBXPath().."\\LuaScriptsLib\\raocoin\\dragoncoin.png";
	ignoreHud = ignoreHud or (ignoreHud == nil and false);
	
	local c = { visible = showUI, ignoreHud = ignoreHud, img = Graphics.loadImage(imgPath), x = UIx, y = UIy, path = imgPath, onCollect = function(currency, item) end };
	setmetatable(c,currencyMT.createMT(memaddress,memType));
	
	c:refreshUI();
	
	if(data:get("memcurrency_"..memaddress) == nil) then
		data:set("memcurrency_"..memaddress, 0);
		data:save();
	elseif(restore) then
		mem(memaddress,memType,data:get("memcurrency_"..memaddress))
	end
	
	memcurrencyCache[memaddress] = mem(memaddress,memType);
	
	memcurrencies[memaddress] = c;
	
	return c;
end

function raocoin.getCurrency(npcID)
	return currencies[npcID];
end

function raocoin.getMemCurrency(address)
	return memcurrencies[address];
end


local items = {};
local tokens = {};

raocoin.IMG_TOKEN_T = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\raocoin\\token_t.png");
raocoin.IMG_TOKEN_P = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\raocoin\\token_p.png");
raocoin.IMG_ARROW = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\raocoin\\arrow.png");

local arrowalpha = 0;

local itemCount = 0;

local actionQueue = {};

raocoin.CODE_CANTAFFORD = 0;
raocoin.CODE_ALREADYBOUGHT = 1;

local itemMT = {}

local drawQueue = {};

function itemMT.createMT(id)
	local mt = {}
	mt.__index = 
		function(tbl, key)
			if(key == "canAfford") then return itemMT.canAfford;
			elseif(key == "showPrice") then return itemMT.showPrice;
			elseif(key == "buy") then return itemMT.buy;
			elseif(key == "forceGet") then return itemMT.forceGet;
			elseif(key == "isBought") then return itemMT.isBought;
			elseif(key == "undoBuy") then return itemMT.undoBuy;
			elseif(key == "placeToken") then return itemMT.placeToken;
			elseif(key == "removeToken") then return itemMT.removeToken;
			elseif(key == "id") then return id; end
		end
	mt.__newindex = function(tbl, key, val) error("Attempted to write to a read-only field.",2); end
	return mt;
end

function itemMT:canAfford()
	return self.currency:get() >= self.price;
end

function itemMT:showPrice(x,y,screenSpace)
		table.insert(drawQueue, function()
			screenSpace = screenSpace or (screenSpace == nil);
			
			if(not screenSpace) then
				x,y = raocoin.worldToScreen(x,y);
			end
			
			local v = self.currency;
			Graphics.placeSprite(1, v.img, x, y, "", 2);
			local val = tostring(self.price);
			Text.print(val, 1, x + 98-18*(string.len(val)), y);
		end)
end

function itemMT:buy()
	if(self:isBought()) then 
		raocoin.onBuyFail(self, raocoin.CODE_ALREADYBOUGHT);
		return; 
	end
	if(not self:canAfford()) then
		raocoin.onBuyFail(self, raocoin.CODE_CANTAFFORD);
		return;
	end
	
	self.currency:set(self.currency:get()-self.price);
	self.currency:save();
	
	self:forceGet();
	raocoin.onBuy(self);
end

function itemMT:forceGet()
	if(self.permanent) then
		data:set("item_"..self.id, 1);
		data:save();
	end
	
	table.insert(actionQueue,self.actions);
end

function itemMT:isBought()
	return self.permanent and data:get("item_"..self.id) == 1;
end

function itemMT:undoBuy()
	if(not self:isBought()) then return; end
	
	if(self.permanent) then
		data:set("item_"..self.id, 0);
		data:save();
	end
end

function itemMT:placeToken(x,y,section, npcid,text,image)
	if(colliders == nil) then
		colliders = loadSharedAPI("colliders");
	end
	if(npcid ~= nil and pnpc == nil) then
		pnpc = loadSharedAPI("pnpc");
	end
	local t = {x=x,y=y,section=section,enableTimer = 0,hitbox=colliders.Box(x,y,32,32),npcID=npcid,npc=nil,npcRefreshCount=0,text=text,image=image};
	tokens[self] = t;
end

function itemMT:removeToken()
	if(tokens[self] ~= nil and tokens[self].npc ~= nil) then
		tokens[self].npc:mem(0x40,FIELD_WORD,0xFFFF);
	end
	tokens[self] = nil;
end

function raocoin.addItem(currency, price, actions, permanent, id)
	permanent = permanent or (permanent == nil);
	
	if(id == nil) then
		while(items[itemCount] ~= nil) do
			itemCount = itemCount + 1;
		end
			id = itemCount;
	elseif(items[id] ~= nil) then
		--error("Item with ID already exists and will be overwritten!",2); --Not actually a problem, as it allows different items to share purchase across levels.
	end
	
	local i = { currency = currency, price = price, permanent = permanent, actions = actions };
	setmetatable(i,itemMT.createMT(id));
	
	if(permanent) then
		local d = data:get("item_"..id);
		if(d == nil) then
			data:set("item_"..id, 0);
			data:save();
		elseif(d == 1) then
			table.insert(actionQueue,actions);
		end
	end
	
	items[id] = i;
	itemCount = itemCount + 1;
	
	return i;
end

local buyingCounter = 0;
local tryBuy = false;
local doBuyAnim = false;
local sourceY = 0;
local buyAction;

function raocoin.inputupdate()
	if(doBuyAnim) then
		player.downKeyPressing = false; --Ensure animation lines up correctly
	end
end

function raocoin.update()
	for _,v in pairs(actionQueue) do
		v();
	end
	
	actionQueue = {};

	--[[for k,v in pairs(currencies) do
		--if(v.visible) then
			Graphics.placeSprite(1, v.img, v.x, v.y, "", 2);
			local val = tostring(v:get());
			Text.print(val, 1, v.x + 98-18*(string.len(val)), v.y);
		end
		
		currencyCache[k].count = 0;
		for _,_ in pairs(findnpcs(k,-1)) do
			currencyCache[k].count = currencyCache[k].count + 1;
		end
		if(currencyCache[k].count < currencyCache[k].lastCount) then
			currencyCache[k].value = currencyCache[k].value + currencyCache[k].lastCount - currencyCache[k].count;
			raocoin.onCollect(v, currencyCache[k].lastCount - currencyCache[k].count);
			v:onCollect(currencyCache[k].lastCount - currencyCache[k].count);
		end
		currencyCache[k].lastCount = currencyCache[k].count;
	end]]
	
	for k,v in pairs(memcurrencies) do
		--[[if(v.visible) then
			Graphics.placeSprite(1, v.img, v.x, v.y, "", 2);
			local val = tostring(v:get());
			Text.print(val, 1, v.x + 98-18*(string.len(val)), v.y);
		end]]
		
		if(memcurrencyCache[v.id] < mem(v.id,v.type)) then

			raocoin.onCollect(v,  mem(v.id,v.type) - memcurrencyCache[k]);
			v:onCollect(mem(v.id,v.type) - memcurrencyCache[k]);
		end
		memcurrencyCache[v.id] = mem(v.id,v.type);
	end
	
	
	arrowalpha = arrowalpha+0.02;
	if(arrowalpha > 1) then
		arrowalpha = -1;
	end
	
	if(player.downKeyPressing) then
		if(buyingCounter == 0) then
			buyingCounter = 15;
		end
	else
		buyingCounter = 0;
	end
	
	if(buyingCounter > 0) then
		buyingCounter = buyingCounter - 1;
		if(buyingCounter == 0) then
			tryBuy = true;
		end
	end
	
	if(doBuyAnim) then
		player:mem(0x122,FIELD_WORD,7); --Set state to "going through door"
		player.y = sourceY+32-player.height;
		if(player:mem(0x12A,FIELD_WORD) >= 16440) then --Warp timer counts from 16368 to 16444 by twos. 16440 is an acceptable buffer to ensure it doesn't try to warp, which will crash the game.
			player:mem(0x122,FIELD_WORD,0);
			player:mem(0x12A,FIELD_WORD,0);
			doBuyAnim = false;
			if(buyAction ~= nil) then
				buyAction:buy();
			end
		end
	end
	
	for k,v in pairs(tokens) do
		if(not k:isBought()) then
		
			--npc icon
			if((v.npc == nil or not v.npc.isValid) and v.npcID ~= nil and v.enableTimer <= 0) then
				v.npc = pnpc.wrap(NPC.spawn(v.npcID,v.x,v.y-70-64,v.section));
				v.npc:mem(0x46,FIELD_WORD,0xFFFF);
				v.npc:mem(0x48,FIELD_WORD,0xFFFF);
				v.npc:mem(0x12A,FIELD_WORD,100);
				v.npc.speedX = 0;
				v.npc.speedY = 0;
			end
			if(v.npc ~= nil and v.npc.isValid) then
				v.npc.x = v.npc:mem(0xA8,FIELD_DFLOAT);
				v.npc.y = v.npc:mem(0xB0,FIELD_DFLOAT);
				v.npc.speedX = 0;
				v.npc.speedY = 0;
				v.npc:mem(0x12A,FIELD_WORD,100);
				if(v.npcRefreshCount == 0 and (v.npcID == 287 or v.npcID == 147)) then
					v.npc:mem(0x40,FIELD_WORD,0xFFFF);
					v.npc = nil;
					v.npcRefreshCount = 10;
				elseif(v.npcRefreshCount > 0) then
					v.npcRefreshCount = v.npcRefreshCount - 1;
				end
			end
			
			--bmp icon
			if(v.image ~= nil and v.enableTimer <= 0) then
				Graphics.drawImageWP(v.image,v.x,v.y-70-64,-1);
			end
			
			--text
			if(v.text ~= nil and v.enableTimer <= 0) then
				table.insert(drawQueue, function()
					local x1,y1 = raocoin.worldToScreen(v.x+12,v.y-72);
					Text.printWP(v.text,x1+8-string.len(v.text)*9,y1-26,-1);
				end);
			end
		
			local img = raocoin.IMG_TOKEN_T;
			if(k.permanent) then
				img = raocoin.IMG_TOKEN_P;
			end
			
			if(v.enableTimer <= 0) then
				Graphics.placeSprite(2,img,v.x,v.y-64,"",2);
				k:showPrice(v.x-32,v.y-82,false);
				
				if(colliders.collide(player,v.hitbox)) then
					Graphics.drawImageToScene(raocoin.IMG_ARROW,v.x,v.y-32,math.abs(arrowalpha))
					if(tryBuy) then
						doBuyAnim = true;
						player.speedX = 0;
						player.speedY = 0;
						player.x = v.x+4;
						sourceY = v.y;
						if(k:canAfford()) then
							Audio.playSFX(getSMBXPath().."\\LuaScriptsLib\\raocoin\\buy.ogg");
						spawnEffect(132,v.x,v.y);
						v.enableTimer = 50;
						if(v.npc ~= nil and v.npc.isValid) then
							v.npc:mem(0x40,FIELD_WORD,0xFFFF);
							v.npc = nil;
						end
						else
							Audio.playSFX(getSMBXPath().."\\LuaScriptsLib\\raocoin\\fail.ogg");
						end
						buyAction = k;
					end
				end

			elseif(player:mem(0x122,FIELD_WORD) == 0 or player:mem(0x122,FIELD_WORD) == 7 or player:mem(0x122,FIELD_WORD) == 500) then
				v.enableTimer = v.enableTimer - 1;
			end
		end
	end
	
	tryBuy = false;
end

function raocoin:onnpcdie(npc, reason)
	if(currencies[npc.id] ~= nil and reason == 9 and (colliders.collide(player,npc) or colliders.speedCollide(player,npc))) then
			currencyCache[npc.id].value = currencyCache[npc.id].value + 1;
			raocoin.onCollect(currencies[npc.id], 1, npc);
			currencies[npc.id]:onCollect(1, npc);
	end
end

function raocoin.drawhud()
		for _,v in ipairs(drawQueue) do
			v();
		end
		
		drawQueue = {};

		for k,v in pairs(currencies) do
			if(v.visible and (v.ignoreHud or Graphics.isHudActivated())) then
				Graphics.drawImage(v.img, v.x, v.y);
				local val = tostring(v:get());
				Text.print(val, 1, v.x + 90-18*(string.len(val)), v.y);
			end
		end
		
		for k,v in pairs(memcurrencies) do
			if(v.visible and (v.ignoreHud or Graphics.isHudActivated())) then
				Graphics.drawImage(v.img, v.x, v.y);
				local val = tostring(v:get());
				Text.print(val, 1, v.x + 90-18*(string.len(val)), v.y);
			end
		end
end


--NOTE: If and when this function gets added to mainV2, remove from here.
local function registerCustomEvent(obj, eventName)
	local queue = {};
	local mt = getmetatable(obj);
	if(mt == nil) then
		mt = {__index = function(tbl,key) return rawget(tbl,key) end, __newindex = function(tbl,key,val) rawset(tbl,key,val) end}
	end
	local index_f = mt.__index;
	local newindex_f = mt.__newindex;
		
	mt.__index = function(tbl, key)
		if(key == eventName) then
			return function(...)
				for _,v in ipairs(queue) do
					v(...);
				end
			end
		else
			return index_f(tbl, key);
		end
	end
		
	mt.__newindex = function (tbl,key,val)
		if(key == eventName) then
			table.insert(queue, val);
		else
			newindex_f(tbl,key,val);
		end
	end
		
	setmetatable(obj,mt);
end

function raocoin.onInitAPI()
	registerEvent(raocoin, "onLoop", "update", true) --Register the loop event
	registerEvent(raocoin, "onInputUpdate", "inputupdate", true) --Register the input event
	registerEvent(raocoin, "onHUDDraw", "drawhud", true) --Register the draw event
	registerEvent(raocoin, "onNPCKill", "onnpcdie", true) --Register the NPC kill event
	
	registerCustomEvent(raocoin, "onBuyFail"); 	--(item, code) -- code: 0=can't afford, 1=already bought
	registerCustomEvent(raocoin, "onBuy");		--(item)
	registerCustomEvent(raocoin, "onCollect");	--(currency, increment, collectedItem) -- collectedItem is nil for mem currencies
end

function raocoin.getScreenBounds()
	local c = Camera.get()[1];
	local b = {left = c.x, right = c.x + 800, top = c.y, bottom = c.y+600};
	
	return b;
	
end

function raocoin.worldToScreen(x,y)
			local b = raocoin.getScreenBounds();
			local x1 = x-b.left;
			local y1 = y-b.top;
			return x1,y1;
end

return raocoin;