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

#include "pge_file_lib_sys.h"
#include "file_formats.h"
#include "file_strlist.h"
#include "pge_x.h"
#include "pge_x_macro.h"
#include <cfloat>

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadExtendedLvlFileHeader(PGESTRING filePath, LevelData &FileData)
{
    CreateLevelHeader(FileData);
    FileData.meta.RecentFormat = LevelData::PGEX;
    PGE_FileFormats_misc::TextFileInput inf;

    if(!inf.open(filePath, true))
    {
        FileData.meta.ReadFileValid = false;
        return false;
    }

    PGESTRING line;
    int str_count = 0;
    bool valid = false;
    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
#define NextLine(line) str_count++;line = inf.readLine();

    //Find level header part
    do
    {
        str_count++;
        NextLine(line);
    }
    while((line != "HEAD") && (!IsNULL(line)));

    PGESTRINGList header;
    bool closed = false;

    if(line != "HEAD")//Header not found, this level is head-less
        goto skipHeaderParse;

    NextLine(line);
    while((line != "HEAD_END") && (!IsNULL(line)))
    {
        header.push_back(line);
        str_count++;
        NextLine(line);
        if(line == "HEAD_END")
            closed = true;
    }

    if(!closed)
        goto badfile;

    for(pge_size_t qq = 0; qq < header.size(); qq++)
    {
        PGESTRING &header_line = header[qq];
        PGELIST<PGESTRINGList >data = PGEFile::splitDataLine(header_line, &valid);

        for(pge_size_t i = 0; i < data.size(); i++)
        {
            if(data[i].size() != 2)
                goto badfile;

            if(data[i][0] == "TL") //Level Title
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.LevelName = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "SZ") //Starz number
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.stars = toInt(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "DL") //Open Level on player's fail
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.open_level_on_fail = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "DE") //Target WarpID of fail-level entrace
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.open_level_on_fail_warpID = toUInt(data[i][1]);
                else
                    goto badfile;
            }
        }
    }

skipHeaderParse:
    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:
    FileData.meta.ERROR_info = "Invalid file format";
    FileData.meta.ERROR_linenum = static_cast<long>(str_count);
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    return false;
}



bool FileFormats::ReadExtendedLvlFileF(PGESTRING  filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;

    if(!file.open(filePath, true))
    {
        errorString = "Failed to open file for read";
        FileData.meta.ERROR_info = errorString;
        FileData.meta.ERROR_linedata = "";
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedLvlFile(file, FileData);
}

bool FileFormats::ReadExtendedLvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;

    if(!file.open(&rawdata, filePath))
    {
        errorString = "Failed to open raw string for read";
        FileData.meta.ERROR_info = errorString;
        FileData.meta.ERROR_linedata = "";
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadExtendedLvlFile(file, FileData);
}

bool FileFormats::ReadExtendedLvlFile(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    PGESTRING errorString;
    PGESTRING filePath = in.getFilePath();
    PGESTRING line;  /*Current Line data*/
    //LevelData FileData;
    CreateLevelData(FileData);
    FileData.meta.RecentFormat = LevelData::PGEX;

    //Add path data
    if(filePath.size() > 0)
    {
        PGE_FileFormats_misc::FileInfo  in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    FileData.meta.untitled = false;
    FileData.meta.modified = false;
    LevelSection lvl_section;
    PlayerPoint player;
    LevelBlock block;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor door;
    LevelPhysEnv physiczone;
    LevelLayer layer;
    LevelSMBX64Event event;
    LevelVariable variable;
    LevelScript script;
    LevelItemSetup38A customcfg38A;
    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEX_FileParseTree(in.readAll());
    PGEX_FetchSection() //look sections
    {
        PGEX_FetchSection_begin()
        ///////////////////HEADER//////////////////////
        PGEX_Section("HEAD")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("TL", FileData.LevelName) //Level Title
                    PGEX_USIntVal("SZ", FileData.stars) //Starz number
                    PGEX_StrVal("DL", FileData.open_level_on_fail) //Open level on fail
                    PGEX_UIntVal("DE", FileData.open_level_on_fail_warpID) //Open level's warpID on fail
                }
            }
        }//HEADER
        ///////////////////////////////MetaDATA/////////////////////////////////////////////
        PGEX_Section("META_BOOKMARKS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName.clear();
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("BM", meta_bookmark.bookmarkName) //Bookmark name
                    PGEX_FloatVal("X", meta_bookmark.x) // Position X
                    PGEX_FloatVal("Y", meta_bookmark.y) // Position Y
                }
                FileData.metaData.bookmarks.push_back(meta_bookmark);
            }
        }
        ////////////////////////meta bookmarks////////////////////////
#ifdef PGE_EDITOR
        PGEX_Section("META_SYS_CRASH")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                FileData.metaData.crash.used = true;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_BoolVal("UT", FileData.metaData.crash.untitled) //Untitled
                    PGEX_BoolVal("MD", FileData.metaData.crash.modifyed) //Modyfied
                    PGEX_SIntVal("FF", FileData.metaData.crash.fmtID) //Recent File format
                    PGEX_UIntVal("FV", FileData.metaData.crash.fmtVer) //Recent File format version
                    PGEX_StrVal("N",  FileData.metaData.crash.filename)  //Filename
                    PGEX_StrVal("P",  FileData.metaData.crash.path)  //Path
                    PGEX_StrVal("FP", FileData.metaData.crash.fullPath)  //Full file Path
                }
            }
        }//meta sys crash
#endif
        ///////////////////////////////MetaDATA//End////////////////////////////////////////
        ///////////////////SECTION//////////////////////
        PGEX_Section("SECTION")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                lvl_section = CreateLvlSection();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_USIntVal("SC", lvl_section.id) //Section ID
                    PGEX_SLongVal("L",  lvl_section.size_left) //Left side
                    PGEX_SLongVal("R",  lvl_section.size_right)//Right side
                    PGEX_SLongVal("T",  lvl_section.size_top) //Top side
                    PGEX_SLongVal("B",  lvl_section.size_bottom)//Bottom side
                    PGEX_UIntVal("MZ", lvl_section.music_id)//Stuff music ID
                    PGEX_UIntVal("BG", lvl_section.background)//Stuff music ID
                    PGEX_StrVal("MF", lvl_section.music_file) //External music file path
                    PGEX_BoolVal("CS", lvl_section.wrap_h)//Connect sides horizontally
                    PGEX_BoolVal("CSV", lvl_section.wrap_v)//Connect sides vertically
                    PGEX_BoolVal("OE", lvl_section.OffScreenEn)//Offscreen exit
                    PGEX_BoolVal("SR", lvl_section.lock_left_scroll)//Right-way scroll only (No Turn-back)
                    PGEX_BoolVal("SL", lvl_section.lock_right_scroll)//Left-way scroll only (No Turn-forward)
                    PGEX_BoolVal("SD", lvl_section.lock_up_scroll)//Down-way scroll only (No Turn-forward)
                    PGEX_BoolVal("SU", lvl_section.lock_down_scroll)//Up-way scroll only (No Turn-forward)
                    PGEX_BoolVal("UW", lvl_section.underwater)//Underwater bit
                }
                lvl_section.PositionX = lvl_section.size_left - 10;
                lvl_section.PositionY = lvl_section.size_top - 10;
                //add captured value into array
                bool found = false;
                pge_size_t q = 0;
                pge_size_t sections_count = FileData.sections.size();

                if(lvl_section.id >= (int)sections_count)
                {
                    pge_size_t needToAdd = (FileData.sections.size() - 1) - static_cast<pge_size_t>(lvl_section.id);
                    while(needToAdd > 0)
                    {
                        LevelSection dummySct = CreateLvlSection();
                        dummySct.id = (int)FileData.sections.size();
                        FileData.sections.push_back(dummySct);
                        needToAdd--;
                    }
                }

                for(q = 0; q < sections_count; q++)
                {
                    if(FileData.sections[q].id == lvl_section.id)
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                    FileData.sections[q] = lvl_section;
                else
                    FileData.sections.push_back(lvl_section);
            }
        }//SECTION
        ///////////////////STARTPOINT//////////////////////
        PGEX_Section("STARTPOINT")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                player = CreateLvlPlayerPoint();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", player.id) //ID of player point
                    PGEX_SLongVal("X", player.x)
                    PGEX_SLongVal("Y", player.y)
                    PGEX_SIntVal("D",  player.direction)
                }
                //add captured value into array
                bool found = false;
                pge_size_t q = 0;
                pge_size_t playersCount = FileData.players.size();
                for(q = 0; q < playersCount; q++)
                {
                    if(FileData.players[q].id == player.id)
                    {
                        found = true;
                        break;
                    }
                }

                PlayerPoint sz = CreateLvlPlayerPoint(player.id);
                player.w = sz.w;
                player.h = sz.h;

                if(found)
                    FileData.players[q] = player;
                else
                    FileData.players.push_back(player);
            }
        }//STARTPOINT
        ///////////////////BLOCK//////////////////////
        PGEX_Section("BLOCK")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                block = CreateLvlBlock();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_ULongVal("ID", block.id) //Block ID
                    PGEX_SLongVal("X", block.x) // Position X
                    PGEX_SLongVal("Y", block.y) //Position Y
                    PGEX_USLongVal("W", block.w) //Width
                    PGEX_USLongVal("H", block.h) //Height
                    PGEX_BoolVal("AS", block.autoscale)//Enable auto-Scaling
                    PGEX_StrVal("GXN", block.gfx_name) //38A GFX-Name
                    PGEX_SLongVal("GXX", block.gfx_dx) //38A graphics extend x
                    PGEX_SLongVal("GXY", block.gfx_dy) //38A graphics extend y
                    PGEX_SLongVal("CN", block.npc_id) //Contains (coins/NPC)
                    PGEX_BoolVal("IV", block.invisible) //Invisible
                    PGEX_BoolVal("SL", block.slippery) //Slippery
                    PGEX_StrVal("LR", block.layer) //Layer name
                    PGEX_StrVal("ED", block.event_destroy) //Destroy event slot
                    PGEX_StrVal("EH", block.event_hit) //Hit event slot
                    PGEX_StrVal("EE", block.event_emptylayer) //Hit event slot
                    PGEX_StrVal("XTRA", block.meta.custom_params)//Custom JSON data tree
                }
                block.meta.array_id = FileData.blocks_array_id++;
                block.meta.index = static_cast<unsigned int>(FileData.blocks.size());
                FileData.blocks.push_back(block);
            }
        }//BLOCK
        ///////////////////BGO//////////////////////
        PGEX_Section("BGO")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                bgodata = CreateLvlBgo();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_ULongVal("ID", bgodata.id)  //BGO ID
                    PGEX_SLongVal("X",  bgodata.x)  //X Position
                    PGEX_SLongVal("Y",  bgodata.y)  //Y Position
                    PGEX_SLongVal("GXX", bgodata.gfx_dx) //38A graphics extend x
                    PGEX_SLongVal("GXY", bgodata.gfx_dy) //38A graphics extend y
                    PGEX_FloatVal("ZO", bgodata.z_offset) //Z Offset
                    PGEX_SIntVal("ZP", bgodata.z_mode)  //Z Position
                    PGEX_SLongVal("SP", bgodata.smbx64_sp)  //SMBX64 Sorting priority
                    PGEX_StrVal("LR", bgodata.layer)   //Layer name
                    PGEX_StrVal("XTRA", bgodata.meta.custom_params)//Custom JSON data tree
                }
                bgodata.meta.array_id = FileData.bgo_array_id++;
                bgodata.meta.index = static_cast<unsigned int>(FileData.bgo.size());
                FileData.bgo.push_back(bgodata);
            }
        }//BGO
        ///////////////////NPC//////////////////////
        PGEX_Section("NPC")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                npcdata = CreateLvlNpc();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_ULongVal("ID", npcdata.id) //NPC ID
                    PGEX_SLongVal("X", npcdata.x) //X position
                    PGEX_SLongVal("Y", npcdata.y) //Y position
                    PGEX_StrVal("GXN", npcdata.gfx_name) //38A GFX-Name
                    PGEX_SLongVal("GXX", npcdata.gfx_dx) //38A graphics extend x
                    PGEX_SLongVal("GXY", npcdata.gfx_dy) //38A graphics extend y
                    PGEX_SIntVal("D", npcdata.direct) //Direction
                    PGEX_SLongVal("CN", npcdata.contents) //Contents of container-NPC
                    PGEX_SLongVal("S1", npcdata.special_data) //Special value 1
                    PGEX_SLongVal("S2", npcdata.special_data2) //Special value 2
                    PGEX_BoolVal("GE", npcdata.generator) //Generator
                    PGEX_SIntVal("GT", npcdata.generator_type) //Generator type
                    PGEX_SIntVal("GD", npcdata.generator_direct) //Generator direction
                    PGEX_USIntVal("GM", npcdata.generator_period) //Generator period
                    PGEX_FloatVal("GA", npcdata.generator_custom_angle) //Generator custom angle
                    PGEX_USIntVal("GB",  npcdata.generator_branches) //Generator number of branches
                    PGEX_FloatVal("GR", npcdata.generator_angle_range) //Generator angle range
                    PGEX_FloatVal("GS", npcdata.generator_initial_speed) //Generator custom initial speed
                    PGEX_StrVal("MG", npcdata.msg) //Message
                    PGEX_BoolVal("FD", npcdata.friendly) //Friendly
                    PGEX_BoolVal("NM", npcdata.nomove) //Don't move
                    PGEX_BoolVal("BS", npcdata.is_boss) //Enable boss mode!
                    PGEX_StrVal("LR", npcdata.layer) //Layer
                    PGEX_StrVal("LA", npcdata.attach_layer) //Attach Layer
                    PGEX_StrVal("SV", npcdata.send_id_to_variable) //Send ID to variable
                    PGEX_StrVal("EA", npcdata.event_activate) //Event slot "Activated"
                    PGEX_StrVal("ED", npcdata.event_die) //Event slot "Death/Take/Destroy"
                    PGEX_StrVal("ET", npcdata.event_talk) //Event slot "Talk"
                    PGEX_StrVal("EE", npcdata.event_emptylayer) //Event slot "Layer is empty"
                    PGEX_StrVal("EG", npcdata.event_grab)//Event slot "On grab"
                    PGEX_StrVal("EO", npcdata.event_touch)//Event slot "On touch"
                    PGEX_StrVal("EF", npcdata.event_nextframe)//Evemt slot "Trigger every frame"
                    PGEX_StrVal("XTRA", npcdata.meta.custom_params)//Custom JSON data tree
                }
                npcdata.meta.array_id = FileData.npc_array_id++;
                npcdata.meta.index = static_cast<unsigned int>(FileData.npc.size());
                FileData.npc.push_back(npcdata);
            }
        }//TILES
        ///////////////////PHYSICS//////////////////////
        PGEX_Section("PHYSICS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                physiczone = CreateLvlPhysEnv();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_USIntVal("ET", physiczone.env_type) //Environment type
                    PGEX_SLongVal("X",  physiczone.x) //X position
                    PGEX_SLongVal("Y",  physiczone.y) //Y position
                    PGEX_USLongVal("W",  physiczone.w) //Width
                    PGEX_USLongVal("H",  physiczone.h) //Height
                    PGEX_StrVal("LR", physiczone.layer)  //Layer
                    PGEX_FloatVal("FR", physiczone.friction) //Friction
                    PGEX_FloatVal("AD", physiczone.accel_direct) //Custom acceleration direction
                    PGEX_FloatVal("AC", physiczone.accel) //Custom acceleration
                    PGEX_FloatVal("MV", physiczone.max_velocity) //Maximal velocity
                    PGEX_StrVal("EO",  physiczone.touch_event) //Touch event/script
                    PGEX_StrVal("XTRA", physiczone.meta.custom_params)//Custom JSON data tree
                }
                physiczone.meta.array_id = FileData.physenv_array_id++;
                physiczone.meta.index = static_cast<unsigned int>(FileData.physez.size());
                FileData.physez.push_back(physiczone);
            }
        }//PHYSICS
        ///////////////////DOORS//////////////////////
        PGEX_Section("DOORS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                door = CreateLvlWarp();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_SLongVal("IX", door.ix) //Input point
                    PGEX_SLongVal("IY", door.iy) //Input point
                    PGEX_SLongVal("OX", door.ox) //Output point
                    PGEX_SLongVal("OY", door.oy) //Output point
                    PGEX_UIntVal("IL", door.length_i) //Length of entrance (input) point
                    PGEX_UIntVal("OL", door.length_o) //Length of exit (output) point
                    PGEX_USIntVal("DT", door.type) //Input point
                    PGEX_USIntVal("ID", door.idirect) //Input direction
                    PGEX_USIntVal("OD", door.odirect) //Output direction
                    PGEX_SLongVal("WX", door.world_x) //Target world map point
                    PGEX_SLongVal("WY", door.world_y) //Target world map point
                    PGEX_StrVal("LF", door.lname)  //Target level file
                    PGEX_USLongVal("LI", door.warpto) //Target level file's input warp
                    PGEX_BoolVal("ET", door.lvl_i) //Level Entrance
                    PGEX_BoolVal("EX", door.lvl_o) //Level exit
                    PGEX_USIntVal("SL", door.stars) //Stars limit
                    PGEX_StrVal("SM", door.stars_msg)  //Message about stars/leeks
                    PGEX_BoolVal("NV", door.novehicles) //No Vehicles
                    PGEX_BoolVal("SH", door.star_num_hide) //Don't show stars number
                    PGEX_BoolVal("AI", door.allownpc) //Allow grabbed items
                    PGEX_BoolVal("LC", door.locked) //Door is locked
                    PGEX_BoolVal("LB", door.need_a_bomb) //Door is blocked, need bomb to unlock
                    PGEX_BoolVal("HS", door.hide_entering_scene) //Don't show entering scene
                    PGEX_BoolVal("AL", door.allownpc_interlevel) //Allow NPC's inter-level
                    PGEX_BoolVal("SR", door.special_state_required) //Required a special state to enter
                    PGEX_BoolVal("PT", door.cannon_exit) //Cannon exit
                    PGEX_FloatVal("PS", door.cannon_exit_speed) //Cannon exit speed
                    PGEX_StrVal("LR", door.layer)  //Layer
                    PGEX_StrVal("EE", door.event_enter)  //On-Enter event slot
                    PGEX_BoolVal("TW", door.two_way) //Two-way warp
                    PGEX_StrVal("XTRA", door.meta.custom_params)//Custom JSON data tree
                }
                door.isSetIn = (!door.lvl_i);
                door.isSetOut = (!door.lvl_o || (door.lvl_i));

                if(!door.isSetIn && door.isSetOut)
                {
                    door.ix = door.ox;
                    door.iy = door.oy;
                }

                if(!door.isSetOut && door.isSetIn)
                {
                    door.ox = door.ix;
                    door.oy = door.iy;
                }

                door.meta.array_id = FileData.doors_array_id++;
                door.meta.index = static_cast<unsigned int>(FileData.doors.size());
                FileData.doors.push_back(door);
            }
        }//DOORS
        ///////////////////LAYERS//////////////////////
        PGEX_Section("LAYERS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                layer = CreateLvlLayer();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("LR", layer.name)  //Layer name
                    PGEX_BoolVal("HD", layer.hidden) //Hidden
                    PGEX_BoolVal("LC", layer.locked) //Locked
                }
                //add captured value into array
                bool found = false;
                pge_size_t q = 0;
                for(q = 0; q < FileData.layers.size(); q++)
                {
                    if(FileData.layers[q].name == layer.name)
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    layer.meta.array_id = FileData.layers[q].meta.array_id;
                    FileData.layers[q] = layer;
                }
                else
                {
                    layer.meta.array_id = FileData.layers_array_id++;
                    FileData.layers.push_back(layer);
                }
            }
        }//LAYERS
        //EVENTS comming soon
        //                else
        //                if(sct.first=="EVENTS_CLASSIC") //Action-styled events
        //                {
        //                    foreach(PGESTRINGList value, sectData) //Look markers and values
        //                    {
        //                            //  if(v.marker=="TL") //Level Title
        //                            //  {
        //                            //      if(PGEFile::IsQStr(v.value))
        //                            //          FileData.LevelName = PGEFile::X2STR(v.value);
        //                            //      else
        //                            //          goto badfile;
        //                            //  }
        //                            //  else
        //                            //  if(v.marker=="SZ") //Starz number
        //                            //  {
        //                            //      if(PGEFile::IsIntU(v.value))
        //                            //          FileData.stars = toInt(v.value);
        //                            //      else
        //                            //          goto badfile;
        //                            //  }
        //                    }
        //                }//EVENTS
        ///////////////////EVENTS_CLASSIC//////////////////////
        PGEX_Section("EVENTS_CLASSIC")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                event = CreateLvlEvent();
                PGESTRINGList musicSets;
                PGESTRINGList bgSets;
                PGESTRINGList ssSets;
                PGESTRINGList movingLayers;
                PGESTRINGList newSectionSettingsSets;
                PGESTRINGList spawnNPCs;
                PGESTRINGList spawnEffectss;
                PGESTRINGList variablesToUpdate;
                PGELIST<bool > controls;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("ET", event.name)  //Event Title
                    PGEX_StrVal("MG", event.msg)  //Event Message
                    PGEX_USLongVal("SD", event.sound_id) //Play Sound ID
                    PGEX_USLongVal("EG", event.end_game) //End game algorithm
                    PGEX_StrArrVal("LH", event.layers_hide) //Hide layers
                    PGEX_StrArrVal("LS", event.layers_show) //Show layers
                    PGEX_StrArrVal("LT", event.layers_toggle) //Toggle layers
                    //Legacy values (without SMBX-38A values support)
                    PGEX_StrArrVal("SM", musicSets)  //Switch music
                    PGEX_StrArrVal("SB", bgSets)     //Switch background
                    PGEX_StrArrVal("SS", ssSets)     //Section Size
                    //-------------------
                    //New values (with SMBX-38A values support)
                    PGEX_StrArrVal("SSS", newSectionSettingsSets) //Section settings in new format
                    //-------------------
                    //---SMBX-38A entries-----
                    PGEX_StrArrVal("MLA",  movingLayers)       //NPC's to spawn
                    PGEX_StrArrVal("SNPC", spawnNPCs)       //NPC's to spawn
                    PGEX_StrArrVal("SEF",  spawnEffectss)    //Effects to spawn
                    PGEX_StrArrVal("UV",   variablesToUpdate) //Variables to update
                    PGEX_StrVal("TSCR", event.trigger_script) //Trigger script
                    PGEX_USIntVal("TAPI", event.trigger_api_id) //Trigger script
                    PGEX_BoolVal("TMR", event.timer_def.enable) //Enable timer
                    PGEX_USLongVal("TMC", event.timer_def.count) //Count of timer units
                    PGEX_FloatVal("TMI", event.timer_def.interval) //Interval of timer tick
                    PGEX_USIntVal("TMD", event.timer_def.count_dir) //Direction of count
                    PGEX_BoolVal("TMV", event.timer_def.show) //Show timer on screen
                    //-------------------
                    PGEX_StrVal("TE", event.trigger) //Trigger event
                    PGEX_USLongVal("TD", event.trigger_timer) //Trigger delay
                    PGEX_BoolVal("DS", event.nosmoke) //Disable smoke
                    PGEX_USIntVal("AU", event.autostart) //Auto start
                    PGEX_StrVal("AUC", event.autostart_condition) //Auto start condition
                    PGEX_BoolArrVal("PC", controls) //Player controls
                    PGEX_StrVal("ML", event.movelayer)   //Move layer
                    PGEX_FloatVal("MX", event.layer_speed_x) //Layer motion speed X
                    PGEX_FloatVal("MY", event.layer_speed_y) //Layer motion speed Y
                    PGEX_SLongVal("AS", event.scroll_section) //Autoscroll section ID
                    PGEX_FloatVal("AX", event.move_camera_x) //Autoscroll speed X
                    PGEX_FloatVal("AY", event.move_camera_y) //Autoscroll speed Y
                }

                //Parse new-style parameters
                if(!newSectionSettingsSets.empty())
                {
                    for(pge_size_t q = 0; q < newSectionSettingsSets.size(); q++)
                    {
                        LevelEvent_Sets sectionSet;
                        bool valid = false;
                        PGELIST<PGESTRINGList> sssData = PGEFile::splitDataLine(newSectionSettingsSets[q], &valid);

                        if(!valid)
                        {
                            errorString = "Wrong section settings event encoded sub-entry";
                            goto badfile;
                        }

                        for(pge_size_t ssi = 0; ssi < sssData.size(); ssi++)
                        {
                            PGESTRINGList &param = sssData[ssi];

                            if(param[0] == "ID")
                            {
                                errorString = "Invalid sectionID value type";

                                if(PGEFile::IsIntU(param[1]))
                                    sectionSet.id = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SL")
                            {
                                errorString = "Invalid Section size left value type";

                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_left = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "ST")
                            {
                                errorString = "Invalid Section size top value type";

                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_top = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SB")
                            {
                                errorString = "Invalid Section size bottom value type";

                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_bottom = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SR")
                            {
                                errorString = "Invalid Section size right value type";

                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_right = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SXX")
                            {
                                errorString = "Invalid Section pos x expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SYX")
                            {
                                errorString = "Invalid Section pos y expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SWX")
                            {
                                errorString = "Invalid Section pos w expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_w = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SWH")
                            {
                                errorString = "Invalid Section pos h expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_h = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "MI")
                            {
                                errorString = "Invalid Section music ID value type";

                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.music_id = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "MF")
                            {
                                errorString = "Invalid Section music file value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.music_file = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "BG")
                            {
                                errorString = "Invalid Section background ID value type";

                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.background_id = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AS")
                            {
                                errorString = "Invalid Section Autoscroll value type";

                                if(PGEFile::IsBool(param[1]))
                                    sectionSet.autoscrol = static_cast<bool>(toInt(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AX")
                            {
                                errorString = "Invalid Section Autoscroll X value type";

                                if(PGEFile::IsFloat(param[1]))
                                    sectionSet.autoscrol_x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AY")
                            {
                                errorString = "Invalid Section Autoscroll Y value type";

                                if(PGEFile::IsFloat(param[1]))
                                    sectionSet.autoscrol_y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AXX")
                            {
                                errorString = "Invalid Section Autoscroll X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_autoscrool_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AYX")
                            {
                                errorString = "Invalid Section Autoscroll y expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_autoscrool_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        if(
                            ((sectionSet.id < 0) || (sectionSet.id >= static_cast<long>(event.sets.size())))
                        )//Append sections
                        {
                            if(sectionSet.id < 0)
                            {
                                errorString = "Section settings event contains negative section ID value or missed!";
                                goto badfile;//Missmatched section ID!
                            }

                            long last = static_cast<long>(event.sets.size() - 1);

                            while(sectionSet.id >= static_cast<long>(event.sets.size()))
                            {
                                LevelEvent_Sets set;
                                set.id = last;
                                event.sets.push_back(set);
                                last++;
                            }
                        }

                        event.sets[static_cast<pge_size_t>(sectionSet.id)] = sectionSet;
                    }//for section settings entries
                }//If new-styled section settings are gotten
                //Parse odl-style parameters
                else
                {
                    //Apply MusicSets
                    pge_size_t q = 0;

                    for(q = 0; q < event.sets.size() && q < musicSets.size(); q++)
                    {
                        event.sets[q].id = static_cast<long>(q);
                        if(!PGEFile::IsIntS(musicSets[q])) goto badfile;
                        event.sets[q].music_id = toLong(musicSets[q]);
                    }

                    //Apply Background sets
                    for(q = 0; q < event.sets.size() && q < bgSets.size(); q++)
                    {
                        event.sets[q].id = static_cast<long>(q);
                        if(!PGEFile::IsIntS(bgSets[q])) goto badfile;
                        event.sets[q].background_id = toLong(bgSets[q]);
                    }

                    //Apply section sets
                    for(q = 0; q < event.sets.size() && q < ssSets.size(); q++)
                    {
                        event.sets[q].id = static_cast<long>(q);
                        PGESTRINGList sizes;
                        PGE_SPLITSTRING(sizes, ssSets[q], ",");

                        if(sizes.size() != 4) goto badfile; //-V112

                        if(!PGEFile::IsIntS(sizes[0])) goto badfile;
                        event.sets[q].position_left = toLong(sizes[0]);

                        if(!PGEFile::IsIntS(sizes[1])) goto badfile;
                        event.sets[q].position_top = toLong(sizes[1]);

                        if(!PGEFile::IsIntS(sizes[2])) goto badfile;
                        event.sets[q].position_bottom = toLong(sizes[2]);

                        if(!PGEFile::IsIntS(sizes[3])) goto badfile;
                        event.sets[q].position_right = toLong(sizes[3]);
                    }
                }

                //Parse Moving layers
                if(!movingLayers.empty())
                {
                    for(pge_size_t q = 0; q < movingLayers.size(); q++)
                    {
                        LevelEvent_MoveLayer moveLayer;
                        bool valid = false;
                        PGELIST<PGESTRINGList> mlaData = PGEFile::splitDataLine(movingLayers[q], &valid);

                        if(!valid)
                        {
                            errorString = "Wrong Move layer event encoded sub-entry";
                            goto badfile;
                        }

                        for(pge_size_t ssi = 0; ssi < mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];

                            if(param[0] == "LN")
                            {
                                errorString = "Invalid Moving layer name value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    moveLayer.name = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SX")
                            {
                                errorString = "Invalid movelayer speed X value type";

                                if(PGEFile::IsFloat(param[1]))
                                    moveLayer.speed_x = toDouble(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SY")
                            {
                                errorString = "Invalid movelayer speed Y value type";

                                if(PGEFile::IsFloat(param[1]))
                                    moveLayer.speed_y = toDouble(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AXX")
                            {
                                errorString = "Invalid movelayer speed X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    moveLayer.expression_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "AYX")
                            {
                                errorString = "Invalid movelayer speed Y expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    moveLayer.expression_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "MW")
                            {
                                errorString = "Invalid movelayer way type value type";

                                if(PGEFile::IsIntU(param[1]))
                                    moveLayer.way = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.moving_layers.push_back(moveLayer);
                    }//for moving layers entries
                }//If SMBX38A moving layers are gotten

                //Parse NPCs to spawn
                if(!spawnNPCs.empty())
                {
                    for(pge_size_t q = 0; q < spawnNPCs.size(); q++)
                    {
                        LevelEvent_SpawnNPC spawnNPC;
                        bool valid = false;
                        PGELIST<PGESTRINGList> mlaData = PGEFile::splitDataLine(spawnNPCs[q], &valid);

                        if(!valid)
                        {
                            errorString = "Wrong Spawn NPC event encoded sub-entry";
                            goto badfile;
                        }

                        for(pge_size_t ssi = 0; ssi < mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];

                            if(param[0] == "ID")
                            {
                                errorString = "Invalid Spawn NPC ID value type";

                                if(PGEFile::IsIntU(param[1]))
                                    spawnNPC.id = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SX")
                            {
                                errorString = "Invalid Spawn NPC X value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.x = static_cast<long>(toFloat(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SY")
                            {
                                errorString = "Invalid Spawn NPC Y value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.y = static_cast<long>(toFloat(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SXX")
                            {
                                errorString = "Invalid  Spawn NPC X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SYX")
                            {
                                errorString = "Invalid Spawn NPC X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSX")
                            {
                                errorString = "Invalid Spawn NPC X value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.speed_x = static_cast<long>(toFloat(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSY")
                            {
                                errorString = "Invalid Spawn NPC Y value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.speed_y = static_cast<long>(toFloat(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSXX")
                            {
                                errorString = "Invalid  Spawn NPC Speed X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_sx = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSYX")
                            {
                                errorString = "Invalid Spawn NPC Speed Y expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_sy = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSS")
                            {
                                errorString = "Invalid  Spawn NPC Special value type";

                                if(PGEFile::IsIntU(param[1]))
                                    spawnNPC.special = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.spawn_npc.push_back(spawnNPC);
                    }//for Spawn NPC
                }//If SMBX38A NPC Spawning lists are gotten

                //Parse Effects to spawn
                if(!spawnEffectss.empty())
                {
                    for(pge_size_t q = 0; q < spawnEffectss.size(); q++)
                    {
                        LevelEvent_SpawnEffect spawnEffect;
                        bool valid = false;
                        PGELIST<PGESTRINGList> mlaData = PGEFile::splitDataLine(spawnEffectss[q], &valid);

                        if(!valid)
                        {
                            errorString = "Wrong Spawn Effect event encoded sub-entry";
                            goto badfile;
                        }

                        for(pge_size_t ssi = 0; ssi < mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];

                            if(param[0] == "ID")
                            {
                                errorString = "Invalid Spawn Effect ID value type";

                                if(PGEFile::IsIntU(param[1]))
                                    spawnEffect.id = toLong(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SX")
                            {
                                errorString = "Invalid Spawn Effect X value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.x = static_cast<long>(toFloat(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SY")
                            {
                                errorString = "Invalid Spawn Effect Y value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.y = static_cast<long>(toFloat(param[1]));
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SXX")
                            {
                                errorString = "Invalid  Spawn NPC X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SYX")
                            {
                                errorString = "Invalid Spawn NPC X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSX")
                            {
                                errorString = "Invalid Spawn NPC X value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.speed_x = toDouble(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSY")
                            {
                                errorString = "Invalid Spawn NPC Y value type";

                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.speed_y = toDouble(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSXX")
                            {
                                errorString = "Invalid  Spawn NPC Speed X expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_sx = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "SSYX")
                            {
                                errorString = "Invalid Spawn NPC Speed Y expression value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_sy = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "FP")
                            {
                                errorString = "Invalid  Spawn Effect FPS value type";

                                if(PGEFile::IsIntS(param[1]))
                                    spawnEffect.fps = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "TTL")
                            {
                                errorString = "Invalid Spawn Effect time to live value type";

                                if(PGEFile::IsIntS(param[1]))
                                    spawnEffect.max_life_time = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "GT")
                            {
                                errorString = "Invalid Spawn Effect Gravity value type";

                                if(PGEFile::IsBool(param[1]))
                                    spawnEffect.gravity = static_cast<bool>(toInt(param[1]));
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.spawn_effects.push_back(spawnEffect);
                    }//for Spawn Effect
                }//If SMBX38A Effect Spawning lists are gotten

                //Parse Variables to update
                if(!variablesToUpdate.empty())
                {
                    for(pge_size_t q = 0; q < variablesToUpdate.size(); q++)
                    {
                        LevelEvent_UpdateVariable variableToUpdate;
                        bool valid = false;
                        PGELIST<PGESTRINGList> mlaData = PGEFile::splitDataLine(variablesToUpdate[q], &valid);

                        if(!valid)
                        {
                            errorString = "Wrong Variable to update event encoded sub-entry";
                            goto badfile;
                        }

                        for(pge_size_t ssi = 0; ssi < mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];

                            if(param[0] == "N")
                            {
                                errorString = "Invalid Variable to update name value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    variableToUpdate.name = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0] == "V")
                            {
                                errorString = "Invalid Variable to update new value type";

                                if(PGEFile::IsQoutedString(param[1]))
                                    variableToUpdate.newval = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.update_variable.push_back(variableToUpdate);
                    }//for Variable update events
                }//If SMBX38A variable update lists are gotten

                //Convert boolean array into control flags
                // SMBX64-only
                if(controls.size() >= 1)  event.ctrl_up = controls[0];
                if(controls.size() >= 2)  event.ctrl_down = controls[1];
                if(controls.size() >= 3)  event.ctrl_left = controls[2];
                if(controls.size() >= 4)  event.ctrl_right = controls[3]; //-V112
                if(controls.size() >= 5)  event.ctrl_run = controls[4];
                if(controls.size() >= 6)  event.ctrl_jump = controls[5];
                if(controls.size() >= 7)  event.ctrl_drop = controls[6];
                if(controls.size() >= 8)  event.ctrl_start = controls[7];
                if(controls.size() >= 9)  event.ctrl_altrun = controls[8];
                if(controls.size() >= 10) event.ctrl_altjump = controls[9];
                // SMBX64-only end
                // SMBX-38A begin
                if(controls.size() >= 11) event.ctrls_enable = controls[10];
                if(controls.size() >= 12) event.ctrl_lock_keyboard = controls[11];
                // SMBX-38A end
                //add captured value into array
                bool found = false;
                pge_size_t q = 0;

                for(q = 0; q < FileData.events.size(); q++)
                {
                    if(FileData.events[q].name == event.name)
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    event.meta.array_id = FileData.events[q].meta.array_id;
                    FileData.events[q] = event;
                }
                else
                {
                    event.meta.array_id = FileData.events_array_id++;
                    FileData.events.push_back(event);
                }
            }
        }//EVENTS_CLASSIC
        ///////////////////VARIABLES//////////////////////
        PGEX_Section("VARIABLES")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                variable = CreateLvlVariable("unknown");
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("N", variable.name) //Variable name
                    PGEX_StrVal("V", variable.value) //Variable value
                }
                FileData.variables.push_back(variable);
            }
        }//VARIABLES
        ///////////////////SCRIPTS//////////////////////
        PGEX_Section("SCRIPTS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                script = CreateLvlScript("unknown", LevelScript::LANG_LUA);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("N", script.name)  //Variable name
                    PGEX_SIntVal("L", script.language)  //Variable name
                    PGEX_StrVal("S", script.script) //Script text
                }

                switch(script.language)
                {
                case LevelScript::LANG_LUA:
                case LevelScript::LANG_TEASCRIPT:
                case LevelScript::LANG_AUTOCODE:
                    break;

                default:
                    script.language = LevelScript::LANG_LUA; //LUA by default if any other language code!
                }

                FileData.variables.push_back(variable);
            }
        }//SCRIPTS
        ///////////////////CUSTOM ITEM CONFIGS (38A)//////////////////////
        PGEX_Section("CUSTOM_ITEMS_38A")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                customcfg38A = LevelItemSetup38A();
                PGESTRINGList data;
                unsigned int type = 0;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("T",  type) //Type of item
                    PGEX_UIntVal("ID", customcfg38A.id)
                    PGEX_StrArrVal("D", data) //Variable value
                }
                errorString = "Wrong pair syntax";
                for(PGESTRING &s : data)
                {
                    LevelItemSetup38A::Entry e;
                    PGESTRINGList pair;
                    PGE_SPLITSTRING(pair, s, "=");
                    if(pair.size() < 2)
                        goto badfile;

                    if(PGEFile::IsIntU(pair[0]))
                        e.key = (int32_t)toUInt(pair[0]);
                    else goto badfile;

                    if(PGEFile::IsIntS(pair[1]))
                        e.value = toLong(pair[1]);
                    else goto badfile;

                    customcfg38A.data.push_back(e);
                }
                customcfg38A.type = (LevelItemSetup38A::ItemType)type;
                FileData.custom38A_configs.push_back(customcfg38A);
            }
        }//CUSTOM_ITEMS_38A
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////
    errorString.clear(); //If no errors, clear string;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:    //If file format is not correct
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linenum = in.getCurrentLineNumber();
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    return false;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteExtendedLvlFileF(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, true, false, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }

    return WriteExtendedLvlFile(file, FileData);
}

bool FileFormats::WriteExtendedLvlFileRaw(LevelData &FileData, PGESTRING &rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open raw string for write";
        return false;
    }

    return WriteExtendedLvlFile(file, FileData);
}

bool FileFormats::WriteExtendedLvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData &FileData)
{
    pge_size_t i;
    FileData.meta.RecentFormat = LevelData::PGEX;
    //Count placed stars on this level
    FileData.stars = 0;

    for(i = 0; i < FileData.npc.size(); i++)
    {
        if(FileData.npc[i].is_star)
            FileData.stars++;
    }

    //HEAD section
    if((!IsEmpty(FileData.LevelName)) ||
       (FileData.stars > 0) ||
       (!IsEmpty(FileData.open_level_on_fail)) ||
       (FileData.open_level_on_fail_warpID > 0))
    {
        out << "HEAD\n";

        if(!IsEmpty(FileData.LevelName))
            out << PGEFile::value("TL", PGEFile::WriteStr(FileData.LevelName)); // Level title

        out << PGEFile::value("SZ", PGEFile::WriteInt(FileData.stars));      // Stars number

        if(!IsEmpty(FileData.open_level_on_fail))
            out << PGEFile::value("DL", PGEFile::WriteStr(FileData.open_level_on_fail)); // Open level on fail

        if(FileData.open_level_on_fail_warpID > 0)
            out << PGEFile::value("DE", PGEFile::WriteInt(FileData.open_level_on_fail_warpID));    // Open WarpID of level on fail

        out << "\n";
        out << "HEAD_END\n";
    }

    //////////////////////////////////////MetaData////////////////////////////////////////////////
    //Bookmarks
    if(!FileData.metaData.bookmarks.empty())
    {
        out << "META_BOOKMARKS\n";

        for(i = 0; i < FileData.metaData.bookmarks.size(); i++)
        {
            Bookmark &bm = FileData.metaData.bookmarks[i];
            //Bookmark name
            out << PGEFile::value("BM", PGEFile::WriteStr(bm.bookmarkName));
            out << PGEFile::value("X", PGEFile::WriteRoundFloat(bm.x));
            out << PGEFile::value("Y", PGEFile::WriteRoundFloat(bm.y));
            out << "\n";
        }

        out << "META_BOOKMARKS_END\n";
    }

#ifdef PGE_EDITOR

    //Some System information
    if(FileData.metaData.crash.used)
    {
        out << "META_SYS_CRASH\n";
        out << PGEFile::value("UT", PGEFile::WriteBool(FileData.metaData.crash.untitled));
        out << PGEFile::value("MD", PGEFile::WriteBool(FileData.metaData.crash.modifyed));
        out << PGEFile::value("FF", PGEFile::WriteInt(FileData.metaData.crash.fmtID));
        out << PGEFile::value("FV", PGEFile::WriteInt(FileData.metaData.crash.fmtVer));
        out << PGEFile::value("N", PGEFile::WriteStr(FileData.metaData.crash.filename));
        out << PGEFile::value("P", PGEFile::WriteStr(FileData.metaData.crash.path));
        out << PGEFile::value("FP", PGEFile::WriteStr(FileData.metaData.crash.fullPath));
        out << "\n";
        out << "META_SYS_CRASH_END\n";
    }

#endif
    //////////////////////////////////////MetaData///END//////////////////////////////////////////
    //SECTION section
    //Count available level sections
    pge_size_t totalSections = 0;

    for(i = 0; i < FileData.sections.size(); i++)
    {
        LevelSection &section = FileData.sections[i];

        if(
            (section.size_bottom == 0) &&
            (section.size_left == 0) &&
            (section.size_right == 0) &&
            (section.size_top == 0)
        )
            continue; //Skip unitialized sections

        totalSections++;
    }

    //Don't store section data entry if no data to add
    if(totalSections > 0)
    {
        out << "SECTION\n";

        for(i = 0; i < FileData.sections.size(); i++)
        {
            LevelSection &section = FileData.sections[i];

            if(
                (section.size_bottom == 0) &&
                (section.size_left == 0) &&
                (section.size_right == 0) &&
                (section.size_top == 0)
            )
                continue; //Skip unitialized sections

            out << PGEFile::value("SC", PGEFile::WriteInt(section.id));  // Section ID
            out << PGEFile::value("L", PGEFile::WriteInt(section.size_left));  // Left size
            out << PGEFile::value("R", PGEFile::WriteInt(section.size_right));  // Right size
            out << PGEFile::value("T", PGEFile::WriteInt(section.size_top));  // Top size
            out << PGEFile::value("B", PGEFile::WriteInt(section.size_bottom));  // Bottom size
            out << PGEFile::value("MZ", PGEFile::WriteInt(section.music_id));  // Music ID
            out << PGEFile::value("MF", PGEFile::WriteStr(section.music_file));  // Music file
            out << PGEFile::value("BG", PGEFile::WriteInt(section.background));  // Background ID
            //out << PGEFile::value("BG", PGEFile::WriteStr(section.background_file));  // Background file

            if(section.wrap_h)
                out << PGEFile::value("CS", PGEFile::WriteBool(section.wrap_h));  // Connect sides horizontally

            if(section.wrap_v)
                out << PGEFile::value("CSV", PGEFile::WriteBool(section.wrap_v));  // Connect sides vertically

            if(section.OffScreenEn)
                out << PGEFile::value("OE", PGEFile::WriteBool(section.OffScreenEn));  // Offscreen exit

            if(section.lock_left_scroll)
                out << PGEFile::value("SR", PGEFile::WriteBool(section.lock_left_scroll));  // Right-way scroll only (No Turn-back)

            if(section.lock_right_scroll)
                out << PGEFile::value("SL", PGEFile::WriteBool(section.lock_right_scroll));  // Left-way scroll only (No Turn-back)

            if(section.lock_up_scroll)
                out << PGEFile::value("SD", PGEFile::WriteBool(section.lock_up_scroll));  // Down-way scroll only (No Turn-back)

            if(section.lock_down_scroll)
                out << PGEFile::value("SU", PGEFile::WriteBool(section.lock_down_scroll));  // Up-way scroll only (No Turn-back)

            if(section.underwater)
                out << PGEFile::value("UW", PGEFile::WriteBool(section.underwater));  // Underwater bit

            //out << PGEFile::value("SL", PGEFile::WriteBool(section.noforward));  // Left-way scroll only (No Turn-forward)
            out << "\n";
        }

        out << "SECTION_END\n";
    }

    //STARTPOINT section
    int totalPlayerPoints = 0;

    for(i = 0; i < FileData.players.size(); i++)
    {
        PlayerPoint &pp = FileData.players[i];

        if((pp.w == 0) && (pp.h == 0))
            continue; //Skip empty points

        totalPlayerPoints++;
    }

    //Don't store section data entry if no data to add
    if(totalPlayerPoints > 0)
    {
        out << "STARTPOINT\n";

        for(i = 0; i < FileData.players.size(); i++)
        {
            PlayerPoint &pp = FileData.players[i];

            if((pp.w == 0) &&
               (pp.h == 0))
                continue; //Skip empty points

            out << PGEFile::value("ID", PGEFile::WriteInt(pp.id));  // Player ID
            out << PGEFile::value("X", PGEFile::WriteInt(pp.x));  // Player X
            out << PGEFile::value("Y", PGEFile::WriteInt(pp.y));  // Player Y
            out << PGEFile::value("D", PGEFile::WriteInt(pp.direction));  // Direction -1 left, 1 right
            out << "\n";
        }

        out << "STARTPOINT_END\n";
    }

    //BLOCK section
    if(!FileData.blocks.empty())
    {
        out << "BLOCK\n";
        LevelBlock defBlock = CreateLvlBlock();

        for(i = 0; i < FileData.blocks.size(); i++)
        {
            LevelBlock &blk = FileData.blocks[i];
            //Type ID
            out << PGEFile::value("ID", PGEFile::WriteInt(blk.id));  // Block ID
            //Position
            out << PGEFile::value("X", PGEFile::WriteInt(blk.x));  // Block X
            out << PGEFile::value("Y", PGEFile::WriteInt(blk.y));  // Block Y
            //Size
            out << PGEFile::value("W", PGEFile::WriteInt(blk.w));  // Block Width (sizable only)
            out << PGEFile::value("H", PGEFile::WriteInt(blk.h));  // Block Height (sizable only)
            if(blk.autoscale != defBlock.autoscale)
                out << PGEFile::value("AS", PGEFile::WriteBool(blk.autoscale));// AutoScale

            if(!IsEmpty(blk.gfx_name))
                out << PGEFile::value("GXN", PGEFile::WriteStr(blk.gfx_name));// 38A GFX-Name
            if(blk.gfx_dx > 0) //38A graphics extend x
                out << PGEFile::value("GXX", PGEFile::WriteInt(blk.gfx_dx));  // 38A graphics extend x
            if(blk.gfx_dy > 0) //38A graphics extend y
                out << PGEFile::value("GXX", PGEFile::WriteInt(blk.gfx_dy));  // 38A graphics extend y

            //Included NPC
            if(blk.npc_id != 0) //Write only if not zero
                out << PGEFile::value("CN", PGEFile::WriteInt(blk.npc_id));  // Included NPC

            //Boolean flags
            if(blk.invisible)
                out << PGEFile::value("IV", PGEFile::WriteBool(blk.invisible));  // Invisible
            if(blk.slippery)
                out << PGEFile::value("SL", PGEFile::WriteBool(blk.slippery));  // Slippery flag
            //Layer
            if(blk.layer != defBlock.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::WriteStr(blk.layer));  // Layer
            //Event Slots
            if(!IsEmpty(blk.event_destroy))
                out << PGEFile::value("ED", PGEFile::WriteStr(blk.event_destroy));
            if(!IsEmpty(blk.event_hit))
                out << PGEFile::value("EH", PGEFile::WriteStr(blk.event_hit));
            if(!IsEmpty(blk.event_emptylayer))
                out << PGEFile::value("EE", PGEFile::WriteStr(blk.event_emptylayer));
            if(!IsEmpty(blk.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(blk.meta.custom_params));

            out << "\n";
        }

        out << "BLOCK_END\n";
    }

    //BGO section
    if(!FileData.bgo.empty())
    {
        out << "BGO\n";
        LevelBGO defBGO = CreateLvlBgo();

        for(i = 0; i < FileData.bgo.size(); i++)
        {
            LevelBGO &bgo = FileData.bgo[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(bgo.id));  // BGO ID
            //Position
            out << PGEFile::value("X", PGEFile::WriteInt(bgo.x));  // BGO X
            out << PGEFile::value("Y", PGEFile::WriteInt(bgo.y));  // BGO Y
            if(bgo.gfx_dx > 0) //38A graphics extend x
                out << PGEFile::value("GXX", PGEFile::WriteInt(bgo.gfx_dx));  // 38A graphics extend x
            if(bgo.gfx_dy > 0) //38A graphics extend y
                out << PGEFile::value("GXX", PGEFile::WriteInt(bgo.gfx_dy));  // 38A graphics extend y
            if(fabs(bgo.z_offset - defBGO.z_offset) > DBL_EPSILON)
                out << PGEFile::value("ZO", PGEFile::WriteFloat(bgo.z_offset));  // BGO Z-Offset
            if(bgo.z_mode != defBGO.z_mode)
                out << PGEFile::value("ZP", PGEFile::WriteInt(bgo.z_mode));  // BGO Z-Mode
            if(bgo.smbx64_sp != -1)
                out << PGEFile::value("SP", PGEFile::WriteInt(bgo.smbx64_sp));  // BGO SMBX64 Sort Priority
            if(bgo.layer != defBGO.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::WriteStr(bgo.layer));  // Layer
            if(!IsEmpty(bgo.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(bgo.meta.custom_params));
            out << "\n";
        }

        out << "BGO_END\n";
    }

    //NPC section
    if(!FileData.npc.empty())
    {
        out << "NPC\n";
        LevelNPC defNPC = CreateLvlNpc();

        for(i = 0; i < FileData.npc.size(); i++)
        {
            LevelNPC &npc = FileData.npc[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(npc.id));  // NPC ID
            //Position
            out << PGEFile::value("X", PGEFile::WriteInt(npc.x));  // NPC X
            out << PGEFile::value("Y", PGEFile::WriteInt(npc.y));  // NPC Y

            if(!IsEmpty(npc.gfx_name))
                out << PGEFile::value("GXN", PGEFile::WriteStr(npc.gfx_name));// 38A GFX-Name
            if(npc.gfx_dx > 0) //38A graphics extend x
                out << PGEFile::value("GXX", PGEFile::WriteInt(npc.gfx_dx));  // 38A graphics extend x
            if(npc.gfx_dy > 0) //38A graphics extend y
                out << PGEFile::value("GXX", PGEFile::WriteInt(npc.gfx_dy));  // 38A graphics extend y

            out << PGEFile::value("D", PGEFile::WriteInt(npc.direct));  // NPC Direction

            if(npc.contents != 0)
                out << PGEFile::value("CN", PGEFile::WriteInt(npc.contents));  // Contents of container
            if(npc.special_data != defNPC.special_data)
                out << PGEFile::value("S1", PGEFile::WriteInt(npc.special_data));  // Special value 1
            if(npc.special_data2 != defNPC.special_data2)
                out << PGEFile::value("S2", PGEFile::WriteInt(npc.special_data2));  // Special value 2

            if(npc.generator)
            {
                out << PGEFile::value("GE", PGEFile::WriteBool(npc.generator));  // NPC Generator
                out << PGEFile::value("GT", PGEFile::WriteInt(npc.generator_type));  // Generator type
                out << PGEFile::value("GD", PGEFile::WriteInt(npc.generator_direct));  // Generator direct
                out << PGEFile::value("GM", PGEFile::WriteInt(npc.generator_period));  // Generator time

                if(npc.generator_direct == 0)
                {
                    out << PGEFile::value("GA", PGEFile::WriteFloat(npc.generator_custom_angle));  // Generator custom angle
                    out << PGEFile::value("GB", PGEFile::WriteInt(npc.generator_branches));  // Generator branches
                    out << PGEFile::value("GR", PGEFile::WriteFloat(npc.generator_angle_range));  // Generator angle range
                    out << PGEFile::value("GS", PGEFile::WriteFloat(npc.generator_initial_speed));  // Generator initial speed
                }
            }

            if(!IsEmpty(npc.msg))
                out << PGEFile::value("MG", PGEFile::WriteStr(npc.msg));  // Message
            if(npc.friendly)
                out << PGEFile::value("FD", PGEFile::WriteBool(npc.friendly));  // Friendly
            if(npc.nomove)
                out << PGEFile::value("NM", PGEFile::WriteBool(npc.nomove));  // Idle
            if(npc.is_boss)
                out << PGEFile::value("BS", PGEFile::WriteBool(npc.is_boss));  // Set as boss
            if(npc.layer != defNPC.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::WriteStr(npc.layer));  // Layer
            if(!IsEmpty(npc.attach_layer))
                out << PGEFile::value("LA", PGEFile::WriteStr(npc.attach_layer));  // Attach layer
            if(!IsEmpty(npc.send_id_to_variable))
                out << PGEFile::value("SV", PGEFile::WriteStr(npc.send_id_to_variable)); //Send ID to variable

            //Event slots
            if(!IsEmpty(npc.event_activate))
                out << PGEFile::value("EA", PGEFile::WriteStr(npc.event_activate));
            if(!IsEmpty(npc.event_die))
                out << PGEFile::value("ED", PGEFile::WriteStr(npc.event_die));
            if(!IsEmpty(npc.event_talk))
                out << PGEFile::value("ET", PGEFile::WriteStr(npc.event_talk));
            if(!IsEmpty(npc.event_emptylayer))
                out << PGEFile::value("EE", PGEFile::WriteStr(npc.event_emptylayer));
            if(!IsEmpty(npc.event_grab))
                out << PGEFile::value("EG", PGEFile::WriteStr(npc.event_grab));
            if(!IsEmpty(npc.event_touch))
                out << PGEFile::value("EO", PGEFile::WriteStr(npc.event_touch));
            if(!IsEmpty(npc.event_touch))
                out << PGEFile::value("EF", PGEFile::WriteStr(npc.event_nextframe));
            if(!IsEmpty(npc.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(npc.meta.custom_params));

            out << "\n";
        }

        out << "NPC_END\n";
    }

    //PHYSICS section
    if(!FileData.physez.empty())
    {
        out << "PHYSICS\n";
        LevelPhysEnv defPhys = CreateLvlPhysEnv();

        for(i = 0; i < FileData.physez.size(); i++)
        {
            LevelPhysEnv &physEnv = FileData.physez[i];
            out << PGEFile::value("ET", PGEFile::WriteInt(physEnv.env_type));
            //Position
            out << PGEFile::value("X", PGEFile::WriteInt(physEnv.x));  // Physic Env X
            out << PGEFile::value("Y", PGEFile::WriteInt(physEnv.y));  // Physic Env Y
            //Size
            out << PGEFile::value("W", PGEFile::WriteInt(physEnv.w));  // Physic Env Width
            out << PGEFile::value("H", PGEFile::WriteInt(physEnv.h));  // Physic Env Height

            if(physEnv.env_type == LevelPhysEnv::ENV_CUSTOM_LIQUID)
                out << PGEFile::value("FR", PGEFile::WriteFloat(physEnv.friction)); //Friction
            if(physEnv.accel_direct >= 0.0)
                out << PGEFile::value("AD", PGEFile::WriteFloat(physEnv.accel_direct)); //Acceleration direction
            if(physEnv.accel != 0.0)
                out << PGEFile::value("AC", PGEFile::WriteFloat(physEnv.accel)); //Acceleration
            if(physEnv.max_velocity != 0.0)
                out << PGEFile::value("MV", PGEFile::WriteFloat(physEnv.max_velocity)); //Max-velocity
            if(physEnv.layer != defPhys.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::WriteStr(physEnv.layer));  // Layer
            if(!IsEmpty(physEnv.touch_event))
                out << PGEFile::value("EO", PGEFile::WriteStr(physEnv.touch_event));  // Touch event slot
            if(!IsEmpty(physEnv.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(physEnv.meta.custom_params));

            out << "\n";
        }

        out << "PHYSICS_END\n";
    }

    //DOORS section
    if(!FileData.doors.empty())
    {
        out << "DOORS\n";
        LevelDoor defDoor = CreateLvlWarp();

        for(i = 0; i < FileData.doors.size(); i++)
        {
            LevelDoor &warp = FileData.doors[i];

            if(((!warp.lvl_o) && (!warp.lvl_i)) || ((warp.lvl_o) && (!warp.lvl_i)))
                if(!warp.isSetIn) continue; // Skip broken door

            if(((!warp.lvl_o) && (!warp.lvl_i)) || ((warp.lvl_i)))
                if(!warp.isSetOut) continue; // Skip broken door

            //Entrance
            if(warp.isSetIn)
            {
                out << PGEFile::value("IX", PGEFile::WriteInt(warp.ix));  // Warp Input X
                out << PGEFile::value("IY", PGEFile::WriteInt(warp.iy));  // Warp Input Y
            }

            if(warp.isSetOut)
            {
                out << PGEFile::value("OX", PGEFile::WriteInt(warp.ox));  // Warp Output X
                out << PGEFile::value("OY", PGEFile::WriteInt(warp.oy));  // Warp Output Y
            }

            if(warp.length_i != 32) //-V112
                out << PGEFile::value("IL", PGEFile::WriteInt(warp.length_i));  //Length of entrance

            if(warp.length_o != 32) //-V112
                out << PGEFile::value("OL", PGEFile::WriteInt(warp.length_o));  //Length of exit

            out << PGEFile::value("DT", PGEFile::WriteInt(warp.type));  // Warp type
            out << PGEFile::value("ID", PGEFile::WriteInt(warp.idirect));  // Warp Input direction
            out << PGEFile::value("OD", PGEFile::WriteInt(warp.odirect));  // Warp Outpu direction

            if(warp.world_x != -1 && warp.world_y != -1)
            {
                out << PGEFile::value("WX", PGEFile::WriteInt(warp.world_x));  // World X
                out << PGEFile::value("WY", PGEFile::WriteInt(warp.world_y));  // World Y
            }

            if(!IsEmpty(warp.lname))
            {
                out << PGEFile::value("LF", PGEFile::WriteStr(warp.lname));  // Warp to level file
                out << PGEFile::value("LI", PGEFile::WriteInt(warp.warpto));  // Warp arrayID
            }

            if(warp.lvl_i)
                out << PGEFile::value("ET", PGEFile::WriteBool(warp.lvl_i));  // Level Entance
            if(warp.lvl_o)
                out << PGEFile::value("EX", PGEFile::WriteBool(warp.lvl_o));  // Level Exit
            if(warp.stars > 0)
                out << PGEFile::value("SL", PGEFile::WriteInt(warp.stars));  // Need a stars
            if(!IsEmpty(warp.stars_msg))
                out << PGEFile::value("SM", PGEFile::WriteStr(warp.stars_msg));  // Message for start requirement
            if(warp.star_num_hide)
                out << PGEFile::value("SH", PGEFile::WriteBool(warp.star_num_hide));  // Don't show number of stars
            if(warp.novehicles)
                out << PGEFile::value("NV", PGEFile::WriteBool(warp.novehicles));  // Deny Vehicles
            if(warp.allownpc)
                out << PGEFile::value("AI", PGEFile::WriteBool(warp.allownpc));  // Allow Items
            if(warp.locked)
                out << PGEFile::value("LC", PGEFile::WriteBool(warp.locked));  // Locked door
            if(warp.need_a_bomb)
                out << PGEFile::value("LB", PGEFile::WriteBool(warp.need_a_bomb));  //Need a bomb to open door
            if(warp.hide_entering_scene)
                out << PGEFile::value("HS", PGEFile::WriteBool(warp.hide_entering_scene));   //Hide entrance scene
            if(warp.allownpc_interlevel)
                out << PGEFile::value("AL", PGEFile::WriteBool(warp.allownpc_interlevel));   //Allow Items inter-level
            if(warp.special_state_required)
                out << PGEFile::value("SR", PGEFile::WriteBool(warp.special_state_required));//Special state required
            if(warp.cannon_exit)
            {
                out << PGEFile::value("PT", PGEFile::WriteBool(warp.cannon_exit));//cannon exit
                out << PGEFile::value("PS", PGEFile::WriteFloat(warp.cannon_exit_speed));//cannon exit projectile speed
            }
            if(warp.layer != defDoor.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::WriteStr(warp.layer));  // Layer
            if(!IsEmpty(warp.event_enter)) //Write only if not default
                out << PGEFile::value("EE", PGEFile::WriteStr(warp.event_enter));  // On-Enter event
            if(warp.two_way)
                out << PGEFile::value("TW", PGEFile::WriteBool(warp.two_way)); //Two-way warp
            if(!IsEmpty(warp.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(warp.meta.custom_params));

            out << "\n";
        }

        out << "DOORS_END\n";
    }

    //LAYERS section
    if(!FileData.layers.empty())
    {
        out << "LAYERS\n";

        for(i = 0; i < FileData.layers.size(); i++)
        {
            LevelLayer &layer = FileData.layers[i];
            out << PGEFile::value("LR", PGEFile::WriteStr(layer.name));  // Layer name

            if(layer.hidden)
                out << PGEFile::value("HD", PGEFile::WriteBool(layer.hidden));  // Hidden

            if(layer.locked)
                out << PGEFile::value("LC", PGEFile::WriteBool(layer.locked));  // Locked

            out << "\n";
        }

        out << "LAYERS_END\n";
    }

    //EVENTS section (action styled)
    //EVENT sub-section of action-styled events

    //EVENTS_CLASSIC (SMBX-Styled events)
    if(!FileData.events.empty())
    {
        out << "EVENTS_CLASSIC\n";
        bool addArray = false;

        for(i = 0; i < FileData.events.size(); i++)
        {
            LevelSMBX64Event &event = FileData.events[i];
            out << PGEFile::value("ET", PGEFile::WriteStr(event.name));  // Event name

            if(!IsEmpty(event.msg))
                out << PGEFile::value("MG", PGEFile::WriteStr(event.msg));  // Show Message

            if(event.sound_id != 0)
                out << PGEFile::value("SD", PGEFile::WriteInt(event.sound_id));  // Play Sound ID

            if(event.end_game != 0)
                out << PGEFile::value("EG", PGEFile::WriteInt(event.end_game));  // End game

            if(!event.layers_hide.empty())
                out << PGEFile::value("LH", PGEFile::WriteStrArr(event.layers_hide));  // Hide Layers

            if(!event.layers_show.empty())
                out << PGEFile::value("LS", PGEFile::WriteStrArr(event.layers_show));  // Show Layers

            if(!event.layers_toggle.empty())
                out << PGEFile::value("LT", PGEFile::WriteStrArr(event.layers_toggle));  // Toggle Layers

            /*
            PGESTRINGList musicSets;
            addArray=false;
            for(int ttt=0; ttt<(signed)event.sets.size(); ttt++)
            {
                musicSets.push_back(fromNum(event.sets[ttt].music_id));
            }
            for(int tt=0; tt<(signed)musicSets.size(); tt++)
            { if(musicSets[tt]!="-1") addArray=true; }

            if(addArray) out << PGEFile::value("SM", PGEFile::WriteStrArr(musicSets));  // Change section's musics


            addArray=false;
            for(int ttt=0; ttt<(signed)event.sets.size(); ttt++)
            {
                musicSets.push_back(event.sets[ttt].music_file);
            }
            for(int tt=0; tt<(signed)musicSets.size(); tt++)
            { if(!musicSets[tt].PGESTRINGisEmpty()) addArray=true; }

            if(addArray) out << PGEFile::value("SMF", PGEFile::WriteStrArr(musicSets));  // Change section's music files


            PGESTRINGList backSets;
            addArray=false;
            for(int tt=0; tt<(signed)event.sets.size(); tt++)
            {
                backSets.push_back(fromNum(event.sets[tt].background_id));
            }
            for(int tt=0; tt<(signed)backSets.size(); tt++)
            { if(backSets[tt]!="-1") addArray=true; }

            if(addArray) out << PGEFile::value("SB", PGEFile::WriteStrArr(backSets));  // Change section's backgrounds


            PGESTRINGList sizeSets;
            addArray=false;
            for(int tt=0; tt<(signed)event.sets.size(); tt++)
            {
                LevelEvent_Sets &x=event.sets[tt];
                QString sizeSect=   fromNum(x.position_left)+","+
                                    fromNum(x.position_top)+","+
                                    fromNum(x.position_bottom)+","+
                                    fromNum(x.position_right);
                if(sizeSect != "-1,0,0,0")
                    addArray=true;
                sizeSets.push_back(sizeSect);
            }
            if(addArray)
                out << PGEFile::value("SS", PGEFile::WriteStrArr(sizeSets));// Change section's sizes
            */
            PGESTRINGList sectionSettingsSets;

            for(pge_size_t tt = 0; tt < event.sets.size(); tt++)
            {
                bool hasParams = false;
                PGESTRING sectionSettings;
                LevelEvent_Sets &x = event.sets[tt];
                sectionSettings += PGEFile::value("ID", PGEFile::WriteInt(x.id));
                bool customSize = (x.position_left != LevelEvent_Sets::LESet_Nothing) &&
                                  (x.position_left != LevelEvent_Sets::LESet_ResetDefault);

                if(x.position_left != -1)
                {
                    sectionSettings += PGEFile::value("SL", PGEFile::WriteInt(x.position_left));
                    hasParams = true;
                }

                if(customSize && (x.position_top != 0))
                {
                    sectionSettings += PGEFile::value("ST", PGEFile::WriteInt(x.position_top));
                    hasParams = true;
                }

                if(customSize && (x.position_bottom != 0))
                {
                    sectionSettings += PGEFile::value("SB", PGEFile::WriteInt(x.position_bottom));
                    hasParams = true;
                }

                if(customSize && (x.position_right != 0))
                {
                    sectionSettings += PGEFile::value("SR", PGEFile::WriteInt(x.position_right));
                    hasParams = true;
                }

                if(!IsEmpty(x.expression_pos_x) && (x.expression_pos_x != "0"))
                {
                    sectionSettings += PGEFile::value("SXX", PGEFile::WriteStr(x.expression_pos_x));
                    hasParams = true;
                }

                if(!IsEmpty(x.expression_pos_y) && (x.expression_pos_y != "0"))
                {
                    sectionSettings += PGEFile::value("SYX", PGEFile::WriteStr(x.expression_pos_y));
                    hasParams = true;
                }

                if(!IsEmpty(x.expression_pos_w) && (x.expression_pos_w != "0"))
                {
                    sectionSettings += PGEFile::value("SWX", PGEFile::WriteStr(x.expression_pos_w));
                    hasParams = true;
                }

                if(!IsEmpty(x.expression_pos_h) && (x.expression_pos_h != "0"))
                {
                    sectionSettings += PGEFile::value("SHX", PGEFile::WriteStr(x.expression_pos_h));
                    hasParams = true;
                }

                if(x.music_id != LevelEvent_Sets::LESet_Nothing)
                {
                    sectionSettings += PGEFile::value("MI", PGEFile::WriteInt(x.music_id));
                    hasParams = true;
                }

                if(!IsEmpty(x.music_file))
                {
                    sectionSettings += PGEFile::value("MF", PGEFile::WriteStr(x.music_file));
                    hasParams = true;
                }

                if(x.background_id != LevelEvent_Sets::LESet_Nothing)
                {
                    sectionSettings += PGEFile::value("BG", PGEFile::WriteInt(x.background_id));
                    hasParams = true;
                }

                if(x.autoscrol)
                {
                    sectionSettings += PGEFile::value("AS", PGEFile::WriteBool(x.autoscrol));
                    hasParams = true;
                }

                if(x.autoscrol_x != 0.0f)
                {
                    sectionSettings += PGEFile::value("AX", PGEFile::WriteFloat(x.autoscrol_x));
                    hasParams = true;
                }

                if(x.autoscrol_y != 0.0f)
                {
                    sectionSettings += PGEFile::value("AY", PGEFile::WriteFloat(x.autoscrol_y));
                    hasParams = true;
                }

                if(!IsEmpty(x.expression_autoscrool_x) && (x.expression_autoscrool_x != "0"))
                {
                    sectionSettings += PGEFile::value("AXX", PGEFile::WriteStr(x.expression_autoscrool_x));
                    hasParams = true;
                }

                if(!IsEmpty(x.expression_autoscrool_y) && (x.expression_autoscrool_y != "0"))
                {
                    sectionSettings += PGEFile::value("AYX", PGEFile::WriteStr(x.expression_autoscrool_y));
                    hasParams = true;
                }

                if(hasParams)
                    sectionSettingsSets.push_back(sectionSettings);
            }

            if(!sectionSettingsSets.empty())
                out << PGEFile::value("SSS", PGEFile::WriteStrArr(sectionSettingsSets));//Change section's settings

            if(!IsEmpty(event.trigger))
            {
                out << PGEFile::value("TE", PGEFile::WriteStr(event.trigger)); // Trigger Event

                if(event.trigger_timer > 0)
                    out << PGEFile::value("TD", PGEFile::WriteInt(event.trigger_timer)); // Trigger delay
            }

            if(!IsEmpty(event.trigger_script))
                out << PGEFile::value("TSCR", PGEFile::WriteStr(event.trigger_script));

            if(event.trigger_api_id != 0)
                out << PGEFile::value("TAPI", PGEFile::WriteInt(event.trigger_api_id));

            if(event.nosmoke)
                out << PGEFile::value("DS", PGEFile::WriteBool(event.nosmoke)); // Disable Smoke

            if(event.autostart > 0)
                out << PGEFile::value("AU", PGEFile::WriteInt(event.autostart)); // Autostart event

            if(!IsEmpty(event.autostart_condition))
                out << PGEFile::value("AUC", PGEFile::WriteStr(event.autostart_condition)); // Autostart condition event

            PGELIST<bool > controls;
            controls.push_back(event.ctrl_up);
            controls.push_back(event.ctrl_down);
            controls.push_back(event.ctrl_left);
            controls.push_back(event.ctrl_right);
            controls.push_back(event.ctrl_run);
            controls.push_back(event.ctrl_jump);
            controls.push_back(event.ctrl_drop);
            controls.push_back(event.ctrl_start);
            controls.push_back(event.ctrl_altrun);
            controls.push_back(event.ctrl_altjump);
            controls.push_back(event.ctrls_enable);
            controls.push_back(event.ctrl_lock_keyboard);
            addArray = false;

            for(pge_size_t tt = 0; tt < controls.size(); tt++)
            {
                if(controls[tt]) addArray = true;
            }

            if(addArray) out << PGEFile::value("PC", PGEFile::WriteBoolArr(controls)); // Create boolean array

            if(!IsEmpty(event.movelayer))
            {
                out << PGEFile::value("ML", PGEFile::WriteStr(event.movelayer)); // Move layer
                out << PGEFile::value("MX", PGEFile::WriteFloat(event.layer_speed_x)); // Move layer X
                out << PGEFile::value("MY", PGEFile::WriteFloat(event.layer_speed_y)); // Move layer Y
            }

            if(!event.moving_layers.empty())
            {
                PGESTRINGList moveLayers;

                for(pge_size_t j = 0; j < event.moving_layers.size(); j++)
                {
                    LevelEvent_MoveLayer &mvl = event.moving_layers[j];
                    PGESTRING moveLayer;

                    if(IsEmpty(mvl.name))
                        continue;

                    moveLayer += PGEFile::value("LN", PGEFile::WriteStr(mvl.name));

                    if(mvl.speed_x != 0.0)
                        moveLayer += PGEFile::value("SX", PGEFile::WriteFloat(mvl.speed_x));

                    if(!IsEmpty(mvl.expression_x) && (mvl.expression_x != "0"))
                        moveLayer += PGEFile::value("SXX", PGEFile::WriteStr(mvl.expression_x));

                    if(mvl.speed_y != 0.0)
                        moveLayer += PGEFile::value("SY", PGEFile::WriteFloat(mvl.speed_y));

                    if(!IsEmpty(mvl.expression_y) && (mvl.expression_y != "0"))
                        moveLayer += PGEFile::value("SYX", PGEFile::WriteStr(mvl.expression_y));

                    if(mvl.way != 0)
                        moveLayer += PGEFile::value("MW", PGEFile::WriteInt(mvl.way));

                    moveLayers.push_back(moveLayer);
                }

                out << PGEFile::value("MLA", PGEFile::WriteStrArr(moveLayers));
            }

            //NPC's to spawn
            if(!event.spawn_npc.empty())
            {
                PGESTRINGList spawnNPCs;

                for(pge_size_t j = 0; j < event.spawn_npc.size(); j++)
                {
                    PGESTRING spawnNPC;
                    LevelEvent_SpawnNPC &npc = event.spawn_npc[j];
                    spawnNPC += PGEFile::value("ID", PGEFile::WriteInt(npc.id));

                    if(npc.x != 0)
                        spawnNPC += PGEFile::value("SX", PGEFile::WriteInt(npc.x));

                    if(!IsEmpty(npc.expression_x) && (npc.expression_x != "0"))
                        spawnNPC += PGEFile::value("SXX", PGEFile::WriteStr(npc.expression_x));

                    if(npc.y != 0)
                        spawnNPC += PGEFile::value("SY", PGEFile::WriteInt(npc.y));

                    if(!IsEmpty(npc.expression_y) && (npc.expression_y != "0"))
                        spawnNPC += PGEFile::value("SYX", PGEFile::WriteStr(npc.expression_y));

                    if(npc.speed_x != 0.0)
                        spawnNPC += PGEFile::value("SSX", PGEFile::WriteFloat(npc.speed_x));

                    if(!IsEmpty(npc.expression_sx)  && (npc.expression_sx != "0"))
                        spawnNPC += PGEFile::value("SSXX", PGEFile::WriteStr(npc.expression_sx));

                    if(npc.speed_y != 0.0)
                        spawnNPC += PGEFile::value("SSY", PGEFile::WriteFloat(npc.speed_y));

                    if(!IsEmpty(npc.expression_sy) && (npc.expression_sy != "0"))
                        spawnNPC += PGEFile::value("SSYX", PGEFile::WriteStr(npc.expression_sy));

                    if(npc.special != 0)
                        spawnNPC += PGEFile::value("SSS", PGEFile::WriteInt(npc.special));

                    spawnNPCs.push_back(spawnNPC);
                }

                out << PGEFile::value("SNPC", PGEFile::WriteStrArr(spawnNPCs));
            }

            //Effects to spawn
            if(!event.spawn_effects.empty())
            {
                PGESTRINGList spawnEffects;

                for(pge_size_t j = 0; j < event.spawn_effects.size(); j++)
                {
                    PGESTRING spawnEffect;
                    LevelEvent_SpawnEffect &effect = event.spawn_effects[j];
                    spawnEffect += PGEFile::value("ID", PGEFile::WriteInt(effect.id));

                    if(effect.x != 0)
                        spawnEffect += PGEFile::value("SX", PGEFile::WriteInt(effect.x));
                    if(!IsEmpty(effect.expression_x) && (effect.expression_x != "0"))
                        spawnEffect += PGEFile::value("SXX", PGEFile::WriteStr(effect.expression_x));
                    if(effect.y != 0)
                        spawnEffect += PGEFile::value("SY", PGEFile::WriteInt(effect.y));
                    if(!IsEmpty(effect.expression_y) && (effect.expression_y != "0"))
                        spawnEffect += PGEFile::value("SYX", PGEFile::WriteStr(effect.expression_y));
                    if(effect.speed_x != 0.0)
                        spawnEffect += PGEFile::value("SSX", PGEFile::WriteFloat(effect.speed_x));
                    if(!IsEmpty(effect.expression_sx) && (effect.expression_sx != "0"))
                        spawnEffect += PGEFile::value("SSXX", PGEFile::WriteStr(effect.expression_sx));
                    if(effect.speed_y != 0.0)
                        spawnEffect += PGEFile::value("SSY", PGEFile::WriteFloat(effect.speed_y));
                    if(!IsEmpty(effect.expression_sy) && (effect.expression_sy != "0"))
                        spawnEffect += PGEFile::value("SSYX", PGEFile::WriteStr(effect.expression_sy));
                    if(effect.fps != 0)
                        spawnEffect += PGEFile::value("FP", PGEFile::WriteInt(effect.fps));
                    if(effect.max_life_time != 0)
                        spawnEffect += PGEFile::value("TTL", PGEFile::WriteInt(effect.max_life_time));
                    if(effect.gravity)
                        spawnEffect += PGEFile::value("GT", PGEFile::WriteBool(effect.gravity));
                    spawnEffects.push_back(spawnEffect);
                }

                out << PGEFile::value("SEF", PGEFile::WriteStrArr(spawnEffects));
            }

            out << PGEFile::value("AS", PGEFile::WriteInt(event.scroll_section)); // Move camera
            out << PGEFile::value("AX", PGEFile::WriteFloat(event.move_camera_x)); // Move camera x
            out << PGEFile::value("AY", PGEFile::WriteFloat(event.move_camera_y)); // Move camera y

            //Variables to update
            if(!event.update_variable.empty())
            {
                PGESTRINGList updateVars;

                for(pge_size_t j = 0; j < event.update_variable.size(); j++)
                {
                    PGESTRING updateVar;
                    LevelEvent_UpdateVariable &updVar = event.update_variable[j];
                    updateVar += PGEFile::value("N", PGEFile::WriteStr(updVar.name));
                    updateVar += PGEFile::value("V", PGEFile::WriteStr(updVar.newval));
                    updateVars.push_back(updateVar);
                }

                out << PGEFile::value("UV", PGEFile::WriteStrArr(updateVars));
            }

            if(event.timer_def.enable)
            {
                out << PGEFile::value("TMR", PGEFile::WriteBool(event.timer_def.enable));     //Enable timer
                out << PGEFile::value("TMC", PGEFile::WriteInt(event.timer_def.count));       //Time left (ticks)
                out << PGEFile::value("TMI", PGEFile::WriteFloat(event.timer_def.interval));    //Tick Interval
                out << PGEFile::value("TMD", PGEFile::WriteInt(event.timer_def.count_dir));   //Count direction
                out << PGEFile::value("TMV", PGEFile::WriteBool(event.timer_def.show));       //Is timer vizible
            }

            out << "\n";
        }

        out << "EVENTS_CLASSIC_END\n";

        //VARIABLES section
        if(!FileData.variables.empty())
        {
            out << "VARIABLES\n";

            for(auto &var : FileData.variables)
            {
                out << PGEFile::value("N", PGEFile::WriteStr(var.name));  // Variable name
                if(!IsEmpty(var.value))
                    out << PGEFile::value("V", PGEFile::WriteStr(var.value));  // Value
                out << "\n";
            }

            out << "VARIABLES_END\n";
        }

        //SCRIPTS section
        if(!FileData.scripts.empty())
        {
            out << "SCRIPTS\n";

            for(auto &script : FileData.scripts)
            {
                out << PGEFile::value("N", PGEFile::WriteStr(script.name));  // Variable name
                out << PGEFile::value("L", PGEFile::WriteInt(script.language));// Code of language
                if(!IsEmpty(script.script))
                    out << PGEFile::value("S", PGEFile::WriteStr(script.script));  // Script text
                out << "\n";
            }

            out << "SCRIPTS_END\n";
        }

        //CUSTOM_ITEMS_38A section
        if(!FileData.custom38A_configs.empty())
        {
            out << "CUSTOM_ITEMS_38A\n";
            for(auto &cfg : FileData.custom38A_configs)
            {
                out << PGEFile::value("T", PGEFile::WriteInt(cfg.type));
                out << PGEFile::value("ID", PGEFile::WriteInt(cfg.id));
                PGESTRINGList data;
                for(auto &e : cfg.data)
                    data.push_back( PGEFile::WriteInt(e.key) + "=" + PGEFile::WriteInt(e.value));
                out << PGEFile::value("D", PGEFile::WriteStrArr(data));
                out << "\n";
            }
            out << "CUSTOM_ITEMS_38A_END\n";
        }
    }

    return true;
}
