local warn = {}

local msgq = {}
local twarnstack = {};

local textblox;
warn.textbloxActive = pcall(function() textblox = API.load("textblox") end) --load textblox if it is available, otherwise use default text.

local starttime;
local lasttime;
local warningOutputPath;

Text.showWarnings = true;

function warn.onInitAPI()
	registerEvent(warn, "onExitLevel", "onLevelExit", false);
	registerEvent(warn, "onKeyboardPress", "onKeyboardPress", false);
	registerEvent(warn, "onDraw", "onDraw", false);
	starttime = os.clock();
	lasttime = os.clock();
end

function warn.onKeyboardPress(keycode)
	if(keycode == VK_F6)then
		Text.showWarnings = not Text.showWarnings;
	end
end

function Text.warn(msg)
	if(mem(0x00B2C62A, FIELD_WORD) == 0) then
		msg = tostring(msg);
		local baseinfo = debug.getinfo(1);
		
		local m = debug.traceback();
		local _,i = m:find("in function 'warn'");
		i = i+1;
		m = "stack traceback:"..m:sub(i);
		
		local message;
		if(msg ~= nil) then
			message = ": "..msg;
		else
			message = ""
		end
		local tick = lunatime.tick();
		local info = debug.getinfo(2);
		table.insert(msgq, tick.."t - Warning"..message.."\n"..m)
		if(msg ~= nil) then
			if(#msg > 60) then
				msg = msg:sub(0,57).."...";
			end
			message = ": "..msg.." - ";
		else
			msg = "";
			message = "arning: "
		end
		local tm = info.short_src:sub(info.short_src:match(".*[/\\]()"))..": "..info.currentline;
		if(warningOutputPath == nil) then
			warningOutputPath = "WARNING_"..os.date("%Y-%m-%d_%H-%M-%S")..".txt";
			Misc.warning("See output "..warningOutputPath.." for complete traceback.");
			Misc.warning("");
		end
		Misc.warning(tick.."t - "..msg.." ("..tm..")");
		table.insert(twarnstack, {m=tick.."t - Warning"..message..tm, t=5})
	end
end

function warn.onLevelExit()
	if(#msgq > 0) then
		local m = "";
		for k,v in ipairs(msgq) do
			m = m..v;
			if(k < #msgq) then
				m = m.."\n\n";
			end
		end
		Audio.MusicStop();
		Audio.SfxStop(-1);
		
		local f;
		if(not pcall(function() f = io.open(getSMBXPath().."\\"..warningOutputPath,"w") end)) then
			error("Could not generate Warning file.")
		else
			f:write("Execution ran for "..(os.clock()-starttime).."s ("..lunatime.tick().." ticks), and produced "..#msgq.." warnings:\n\n\n"..m);
			f:close();
		end
	end
end

function warn.onDraw()
	if(mem(0x00B2C62A, FIELD_WORD) == 0 and Text.showWarnings and #twarnstack > 0) then
		local i = 1;
		local m = "";
		while i < #twarnstack do
			twarnstack[i].t = twarnstack[i].t - (os.clock()-lasttime);
			if(twarnstack[i].t <= 0) then
				table.remove(twarnstack,i);
			else
			if(not warn.textbloxActive) then
				Text.printWP(twarnstack[i].m,0,i*18,10);
			elseif(i > #twarnstack-55) then
				m = m.."<br>"..twarnstack[i].m;
			end
				i = i + 1;
			end
		end
		
		lasttime = os.clock();
		
		if(warn.textbloxActive) then
			textblox.printExt(m,{x=4,y=-6,font=textblox.FONT_SPRITEDEFAULT4,color=0xFF0000FF,z=10})
		end
	end
end

return warn;