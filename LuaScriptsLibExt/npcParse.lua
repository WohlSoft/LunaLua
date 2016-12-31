-- npcParse.lua
-- v1.2

local lunajson = API.load("ext/lunajson")
local pnpc = API.load("pnpc")
local npcParse = {}

local textblox
local textbloxActive = false

if  pcall(function() textblox = API.load("textblox") end)  then
	textbloxActive = true;
end



npcParse.debug = false



npcParse.externalJson = {}
npcParse.idGroups = {}


local areAllLoaded = false
npcParse.active = true
npcParse.clearAfter = true
local useLoadstring = true


function npcParse.onInitAPI ()
	registerEvent(npcParse, "onStart", "onStartStart", true)
	registerEvent(npcParse, "onStart", "onStartEnd", false)
	registerEvent(npcParse, "onTick", "onTickStart", true)
	registerEvent(npcParse, "onTick", "onTickEnd", false)
	registerEvent(npcParse, "onCameraUpdate", "onCameraUpdate", false)
end


-- This function loads the data from the npcdata.json file
local function loadExternalJson ()	
	-- Get the full file path
	local p = Misc.resolveFile("npcdata.json");
	
	-- If an invalid path, end now
	if (p == nil) then 
		return
	end
	
	-- Create the table and open the file
	local f = io.open (p, "r");
	local str = f:read ("*all")
	
	-- Parse the data on a line-by-line basis
	pcall(function () npcParse.externalJson = lunajson.decode(str) end)
end


-- Call this function to check whether a given NPC has been parsed yet and whether it was successful; 
-- returns nil if not parsed, false if unsuccessful attempt and true if successful parsing
function npcParse.getSuccessful (pnpcRef)
	if  pnpcRef.data.npcParse ~= nil  then
		return pnpcRef.data.npcParse.success
	else
		return nil
	end
end


-- Call this function to parse non-NPC.msg strings
local function getStringTable (str)
	local data = nil
	
	-- Parse with the configured method
	if  useLoadstring  then	
		
		-- Ensure the string starts with a curly bracket, otherwise leave data nil
		if  string.sub(str, 1,1) == "{"  then
			
			-- Load the table string
			local f, errorStr = loadstring ("return "..str);

			-- Does the parsed table return any errors?
			if  f == nil  then
				return nil, errorStr
			end

			-- Get the table
			data = f()
		end
		
	else
		-- If parsing the table would cause an error, leave data nil
		pcall(function () data = lunajson.decode(str) end)
	end
	
	
	-- Validation check 2: make sure the parsed code functions as a table
	if  type(data) == "table"  then
		
		-- Return the table
		return data, "TABLE SUCCESSFULLY PARSED"
	else
		return nil, "ERROR: NOT RECOGNIZED AS A TABLE"
	end
end

-- Call this function to parse a specific NPC
function npcParse.loadMsgData (npc, force)
	
	-- Only wrap non-generators until that pnpc bug is fixed
	if  npc:mem(0x64, FIELD_BOOL) == false  then
	
		-- Wrap for the reference
		local pnpcRef = pnpc.wrap (npc)

		-- Initialize the parse data
		local parseData = {}

		-- Determine where the message string should be copied from and whether it should be parsed and loaded
		parseData.success = npcParse.getSuccessful(pnpcRef)
		local shouldLoad = true
			
		if  parseData.success == nil  then
			areAllLoaded = false
			parseData.msg = npc.msg.str
		else
			parseData.msg = pnpcRef.data.npcParse.msg  or  ""
			parseData.success = pnpcRef.data.npcParse.success
			shouldLoad = force
		end
		
		
		-- Parse and load the table if necessary/forced
		if  shouldLoad  then

			-- Get data table
			local dataTable = getStringTable (parseData.msg)

			-- Get other data from external json (all NPCs)
			if  npcParse.externalJson["_ALL"] ~= nil  then
				local extData = npcParse.externalJson["_ALL"]
				for  k,v in pairs (extData)  do
					pnpcRef.data[k] = v
				end
			end
					
			-- If a valid table, split master data table up into separate pnpc data tables and load data from external JSON
			if dataTable ~= nil  then

				-- In-message properties
				for  k,v in pairs (dataTable)  do
					pnpcRef.data[k] = v
				end
			
				-- Get other data from external json (all IDed NPCs)
				if  npcParse.externalJson["_ID"] ~= nil  and  dataTable.id ~= nil  then
					local extData = npcParse.externalJson["_ID"]
					for  k,v in pairs (extData)  do
						pnpcRef.data[k] = v
					end
				end
				
				-- Get other data from external json
				if  npcParse.externalJson [dataTable.id] ~= nil  then
					local extData = npcParse.externalJson [dataTable.id]
					for  k,v in pairs (extData)  do
						pnpcRef.data[k] = v
					end
				end
				parseData.success = true
				
				-- Store reference in the appropriate id group
				if  dataTable.id ~= nil  then
					if  npcParse.idGroups[dataTable.id] == nil  then
						npcParse.idGroups[dataTable.id] = {}
					end
					local alreadyIndexed = false
					for _, value in pairs(npcParse.idGroups[dataTable.id]) do
						if value == pnpcRef then
							alreadyIndexed = true
						end
					end
					if  not alreadyIndexed  then
						table.insert(npcParse.idGroups[dataTable.id], pnpcRef)
					end
				end
				
					
				-- Clear the NPC's message string after successful loading if configured to do so
				if  npcParse.clearAfter == true then

					if  pnpcRef.data.newMsg ~= nil  then
						npc.msg = pnpcRef.data.newMsg
					else
						npc.msg = ""
					end
				end			
			end
			
			-- add parseData as this library's data
			pnpcRef.data.npcParse = parseData
		end
	end
end

-- Call this function to parse all NPCs
function npcParse.loadAllMsgData (force)		
	for  k,v in pairs(NPC.get())  do
		npcParse.loadMsgData (v, force)
	end
	
	areAllLoaded = true
end

-- Check whether all NPCs are loaded
function npcParse.allLoaded ()
	return areAllLoaded;
end



-- Update npcParse
local function updateParsing ()
	for  k,v in pairs(NPC.get())  do
		if  npcParse.getSuccessful (pnpc.wrap(v)) == nil  then
			areAllLoaded = false
			break;
		end
	end
	
	if npcParse.active == true  and  areAllLoaded == false  then
		npcParse.loadAllMsgData ()
	end
end

-- Call update in onStart and then at the beginning and end of every tick
function npcParse.onStartStart ()
	loadExternalJson ()
	updateParsing ()
end
function npcParse.onStartEnd ()
	updateParsing ()
end
function npcParse.onTickStart ()
	--updateParsing ()
end
function npcParse.onTickEnd ()
	--updateParsing ()
end


-- Debug stuff
function npcParse.onCameraUpdate (eventObj, cameraIndex)

	if  npcParse.debug ~= true  then  return;  end;
	
	-- Draw the tables
	local cam = Camera.get()[cameraIndex]
	
	for k,v in pairs (NPC.get()) do
		local pnpcRef = pnpc.wrap (v)
		local within = v.x > cam.x+128  and  v.x < cam.x+cam.width-128  and  v.y > cam.y+96  and  v.y < cam.y+cam.height-96
		
		if  pnpcRef.data ~= nil  and  textbloxActive  and  within  then
			textblox.printTable (pnpcRef.data, 
								 {x = pnpcRef.x,
								  y = math.floor(pnpcRef.y/16)*16 - 32,
								  bind = textblox.BIND_LEVEL,
								  halign = textblox.ALIGN_LEFT,
								  valign = textblox.ALIGN_BOTTOM,
								  font = textblox.FONT_SPRITEDEFAULT4})
		end
	end
end


return npcParse