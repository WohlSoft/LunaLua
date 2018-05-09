/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "file_formats.h"
#include "lvl_filedata.h"

#include <stack>

/*********************************************************************************/
/***************************SMBX64-Specific features******************************/
/*********************************************************************************/

//Built-in order priorities per SMBX-64 BGO's
const int _smbx64_bgo_sort_priorities[190] =
{
    77, 75, 75, 75, 75, 75, 75, 75, 75, 75, 20, 20, 75, 10, 75, 75, 75, 75, 75, 75, 75, 75, 125, 125, 125, 26,
    75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 125, 125, 75, 80, 125, 125, 125, 30,
    75, 75, 75, 75, 75, 75, 75, 20, 20, 75, 75, 75, 26, 25, 75, 125, 125, 90, 90, 90, 90, 90, 10, 10, 10, 10, 30,
    75, 75, 26, 26, 75, 75, 75, 98, 98, 75, 75, 75, 98, 75, 75, 75, 75, 75, 75, 99, 75, 75, 75, 75, 98, 98, 125,
    98, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 76, 76, 76, 75, 75,
    75, 75, 75, 125, 125, 80, 80, 90, 75, 125, 75, 125, 75, 75, 75, 75, 75, 75, 75, 75, 125, 125, 125, 125, 25,
    25, 75, 75, 75, 75, 26, 26, 26, 26, 26, 26, 75, 75, 25, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,
    75, 125, 125, 75, 75
};

void FileFormats::smbx64LevelPrepare(LevelData &lvl)
{
    //Set SMBX64 specific option to BGO
    for(pge_size_t q = 0; q < lvl.bgo.size(); q++)
    {
        if(lvl.bgo[q].smbx64_sp < 0)
        {
            if((lvl.bgo[q].id > 0) && (lvl.bgo[q].id <= (unsigned)190))
                lvl.bgo[q].smbx64_sp_apply = _smbx64_bgo_sort_priorities[lvl.bgo[q].id - 1];
        }
        else
            lvl.bgo[q].smbx64_sp_apply = lvl.bgo[q].smbx64_sp;
    }

    //Mark & Count Stars
    lvl.stars = smbx64CountStars(lvl);
}

int FileFormats::smbx64CountStars(LevelData &lvl)
{
    int stars = 0;
    for(pge_size_t q = 0; q < lvl.npc.size(); q++)
    {
        LevelNPC &npc = lvl.npc[q];
        npc.is_star = ((npc.id == 97) || (npc.id == 196)) && !npc.friendly;
        if(npc.is_star)
            stars += 1;
    }
    return stars;
}

void FileFormats::smbx64LevelSortBlocks(LevelData &lvl)
{
    if(lvl.blocks.size() <= 1) return; //Nothing to sort!

    class my_stack : public std::stack< int >
    {
    public:
        using std::stack<int>::c; // expose the container
    };

    my_stack beg;
    my_stack end;
    LevelBlock piv;
    int i = 0, L, R, swapv;
    beg.push(0);
    end.push((int)lvl.blocks.size());
#define ST(x) static_cast<pge_size_t>(x)
    while(i >= 0)
    {
        L = beg.c[ST(i)];
        R = end.c[ST(i)] - 1;
        if(L < R)
        {
            piv = lvl.blocks[ST(L)];
            while(L < R)
            {
                while((
                          (lvl.blocks[ST(R)].x > piv.x) ||
                          ((lvl.blocks[ST(R)].x == piv.x) && (lvl.blocks[ST(R)].y > piv.y)) ||
                          ((lvl.blocks[ST(R)].x == piv.x) && (lvl.blocks[ST(R)].y == piv.y) && (lvl.blocks[ST(R)].meta.array_id >= piv.meta.array_id))
                      ) && (L < R)) R--;
                if(L < R) lvl.blocks[ST(L++)] = lvl.blocks[ST(R)];

                while(
                    (
                        (lvl.blocks[ST(L)].x < piv.x) ||
                        ((lvl.blocks[ST(L)].x == piv.x) && (lvl.blocks[ST(L)].y < piv.y)) ||
                        ((lvl.blocks[ST(L)].x == piv.x) && (lvl.blocks[ST(L)].y == piv.y) && (lvl.blocks[ST(L)].meta.array_id <= piv.meta.array_id))
                    ) && (L < R)) L++;
                if(L < R) lvl.blocks[ST(R--)] = lvl.blocks[ST(L)];
            }
            lvl.blocks[ST(L)] = piv;
            beg.push(L + 1);
            end.push(end.c[ST(i)]);
            end.c[ST(i++)] = (L);
            if((end.c[ST(i)] - beg.c[ST(i)]) > (end.c[ST(i - 1)] - beg.c[ST(i - 1)]))
            {
                swapv = beg.c[ST(i)];
                beg.c[ST(i)] = beg.c[ST(i - 1)];
                beg.c[ST(i - 1)] = swapv;
                swapv = end.c[ST(i)];
                end.c[ST(i)] = end.c[ST(i - 1)];
                end.c[ST(i - 1)] = swapv;
            }
        }
        else
        {
            i--;
            beg.pop();
            end.pop();
        }
    }
#undef ST
}

void FileFormats::smbx64LevelSortBGOs(LevelData &lvl)
{
    if(lvl.bgo.size() <= 1) return; //Nothing to sort!

    class my_stack : public std::stack< int >
    {
    public:
        using std::stack<int>::c; // expose the container
    };

    my_stack beg;
    my_stack end;
    LevelBGO piv;
    int i = 0, L, R, swapv;
    beg.push(0);
    end.push((int)lvl.bgo.size());
#define ST(x) static_cast<pge_size_t>(x)
    while(i >= 0)
    {
        L = beg.c[ST(i)];
        R = end.c[ST(i)] - 1;
        if(L < R)
        {
            piv = lvl.bgo[ST(L)];
            while(L < R)
            {
                while((
                          (lvl.bgo[ST(R)].smbx64_sp_apply > piv.smbx64_sp_apply) ||
                          /*((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].x > piv.x))||
                            ((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].x == piv.x) && (lvl.bgo[R].y > piv.y))||
                            ((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].x == piv.x) && (lvl.bgo[R].y == piv.y) && (lvl.bgo[R].array_id >= piv.array_id))*/
                          ((lvl.bgo[ST(R)].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[ST(R)].meta.array_id >= piv.meta.array_id))
                      ) && (L < R)) R--;
                if(L < R) lvl.bgo[ST(L++)] = lvl.bgo[ST(R)];

                while(
                    (
                        (lvl.bgo[ST(L)].smbx64_sp_apply < piv.smbx64_sp_apply) ||
                        /*((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].x < piv.x))||
                          ((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].x == piv.x) && (lvl.bgo[L].y < piv.y))||
                          ((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].x == piv.x) && (lvl.bgo[L].y == piv.y) && (lvl.bgo[L].array_id <= piv.array_id))*/
                        ((lvl.bgo[ST(L)].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[ST(L)].meta.array_id <= piv.meta.array_id))
                    ) && (L < R)) L++;
                if(L < R) lvl.bgo[ST(R--)] = lvl.bgo[ST(L)];
            }
            lvl.bgo[ST(L)] = piv;
            beg.push(L + 1);
            end.push(end.c[ST(i)]);
            end.c[ST(i++)] = (L);
            if((end.c[ST(i)] - beg.c[ST(i)]) > (end.c[ST(i - 1)] - beg.c[ST(i - 1)]))
            {
                swapv = beg.c[ST(i)];
                beg.c[ST(i)] = beg.c[ST(i - 1)];
                beg.c[ST(i - 1)] = swapv;
                swapv = end.c[ST(i)];
                end.c[ST(i)] = end.c[ST(i - 1)];
                end.c[ST(i - 1)] = swapv;
            }
        }
        else
        {
            i--;
            beg.pop();
            end.pop();
        }
    }
#undef ST
}

int FileFormats::smbx64LevelCheckLimits(LevelData &lvl)
{
    int errorCode = SMBX64_FINE;
    //Sections limit
    if(lvl.sections.size() > 21) errorCode |= SMBX64EXC_SECTIONS;
    //Blocks limit
    if(lvl.blocks.size() > 16384) errorCode |= SMBX64EXC_BLOCKS;
    //BGO limits
    if(lvl.bgo.size() > 8000) errorCode |= SMBX64EXC_BGOS;
    //NPC limits
    if(lvl.npc.size() > 5000) errorCode |= SMBX64EXC_NPCS;
    //Warps limits
    if(lvl.doors.size() > 199) errorCode |= SMBX64EXC_WARPS;
    //Physical Environment zones
    if(lvl.physez.size() > 450) errorCode |= SMBX64EXC_WATERBOXES;
    //Layers limits
    if(lvl.layers.size() > 100) errorCode |= SMBX64EXC_LAYERS;
    //Events limits
    if(lvl.events.size() > 100) errorCode |= SMBX64EXC_EVENTS;

    return errorCode;
}
/*********************************************************************************/



//********************************************************************
//*******************Structure initializators*************************
//********************************************************************

//Default dataSets
LevelNPC    FileFormats::CreateLvlNpc()
{
    return LevelNPC();
}

LevelDoor  FileFormats::CreateLvlWarp()
{
    return LevelDoor();
}

LevelBlock  FileFormats::CreateLvlBlock()
{
    return LevelBlock();
}

LevelBGO FileFormats::CreateLvlBgo()
{
    return LevelBGO();
}

LevelPhysEnv FileFormats::CreateLvlPhysEnv()
{
    return LevelPhysEnv();
}

LevelSMBX64Event FileFormats::CreateLvlEvent()
{
    LevelSMBX64Event dummyEvent;

    dummyEvent.name = "";
    dummyEvent.msg = "";
    dummyEvent.sound_id = 0;
    dummyEvent.end_game = 0;
    dummyEvent.trigger = "";
    dummyEvent.trigger_timer = 0;
    dummyEvent.nosmoke = false;
    dummyEvent.ctrls_enable = false;
    dummyEvent.ctrl_altjump = false;
    dummyEvent.ctrl_altrun = false;
    dummyEvent.ctrl_down = false;
    dummyEvent.ctrl_drop = false;
    dummyEvent.ctrl_jump = false;
    dummyEvent.ctrl_left = false;
    dummyEvent.ctrl_right = false;
    dummyEvent.ctrl_run = false;
    dummyEvent.ctrl_start = false;
    dummyEvent.ctrl_up = false;
    dummyEvent.ctrl_lock_keyboard = false;
    dummyEvent.autostart = LevelSMBX64Event::AUTO_None;
    dummyEvent.autostart_condition = "";
    dummyEvent.movelayer = "";
    dummyEvent.layer_speed_x = 0.0;
    dummyEvent.layer_speed_y = 0.0;
    dummyEvent.move_camera_x = 0.0;
    dummyEvent.move_camera_y = 0.0;
    dummyEvent.scroll_section = 0;
    dummyEvent.trigger_api_id = 0;
    dummyEvent.layers_hide.clear();
    dummyEvent.layers_show.clear();
    dummyEvent.layers_toggle.clear();

    LevelEvent_Sets events_sets;
    dummyEvent.sets.clear();
    for(int j = 0; j < 21; j++)
    {
        events_sets.id = j;
        events_sets.music_id = -1;
        events_sets.background_id = -1;
        events_sets.position_left = -1;
        events_sets.position_top = 0;
        events_sets.position_bottom = 0;
        events_sets.position_right = 0;
        dummyEvent.sets.push_back(events_sets);
    }

    return dummyEvent;
}

LevelVariable FileFormats::CreateLvlVariable(PGESTRING vname)
{
    LevelVariable var;
    var.name = vname;
    var.value = "";
    return var;
}

LevelScript FileFormats::CreateLvlScript(PGESTRING name, int lang)
{
    LevelScript scr;
    scr.language = lang;
    scr.name = name;
    scr.script = "";
    return scr;
}

LevelSection FileFormats::CreateLvlSection()
{
    return LevelSection();
}

LevelLayer FileFormats::CreateLvlLayer()
{
    return LevelLayer();
}

PlayerPoint FileFormats::CreateLvlPlayerPoint(unsigned int id)
{
    PlayerPoint dummyPlayer;
    dummyPlayer.id = id;
    switch(id)
    {
    case 1:
        dummyPlayer.h = 54;
        break;
    case 2:
        dummyPlayer.h = 60;
        break;
    default:
        dummyPlayer.h = 32; //-V112
    }

    return dummyPlayer;
}

void FileFormats::LevelAddInternalEvents(LevelData &FileData)
{
    LevelLayer layers;
    LevelSMBX64Event events;

    layers = CreateLvlLayer();

    //Append system layers if not exist
    bool def = false, desb = false, spawned = false;

    for(pge_size_t lrID = 0; lrID < FileData.layers.size(); lrID++)
    {
        LevelLayer &lr = FileData.layers[lrID];
        if(lr.name == "Default") def = true;
        else if(lr.name == "Destroyed Blocks") desb = true;
        else if(lr.name == "Spawned NPCs") spawned = true;
    }

    if(!def)
    {
        layers.hidden = false;
        layers.name = "Default";
        FileData.layers.push_back(layers);
    }
    if(!desb)
    {
        layers.hidden = true;
        layers.name = "Destroyed Blocks";
        FileData.layers.push_back(layers);
    }
    if(!spawned)
    {
        layers.hidden = false;
        layers.name = "Spawned NPCs";
        FileData.layers.push_back(layers);
    }

    //Append system events if not exist
    //Level - Start
    //P Switch - Start
    //P Switch - End
    bool lstart = false, pstart = false, pend = false;
    for(pge_size_t evID = 0; evID < FileData.events.size(); evID++)
    {
        LevelSMBX64Event &ev = FileData.events[evID];
        if(ev.name == "Level - Start") lstart = true;
        else if(ev.name == "P Switch - Start") pstart = true;
        else if(ev.name == "P Switch - End") pend = true;
    }

    events = CreateLvlEvent();

    if(!lstart)
    {
        events.meta.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "Level - Start";
        FileData.events.push_back(events);
    }
    if(!pstart)
    {
        events.meta.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "P Switch - Start";
        FileData.events.push_back(events);
    }
    if(!pend)
    {
        events.meta.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "P Switch - End";
        FileData.events.push_back(events);
    }
}


void FileFormats::CreateLevelHeader(LevelData &NewFileData)
{
    NewFileData.CurSection = 0;
    NewFileData.playmusic = 0;
    NewFileData.meta = FileFormatMeta();

    NewFileData.open_level_on_fail = "";
    NewFileData.open_level_on_fail_warpID = 0;

    NewFileData.LevelName = "";
    NewFileData.stars = 0;
}

void FileFormats::CreateLevelData(LevelData &NewFileData)
{
    CreateLevelHeader(NewFileData);

    NewFileData.bgo_array_id = 1;
    NewFileData.blocks_array_id = 1;
    NewFileData.doors_array_id = 1;
    NewFileData.events_array_id = 1;
    NewFileData.layers_array_id = 1;
    NewFileData.npc_array_id = 1;
    NewFileData.physenv_array_id = 1;

    NewFileData.sections.clear();
    //Create Section array
    LevelSection section;
    for(int i = 0; i < 21; i++)
    {
        section = CreateLvlSection();
        section.id = i;
        NewFileData.sections.push_back(section);
    }

    NewFileData.players.clear();
    //Create players array
    //PlayerPoint players = dummyLvlPlayerPoint();
    //    for(int i=0; i<2;i++)
    //    {
    //        players.id++;
    //        NewFileData.players.push_back(players);
    //    }

    NewFileData.blocks.clear();
    NewFileData.bgo.clear();
    NewFileData.npc.clear();
    NewFileData.doors.clear();
    NewFileData.physez.clear();

    NewFileData.layers.clear();
    NewFileData.variables.clear();
    NewFileData.scripts.clear();

    //Create system layers
    //Default
    //Destroyed Blocks
    //Spawned NPCs

    LevelLayer layers;
    layers.hidden = false;
    layers.locked = false;
    layers.name = "Default";
    layers.meta.array_id = NewFileData.layers_array_id++;
    NewFileData.layers.push_back(layers);

    layers.hidden = true;
    layers.locked = false;
    layers.name = "Destroyed Blocks";
    layers.meta.array_id = NewFileData.layers_array_id++;
    NewFileData.layers.push_back(layers);

    layers.hidden = false;
    layers.locked = false;
    layers.name = "Spawned NPCs";
    layers.meta.array_id = NewFileData.layers_array_id++;
    NewFileData.layers.push_back(layers);

    NewFileData.events.clear();
    //Create system events
    //Level - Start
    //P Switch - Start
    //P Switch - End

    LevelSMBX64Event events = CreateLvlEvent();

    events.meta.array_id = NewFileData.events_array_id;
    NewFileData.events_array_id++;

    events.name = "Level - Start";
    NewFileData.events.push_back(events);

    events.meta.array_id = NewFileData.events_array_id;
    NewFileData.events_array_id++;

    events.name = "P Switch - Start";
    NewFileData.events.push_back(events);

    events.meta.array_id = NewFileData.events_array_id;
    NewFileData.events_array_id++;

    events.name = "P Switch - End";
    NewFileData.events.push_back(events);
}

LevelData FileFormats::CreateLevelData()
{
    LevelData NewFileData;
    CreateLevelData(NewFileData);
    return NewFileData;
}



bool LevelData::eventIsExist(PGESTRING title)
{
    for(auto &e : events)
    {
        if(e.name == title)
            return true;
    }
    return false;
}

bool LevelData::layerIsExist(PGESTRING title)
{
    for(auto &l : layers)
    {
        if(l.name == title)
            return true;
    }
    return false;
}

bool LevelSMBX64Event::ctrlKeyPressed()
{
    return ctrl_up ||
           ctrl_down ||
           ctrl_left ||
           ctrl_right ||
           ctrl_jump ||
           ctrl_altjump ||
           ctrl_run ||
           ctrl_altrun ||
           ctrl_drop ||
           ctrl_start;
}
