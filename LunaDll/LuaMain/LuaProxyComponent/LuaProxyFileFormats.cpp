#include "../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../libs/PGE_File_Formats/file_formats.h"
#include "../../libs/json/json.hpp"
#include "../../Misc/TestMode.h"
#include <stack>

using json = nlohmann::json;

struct LuaSACKS : public nlohmann::json_sax<json>
{
    luabind::object data;
    lua_State *L = nullptr;

    LuaSACKS() : nlohmann::json_sax<json>()
    {}

    ~LuaSACKS()
    {
        cleanStack();
    }

    void cleanStack()
    {
        while(!stack.empty())
            stack.pop();
    }

    struct State
    {
        std::string     key;
        size_t          couter = 1;
        size_t          totalElements = 0;
        bool            isArray = false;
        luabind::object lua;
    };

    std::stack<State> stack;
    State             cs;
    bool              isRoot = true;

    template<class T>
    void addValue(const T &value)
    {
        assert(cs.lua.is_valid());
        if(cs.isArray)
            cs.lua[cs.couter++] = value;
        else
            cs.lua[cs.key] = value;
    }

    // called when null is parsed
    bool null()
    {
        // Do nothing
        return true;
    }

    // called when a boolean is parsed; value is passed
    bool boolean(bool val)
    {
        addValue(val);
        return true;
    }

    // called when a signed or unsigned integer number is parsed; value is passed
    bool number_integer(number_integer_t val)
    {
        addValue(static_cast<long>(val));
        return true;
    }
    bool number_unsigned(number_unsigned_t val)
    {
        addValue(static_cast<unsigned long>(val));
        return true;
    }

    // called when a floating-point number is parsed; value and original string is passed
    bool number_float(number_float_t val, const string_t &)
    {
        addValue(static_cast<double>(val));
        return true;
    }

    // called when a string is parsed; value is passed and can be safely moved away
    bool string(string_t& val)
    {
        addValue(val);
        return true;
    }

    bool startObj(std::size_t elements, bool isArray)
    {
        if(isRoot)
        {
            isRoot = false;
            cs = State();
            data = luabind::newtable(L);
            cs.lua = data;
        }
        else
        {
            State ncs = State();
            if(cs.isArray)
            {
                size_t count = cs.couter++;
                cs.lua[count] = luabind::newtable(L);
                ncs.lua = cs.lua[count];
            }
            else
            {
                cs.lua[cs.key] = luabind::newtable(L);
                ncs.lua = cs.lua[cs.key];
            }
            stack.push(cs);
            cs = ncs;
        }
        cs.couter = 1;
        cs.totalElements = elements;
        cs.isArray = isArray;
        cs.key.clear();
        return true;
    }

    bool endObj()
    {
        if(stack.empty())
        {
            isRoot = true;
        }
        else
        {
            cs = stack.top();
            stack.pop();
        }
        return true;
    }

    // called when an object or array begins or ends, resp. The number of elements is passed (or -1 if not known)
    bool start_object(std::size_t elements)
    {
        return startObj(elements, false);
    }
    bool end_object()
    {
        return endObj();
    }

    bool start_array(std::size_t elements)
    {
        return startObj(elements, true);
    }

    bool end_array()
    {
        return endObj();
    }

    // called when an object key is parsed; value is passed and can be safely moved away
    bool key(string_t &val)
    {
        cs.key = val;
        return true;
    }

    // called when a parse error occurs; byte position, the last token, and an exception is passed
    bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::detail::exception &ex)
    {
        if(!data.is_valid())
            data = luabind::newtable(L);
        data["error"] = luabind::newtable(L);
        data["error"]["position"] = position;
        data["error"]["last_token"] = last_token;
        data["error"]["exception"] = ex.what();
        cleanStack();
        return true;
    }
};

static luabind::object dumpFromJson(const std::string &son, lua_State *L)
{
    if(!son.empty())
    {
        LuaSACKS sax;
        sax.L = L;
        json::sax_parse(son, &sax);
        return sax.data;
    }
    else
        return luabind::newtable(L);
}

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
        metaError["lineData"]   = inMeta.ERROR_linedata;
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
    meta["data"] = dumpFromJson(inMeta.custom_params, L);
    return meta;
}

luabind::object LuaProxy::Formats::openLevelHeader(const std::string &filePath, lua_State *L)
{
    LevelData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenLevelFileHeader(full_path, data);

    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    outData["levelName"]              = data.LevelName;
    outData["stars"]                  = data.stars;
    outData["openLevelOnFail"]        = data.open_level_on_fail;
    outData["openLevelOnFailWarpId"]  = data.open_level_on_fail_warpID;
    outData["data"]                   = dumpFromJson(data.custom_params, L);

    return outData;
}


static luabind::object openLevelInternal(const LevelData &data, lua_State *L)
{
    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    outData["levelName"]              = data.LevelName;
    outData["stars"]                  = data.stars;
    outData["openLevelOnFail"]        = data.open_level_on_fail;
    outData["openLevelOnFailWarpId"]  = data.open_level_on_fail_warpID;
    outData["data"]                   = dumpFromJson(data.custom_params, L);

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelData::MusicOverrider & mo : data.music_overrides)
        {
            luabind::object e = luabind::newtable(L);
            e["type"]       = static_cast<int>(mo.type);
            e["id"]         = mo.id;
            e["fileName"]   = mo.fileName;
            arr[++counter] = e;
        }
        outData["musicOverrides"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelData::MusicOverrider & mo : data.sound_overrides)
        {
            luabind::object e = luabind::newtable(L);
            e["type"]       = static_cast<int>(mo.type);
            e["id"]         = mo.id;
            e["fileName"]   = mo.fileName;
            arr[++counter] = e;
        }
        outData["soundOverrides"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelSection & sct : data.sections)
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
            e["lightingValue"] = sct.lighting_value;
            e["lockLeftScrool"] = sct.lock_left_scroll;
            e["lockRightScrool"] = sct.lock_right_scroll;
            e["lockUpScrool"] = sct.lock_up_scroll;
            e["lockDownScrool"] = sct.lock_down_scroll;
            e["isUnderWater"] = sct.underwater;

            e["data"] = dumpFromJson(sct.custom_params, L);

            arr[++counter] = e;
        }
        outData["sections"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const PlayerPoint & plr : data.players)
        {
            luabind::object e = luabind::newtable(L);
            e["id"]        = plr.id;
            e["x"]         = plr.x;
            e["y"]         = plr.y;
            e["w"]         = plr.w;
            e["h"]         = plr.h;
            e["direction"] = plr.direction;
            arr[++counter] = e;
        }
        outData["players"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelBlock & block : data.blocks)
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

            arr[++counter] = e;
        }
        outData["blocks"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelBGO & bgo : data.bgo)
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

            arr[++counter] = e;
        }
        outData["bgo"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelNPC & npc : data.npc)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = npc.x;
            e["y"]         = npc.y;
            e["direction"] = npc.direct;
            e["id"]        = static_cast<unsigned long>(npc.id);

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
                gen["periodOrignUnit"] = static_cast<int>(npc.generator_period_orig_unit);
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

            arr[++counter] = e;
        }
        outData["npc"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelDoor & warp : data.doors)
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

            arr[++counter] = e;
        }
        outData["warps"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelPhysEnv & pez : data.physez)
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

            arr[++counter] = e;
        }
        outData["physEnvZones"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelLayer & layer : data.layers)
        {
            luabind::object e = luabind::newtable(L);
            e["name"]         = layer.name;
            e["hidden"]       = layer.hidden;
            e["locked"]       = layer.locked;
            e["meta"] = getMeta(layer.meta, L);
            arr[++counter] = e;
        }
        outData["layers"] = arr;
    }

    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const LevelSMBX64Event & event : data.events)
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
                for(const PGESTRING &l : event.layers_hide)
                    ll[++counter_2] = l;
                e["layersToHide"] = ll;
            }
            {
                luabind::object ll = luabind::newtable(L);
                size_t counter_2 = 0;
                for(const PGESTRING &l : event.layers_show)
                    ll[++counter_2] = l;
                e["layersToShow"] = ll;
            }
            {
                luabind::object ll = luabind::newtable(L);
                size_t counter_2 = 0;
                for(const PGESTRING &l : event.layers_toggle)
                    ll[++counter_2] = l;
                e["layersToToggle"] = ll;
            }

            {
                size_t counter_2 = 0;
                luabind::object sets = luabind::newtable(L);
                for(const LevelEvent_Sets &s : event.sets)
                {
                    luabind::object set = luabind::newtable(L);
                    set["id"] = s.id;
                    set["musicId"] = s.music_id;
                    set["musicFile"] = s.music_file;
                    set["backgroundId"] = s.background_id;

                    set["positionLeft"] = s.position_left;
                    set["positionTop"] = s.position_top;
                    set["positionBottom"] = s.position_bottom;
                    set["positionRight"] = s.position_right;

                    set["expressionPosX"] = s.expression_pos_x;
                    set["expressionPosY"] = s.expression_pos_y;
                    set["expressionPosW"] = s.expression_pos_w;
                    set["expressionPosH"] = s.expression_pos_h;

                    set["autoscroll"] = s.autoscrol;
                    set["autoscrollSpeedX"] = s.autoscrol_x;
                    set["autoscrollSpeedY"] = s.autoscrol_y;

                    set["expressionAutoscroolSpeedX"] = s.expression_autoscrool_x;
                    set["expressionAutoscroolSpeedY"] = s.expression_autoscrool_y;

                    sets[++counter_2] = set;
                }
                e["sectionSets"] = sets;
            }

            e["triggerEvent"] = event.trigger;
            e["triggerEventTimer"] = event.trigger_timer;
            e["triggerEventTimerOrigUnit"] = static_cast<int>(event.trigger_timer_unit);
            e["triggerEventTimerOrig"] = event.trigger_timer_orig;

            e["controlsEnable"] = event.ctrls_enable;

            e["controlUp"] = event.ctrl_up;
            e["controlDown"] = event.ctrl_down;
            e["controlLeft"] = event.ctrl_left;
            e["controlRight"] = event.ctrl_right;

            e["controlJump"] = event.ctrl_jump;
            e["controlAltJump"] = event.ctrl_altjump;
            e["controlRun"] = event.ctrl_run;
            e["controlAltRun"] = event.ctrl_altrun;

            e["controlStart"] = event.ctrl_start;
            e["controlDrop"] = event.ctrl_drop;

            e["controlLockKeyboard"] = event.ctrl_lock_keyboard;

            // TODO: BIND ENUMS, OTHERWISE UNBUILDABLE!!!
            e["autostart"] = event.autostart;

            e["autostartCondition"] = event.autostart_condition;

            // Move layers
            // Spawn Effect
            // Spawn NPC
            // Update variable
            // timerDef

            e["triggerScript"] = event.trigger_script;
            e["triggerApiId"] = event.trigger_api_id;

            e["moveLayer"] = event.movelayer;
            e["moveLayerSpeedX"] = event.layer_speed_x;
            e["moveLayerSpeedY"] = event.layer_speed_y;

            e["moveCameraX"] = event.move_camera_x;
            e["moveCameraY"] = event.move_camera_y;
            e["moveCameraSection"] = event.scroll_section;

            e["meta"] = getMeta(event.meta, L);
            arr[++counter] = e;
        }
        outData["events"] = arr;
    }

    return outData;
}

luabind::object LuaProxy::Formats::openLevel(const std::string &filePath, lua_State *L)
{
    LevelData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenLevelFile(full_path, data);
    return openLevelInternal(data, L);
}

luabind::object LuaProxy::Formats::getLevelData(lua_State *L)
{
    return openLevelInternal(getCurrentLevelData(), L);
}

luabind::object LuaProxy::Formats::openWorldHeader(const std::string &filePath, lua_State *L)
{
    WorldData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenWorldFileHeader(full_path, data);

    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);


    outData["episodeTitle"]           = data.EpisodeTitle;
    {
        size_t counter = 0;
        luabind::object sets = luabind::newtable(L);
        for(bool noCharacter : data.nocharacter)
        {
            sets[++counter] = noCharacter;
        }
        outData["disableCharacters"] = sets;
    }

    outData["introLevel"]           = data.IntroLevel_file;
    outData["gameOverLevel"]        = data.GameOverLevel_file;
    outData["isHubStyleWorld"]      = data.HubStyledWorld;
    outData["autoRestartLevel"]     = data.restartlevel;
    outData["restrictSinglePlayer"] = data.restrictSinglePlayer;
    outData["restrictSecureGameSave"] = data.restrictSecureGameSave;
    outData["disableEnterScreen"]   = data.disableEnterScreen;

    outData["cheatsPolicy"] = data.cheatsPolicy;
    {
        size_t counter = 0;
        luabind::object sets = luabind::newtable(L);
        for(PGESTRING &cheat : data.cheatsList)
        {
            sets[++counter] = cheat;
        }
        outData["cheatsList"] = sets;
    }
    outData["saveResumePolicy"] = data.saveResumePolicy;
    outData["saveAuto"]         = data.saveAuto;
    outData["saveLocker"]       = data.saveLocker;
    outData["saveLockerEx"]     = data.saveLockerEx;
    outData["saveLockerMsg"]    = data.saveLockerMsg;
    outData["showEverything"]   = data.showEverything;

    outData["stars"]            = data.stars;
    outData["inventoryLimit"]   = data.inventoryLimit;

    outData["authors"]          = data.authors;
    outData["data"]             = dumpFromJson(data.custom_params, L);

    return outData;
}

static luabind::object openWorldInternal(const WorldData& data, lua_State* L)
{
    luabind::object outData = luabind::newtable(L);
    outData["meta"] = getMeta(data.meta, L);

    outData["episodeTitle"]           = data.EpisodeTitle;
    {
        size_t counter = 0;
        luabind::object sets = luabind::newtable(L);
        for(bool noCharacter : data.nocharacter)
        {
            sets[++counter] = noCharacter;
        }
        outData["disableCharacters"] = sets;
    }

    outData["introLevel"]           = data.IntroLevel_file;
    outData["gameOverLevel"]        = data.GameOverLevel_file;
    outData["isHubStyleWorld"]      = data.HubStyledWorld;
    outData["autoRestartLevel"]     = data.restartlevel;
    outData["restrictSinglePlayer"] = data.restrictSinglePlayer;
    outData["restrictSecureGameSave"] = data.restrictSecureGameSave;
    outData["disableEnterScreen"]   = data.disableEnterScreen;

    outData["cheatsPolicy"] = data.cheatsPolicy;
    {
        size_t counter = 0;
        luabind::object sets = luabind::newtable(L);
        for(const PGESTRING &cheat : data.cheatsList)
        {
            sets[++counter] = cheat;
        }
        outData["cheatsList"] = sets;
    }
    outData["saveResumePolicy"] = data.saveResumePolicy;
    outData["saveAuto"]         = data.saveAuto;
    outData["saveLocker"]       = data.saveLocker;
    outData["saveLockerEx"]     = data.saveLockerEx;
    outData["saveLockerMsg"]    = data.saveLockerMsg;
    outData["showEverything"]   = data.showEverything;

    outData["stars"]            = data.stars;
    outData["inventoryLimit"]   = data.inventoryLimit;

    outData["authors"]          = data.authors;
    outData["data"]             = dumpFromJson(data.custom_params, L);

    // Terrain tiles
    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const WorldTerrainTile & terra : data.tiles)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = terra.x;
            e["y"]         = terra.y;
            e["id"]        = terra.id;
            e["gfx_dx"]    = terra.gfx_dx;
            e["gfx_dy"]    = terra.gfx_dy;
            e["layer"]     = terra.layer;
            e["meta"] = getMeta(terra.meta, L);
            arr[++counter] = e;
        }
        outData["terrain"] = arr;
    }

    // Scenery
    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const WorldScenery & scene : data.scenery)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = scene.x;
            e["y"]         = scene.y;
            e["id"]        = scene.id;
            e["gfx_dx"]    = scene.gfx_dx;
            e["gfx_dy"]    = scene.gfx_dy;
            e["layer"]     = scene.layer;
            e["meta"] = getMeta(scene.meta, L);
            arr[++counter] = e;
        }
        outData["scenery"] = arr;
    }

    // Paths
    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const WorldPathTile & path : data.paths)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = path.x;
            e["y"]         = path.y;
            e["id"]        = path.id;
            e["gfx_dx"]    = path.gfx_dx;
            e["gfx_dy"]    = path.gfx_dy;
            e["layer"]     = path.layer;
            e["meta"]      = getMeta(path.meta, L);
            arr[++counter] = e;
        }
        outData["paths"] = arr;
    }

    // Levels
    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const WorldLevelTile & path : data.levels)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = path.x;
            e["y"]         = path.y;
            e["id"]        = path.id;
            e["gfx_dx"]    = path.gfx_dx;
            e["gfx_dy"]    = path.gfx_dy;

            e["levelFile"] = path.lvlfile;
            e["title"]     = path.title;
            e["exitTop"]   = path.top_exit;
            e["exitLeft"]  = path.left_exit;
            e["exitRight"] = path.right_exit;
            e["exitBottom"] = path.bottom_exit;

            e["isAlwaysVisible"]  = path.alwaysVisible;
            e["pathBg"]         = path.pathbg;
            e["pathBigBg"]      = path.bigpathbg;
            e["isGameStartPoint"] = path.gamestart;
            e["isForceStart"] = path.forceStart;
            e["disableStarCoinsCount"] = path.disableStarCoinsCount;
            e["destroyOnCompleting"] = path.destroyOnCompleting;

            e["goToX"] = path.gotox;
            e["goToY"] = path.gotoy;

            e["layer"]     = path.layer;
            e["meta"]      = getMeta(path.meta, L);
            arr[++counter] = e;
        }
        outData["levels"] = arr;
    }

    // Music
    {
        luabind::object arr = luabind::newtable(L);
        size_t counter = 0;
        for(const WorldMusicBox & musicBox : data.music)
        {
            luabind::object e = luabind::newtable(L);
            e["x"]         = musicBox.x;
            e["y"]         = musicBox.y;
            e["id"]        = musicBox.id;
            e["musicFile"] = musicBox.music_file;
            e["layer"]     = musicBox.layer;
            e["meta"]      = getMeta(musicBox.meta, L);
            arr[++counter] = e;
        }
        outData["musicBoxes"] = arr;
    }

    return outData;
}

luabind::object LuaProxy::Formats::openWorld(const std::string& filePath, lua_State* L)
{
    WorldData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenWorldFile(full_path, data);
    return openWorldInternal(data, L);
}

WorldData& getCurrentWorldData();
luabind::object LuaProxy::Formats::getWorldData(lua_State* L)
{
    return openWorldInternal(getCurrentWorldData(), L);
}

luabind::object LuaProxy::Formats::openNpcConfig(const std::string &filePath, lua_State *L)
{
    NPCConfigFile data;
    std::string full_path = getFullPath(filePath);
    FileFormats::ReadNpcTXTFileF(full_path, data);

    luabind::object outData = luabind::newtable(L);

    outData["isValid"] = data.ReadFileValid;
    outData["errorString"] = data.errorString;
    outData["unknownLines"] = data.unknownLines;

    /* SMBX64 */

    if(data.en_gfxoffsetx)
        outData["gfxoffsetx"] = data.gfxoffsetx;
    if(data.en_gfxoffsety)
        outData["gfxoffsety"] = data.gfxoffsety;
    if(data.en_width)
        outData["width"] = data.width;
    if(data.en_height)
        outData["height"] = data.height;
    if(data.en_gfxwidth)
        outData["gfxwidth"] = data.gfxwidth;
    if(data.en_gfxheight)
        outData["gfxheight"] = data.gfxheight;
    if(data.en_score)
        outData["score"] = data.score;
    if(data.en_playerblock)
        outData["playerblock"] = data.playerblock;
    if(data.en_playerblocktop)
        outData["playerblocktop"] = data.playerblocktop;
    if(data.en_npcblock)
        outData["npcblock"] = data.npcblock;
    if(data.en_npcblocktop)
        outData["npcblocktop"] = data.npcblocktop;
    if(data.en_grabside)
        outData["grabside"] = data.grabside;
    if(data.en_grabtop)
        outData["grabtop"] = data.grabtop;
    if(data.en_jumphurt)
        outData["jumphurt"] = data.jumphurt;
    if(data.en_nohurt)
        outData["nohurt"] = data.nohurt;
    if(data.en_noblockcollision)
        outData["noblockcollision"] = data.noblockcollision;
    if(data.en_cliffturn)
        outData["cliffturn"] = data.cliffturn;
    if(data.en_noyoshi)
        outData["noyoshi"] = data.noyoshi;
    if(data.en_foreground)
        outData["foreground"] = data.foreground;
    if(data.en_speed)
        outData["speed"] = data.speed;
    if(data.en_nofireball)
        outData["nofireball"] = data.nofireball;
    if(data.en_nogravity)
        outData["nogravity"] = data.nogravity;
    if(data.en_frames)
        outData["frames"] = data.frames;
    if(data.en_framespeed)
        outData["framespeed"] = data.framespeed;
    if(data.en_framestyle)
        outData["framestyle"] = data.framestyle;
    if(data.en_noiceball)
        outData["noiceball"] = data.noiceball;

    /* Extended */

    if(data.en_nohammer)
        outData["nohammer"] = data.nohammer;
    if(data.en_noshell)
        outData["noshell"] = data.noshell;
    if(data.en_name)
        outData["name"] = data.name;
    if(data.en_description)
        outData["description"] = data.description;
    if(data.en_health)
        outData["health"] = data.health;
    if(data.en_image)
        outData["image"] = data.image;
    if(data.en_icon)
        outData["icon"] = data.icon;
    if(data.en_script)
        outData["script"] = data.script;
    if(data.en_group)
        outData["group"] = data.group;
    if(data.en_category)
        outData["category"] = data.category;
    if(data.en_grid)
        outData["grid"] = data.grid;
    if(data.en_gridoffsetx)
        outData["gridoffsetx"] = data.gridoffsetx;
    if(data.en_gridoffsety)
        outData["gridoffsety"] = data.gridoffsety;
    if(data.en_gridalign)
        outData["gridalign"] = data.gridalign;

    return outData;
}
