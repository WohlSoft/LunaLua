--horikawaTools.lua
--v1.0.1
--Created by Horikawa Otane, 2015
--Contact me at https://www.youtube.com/subscription_center?add_user=msotane

local horikawaTools = {}

horikawaTools.npcList = {}

horikawaTools.npcList[1] = true
horikawaTools.npcList[2] = true
horikawaTools.npcList[3] = true
horikawaTools.npcList[4] = true
horikawaTools.npcList[5] = true
horikawaTools.npcList[6] = true
horikawaTools.npcList[7] = true
horikawaTools.npcList[8] = true
horikawaTools.npcList[9] = false
horikawaTools.npcList[10] = false
horikawaTools.npcList[11] = false
horikawaTools.npcList[12] = true
horikawaTools.npcList[13] = false
horikawaTools.npcList[14] = false
horikawaTools.npcList[15] = 2
horikawaTools.npcList[16] = false
horikawaTools.npcList[17] = true
horikawaTools.npcList[18] = true
horikawaTools.npcList[19] = true
horikawaTools.npcList[20] = true
horikawaTools.npcList[21] = false
horikawaTools.npcList[22] = false
horikawaTools.npcList[23] = true
horikawaTools.npcList[24] = true
horikawaTools.npcList[25] = true
horikawaTools.npcList[26] = false
horikawaTools.npcList[27] = true
horikawaTools.npcList[28] = true
horikawaTools.npcList[29] = true
horikawaTools.npcList[30] = false
horikawaTools.npcList[31] = false
horikawaTools.npcList[32] = false
horikawaTools.npcList[33] = false
horikawaTools.npcList[34] = false
horikawaTools.npcList[35] = false
horikawaTools.npcList[36] = true
horikawaTools.npcList[37] = true
horikawaTools.npcList[38] = true
horikawaTools.npcList[39] = true
horikawaTools.npcList[40] = false
horikawaTools.npcList[41] = false
horikawaTools.npcList[42] = true
horikawaTools.npcList[43] = true
horikawaTools.npcList[44] = 2
horikawaTools.npcList[45] = false
horikawaTools.npcList[46] = false
horikawaTools.npcList[47] = true
horikawaTools.npcList[48] = true
horikawaTools.npcList[49] = true
horikawaTools.npcList[50] = true
horikawaTools.npcList[51] = true
horikawaTools.npcList[52] = true
horikawaTools.npcList[53] = true
horikawaTools.npcList[54] = true
horikawaTools.npcList[55] = true
horikawaTools.npcList[56] = false
horikawaTools.npcList[57] = false
horikawaTools.npcList[58] = false
horikawaTools.npcList[59] = true
horikawaTools.npcList[60] = false
horikawaTools.npcList[61] = true
horikawaTools.npcList[62] = false
horikawaTools.npcList[63] = true
horikawaTools.npcList[64] = false
horikawaTools.npcList[65] = true
horikawaTools.npcList[66] = false
horikawaTools.npcList[67] = false
horikawaTools.npcList[68] = false
horikawaTools.npcList[69] = false
horikawaTools.npcList[70] = false
horikawaTools.npcList[71] = true
horikawaTools.npcList[72] = true
horikawaTools.npcList[73] = true
horikawaTools.npcList[74] = true
horikawaTools.npcList[75] = false
horikawaTools.npcList[76] = true
horikawaTools.npcList[77] = true
horikawaTools.npcList[78] = false
horikawaTools.npcList[79] = false
horikawaTools.npcList[80] = false
horikawaTools.npcList[81] = false
horikawaTools.npcList[82] = false
horikawaTools.npcList[83] = false
horikawaTools.npcList[84] = false
horikawaTools.npcList[85] = false
horikawaTools.npcList[86] = true
horikawaTools.npcList[87] = false
horikawaTools.npcList[88] = false
horikawaTools.npcList[89] = true
horikawaTools.npcList[90] = false
horikawaTools.npcList[91] = false
horikawaTools.npcList[92] = false
horikawaTools.npcList[93] = true
horikawaTools.npcList[94] = false
horikawaTools.npcList[95] = false
horikawaTools.npcList[96] = false
horikawaTools.npcList[97] = false
horikawaTools.npcList[98] = false
horikawaTools.npcList[99] = false
horikawaTools.npcList[100] = false
horikawaTools.npcList[101] = false
horikawaTools.npcList[102] = false
horikawaTools.npcList[103] = false
horikawaTools.npcList[104] = false
horikawaTools.npcList[105] = false
horikawaTools.npcList[106] = false
horikawaTools.npcList[107] = false
horikawaTools.npcList[108] = true
horikawaTools.npcList[109] = true
horikawaTools.npcList[110] = true
horikawaTools.npcList[111] = true
horikawaTools.npcList[112] = true
horikawaTools.npcList[113] = true
horikawaTools.npcList[114] = true
horikawaTools.npcList[115] = true
horikawaTools.npcList[116] = true
horikawaTools.npcList[117] = true
horikawaTools.npcList[118] = true
horikawaTools.npcList[119] = true
horikawaTools.npcList[120] = true
horikawaTools.npcList[121] = true
horikawaTools.npcList[122] = true
horikawaTools.npcList[123] = true
horikawaTools.npcList[124] = true
horikawaTools.npcList[125] = true
horikawaTools.npcList[126] = true
horikawaTools.npcList[127] = true
horikawaTools.npcList[128] = true
horikawaTools.npcList[129] = true
horikawaTools.npcList[130] = true
horikawaTools.npcList[131] = true
horikawaTools.npcList[132] = true
horikawaTools.npcList[133] = false
horikawaTools.npcList[134] = false
horikawaTools.npcList[135] = true
horikawaTools.npcList[136] = true
horikawaTools.npcList[137] = true
horikawaTools.npcList[138] = false
horikawaTools.npcList[139] = false
horikawaTools.npcList[140] = false
horikawaTools.npcList[141] = false
horikawaTools.npcList[142] = false
horikawaTools.npcList[143] = false
horikawaTools.npcList[144] = false
horikawaTools.npcList[145] = false
horikawaTools.npcList[146] = false
horikawaTools.npcList[147] = false
horikawaTools.npcList[148] = false
horikawaTools.npcList[149] = false
horikawaTools.npcList[150] = false
horikawaTools.npcList[151] = false
horikawaTools.npcList[152] = false
horikawaTools.npcList[153] = false
horikawaTools.npcList[154] = false
horikawaTools.npcList[155] = false
horikawaTools.npcList[156] = false
horikawaTools.npcList[157] = false
horikawaTools.npcList[158] = false
horikawaTools.npcList[159] = false
horikawaTools.npcList[160] = false
horikawaTools.npcList[161] = true
horikawaTools.npcList[162] = true
horikawaTools.npcList[163] = true
horikawaTools.npcList[164] = true
horikawaTools.npcList[165] = true
horikawaTools.npcList[166] = true
horikawaTools.npcList[167] = true
horikawaTools.npcList[168] = true
horikawaTools.npcList[169] = false
horikawaTools.npcList[170] = false
horikawaTools.npcList[171] = false
horikawaTools.npcList[172] = true
horikawaTools.npcList[173] = true
horikawaTools.npcList[174] = true
horikawaTools.npcList[175] = true
horikawaTools.npcList[176] = true
horikawaTools.npcList[177] = true
horikawaTools.npcList[178] = false
horikawaTools.npcList[179] = false
horikawaTools.npcList[180] = true
horikawaTools.npcList[181] = false
horikawaTools.npcList[182] = false
horikawaTools.npcList[183] = false
horikawaTools.npcList[184] = false
horikawaTools.npcList[185] = false
horikawaTools.npcList[186] = false
horikawaTools.npcList[187] = false
horikawaTools.npcList[188] = false
horikawaTools.npcList[189] = true
horikawaTools.npcList[190] = false
horikawaTools.npcList[191] = false
horikawaTools.npcList[192] = false
horikawaTools.npcList[193] = false
horikawaTools.npcList[194] = true
horikawaTools.npcList[195] = true
horikawaTools.npcList[196] = false
horikawaTools.npcList[197] = false
horikawaTools.npcList[198] = false
horikawaTools.npcList[199] = true
horikawaTools.npcList[200] = true
horikawaTools.npcList[201] = true
horikawaTools.npcList[202] = false
horikawaTools.npcList[203] = true
horikawaTools.npcList[204] = true
horikawaTools.npcList[205] = true
horikawaTools.npcList[206] = true
horikawaTools.npcList[207] = true
horikawaTools.npcList[208] = 2
horikawaTools.npcList[209] = 2
horikawaTools.npcList[210] = true
horikawaTools.npcList[211] = false
horikawaTools.npcList[212] = false
horikawaTools.npcList[213] = false
horikawaTools.npcList[214] = false
horikawaTools.npcList[215] = false
horikawaTools.npcList[216] = false
horikawaTools.npcList[217] = false
horikawaTools.npcList[218] = false
horikawaTools.npcList[219] = false
horikawaTools.npcList[220] = false
horikawaTools.npcList[221] = false
horikawaTools.npcList[222] = false
horikawaTools.npcList[223] = false
horikawaTools.npcList[224] = false
horikawaTools.npcList[225] = false
horikawaTools.npcList[226] = false
horikawaTools.npcList[227] = false
horikawaTools.npcList[228] = false
horikawaTools.npcList[229] = true
horikawaTools.npcList[230] = true
horikawaTools.npcList[231] = true
horikawaTools.npcList[232] = true
horikawaTools.npcList[233] = true
horikawaTools.npcList[234] = true
horikawaTools.npcList[235] = true
horikawaTools.npcList[236] = true
horikawaTools.npcList[237] = false
horikawaTools.npcList[238] = false
horikawaTools.npcList[239] = false
horikawaTools.npcList[240] = false
horikawaTools.npcList[241] = false
horikawaTools.npcList[242] = true
horikawaTools.npcList[243] = true
horikawaTools.npcList[244] = true
horikawaTools.npcList[245] = true
horikawaTools.npcList[246] = false
horikawaTools.npcList[247] = true
horikawaTools.npcList[248] = false
horikawaTools.npcList[249] = false
horikawaTools.npcList[250] = false
horikawaTools.npcList[251] = false
horikawaTools.npcList[252] = false
horikawaTools.npcList[253] = false
horikawaTools.npcList[254] = false
horikawaTools.npcList[255] = false
horikawaTools.npcList[256] = 2
horikawaTools.npcList[257] = 2
horikawaTools.npcList[258] = false
horikawaTools.npcList[259] = false
horikawaTools.npcList[260] = false
horikawaTools.npcList[261] = true
horikawaTools.npcList[262] = true
horikawaTools.npcList[263] = 2
horikawaTools.npcList[264] = false
horikawaTools.npcList[265] = false
horikawaTools.npcList[266] = false
horikawaTools.npcList[267] = 2
horikawaTools.npcList[268] = 2
horikawaTools.npcList[269] = false
horikawaTools.npcList[270] = true
horikawaTools.npcList[271] = true
horikawaTools.npcList[272] = true
horikawaTools.npcList[273] = false
horikawaTools.npcList[274] = false
horikawaTools.npcList[275] = true
horikawaTools.npcList[276] = false
horikawaTools.npcList[277] = false
horikawaTools.npcList[278] = false
horikawaTools.npcList[279] = false
horikawaTools.npcList[280] = 2
horikawaTools.npcList[281] = 2
horikawaTools.npcList[282] = false
horikawaTools.npcList[283] = false
horikawaTools.npcList[284] = true
horikawaTools.npcList[285] = true
horikawaTools.npcList[286] = true
horikawaTools.npcList[287] = false
horikawaTools.npcList[288] = false
horikawaTools.npcList[289] = false
horikawaTools.npcList[290] = false
horikawaTools.npcList[291] = false
horikawaTools.npcList[292] = false

horikawaTools.powerUps = {}
horikawaTools.powerUps[9] = true
horikawaTools.powerUps[14] = true
horikawaTools.powerUps[34] = true
horikawaTools.powerUps[90] = true
horikawaTools.powerUps[169] = true
horikawaTools.powerUps[170] = true
horikawaTools.powerUps[182] = true
horikawaTools.powerUps[183] = true
horikawaTools.powerUps[184] = true
horikawaTools.powerUps[185] = true
horikawaTools.powerUps[186] = true
horikawaTools.powerUps[187] = true
horikawaTools.powerUps[249] = true
horikawaTools.powerUps[264] = true
horikawaTools.powerUps[273] = true
horikawaTools.powerUps[277] = true
horikawaTools.powerUps[287] = true

horikawaTools.isAShell = {}
horikawaTools.isAShell[5] = true
horikawaTools.isAShell[7] = true
horikawaTools.isAShell[24] = true
horikawaTools.isAShell[73] = true
horikawaTools.isAShell[113] = true
horikawaTools.isAShell[114] = true
horikawaTools.isAShell[115] = true
horikawaTools.isAShell[116] = true
horikawaTools.isAShell[172] = true
horikawaTools.isAShell[174] = true
horikawaTools.isAShell[195] = true
horikawaTools.isAShell[268] = true
horikawaTools.isAShell[281] = true


horikawaTools.powerUpList = {273, 187, 186, 90, 249, 185, 184, 9, 183, 182, 14, 277, 264, 170, 287, 169, 34}
horikawaTools.unHittableNPCs = {9, 10, 11, 13, 14, 16, 21, 22, 26, 30, 31, 32, 33, 34, 35, 40, 41, 45, 46, 56, 57, 58, 60, 62, 64, 66, 67, 68, 69, 70, 75, 78, 79, 80, 81, 82, 83, 84, 85, 87, 88, 90, 91, 92, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 133, 134, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 169, 170, 171, 178, 179, 181, 182, 183, 184, 185, 186, 187, 188, 190, 191, 192, 193, 196, 197, 198, 202, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 237, 238, 239, 240, 241, 246, 248, 249, 250, 251, 252, 253, 254, 255, 258, 259, 260, 264, 265, 266, 269, 273, 274, 276, 277, 278, 279, 282, 283, 287, 288, 289, 290, 291, 292}
horikawaTools.multiHitNPCs = {15, 44, 208, 209, 256, 257, 263, 267, 268, 280, 281}
horikawaTools.hittableNPCs = {1, 2, 3, 4, 5, 6, 7, 8, 12, 15, 17, 18, 19, 20, 23, 24, 25, 27, 28, 29, 36, 37, 38, 39, 42, 43, 44, 47, 48, 49, 50, 51, 52, 53, 54, 55, 59, 61, 63, 65, 71, 72, 73, 74, 76, 77, 86, 89, 93, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 135, 136, 137, 161, 162, 163, 164, 165, 166, 167, 168, 172, 173, 174, 175, 176, 177, 180, 189, 194, 195, 199, 200, 201, 203, 204, 205, 206, 207, 208, 209, 210, 229, 230, 231, 232, 233, 234, 235, 236, 242, 243, 244, 245, 247, 256, 257, 261, 262, 263, 267, 268, 270, 271, 272, 275, 280, 281, 284, 285, 286}

function horikawaTools.tableConcat(t1, t2)
	for i=1,#t2 do
		t1[#t1+1] = t2[i]
	end
	return t1
end

function horikawaTools.transformWorldCoordinates(x, y)
	return x - world.playerX, y - world.playerY
end

function horikawaTools.coordinatesToPixels(xCoordinate, yCoordinate)
	local leftSide, rightSide, topSide, bottomSide, returnX, returnY
	leftSide = math.floor(player.x) - player.screen.left
	rightSide = math.floor(player.x) + player.screen.right
	topSide = math.floor(player.y) - player.screen.top
	bottomSide = math.floor(player.y) + player.screen.bottom
	returnX = xCoordinate - leftSide
	returnY = yCoordinate - topSide
	return returnX, returnY
end


function horikawaTools.spairs(t, order)
	-- collect the keys
	local keys = {}
	for k in pairs(t) do keys[#keys+1] = k end

	-- if order function given, sort by it by passing the table and keys a, b,
	-- otherwise just sort the keys 
	if order then
		table.sort(keys, function(a,b) return order(t, a, b) end)
	else
		table.sort(keys)
	end

	-- return the iterator function
	local i = 0
	return function()
		i = i + 1
		if keys[i] then
			return keys[i], t[keys[i]]
		end
	end
end

function horikawaTools.makeBlockInvisible(blockRef)
	blockRef:mem(0x1E, FIELD_WORD, 0)
end

function horikawaTools.freezePlayer(pauseMusic, pauseSound, playerX, playerY)
	pauseMusic = pauseMusic or false
	pauseSound = pauseSound or false
	playerX = playerX or player.x
	playerY = playerY or player.y
	mem(0x00B2C8B4, FIELD_WORD, -1)
	player.x = playerX
	player.y = playerY
	player:mem(0x04, FIELD_WORD, -1)
	player:mem(0x11E, FIELD_WORD, 1)
	if pauseMusic then
		Audio.SeizeStream(-1)
		Audio.MusicPause()
	end
	if pauseSound then
		Audio.SfxPause(-1)
	end
end

function horikawaTools.makePlayerInvisible()
	player:mem(0x140, FIELD_WORD, 2)
	player:mem(0x142, FIELD_WORD, -1)
end


function horikawaTools.makeNPCInvisible(npcRef)
	npcRef:mem(0xE4, FIELD_WORD, 255)
	npcRef:mem(0xE8, FIELD_DFLOAT, 0)
end

function horikawaTools.removeInvalidNPCS(npcList)
	local i, j
	for i, j in pairs(npcList) do
		if (not j.isValid) then
			table.remove(npcList, i)
		end
	end
end

function horikawaTools.unFreezePlayer(musicPaused, soundPaused)
	musicPaused = musicPaused or false
	soundPaused = soundPaused or false
	mem(0x00B2C8B4, FIELD_WORD, 0)
	player:mem(0x04, FIELD_WORD, 0)
	player:mem(0x11E, FIELD_WORD, -1)
	if musicPaused then
		Audio.MusicResume()
		Audio.ReleaseStream(-1)
	end
	if soundPaused then
		Audio.SfxResume(-1)
	end
end

function horikawaTools.joinTables(t1, t2)
	for k,v in ipairs(t2) do
		table.insert(t1, v)
	end 
	return t1
end

function horikawaTools.isWorthHitting(inputNPC)
	local isHittable, isBoss
	for _, i in pairs(horikawaTools.unHittableNPCS) do
		if inputNPC.id == i then
			isHittable = false
			break
		else
			isHittable = true
		end
	end
	for _, i in pairs(horikawaTools.multiHitNPCs) do
		if inputNPC.id == i then
			isBoss = true
			break
		else
			isBoss = false
		end
	end
	if isHittable and not isBoss then
		return 1
	elseif isBoss then
		return 0
	else
		return false
	end
end

function horikawaTools.hitNPC(inputNPC)
	local hitType
	hitType = horikawaTools.isWorthHitting(inputNPC)
	if hitType == 1 then
		inputNPC:kill()
	elseif hitType == 0 then
		inputNPC:mem(0x148, FIELD_FLOAT, inputNPC:mem(0x148, FIELD_FLOAT) + 1)
	end
end


function horikawaTools.findRoot()
	local listOfFiles, directoryString, directoryLevelsUp
	listOfFiles = {}
	directoryLevelsUp = ""
	while (table.getn(listOfFiles) == 0) do
		directoryString = ".\\" .. directoryLevelsUp .. "LuaScriptsLib"
		listOfFiles = Misc.listLocalFiles(directoryString)
		directoryLevelsUp = directoryLevelsUp .. "..\\"
	end
	return directoryString
end

return horikawaTools