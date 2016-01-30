--graphX.lua 
--v0.0.2
--Pretty blatantly based on colliders.lua by Hoeloe

local graphX = {}


	local function getScreenBounds()
		local h = (player:mem(0xD0, FIELD_DFLOAT));
		local b = { left = player.x-400+player.speedX, right = player.x+400+player.speedX, top = player.y-260+player.speedY, bottom = player.y+340+player.speedY };
		
		local sect = Section(player.section);
		local bounds = sect.boundary;

		if(b.left < bounds.left - 10) then
			b.left = bounds.left - 10;
			b.right = b.left + 800;
		end
		
		if(b.right > bounds.right - 10) then
			b.right = bounds.right - 10;
			b.left = b.right - 800;
		end
		
		if(b.top < bounds.top+40-h) then
			b.top = bounds.top+40-h;
			b.bottom = b.top + 600;
		end
		
		if(b.bottom > bounds.bottom+40-h) then
			b.bottom = bounds.bottom+40-h;
			b.top = b.bottom - 600;
		end
		
		return b;
		
	end

	local function worldToScreen(x,y)
		local b = getScreenBounds();
		local x1 = x-b.left;
		local y1 = y-b.top-(player:mem(0xD0, FIELD_DFLOAT))+30;
		return x1,y1;
	end

	
	function graphX.boxLevel (x,y,w,h, col)
		local x1,y1 = worldToScreen (x - 10, y + 10);
		graphX.boxScreen (x1,y1,w,h, col)
	end
	
	function graphX.boxScreen (x,y,w,h, col)
		col = col or 0xFF000099;
		Graphics.glSetTextureRGBA (nil, col);
		local pts = {};
		local x1,y1 = x,y;
		pts[0] = x1; 	pts[1] = y1;
		pts[2] = x1+w;	pts[3] = y1;
		pts[4] = x1;	pts[5] = y1+h;
		pts[6] = x1;	pts[7] = y1+h;
		pts[8] = x1+w;	pts[9] = y1+h;
		pts[10] = x1+w; pts[11] = y1;
		
		Graphics.glDrawTriangles (pts, {}, 6);
		Graphics.glSetTextureRGBA (nil, 0xFFFFFFFF);
	end
	
	
	function graphX.circleLevel (x,y,r, col)
		local x1,y1 = worldToScreen (x - 10, y + 10);
		graphX.circleScreen (x1,y1,r, col)
	end
	
	function graphX.circleScreen (x,y,r, col)
		col = col or 0xFF000099;
		Graphics.glSetTextureRGBA (nil, col);
		
		local pts = circleToTris(x,y,r);
		
		Graphics.glDrawTriangles (pts, {}, (#pts + 1)/2);
		Graphics.glSetTextureRGBA (nil, 0xFFFFFFFF);
	end
		

	function circleToTris(x,y,r)
		local x1 = x
		local y1 = y;
		local pts = {};
		local m = math.ceil(math.sqrt(r));
		if(m < 1) then m = 1; end
		local s = (math.pi/2)/m;
		local ind = 0;
		local xmult = 1;
		local ymult = -1;
		for n=1,4 do
			local lx = 0;
			local ly = 1;
			for i=1,m do
				local xs = math.cos((math.pi/2)-s*i);
				local ys = math.sin((math.pi/2)-s*i);
				pts[ind] = x1;
				pts[ind+1] = y1;
				pts[ind+2] = x1+xmult*r*lx;
				pts[ind+3] = y1+ymult*r*ly;
				pts[ind+4] = x1+xmult*r*xs;
				pts[ind+5] = y1+ymult*r*ys;
				ind = ind+6;
				lx = xs;
				ly = ys;
			end
			if xmult == 1 then
				if ymult == -1 then
					ymult = 1;
				elseif ymult == 1 then
					xmult = -1;
				end
			elseif xmult == -1 then
				if ymult == -1 then
					xmult = 1;
				elseif ymult == 1 then
					ymult = -1;
				end
			end
		end
		return pts;
	end

		
		
		
return graphX;