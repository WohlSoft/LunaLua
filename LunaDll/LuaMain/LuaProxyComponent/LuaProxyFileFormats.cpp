#include "../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../libs/PGE_File_Formats/file_formats.h"

static std::string getFullPath(const std::string &p)
{
    if (!isAbsolutePath(p))
        return WStr2Str(getEpisodeFolderPath() + Str2WStr(p));
    else
        return p;
}

static luabind::object getMeta(const FileFormatMeta &inMeta, lua_State *L)
{
    luabind::object meta = luabind::newtable(L);
    meta["isValid"] = inMeta.ReadFileValid;
    {
        luabind::object metaError = luabind::newtable(L);
        metaError["line"]       = inMeta.ERROR_linenum;
        metaError["lineDate"]   = inMeta.ERROR_linedata;
        metaError["info"]       = inMeta.ERROR_info;
        meta["error"] = metaError;
    }
    {
        luabind::object format  = luabind::newtable(L);
        format["type"]          = inMeta.RecentFormat;
        format["recentVersion"] = inMeta.RecentFormatVersion;
        meta["format"]          = format;
    }
    meta["fileName"] = inMeta.filename;
    meta["path"]     = inMeta.path;
    return meta;
}

static luabind::object getMeta(const ElementMeta &inMeta, lua_State *L)
{
    luabind::object meta = luabind::newtable(L);
    meta["arrayId"] = inMeta.array_id;
    meta["index"] = inMeta.index;
    meta["customParams"] = inMeta.custom_params;
    return meta;
}

luabind::object LuaProxy::Formats::openLevel(const std::string &filePath, lua_State *L)
{
    LevelData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenLevelFile(full_path, data);

    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    outData["levelName"]              = data.LevelName;
    outData["openLevelOnFail"]        = data.open_level_on_fail;
    outData["openLevelOnFailWarpId"]  = data.open_level_on_fail_warpID;

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelData::MusicOverrider & mo : data.music_overrides)
        {
            luabind::object e = luabind::newtable(L);
            e["type"]       = mo.type;
            e["id"]         = mo.id;
            e["fileName"]   = mo.fileName;
            arr[counter++] = e;
        }
        outData["musicOverrides"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelData::MusicOverrider & mo : data.sound_overrides)
        {
            luabind::object e = luabind::newtable(L);
            e["type"]       = mo.type;
            e["id"]         = mo.id;
            e["fileName"]   = mo.fileName;
            arr[counter++] = e;
        }
        outData["soundOverrides"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelSection & sct : data.sections)
        {
            luabind::object e = luabind::newtable(L);
            e["id"]         = sct.id;

            e["sizeTop"]    = sct.size_top;
            e["sizeBottom"] = sct.size_bottom;
            e["sizeLeft"]   = sct.size_left;
            e["sizeRight"]  = sct.size_right;

            e["musicId"]    = sct.music_id;
            e["musicFile"]  = sct.music_file;
            e["bgColor"]    = sct.bgcolor;
            e["backgroundId"] = sct.background;

            e["wrapH"]      = sct.wrap_h;
            e["wrapV"]      = sct.wrap_v;
            e["offScreenExit"] = sct.OffScreenEn;
            e["lockLeftScrool"] = sct.lock_left_scroll;
            e["lockRightScrool"] = sct.lock_right_scroll;
            e["lockUpScrool"] = sct.lock_up_scroll;
            e["lockDownScrool"] = sct.lock_down_scroll;
            e["isUnderWater"] = sct.underwater;

            arr[counter++] = e;
        }
        outData["sections"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(PlayerPoint & plr : data.players)
        {
            luabind::object e = luabind::newtable(L);
            e["id"]        = plr.id;
            e["x"]         = plr.x;
            e["y"]         = plr.y;
            e["w"]         = plr.w;
            e["h"]         = plr.h;
            e["direction"] = plr.direction;
            arr[counter++] = e;
        }
        outData["players"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelBlock & block : data.blocks)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = block.x;
            e["y"]         = block.y;
            e["w"]         = block.w;
            e["h"]         = block.h;
            e["autoscale"] = block.autoscale;
            e["id"]        = block.id;
            e["npcId"]     = block.npc_id;

            e["invisible"] = block.invisible;
            e["motionAiId"]= block.motion_ai_id;

            e["gfxName"]   = block.gfx_name;
            e["gfx_dx"]    = block.gfx_dx;
            e["gfx_dy"]    = block.gfx_dy;

            e["layer"]           = block.layer;
            e["eventDestroy"]    = block.event_destroy;
            e["eventHit"]        = block.event_hit;
            e["eventEmptyLayer"] = block.event_emptylayer;
            e["eventOnScreen"]   = block.event_on_screen;

            e["meta"] = getMeta(block.meta, L);

            arr[counter++] = e;
        }
        outData["blocks"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelBGO & bgo : data.bgo)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = bgo.x;
            e["y"]         = bgo.y;
            e["id"]        = bgo.id;

            e["zMode"]     = bgo.z_mode;
            e["zOffset"]   = bgo.z_offset;
            e["gfx_dx"]    = bgo.gfx_dx;
            e["gfx_dy"]    = bgo.gfx_dy;

            e["layer"]     = bgo.layer;

            e["meta"] = getMeta(bgo.meta, L);

            arr[counter++] = e;
        }
        outData["bgo"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelNPC & npc : data.npc)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = npc.x;
            e["y"]         = npc.y;
            e["direction"] = npc.direct;
            e["id"]        = npc.id;

            e["gfxName"]   = npc.gfx_name;
            e["gfx_dx"]    = npc.gfx_dx;
            e["gfx_dy"]    = npc.gfx_dy;

            e["contents"]  = npc.contents;
            e["specialData"] = npc.special_data;
            e["specialData2"] = npc.special_data2;
            {
                luabind::object gen = luabind::newtable(L);
                gen["enabled"]   = npc.generator;
                gen["direction"] = npc.generator_direct;
                gen["type"] = npc.generator_type;
                gen["period"] = npc.generator_period;
                gen["periodFrames"] = npc.generator_period_orig;
                gen["customAngle"] = npc.generator_custom_angle;
                gen["initialSpeed"] = npc.generator_initial_speed;
                e["generator"]     = gen;
            }

            e["msg"] = npc.msg;
            e["friendly"] = npc.friendly;
            e["noMove"] = npc.nomove;
            e["isBoss"] = npc.is_boss;

            e["layer"]     = npc.layer;
            e["attachLayer"]     = npc.attach_layer;

            {
                luabind::object evt = luabind::newtable(L);
                evt["activate"] = npc.event_activate;
                evt["die"] = npc.event_die;
                evt["talk"] = npc.event_talk;
                evt["emptyLayer"] = npc.event_emptylayer;
                evt["grab"] = npc.event_grab;
                evt["nextFrame"] = npc.event_nextframe;
                evt["touch"] = npc.event_touch;
                e["event"] = evt;
            }

            e["sendIdToVariable"]     = npc.send_id_to_variable;

            e["meta"] = getMeta(npc.meta, L);

            arr[counter++] = e;
        }
        outData["npc"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelDoor & warp : data.doors)
        {
            luabind::object e = luabind::newtable(L);
            e["ix"]         = warp.ix;
            e["iy"]         = warp.iy;
            e["isSetIn"]    = warp.isSetIn;

            e["ox"]         = warp.ox;
            e["oy"]         = warp.oy;
            e["isSetOut"]   = warp.isSetOut;

            e["twoWay"]     = warp.two_way;

            e["iDirect"]    = warp.idirect;
            e["oDirect"]    = warp.odirect;

            e["type"]       = warp.type;

            e["transitionEffect"] = warp.transition_effect;

            e["dstLevel"] = warp.lname;
            e["dstLevelWarpId"] = warp.warpto;

            e["levelEntrance"] = warp.lvl_i;
            e["levelExit"] = warp.lvl_o;

            e["dstWorldX"] = warp.world_x;
            e["dstWorldY"] = warp.world_y;

            e["unknown"]         = warp.unknown;
            e["isWarpInvisible"] = warp.unknown;

            e["noVehicles"] = warp.novehicles;
            e["allowNPC"] = warp.allownpc;
            e["locked"] = warp.locked;
            e["needABomb"] = warp.need_a_bomb;
            e["hideEnteringScene"] = warp.hide_entering_scene;
            e["allowNPCInterLevel"] = warp.allownpc_interlevel;
            e["specialStateRequired"] = warp.special_state_required;

            e["iLenght"] = warp.length_i;
            e["oLength"] = warp.length_o;

            {
                luabind::object evt = luabind::newtable(L);
                evt["enable"] = warp.cannon_exit;
                evt["speed"] = warp.cannon_exit_speed;
                e["cannonExit"] = evt;
            }

            e["layer"]     = warp.layer;

            {
                luabind::object evt = luabind::newtable(L);
                evt["enter"] = warp.event_enter;
                e["event"] = evt;
            }

            e["meta"] = getMeta(warp.meta, L);

            arr[counter++] = e;
        }
        outData["warps"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelPhysEnv & pez : data.physez)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = pez.x;
            e["y"]         = pez.y;
            e["w"]         = pez.w;
            e["h"]         = pez.h;
            e["envType"]   = pez.env_type;

            e["layer"]     = pez.layer;

            {
                luabind::object evt = luabind::newtable(L);
                evt["touch"] = pez.touch_event;
                e["event"] = evt;
            }

            e["meta"] = getMeta(pez.meta, L);

            arr[counter++] = e;
        }
        outData["physEnvZones"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelLayer & layer : data.layers)
        {
            luabind::object e = luabind::newtable(L);
            e["name"]         = layer.name;
            e["hidden"]       = layer.hidden;
            e["locked"]       = layer.locked;
            e["meta"] = getMeta(layer.meta, L);
            arr[counter++] = e;
        }
        outData["layers"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(LevelSMBX64Event & event : data.events)
        {
            luabind::object e = luabind::newtable(L);
            e["name"]         = event.name;
            e["msg"]          = event.msg;
            e["soundId"]      = event.sound_id;
            e["endGameType"]  = event.end_game;

            e["noSmoke"]      = event.nosmoke;

            {
                luabind::object ll = luabind::newtable(L);
                size_t counter_2 = 0;
                for(PGESTRING &l : event.layers_hide)
                    ll[counter_2++] = l;
                e["layersToHide"] = ll;
            }
            {
                luabind::object ll = luabind::newtable(L);
                size_t counter_2 = 0;
                for(PGESTRING &l : event.layers_show)
                    ll[counter_2++] = l;
                e["layersToShow"] = ll;
            }
            {
                luabind::object ll = luabind::newtable(L);
                size_t counter_2 = 0;
                for(PGESTRING &l : event.layers_toggle)
                    ll[counter_2++] = l;
                e["layersToToggle"] = ll;
            }
            e["meta"] = getMeta(event.meta, L);
            arr[counter++] = e;
        }
        outData["events"] = arr;
    }


    return outData;
}


luabind::object LuaProxy::Formats::openLevelHeader(const std::string &filePath, lua_State *L)
{
    LevelData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenLevelFileHeader(full_path, data);

    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    outData["levelName"]              = data.LevelName;
    outData["openLevelOnFail"]        = data.open_level_on_fail;
    outData["openLevelOnFailWarpId"]  = data.open_level_on_fail_warpID;

    return outData;
}


luabind::object LuaProxy::Formats::openWorld(const std::string &filePath, lua_State *L)
{
    WorldData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenWorldFile(full_path, data);

    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    return outData;
}


luabind::object LuaProxy::Formats::openWorldHeader(const std::string &filePath, lua_State *L)
{
    WorldData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenWorldFileHeader(full_path, data);

    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    return outData;
}


NPCConfigFile LuaProxy::Formats::openNpcConfig(const std::string &filePath, lua_State *L)
{
    NPCConfigFile data;
    std::string full_path = getFullPath(filePath);
    FileFormats::ReadNpcTXTFileF(full_path, data);
    return data;
}
