local exts={
				"webm","mkv","flv","ogv","ogm","avi","mov","rm","rmvb","asf","mp4","m4p","m4v","mpg","mp2","mpeg","mpe","mpv","m2v","3gp","3g2","f4v","f4p","f4a","f4b",
				"vob","mng"--works?
				}
local extFlags={}
local gfxPrefs={"block","background2","npc","effect","background","mario","luigi","toad","link","yoshib","yoshit"}
local gfxPFlags={}
for _,v in pairs(exts) do
    extFlags[v]=true
end
for _,v in pairs(gfxPrefs) do
    gfxPFlags[v]=true
end

local function loadVideo(fList)
    local pref,num,ex
    for _,v in pairs(fList) do
        pref,num,ex=string.match(v,"^(.+)%-(%d+)%.(.+)$")
        
        if pref and num and ex and gfxPFlags[pref] and tonumber(num,10) and extFlags[ex] then
            Graphics.sprites[pref][tonumber(num,10)].img=Graphics.loadImage(Misc.resolveFile(v))
        end
    end
end

function onStart()
    loadVideo(Misc.listLocalFiles("../"))
    loadVideo(Misc.listLocalFiles(""))
end