--************************************************************************************--
--** '||  ||` '||   ||` '||'''|.            .|';    |''||''|                        **--
--**  ||  ||   ||   ||   ||   ||            ||         ||     ''                    **--
--**  ||''||   ||   ||   ||   ||    .|''|, '||'        ||     ||  '||),,(|,  .|''|, **--
--**  ||  ||   ||   ||   ||   ||    ||  ||  ||         ||     ||   || || ||  ||..|| **--
--** .||  ||.  `|...|'  .||...|'    `|..|' .||.       .||.   .||. .||    ||. `|...  **--
--**                                                                                **--
--************************************************************************************--
----------------------------------------------------------------------------------------             
--------------------------------Created by Hoeloe - 2015--------------------------------
----------------------------Open-Source Zelda HUD Framework-----------------------------
---------------------------------For Super Mario Bros X---------------------------------
----------------------------------------v1.0.2------------------------------------------

local zelda = {}

Graphics.activateHud(false);

zelda.HEART_FULL = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\heart_full.png");
zelda.HEART_EMPT = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\heart_empty.png");
zelda.RUPEE = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\rupee.png");
zelda.SWORD = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\sword.png");
zelda.BOMB = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\bomb.png");
zelda.BTN_X = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\btn_X.png");
zelda.BTN_S = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\btn_S.png");
zelda.KEY = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\small_key.png");
zelda.MAGIC_BG = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\magic_bg.png");
zelda.MAGIC_BAR = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\magic_bar.png");
zelda.NAVI = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\navi.png");
zelda.TRIFORCE = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\stars.png");
zelda.FAIRY = Graphics.loadImage(getSMBXPath().."\\LuaScriptsLib\\hudoftime\\lives.png");

zelda.hearts = 0;
zelda.maxhearts = 6;

zelda.magic = 1;

zelda.rupees = 0;
zelda.maxrupees = 99;

zelda.keys = 0;

zelda.buttons = {zelda.BTN_X, zelda.BTN_S};

local items = {};
items[zelda.BTN_X] = zelda.SWORD;
items[zelda.BTN_S] = {zelda.BOMB,0};

zelda.triforce = 0;
zelda.maxtriforce = 999;

zelda.fairy = 0;
zelda.maxfairy = 99;

zelda.navi = false;


--Automatic data bindings. "Hearts" also allows for extra functionality in increasing the maximum heart cap.
zelda.auto_hearts = true;
zelda.auto_magic = true;
zelda.auto_rupees = true;
zelda.auto_keys = true;
zelda.auto_items = true;
zelda.auto_triforce = true;
zelda.auto_fairy = true;
zelda.auto_navi = true;

--Automatically enables and disables the HUD based on whether the player is Link.
zelda.auto_enable = true;

--HUD show flags.
zelda.show_hearts = true;
zelda.show_magic = true;
zelda.show_rupees = true;
zelda.show_keys = true;
zelda.show_items = true;
zelda.show_triforce = true;
zelda.show_fairy = true;
zelda.show_navi = true;
zelda.show_score = true;

zelda.show_HUD = true;

function zelda.Enable()
	zelda.show_HUD = true;
	Graphics.activateHud(false);
end

function zelda.Disable()
	zelda.show_HUD = false;
	Graphics.activateHud(true);
end

local first = true;

function zelda.onInitAPI()
	registerEvent(zelda, "onLoop", "onLoop", true) --Register the loop event
	registerEvent(zelda, "onHUDDraw", "onHUDDraw", true) --Register the HUD event
end

function zelda.setItem(button,icon,count)
	if(zelda.auto_items) then
		return;
	else
		if(count == nil) then
			items[button] = icon;
		else
			items[button] = {icon,count};
		end
	end
end

function zelda.getItem(button)
	if(type(items[button]) == "table") then
		return items[button][1];
	else
		return items[button];
	end
end

function zelda.getItemCount(button)
	if(type(items[button]) == "table") then
		return items[button][2];
	else
		return -1;
	end
end

local powercache = PLAYER_BIG;

local function UpdateEnabled()
	if(player:mem(0xF0, FIELD_WORD) == 5) then
		zelda.Enable();
	else
		zelda.Disable();
	end
end

local function UpdateHearts()
	if(zelda.auto_hearts) then
		if(first) then
			zelda.hearts = player:mem(0x16, FIELD_WORD);
			if(zelda.hearts >= 2) then
				player:mem(0x16, FIELD_WORD, 2);
			end
			first = false;
		end
		
		if(player:mem(0x16, FIELD_WORD) == 3) then
			zelda.hearts = zelda.hearts + 1;
			if(zelda.hearts > zelda.maxhearts) then
				zelda.hearts = zelda.maxhearts;
			end
			powercache = player.powerup;
			player:mem(0x16, FIELD_WORD,2);
		end
		
		if(player:mem(0x16, FIELD_WORD) == 1) and (zelda.hearts > 1) then
			zelda.hearts = zelda.hearts - 1;
		end
		
		if(player:mem(0x16, FIELD_WORD) > zelda.hearts) then
			zelda.hearts = player:mem(0x16, FIELD_WORD);
		end
		
		if(player:mem(0x16, FIELD_WORD) == 0) then
			zelda.hearts = 0;
		end
		
		if (player.powerup == PLAYER_SMALL) and (zelda.hearts >= 2) then
			 player.powerup = powercache;
		end
	end
end

local function UpdateMagic()
		if(not wasfairy and player:mem(0x0C,FIELD_WORD) == -1) then
			maxfairycount = player:mem(0x10,FIELD_WORD);
			wasfairy = true;
		elseif(wasfairy and player:mem(0x0C,FIELD_WORD) ~= -1) then
			wasfairy = false;
		end
		
		if(player:mem(0x0C,FIELD_WORD) == -1 and player:mem(0x10,FIELD_WORD) >= 0 and maxfairycount > 0) then
			zelda.magic = player:mem(0x10,FIELD_WORD)/maxfairycount;
		elseif(player:mem(0x0E,FIELD_WORD) == 1 and player:mem(0x4A,FIELD_WORD) ~= -1) then
			zelda.magic = 0;
		elseif(player:mem(0x4A,FIELD_WORD) == -1) then
			zelda.magic = 1-(player:mem(0x4E,FIELD_WORD)/240);
		else
			zelda.magic = 1;
		end
end

local function UpdateRupees()
	zelda.maxrupees = 99;
	zelda.rupees = mem(0x00B2C5A8,FIELD_WORD);
end

local function UpdateKeys()
	if(player:mem(0x12,FIELD_WORD) == -1) then
		zelda.keys = 1;
	else
		zelda.keys = 0;
	end
end

local function UpdateItems()
	items[zelda.BTN_X] = zelda.SWORD;
	items[zelda.BTN_S] = {zelda.BOMB,player:mem(0x08, FIELD_WORD)};
end

local function UpdateTriforce()
	zelda.triforce = mem(0x00B251E0,FIELD_WORD);
	zelda.maxtriforce = 999;
end

local function UpdateFairy()
	zelda.fairy = mem(0x00B2C5AC,FIELD_FLOAT);
	zelda.maxfairy = 99;
end

local function UpdateNavi()
	zelda.navi = false;
	for _,v in ipairs(NPC.getIntersecting(player.x,player.y,player.x+player.width,player.y+player.height)) do
		if(v:mem(0x46,FIELD_WORD) == -1 --[[Friendly]] and (v:mem(0x4C,FIELD_DWORD) > 0 and v.msg.str ~= nil and v.msg.str ~= "") --[[Has message]]) then
			zelda.navi = true;
			break;
		end
	end
end

function zelda.onLoop()
	if(zelda.auto_hearts) then
		UpdateHearts();
	end
	
	if(zelda.auto_magic) then
		UpdateMagic();
	end
	
	if(zelda.auto_rupees) then
		UpdateRupees();
	end
	
	if(zelda.auto_keys) then
		UpdateKeys();
	end
	
	if(zelda.auto_items) then
		UpdateItems();
	end
	
	if(zelda.auto_triforce) then
		UpdateTriforce();
	end
	
	if(zelda.auto_fairy) then
		UpdateFairy();
	end
	
	if(zelda.auto_navi) then
		UpdateNavi();
	end
end

local maxfairycount = 0;
local wasfairy = false;

local function pad(n,d)
	return string.format("%0"..d.."d",n);
end

local function getDigits(n)
	local c = 0;
	while(n > 1) do
		c = c + 1;
		n = n/10;
	end
	return math.max(c,1);
end

local function DrawHearts()
	local maxy = 0;
	for i=0,zelda.maxhearts-1,1 do
		local g;
		if(i<=zelda.hearts-1) then
			g = zelda.HEART_FULL;
		else
			g = zelda.HEART_EMPT;
		end
		Graphics.drawImage(g, (i%10)*16 + 16, math.floor(i/10)*16 + 16);
		maxy = math.floor(i/10)*16;
	end
	return maxy + 24;
end

local function DrawMagic(y)
	Graphics.drawImage(zelda.MAGIC_BG, 14, y);
	
	Graphics.glSetTextureRGBA(zelda.MAGIC_BAR, 0xFFFFFFFF);
		local pts = {};
		pts[0] = 16; pts[1] = y;
		pts[2] = 16+158*zelda.magic; pts[3] = y;
		pts[4] = 16; pts[5] = y+16;
		pts[6] = 16; pts[7] = y+16;
		pts[8] = 16+158*zelda.magic; pts[9] = y;
		pts[10] = 16+158*zelda.magic; pts[11] = y+16;
		
		local tx = {};
		tx[0] = 0; tx[1] = 0;
		tx[2] = 1; tx[3] = 0;
		tx[4] = 0; tx[5] = 1;
		tx[6] = 0; tx[7] = 1;
		tx[8] = 1; tx[9] = 0;
		tx[10] = 1; tx[11] = 1;
		
		Graphics.glDrawTriangles(pts, tx, 6);
		Graphics.glSetTextureRGBA(nil, 0xFFFFFFFF);
end

local function DrawRupees()
	Graphics.drawImage(zelda.RUPEE, 16, 600 - 32);
	zelda.rupees = math.min(zelda.rupees,zelda.maxrupees);
	local r = pad(zelda.rupees,getDigits(zelda.maxrupees))
	Text.print(r,36,600-32);
end

local function DrawKeys()
	for i=1,zelda.keys,1 do
		Graphics.drawImage(zelda.KEY, 16*i, 600 - 32 - 24);
	end
end

local function DrawButton(x, button)
	Graphics.drawImage(button, (800-16)-(32*x), 16);
	
	local item = zelda.getItem(button);
	if(item ~= nil) then
		local count = zelda.getItemCount(button);
		if(count ~= nil) then
			if(count ~= 0) then
				Graphics.drawImage(item, (800-16)-(32*x), 16);
			end
			if(count > 0) then
				Text.print(tostring(count),(800+8)-(32*x),40);
			end
		end
	end
end

local function DrawTriforce()
	Graphics.drawImage(zelda.TRIFORCE, 800-32, 600-32);
	zelda.triforce = math.min(zelda.triforce, zelda.maxtriforce);
	local t = pad(zelda.triforce,getDigits(zelda.maxtriforce));
	Text.print(t,800-30-18*(#t),600-32);
end

local function DrawFairy()
	Graphics.drawImage(zelda.FAIRY, 800-32, 600-32-24);
	zelda.fairy = math.min(zelda.fairy, zelda.maxfairy);
	local t = pad(zelda.fairy,getDigits(zelda.maxfairy));
	Text.print(t,800-30-18*(#t),600-32-24);
end

local function DrawNavi()
	if(zelda.navi) then
		Graphics.drawImage(zelda.NAVI, 800-16-32*(#zelda.buttons + 1), 16);
	end
end

local function DrawScore()
	local p = mem(0x00B2C8E4,FIELD_DWORD);
	Text.print(pad(p,6),800-24-96,600-32-48);
end

function zelda.onHUDDraw()
	if(zelda.auto_enable) then
		UpdateEnabled();
	end
	
	if(not zelda.show_HUD) then return end;
	
	local maxy = 0;
	
	if(zelda.show_hearts) then
		maxy = DrawHearts();
	end
	
	if(zelda.show_magic) then
		DrawMagic(maxy+16);
	end
	
	if(zelda.show_rupees) then
		DrawRupees();
	end
	
	if(zelda.show_keys) then
		DrawKeys();
	end
	
	if(zelda.show_items) then
		for k,v in ipairs(zelda.buttons) do
			DrawButton(#zelda.buttons-(k-1), v);
		end
	end
	
	if(zelda.show_triforce) then
		DrawTriforce();
	end
	
	if(zelda.show_fairy) then
		DrawFairy();
	end
	
	if(zelda.show_navi) then
		DrawNavi();
	end
	
	if(zelda.show_score) then
		DrawScore();
	end
end

return zelda;