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

#include "smbx38a_private.h"

// Settings
static constexpr uint32_t newest_file_format = 68;

/***********  Pre-defined values dependent to NPC Generator Effect field value  **************/

/*

FIELD to Types/Directions conversion table
2   1   0   3   4 <- types (PGE/SMBX64)
0   1   2   3   4 <- types (SMBX-38A)
                          ___ directions
                         /
0   0   0   0   0       0
1   5   9   13  17      1
2   6   10  14  18      2
3   7   11  15  19      3
4   8   12  16  20      4
9   13  17  21  25      9
10  14  18  22  26      10
11  15  19  23  27      11
12  16  20  24  28      12

*/

const int SMBX38A_NpcGeneratorTypes[29] =
    //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
{ 0, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 3, 3, 3, 3, 4, 4, 4, 4, 3, 3, 3, 3, 4, 4, 4, 4 };

const int SMBX38A_NpcGeneratorDirections[29] =
    //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
{ 0, 1, 2, 3, 4, 1, 2, 3, 4, 9, 10, 11, 12, 1, 2, 3, 4, 1, 2, 3, 4, 9, 10, 11, 12, 9, 10, 11, 12};


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX38ALvlFileHeader(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    CreateLevelHeader(FileData);
    FileData.meta.RecentFormat = LevelData::SMBX38A;
    #if !defined(_MSC_VER) || _MSC_VER > 1800
    PGE_FileFormats_misc::TextFileInput inf;
    uint32_t file_version = 0;

    if(!inf.open(filePath, false))
    {
        FileData.meta.ReadFileValid = false;
        return false;
    }

    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);

    try
    {
        CSVPGEReader readerBridge(&inf);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        file_version = toUInt(PGE_SubStr(fileIndentifier, 8, -1));

        while(!inf.eof())
        {
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);

            if(identifier == "A")
            {
                PGESTRING s[4];
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        &FileData.stars,
                                        MakeCSVPostProcessor(&FileData.LevelName, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail, PGESTRING(""), nullptr, PGEUrlDecodeFunc),//3
                                        MakeCSVOptionalEmpty(&FileData.open_level_on_fail_warpID, 0u),
                                        MakeCSVOptionalSubReader(dataReader, ',',
                                            MakeCSVOptional(&s[0], PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                            MakeCSVOptional(&s[1], PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                            MakeCSVOptional(&s[2], PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                            MakeCSVOptional(&s[3], PGESTRING(""), nullptr, PGEUrlDecodeFunc)
                                        ));

                for(uint32_t i = 0; i < 4; i++)
                {
                    if(!IsEmpty(s[i]))
                    {
                        LevelData::MusicOverrider mo;
                        mo.type = LevelData::MusicOverrider::SPECIAL;
                        mo.id = (i + 1);
                        mo.fileName = s[i];
                        FileData.music_overrides.push_back(mo);
                    }
                }
            }
            else
                dataReader.ReadDataLine();
        }
    }
    catch(const std::exception &err)
    {
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(file_version) + " format\n"
                                   "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());
        FileData.meta.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = "";
        return false;
    }
    catch(...)
    {
        /*
         * This is an attempt to fix crash on Windows 32 bit release assembly,
         * and possible, on some other platforms too
         */
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(file_version) + " format\n"
                                   "Caused by unknown exception\n";
        FileData.meta.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = "";
        return false;
    }

    FileData.CurSection = 0;
    FileData.playmusic = 0;
    return true;
    #else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013";
    return false;
    #endif
}

bool FileFormats::ReadSMBX38ALvlFileF(PGESTRING  filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;

    if(!file.open(filePath, false))
    {
        errorString = "Failed to open file for read";
        FileData.meta.ERROR_info = errorString;
        FileData.meta.ERROR_linedata = "";
        FileData.meta.ERROR_linenum = -1;
        FileData.meta.ReadFileValid = false;
        return false;
    }

    return ReadSMBX38Level(file, FileData);
}

bool FileFormats::ReadSMBX38ALvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  LevelData &FileData)
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

    return ReadSMBX38Level(file, FileData);
}

struct LevelEvent_layers
{
    PGESTRING hide;
    PGESTRING show;
    PGESTRING toggle;
};



/**********************************************************************************************/
bool FileFormats::ReadSMBX38ALvlFile(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    SMBX38A_FileBeginN();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    CreateLevelData(FileData);
    FileData.meta.RecentFormat = LevelData::SMBX38A;
    #if !defined(_MSC_VER) || _MSC_VER > 1800
    FileData.LevelName = "" ;
    FileData.stars = 0;
    FileData.CurSection = 0;
    FileData.playmusic = 0;
    //Enable strict mode for SMBX LVL file format
    FileData.meta.smbx64strict = false;
    //Begin all ArrayID's here;
    FileData.blocks_array_id = 1;
    FileData.bgo_array_id = 1;
    FileData.npc_array_id = 1;
    FileData.doors_array_id = 1;
    FileData.physenv_array_id = 1;
    FileData.layers_array_id = 1;
    FileData.events_array_id = 1;
    FileData.layers.clear();
    FileData.events.clear();
    LevelSection section;
    PlayerPoint playerdata;
    LevelBlock blockdata;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor doordata;
    LevelPhysEnv phyEnv;
    LevelLayer layerdata;
    LevelSMBX64Event eventdata;
    //LevelEvent_Sets event_sets;
    LevelVariable vardata;
    LevelScript scriptdata;
    LevelItemSetup38A customcfg;

    PGESTRING   identifier;
    uint32_t    file_version = 0;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);

    try
    {
        CSVPGEReader readerBridge(&in);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        file_version = toUInt(PGE_SubStr(fileIndentifier, 8, -1));

        while(!in.eof())
        {
            identifier = dataReader.ReadField<PGESTRING>(1);

            if(identifier == "A")
            {
                // FIXME: Remove copy from line 77
                // A|param1|param2[|param3|param4]|
                // A|param1|param2|param3|param4|s1,s2,s3,s4
                /*
                    exception: Failed to parse field 4 at line 2
                     exception: Could not convert to unsigned int

                     Field type A
                */
                // 0 1   2                               3  4{}
                // A|0|%4C%61%79%65%72%20%53%70%69%6E%21| |,,,
                PGESTRING s[4];
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        &FileData.stars,
                                        MakeCSVPostProcessor(&FileData.LevelName, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail, PGESTRING(""), nullptr, PGEUrlDecodeFunc),//3
                                        MakeCSVOptionalEmpty(&FileData.open_level_on_fail_warpID, 0u),
                                        MakeCSVOptionalSubReader(dataReader, ',',
                                            MakeCSVOptional(&s[0], PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                            MakeCSVOptional(&s[1], PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                            MakeCSVOptional(&s[2], PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                            MakeCSVOptional(&s[3], PGESTRING(""), nullptr, PGEUrlDecodeFunc)
                                        ));

                for(uint32_t i = 0; i < 4; i++)
                {
                    if(!IsEmpty(s[i]))
                    {
                        LevelData::MusicOverrider mo;
                        mo.type = LevelData::MusicOverrider::SPECIAL;
                        mo.id = (i + 1);
                        mo.fileName = s[i];
                        FileData.music_overrides.push_back(mo);
                    }
                }
            }
            else if(identifier == "P1")
            {
                // P1|x1|y1
                playerdata = CreateLvlPlayerPoint(1);
                dataReader.ReadDataLine(CSVDiscard(), &playerdata.x, &playerdata.y);
                FileData.players.push_back(playerdata);
            }
            else if(identifier == "P2")
            {
                // P2|x2|y2
                // FIXME: Copy from above (can be solved with switch?)
                playerdata = CreateLvlPlayerPoint(2);
                dataReader.ReadDataLine(CSVDiscard(), &playerdata.x, &playerdata.y);
                FileData.players.push_back(playerdata);
            }
            else if(identifier == "M")
            {
                // M|id|x|y|w|h|b1|b2|b3|b4|b5|b6|music|background|musicfile
                section = CreateLvlSection();
                double x = 0.0, y = 0.0, w = 0.0, h = 0.0;
                PGESTRING scroll_lock_x;
                PGESTRING scroll_lock_y;
                dataReader.ReadDataLine(CSVDiscard(),
                                        //id=[1-SectionMAX]
                                        MakeCSVPostProcessor(&section.id, [](int &sectionID)
                {
                    sectionID--;
                    if(sectionID < 0) sectionID = 0;
                }),
                //x=Left size[-left/+right]
                &x,
                //y=Top size[-down/+up]
                &y,
                //w=width of the section[if (w < 800) w = 800]
                &w,//MakeCSVPostProcessor(&w, MakeMinFunc(800.0)),
                //h=height of the section[if (h < 600) h = 600]
                &h,//MakeCSVPostProcessor(&h, MakeMinFunc(600.0)),
                //b1=under water?[0=false !0=true]
                &section.underwater,
                //b2=is x-level wrap[0=false !0=true]
                &section.wrap_h,
                //b3=enable off screen exit[0=false !0=true]
                &section.OffScreenEn,
                //b4=no turn back(x)[0=no x-scrolllock 1=scrolllock left 2=scrolllock right]
                &scroll_lock_x,
                //b5=no turn back(y)[0=no y-scrolllock 1=scrolllock up 2=scrolllock down]
                &scroll_lock_y,
                //b6=is y-level wrap[0=false !0=true]
                &section.wrap_v,
                //music=music number[same as smbx1.3]
                &section.music_id,
                //background=background number[same as the filename in 'background2' folder]
                &section.background,
                //musicfile=custom music file[***urlencode!***]
                MakeCSVPostProcessor(&section.music_file, PGEUrlDecodeFunc));
                SMBX38A_mapBGID_From(section.background);//Convert into SMBX64 ID set
                section.lock_left_scroll =  (scroll_lock_x == "1");
                section.lock_right_scroll = (scroll_lock_x == "2");
                section.lock_up_scroll =    (scroll_lock_y == "1");
                section.lock_down_scroll =  (scroll_lock_y == "2");

                if((x != 0.0) || (y != 0.0) || (w != 0.0) || (h != 0.0))
                {
                    section.size_left = static_cast<long>(round(x));
                    section.size_top = static_cast<long>(round(y));
                    section.size_right = static_cast<long>(round(x + w));
                    section.size_bottom = static_cast<long>(round(y + h));
                }

                //Very important data! I'ts a camera position in the editor!
                section.PositionX = section.size_left - 10;
                section.PositionY = section.size_top - 10;

                if(section.id < static_cast<signed>(FileData.sections.size()))
                    FileData.sections[static_cast<pge_size_t>(section.id)] = section;//Replace if already exists
                else
                    FileData.sections.push_back(section); //Add Section in main array
            }
            else if(identifier == "B")
            {
                // B|layer[,name]|id[,dx,dy]|x|y|contain|b11[,b12]|b2|[e1,e2,e3,e4]|w|h
                blockdata = CreateLvlBlock();
                dataReader.ReadDataLine(CSVDiscard(),
                MakeCSVSubReader(dataReader, ',',
                        //layer=layer name["" == "Default"][***urlencode!***]
                        MakeCSVPostProcessor(&blockdata.layer, PGELayerOrDefault),
                        //name=block's name[***urlencode!***]
                        MakeCSVOptional(&blockdata.gfx_name, "")
                                ),
                MakeCSVSubReader(dataReader, ',',
                        //id=block id
                        &blockdata.id,
                        //dx=graphics extend x
                        MakeCSVOptional(&blockdata.gfx_dx, 0),
                        //dy=graphics extend y
                        MakeCSVOptional(&blockdata.gfx_dy, 0)
                                ),
                //x=block position x
                &blockdata.x, //FIXME rounding error?
                //y=block position y
                &blockdata.y,
                /* contain=containing npc number
                                [1001-1000+NPCMAX] npc-id
                                [1-999] coin number
                                [0] nothing
                */
                MakeCSVOptionalEmpty(&blockdata.npc_id, 0, nullptr, [](long & npcValue)
                {
                    npcValue = (npcValue < 1000 ? -1 * npcValue : npcValue - 1000);
                }),
                MakeCSVSubReader(dataReader, ',',
                        //b11=slippery[0=false !0=true]
                        &blockdata.slippery,
                        //b12=wing type
                        MakeCSVOptional(&blockdata.motion_ai_id, 0)
                        ),
                //b2=invisible[0=false !0=true]
                &blockdata.invisible,
                MakeCSVSubReader(dataReader, ',',
                                 //e1=block destory event name[***urlencode!***]
                                 MakeCSVPostProcessor(&blockdata.event_destroy, PGEUrlDecodeFunc),
                                 //e2=block hit event name[***urlencode!***]
                                 MakeCSVPostProcessor(&blockdata.event_hit, PGEUrlDecodeFunc),
                                 //e3=no more object in layer event name[***urlencode!***]
                                 MakeCSVPostProcessor(&blockdata.event_emptylayer, PGEUrlDecodeFunc),
                                 //e4=block onscreen event name[***urlencode!***]
                                 MakeCSVOptional(&blockdata.event_on_screen, "", nullptr, PGEUrlDecodeFunc)
                                ),
                //w=width, if w < 0 then block's autoscale = true
                &blockdata.w,
                //h=height
                &blockdata.h);
                blockdata.autoscale = (blockdata.w < 0);
                if(blockdata.w < 0)
                    blockdata.w *= -1;
                blockdata.meta.array_id = FileData.blocks_array_id++;
                FileData.blocks.push_back(blockdata);
            }
            else if(identifier == "T")
            {
                // T|layer|id[,dx,dy]|x|y
                bgodata = CreateLvlBgo();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&bgodata.layer, PGELayerOrDefault),
                                        MakeCSVSubReader(dataReader, ',',
                                                &bgodata.id,
                                                MakeCSVOptional(&bgodata.gfx_dx, 0),
                                                MakeCSVOptional(&bgodata.gfx_dy, 0)
                                                        ),
                                        &bgodata.x,
                                        &bgodata.y);
                bgodata.meta.array_id = FileData.bgo_array_id++;
                FileData.bgo.push_back(bgodata);
            }
            else if(identifier == "N")
            {
                // N|layer[,name]|id[,dx,dy]|x|y|b1,b2,b3,b4|sp|[e1,e2,e3,e4,e5,e6,e7]|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
                // N|layer[,name]|id[,dx,dy]|x|y|b1,b2,b3,b4|sp|[e1,e2,e3,e4,e5,e6,e7]|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
                npcdata = CreateLvlNpc();
                npcdata.generator_period_orig_unit = PGE_FileLibrary::TimeUnit::FrameOneOf65sec;
                double specialData;
                int genType; // We have to handle that later :(
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                MakeCSVPostProcessor(&npcdata.layer, PGELayerOrDefault),
                                                MakeCSVOptional(&npcdata.gfx_name, "")
                                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                &npcdata.id,
                                                MakeCSVOptional(&npcdata.gfx_dx, 0),
                                                MakeCSVOptional(&npcdata.gfx_dy, 0)
                                                        ),
                                        &npcdata.x,
                                        &npcdata.y,
                                        MakeCSVSubReader(dataReader, ',',
                                                MakeCSVPostProcessor(&npcdata.direct, [](int &value)
                {
                    value = value * -1;
                }),
                &npcdata.friendly,
                &npcdata.nomove,
                &npcdata.contents),
                &specialData,
                MakeCSVSubReader(dataReader, ',',
                                 MakeCSVOptional(&npcdata.event_die, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptional(&npcdata.event_talk, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptional(&npcdata.event_activate, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptional(&npcdata.event_emptylayer, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptional(&npcdata.event_grab, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptional(&npcdata.event_nextframe, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptional(&npcdata.event_touch, "", nullptr, PGEUrlDecodeFunc)
                                ),
                MakeCSVSubReader(dataReader, ',',
                                 MakeCSVOptionalEmpty(&npcdata.attach_layer, "", nullptr, PGEUrlDecodeFunc),
                                 MakeCSVOptionalEmpty(&npcdata.send_id_to_variable, "", nullptr, PGEUrlDecodeFunc)
                                ),
                MakeCSVSubReader(dataReader, ',',
                                 &npcdata.generator,
                                 MakeCSVOptional(&npcdata.generator_period_orig, 65),
                                 MakeCSVOptional(&genType, 0),
                                 MakeCSVOptional(&npcdata.generator_custom_angle, 0.0),
                                 MakeCSVOptional(&npcdata.generator_branches, 1),
                                 MakeCSVOptional(&npcdata.generator_angle_range, 360.0),
                                 MakeCSVOptional(&npcdata.generator_initial_speed, 10.0)
                                ),
                MakeCSVPostProcessor(&npcdata.msg, PGEUrlDecodeFunc)
                                       );

                if(npcdata.contents > 0)
                {
                    long contID = npcdata.contents;
                    npcdata.contents = static_cast<long>(npcdata.id);
                    //b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300][6=npc347]
                    static const uint64_t ContNPCID[] =
                    {
                        //  1  2    3    4    5    6
                        0, 91, 96, 283, 284, 300, 347, 0
                    };
                    if((contID > 0) && (contID <= 6))
                        npcdata.id = ContNPCID[contID];
                    else
                        npcdata.contents = 0; //Invalid container type

                    if((contID > 101) && (contID <= 107))
                        npcdata.meta.custom_params += "\"wings:\"" + fromNum(contID) + ",";
                }

                npcdata.special_data = static_cast<long>(round(specialData));

                switch(npcdata.id)
                {
                case 15:
                case 39:
                case 86: //Bind "Is Boss" flag for supported NPC's
                    npcdata.is_boss = static_cast<bool>(npcdata.special_data);
                    npcdata.special_data = 0;
                    break;
                default:
                    break;
                }

                switch(genType)
                {
                case 0:
                    npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                    npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                    break;
                default:
                    if(genType < 29)
                    {
                        npcdata.generator_type   = SMBX38A_NpcGeneratorTypes[genType];
                        npcdata.generator_direct = SMBX38A_NpcGeneratorDirections[genType];
                    }
                    else
                    {
                        npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                        npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                    }
                }

                //Convert value into SMBX64 and PGEX compatible
                switch(npcdata.generator_type)
                {
                case 0:
                    npcdata.generator_type = LevelNPC::NPC_GENERATPR_PROJECTILE;
                    break;
                case 1:
                    npcdata.generator_type = LevelNPC::NPC_GENERATOR_WARP;
                    break;
                case 4:
                    npcdata.generator_type = LevelNPC::NPC_GENERATOR_APPEAR;
                    break;
                }

                npcdata.generator_period = PGE_FileLibrary::TimeUnitsCVT(static_cast<int>(npcdata.generator_period_orig),
                                           PGE_FileLibrary::TimeUnit::FrameOneOf65sec,
                                           PGE_FileLibrary::TimeUnit::Decisecond);
                npcdata.meta.array_id = FileData.npc_array_id++;
                FileData.npc.push_back(npcdata);
            }
            else if(identifier == "Q")
            {
                // Q|layer|x|y|w|h|b1,b2,b3,b4,b5|event
                phyEnv = CreateLvlPhysEnv();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&phyEnv.layer, PGELayerOrDefault),
                                        &phyEnv.x,
                                        &phyEnv.y,
                                        &phyEnv.w,
                                        &phyEnv.h,
                                        MakeCSVSubReader(dataReader, ',',
                                                MakeCSVPostProcessor(&phyEnv.env_type, [](int &value)
                {
                    value--;
                }),
                &phyEnv.friction,
                &phyEnv.accel_direct,
                &phyEnv.accel,
                &phyEnv.accel),
                MakeCSVPostProcessor(&phyEnv.touch_event, PGEUrlDecodeFunc)
                                       );
                phyEnv.meta.array_id = FileData.physenv_array_id++;
                FileData.physez.push_back(phyEnv);
            }
            else if(identifier == "W")
            {
                // W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc,mini,size|lik|liid|noexit|wx|wy|le|we
                doordata = CreateLvlWarp();
                int type = 0;
                dataReader.ReadDataLine(CSVDiscard(),
                                        //layer=layer name["" == "Default"][***urlencode!***]
                                        MakeCSVPostProcessor(&doordata.layer, PGELayerOrDefault),
                                        //x=entrance position x
                                        &doordata.ix,
                                        //y=entrance postion y
                                        &doordata.iy,
                                        //ex=exit position x
                                        &doordata.ox,
                                        //ey=exit position y
                                        &doordata.oy,
                                        //type=[1=pipe][2=door][0=instant][3=portal/loop]
                                        &type,
                                        //enterd=entrance direction[1=up 2=left 3=down 4=right]
                                        &doordata.idirect,
                                        //exitd=exit direction[1=up 2=left 3=down 4=right]
                                        MakeCSVPostProcessor(&doordata.odirect, [](int &value)
                {
                    switch(value)//Convert into SMBX64/PGE-X Compatible form
                    {
                    case 1:
                        value = LevelDoor::EXIT_UP;
                        break;
                    case 2:
                        value = LevelDoor::EXIT_LEFT;
                        break;
                    case 3:
                        value = LevelDoor::EXIT_DOWN;
                        break;
                    case 4:
                        value = LevelDoor::EXIT_RIGHT;
                        break;
                    }
                }),
                MakeCSVSubReader(dataReader, ',',
                                 //sn=need stars for enter
                                 &doordata.stars,
                                 //msg=a message when you have not enough stars
                                 MakeCSVPostProcessor(&doordata.stars_msg, PGEUrlDecodeFunc),
                                 //hide=hide the star number in this warp
                                 &doordata.star_num_hide),
                MakeCSVSubReader(dataReader, ',',
                                 //locked=locked
                                 &doordata.locked,
                                 //noyoshi=no yoshi
                                 &doordata.novehicles,
                                 //canpick=allow npc
                                 &doordata.allownpc,
                                 //bomb=need a bomb
                                 &doordata.need_a_bomb,
                                 //hide=hide the entry scene
                                 &doordata.hide_entering_scene,
                                 //anpc=allow npc interlevel
                                 &doordata.allownpc_interlevel,
                                 //mini=Mini-Only
                                 MakeCSVOptional(&doordata.special_state_required, false),
                                 //size=Warp Size(pixel)
                                 MakeCSVOptional(&doordata.length_i, 32u),
                                 MakeCSVOptional(&doordata.two_way, false),
                                 MakeCSVOptional(&doordata.cannon_exit_speed, 0.0)
                                ),
                                //lik=warp to level[***urlencode!***]
                                MakeCSVPostProcessor(&doordata.lname, PGEUrlDecodeFunc),
                                //liid=normal enterance / to warp[0-WARPMAX]
                                &doordata.warpto,
                                //noexit=level entrance
                                &doordata.lvl_i,
                                //wx=warp to x on world map
                                &doordata.world_x,
                                //wy=warp to y on world map
                                &doordata.world_y,
                                //le=level exit
                                MakeCSVOptional(&doordata.lvl_o, false),
                                //we=warp event[***urlencode!***]
                                MakeCSVOptional(&doordata.event_enter, "", nullptr, PGEUrlDecodeFunc)
                            );
                // type%100=[0=instant][1=pipe][2=door][3=loop]
                doordata.type = type % 100;
                // type/100=[0=none][1=Scroll][2=Fade]
                doordata.transition_effect = type / 100;
                doordata.length_o = doordata.length_i;
                doordata.isSetIn = (doordata.lvl_i ? false : true);
                doordata.isSetOut = (doordata.lvl_o ? false : true) || doordata.lvl_i;
                doordata.cannon_exit = (doordata.cannon_exit_speed > 0.0);
                if(doordata.cannon_exit_speed <= 0)
                    doordata.cannon_exit_speed = 10.0;
                doordata.meta.array_id = FileData.doors_array_id++;
                FileData.doors.push_back(doordata);
            }
            else if(identifier == "L")
            {
                // L|name|status
                layerdata = CreateLvlLayer();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&layerdata.name, PGELayerOrDefault),
                                        MakeCSVPostProcessor(&layerdata.hidden, PGEFilpBool)
                                       );
                layerdata.meta.array_id = FileData.layers_array_id++;
                FileData.layers.push_back(layerdata);
            }
            else if(identifier == "E")
            {
                // E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
                eventdata = CreateLvlEvent();
                // Here we can just align the section id with the index of the set
                // It is an unsafe method, however, we should be safe when reading from the file, where the data object is empty.
                eventdata.sets.clear();

                for(int q = 0; q < static_cast<signed>(FileData.sections.size()); q++)
                {
                    LevelEvent_Sets set;
                    set.id = static_cast<long>(q);
                    eventdata.sets.push_back(set);
                }

                // Temp Field 11
                double timer_def_interval_raw;
                // This variable is used for the spawn npc section.
                // The first two values are static ones, after that they come in packages (see below)
                int spawnNpcReaderCurrentIndex = 0;
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&eventdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&eventdata.msg, PGEUrlDecodeFunc),
                                        MakeCSVSubReader(dataReader, ',',
                                                &eventdata.autostart,
                                                MakeCSVPostProcessor(&eventdata.autostart_condition, PGEUrlDecodeFunc)
                                                        ),
                                        MakeCSVSubReader(dataReader, '/',
                                                &eventdata.nosmoke,
                                                MakeCSVBatchReader(dataReader, ',', &eventdata.layers_show, PGEUrlDecodeFunc),
                                                MakeCSVBatchReader(dataReader, ',', &eventdata.layers_hide, PGEUrlDecodeFunc),
                                                MakeCSVBatchReader(dataReader, ',', &eventdata.layers_toggle, PGEUrlDecodeFunc)
                                                        ),
                                        MakeCSVIterator(dataReader, '/', [&eventdata](const PGESTRING & nextFieldStr)
                {
                    auto fieldReader = MakeDirectReader(nextFieldStr);
                    auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                    LevelEvent_MoveLayer movingLayer;
                    fullReader.ReadDataLine(MakeCSVPostProcessor(&movingLayer.name, PGEUrlDecodeFunc),
                                            MakeCSVPostProcessor(&movingLayer.expression_x, PGEUrlDecodeFunc),
                                            MakeCSVPostProcessor(&movingLayer.expression_y, PGEUrlDecodeFunc),
                                            &movingLayer.way
                                           );
                    SMBX38A_Exp2Double(movingLayer.expression_x, movingLayer.speed_x);
                    SMBX38A_Exp2Double(movingLayer.expression_y, movingLayer.speed_y);
                    eventdata.moving_layers.push_back(movingLayer);
                    eventdata.movelayer = movingLayer.name;
                    eventdata.layer_speed_x = movingLayer.speed_x;
                    eventdata.layer_speed_y = movingLayer.speed_y;
                }),
                MakeCSVSubReader(dataReader, ',',
                                 &eventdata.ctrls_enable,
                                 &eventdata.ctrl_drop,
                                 &eventdata.ctrl_altrun,
                                 &eventdata.ctrl_run,
                                 &eventdata.ctrl_jump,
                                 &eventdata.ctrl_altjump,
                                 &eventdata.ctrl_up,
                                 &eventdata.ctrl_down,
                                 &eventdata.ctrl_left,
                                 &eventdata.ctrl_right,
                                 &eventdata.ctrl_start,
                                 &eventdata.ctrl_lock_keyboard
                                ),
                MakeCSVSubReader(dataReader, '/',
                                 MakeCSVIterator(dataReader, ':', [&eventdata](const PGESTRING & nextFieldStr)
                {
                    auto fieldReader = MakeDirectReader(nextFieldStr);
                    auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                    int sectionID = fullReader.ReadField<int>(1) - 1;
                    LevelEvent_Sets &nextSet = eventdata.sets[static_cast<pge_size_t>(sectionID)];
                    bool customSize = false;
                    bool canAutoscroll = false;
                    fullReader.ReadDataLine(CSVDiscard(),
                                            MakeCSVPostProcessor(&nextSet.position_left, [&customSize](long & value)
                    {
                        switch(value)
                        {
                        case 0:
                            value = LevelEvent_Sets::LESet_Nothing;
                            break;

                        case 1:
                            value = LevelEvent_Sets::LESet_ResetDefault;
                            break;

                        case 2:
                            customSize = true;
                            value = 0;
                            break;
                        }
                    }),
                    MakeCSVPostProcessor(&nextSet.expression_pos_x, PGEUrlDecodeFunc),
                    MakeCSVPostProcessor(&nextSet.expression_pos_y, PGEUrlDecodeFunc),
                    MakeCSVPostProcessor(&nextSet.expression_pos_w, PGEUrlDecodeFunc),
                    MakeCSVPostProcessor(&nextSet.expression_pos_h, PGEUrlDecodeFunc),
                    MakeCSVPostProcessor(&nextSet.autoscrol, [&canAutoscroll](bool & value)
                    {
                        canAutoscroll = value;
                    }),
                    MakeCSVPostProcessor(&nextSet.expression_autoscrool_x, PGEUrlDecodeFunc),
                    MakeCSVPostProcessor(&nextSet.expression_autoscrool_y, PGEUrlDecodeFunc)
                                           );

                    if(customSize)
                    {
                        SMBX38A_Exp2Int(nextSet.expression_pos_x, nextSet.position_left);
                        SMBX38A_Exp2Int(nextSet.expression_pos_y, nextSet.position_top);
                        SMBX38A_Exp2Int(nextSet.expression_pos_w, nextSet.position_right);
                        SMBX38A_Exp2Int(nextSet.expression_pos_h, nextSet.position_bottom);

                        if(IsEmpty(nextSet.expression_pos_w))
                            nextSet.position_right += nextSet.position_left;

                        if(IsEmpty(nextSet.expression_pos_h))
                            nextSet.position_bottom += nextSet.position_top;
                    }

                    if(canAutoscroll)
                    {
                        SMBX38A_Exp2Float(nextSet.expression_autoscrool_x, nextSet.autoscrol_x);
                        SMBX38A_Exp2Float(nextSet.expression_autoscrool_y, nextSet.autoscrol_y);
                        //SMBX64 backwarth compatibility:
                        eventdata.scroll_section = nextSet.id;//Set ID of autoscrollable section :-P
                        eventdata.move_camera_x = static_cast<double>(nextSet.autoscrol_x);
                        eventdata.move_camera_y = static_cast<double>(nextSet.autoscrol_y);
                    }
                    else
                    {
                        nextSet.autoscrol_x = 0.f;
                        nextSet.autoscrol_y = 0.f;
                        // Doesn't even make sense:
                        // eventdata.move_camera_x = 0.f;
                        // eventdata.move_camera_y = 0.f;
                    }

                    eventdata.scroll_section = static_cast<long>(sectionID);
                }),
                MakeCSVIterator(dataReader, ':', [&eventdata](const PGESTRING & nextFieldStr)
                {
                    auto fieldReader = MakeDirectReader(nextFieldStr);
                    auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                    int sectionID = fullReader.ReadField<int>(1) - 1;
                    LevelEvent_Sets &nextSet = eventdata.sets[static_cast<pge_size_t>(sectionID)];
                    bool customBG = false;
                    long bgID = 0;
                    fullReader.ReadDataLine(CSVDiscard(),
                                            MakeCSVPostProcessor(&nextSet.background_id, [&customBG](long & value)
                    {
                        switch(value)
                        {
                        case 0:
                            value = LevelEvent_Sets::LESet_Nothing;
                            break;

                        case 1:
                            value = LevelEvent_Sets::LESet_ResetDefault;
                            break;

                        case 2:
                            customBG = true;
                            value = 0;
                            break;
                        }
                    }),
                    &bgID
                                           );

                    if(customBG)
                        nextSet.background_id = bgID;

                    SMBX38A_mapBGID_From(nextSet.background_id);//Convert into SMBX64 ID set
                }),
                MakeCSVIterator(dataReader, ':', [&eventdata](const PGESTRING & nextFieldStr)
                {
                    auto fieldReader = MakeDirectReader(nextFieldStr);
                    auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                    int sectionID = fullReader.ReadField<int>(1) - 1;
                    LevelEvent_Sets &nextSet = eventdata.sets[static_cast<pge_size_t>(sectionID)];
                    bool customMusic = false;
                    long music_id;
                    fullReader.ReadDataLine(CSVDiscard(),
                                            MakeCSVPostProcessor(&nextSet.music_id, [&customMusic](long & value)
                    {
                        switch(value)
                        {
                        case 0:
                            value = LevelEvent_Sets::LESet_Nothing;
                            break;

                        case 1:
                            value = LevelEvent_Sets::LESet_ResetDefault;
                            break;

                        default:
                        case 2:
                            customMusic = true;
                            value = 0;
                            break;
                        }
                    }),
                    &music_id,
                    MakeCSVOptional(&nextSet.music_file, "", nullptr, PGEUrlDecodeFunc)
                                           );

                    if(customMusic)
                        nextSet.music_id = music_id;
                })
                                ),
                MakeCSVIterator(dataReader, '/', [&eventdata, &spawnNpcReaderCurrentIndex](const PGESTRING & nextFieldStr)
                {
                    switch(spawnNpcReaderCurrentIndex)
                    {
                    case 0:
                        if(!SMBX64::IsUInt(nextFieldStr))
                            throw std::invalid_argument("Cannot convert field 1 to int.");

                        eventdata.sound_id = toLong(nextFieldStr);
                        spawnNpcReaderCurrentIndex++;
                        break;

                    case 1:
                        if(!SMBX64::IsUInt(nextFieldStr))
                            throw std::invalid_argument("Cannot convert field 2 to int.");

                        eventdata.end_game = toLong(nextFieldStr);
                        spawnNpcReaderCurrentIndex++;
                        break;

                    default:
                        auto fieldReader = MakeDirectReader(nextFieldStr);
                        auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                        LevelEvent_SpawnEffect effect;
                        fullReader.ReadDataLine(&effect.id,
                                                MakeCSVPostProcessor(&effect.expression_x, PGEUrlDecodeFunc),
                                                MakeCSVPostProcessor(&effect.expression_y, PGEUrlDecodeFunc),
                                                MakeCSVPostProcessor(&effect.expression_sx, PGEUrlDecodeFunc),
                                                MakeCSVPostProcessor(&effect.expression_sy, PGEUrlDecodeFunc),
                                                &effect.gravity,
                                                &effect.fps,
                                                &effect.max_life_time
                                               );
                        SMBX38A_Exp2Int(effect.expression_x, effect.x);
                        SMBX38A_Exp2Int(effect.expression_y, effect.y);
                        SMBX38A_Exp2Double(effect.expression_sx, effect.speed_x);
                        SMBX38A_Exp2Double(effect.expression_sy, effect.speed_y);
                        eventdata.spawn_effects.push_back(effect);
                        break;
                    }
                }),
                // &effects,
                MakeCSVIterator(dataReader, '/', [&eventdata](const PGESTRING & nextFieldStr)
                {
                    auto fieldReader = MakeDirectReader(nextFieldStr);
                    auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                    LevelEvent_SpawnNPC spawnnpc;
                    fullReader.ReadDataLine(&spawnnpc.id,
                                            MakeCSVPostProcessor(&spawnnpc.expression_x, PGEUrlDecodeFunc),
                                            MakeCSVPostProcessor(&spawnnpc.expression_y, PGEUrlDecodeFunc),
                                            MakeCSVPostProcessor(&spawnnpc.expression_sx, PGEUrlDecodeFunc),
                                            MakeCSVPostProcessor(&spawnnpc.expression_sy, PGEUrlDecodeFunc),
                                            &spawnnpc.special
                                           );
                    SMBX38A_Exp2Int(spawnnpc.expression_x, spawnnpc.x);
                    SMBX38A_Exp2Int(spawnnpc.expression_y, spawnnpc.y);
                    SMBX38A_Exp2Double(spawnnpc.expression_sx, spawnnpc.speed_x);
                    SMBX38A_Exp2Double(spawnnpc.expression_sy, spawnnpc.speed_y);
                    eventdata.spawn_npc.push_back(spawnnpc);
                }),
                // &spawn_npcs,
                MakeCSVIterator(dataReader, '/', [&eventdata](const PGESTRING & nextFieldStr)
                {
                    auto fieldReader = MakeDirectReader(nextFieldStr);
                    auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                    LevelEvent_UpdateVariable updVar;
                    fullReader.ReadDataLine(MakeCSVPostProcessor(&updVar.name, PGEUrlDecodeFunc),
                                            MakeCSVPostProcessor(&updVar.newval, PGEUrlDecodeFunc)
                                           );
                    eventdata.update_variable.push_back(updVar);
                }),
                // &update_var,
                MakeCSVSubReader(dataReader, '/',
                                 MakeCSVSubReader(dataReader, ',',
                                                  MakeCSVPostProcessor(&eventdata.trigger, PGEUrlDecodeFunc),
                                                  &eventdata.trigger_timer_orig
                                                 ),
                                 MakeCSVSubReader(dataReader, ',',
                                                  &eventdata.timer_def.enable,
                                                  &eventdata.timer_def.count,
                                                  &timer_def_interval_raw,
                                                  &eventdata.timer_def.count_dir,
                                                  &eventdata.timer_def.show),
                                 &eventdata.trigger_api_id,
                                 MakeCSVPostProcessor(&eventdata.trigger_script, PGEUrlDecodeFunc)
                                )
                                       );
                eventdata.trigger_timer_unit = PGE_FileLibrary::TimeUnit::FrameOneOf65sec;
                eventdata.trigger_timer = PGE_FileLibrary::TimeUnitsCVT(eventdata.trigger_timer_orig,
                                          PGE_FileLibrary::TimeUnit::FrameOneOf65sec,
                                          PGE_FileLibrary::TimeUnit::Decisecond);
                eventdata.timer_def.interval = PGE_FileLibrary::TimeUnitsCVT(timer_def_interval_raw,
                                               PGE_FileLibrary::TimeUnit::FrameOneOf65sec,
                                               PGE_FileLibrary::TimeUnit::Millisecond);
                eventdata.meta.array_id = FileData.events_array_id++;
                FileData.events.push_back(eventdata);
            }
            else if(identifier == "V")
            {
                // V|name|value
                vardata = CreateLvlVariable("var");
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&vardata.name, PGEUrlDecodeFunc),
                                        &vardata.value /* save variable value as string
                                                          because in PGE is planned to have
                                                          variables to be universal */
                                       );
                FileData.variables.push_back(vardata);
            }
            else if(identifier == "S")
            {
                // S|name|script
                scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&scriptdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&scriptdata.script, PGEBase64DecodeFunc)
                                       );
                FileData.scripts.push_back(scriptdata);
            }
            else if(identifier == "Su")
            {
                // Su|name|scriptu
                scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&scriptdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&scriptdata.script, PGEBase64DecodeFuncA)
                                       );
                //Convert to LF
                PGE_ReplSTRING(scriptdata.script, "\r\n", "\n");
                FileData.scripts.push_back(scriptdata);
            }
            else if((identifier == "CB") || (identifier == "CT") || (identifier == "CE") )
            {
                // CB|id|data   :custom block/background/effect
                customcfg = LevelItemSetup38A();
                if(identifier == "CB")
                    customcfg.type = LevelItemSetup38A::BLOCK;
                else if(identifier == "CT")
                    customcfg.type = LevelItemSetup38A::BGO;
                else
                    customcfg.type = LevelItemSetup38A::EFFECT;

                dataReader.ReadDataLine(CSVDiscard(),
                                        &customcfg.id,
                                        MakeCSVIterator(dataReader, ',',
                                                        [&customcfg](const PGESTRING & nextFieldStr)
                {
                    LevelItemSetup38A::Entry e;
                    SMBX38A_CC_decode(e.key, e.value, nextFieldStr);
                    customcfg.data.push_back(e);
                })
                                       );
                FileData.custom38A_configs.push_back(customcfg);
            }
            else if(identifier == "CW")
            {
                // CW|cdata1|cdata2|...|cdatan	:custom sound:	same as wls file format
                dataReader.IterateDataLine([&FileData](const PGESTRING & nextFieldStr)
                    {
                        if(nextFieldStr == "CW")
                            return;

                        auto fieldReader = MakeDirectReader(nextFieldStr);
                        auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                        LevelData::MusicOverrider mo;
                        fullReader.ReadDataLine(&mo.id,
                                                MakeCSVPostProcessor(&mo.fileName, PGEUrlDecodeFunc)
                                               );
                        FileData.sound_overrides.push_back(mo);
                    });
            }
            else
                dataReader.ReadDataLine();
        }//while is not EOF
    }
    catch(const std::exception &err)
    {
        // First we try to extract the line number out of the nested exception.
        const std::exception *curErr = &err;
        const std::nested_exception *possibleNestedException = dynamic_cast<const std::nested_exception *>(curErr);

        if(possibleNestedException)
        {
            try
            {
                std::rethrow_exception(possibleNestedException->nested_ptr());
            }
            catch(const parse_error &parseErr)
            {
                FileData.meta.ERROR_linenum = static_cast<long>(parseErr.get_line_number());
            }
            catch(...)
            {
                // Do Nothing
            }
        }

        // Now fill in the error data.
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(file_version) + " format\n"
                                   "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());
        if(!IsEmpty(identifier))
            FileData.meta.ERROR_info += "\n Field type " + identifier;

        // If we were unable to find error line number from the exception, then get the line number from the file reader.
        if(FileData.meta.ERROR_linenum == 0)
            FileData.meta.ERROR_linenum = in.getCurrentLineNumber();

        FileData.meta.ERROR_linedata = "";
        return false;
    }
    catch(...)
    {
        /*
         * This is an attempt to fix crash on Windows 32 bit release assembly,
         * and possible, on some other platforms too
         */
        // Now fill in the error data.
        FileData.meta.ReadFileValid = false;
        FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(file_version) + " format\n"
                                   "Caused by unknown exception\n";
        if(!IsEmpty(identifier))
            FileData.meta.ERROR_info += "\n Field type " + identifier;
        // If we were unable to find error line number from the exception, then get the line number from the file reader.
        if(FileData.meta.ERROR_linenum == 0)
            FileData.meta.ERROR_linenum = in.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = "";
        return false;
    }

    LevelAddInternalEvents(FileData);
    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    return true;
    #else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013";
    return false;
    #endif
}


//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX38ALvlFileF(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }

    return WriteSMBX38ALvlFile(file, FileData);
}

bool FileFormats::WriteSMBX38ALvlFileRaw(LevelData &FileData, PGESTRING &rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open raw string for write";
        return false;
    }

    return WriteSMBX38ALvlFile(file, FileData);
}

bool FileFormats::WriteSMBX38ALvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData &FileData)
{
    pge_size_t i = 0;
    FileData.meta.RecentFormat = LevelData::SMBX38A;
    //Count placed stars on this level
    FileData.stars = smbx64CountStars(FileData);
#define layerNotDef(lr) ( ((lr) != "Default") ? PGE_URLENC(lr) : "" )
    //========================================================
    //Data type markers:
    //A         – Level header settings
    //P1, P2    – Player spawn points
    //M       – Section settings
    //B       – blocks
    //T       – Background objects
    //N       – Non-playable characters
    //Q       – Liquid/Environment boxes
    //W       – Warp entries
    //L       – Layers
    //E       – Events
    //V       – Local level variables
    //S       – UTF-8 encoded local level scripts
    //Su      – ASCII-encoded local level scripts
    //--------------------------------------------------------
    //line 1:
    //    SMBXFile??
    //    ??=Version number
    out << "SMBXFile" << fromNum(newest_file_format) << "\n";
    //next line: level settings
    //    A|param1|param2[|param3|param4]
    //    []=optional
    out << "A";
    //    param1=the number of stars on this level
    out << "|" << fromNum(FileData.stars);
    //    param2=level title
    out << "|" << PGE_URLENC(FileData.LevelName);

    if(!IsEmpty(FileData.open_level_on_fail))
    {
        //    param3=a filename, when player died, the player will be sent to this level.
        out << "|" << PGE_URLENC(FileData.open_level_on_fail);
        //    param4=normal entrance / to warp [0-WARPMAX]
        out << "|" << fromNum(FileData.open_level_on_fail_warpID);
    } else {
        out << "|||";
    }
    //Custom special musics
    {
        //s1=P-Switch Music Filename[***urlencode!***]
        //s2=Stopwatch Music Filename[***urlencode!***]
        //s3=Starman Music Filename[***urlencode!***]
        //s4=MegaMushroom Music Filename[***urlencode!***]
        PGESTRING s[4];
        for(pge_size_t i = 0; i < FileData.music_overrides.size(); i++)
        {
            LevelData::MusicOverrider &mo = FileData.music_overrides[i];
            if(mo.type == LevelData::MusicOverrider::SPECIAL)
            {
                if(mo.id < 4)
                    s[i] = mo.fileName;
            }
        }

        for(int i = 0; i < 4; i++)
        {
            if(i > 0)
                out << ",";
            out << PGE_URLENC(s[i]);
        }
    }

    out << "\n";

    //next line: player start points
    for(i = 0; i < FileData.players.size(); i++)
    {
        //    P1|x1|y1
        //    P2|x2|y2
        PlayerPoint &pl = FileData.players[i];
        out << "P" << fromNum(pl.id);
        //    x1=first player position x
        //    x2=second player position x
        out << "|" << fromNum(pl.x);
        //    y1=first player position y
        //    y2=second player position y
        out << "|" << fromNum(pl.y);
        out << "\n";
    }

    //next line: section properties
    for(i = 0; i < FileData.sections.size(); i++)
    {
        //    M|id|x|y|w|h|b1|b2|b3|b4|b5|b6|music|background|musicfile
        LevelSection &sct = FileData.sections[i];
        out << "M";
        //    id=[1-SectionMAX]
        out << "|" << fromNum(sct.id + 1);
        //    x=Left size[-left/+right]
        out << "|" << fromNum(sct.size_left);
        //    y=Top size[-down/+up]
        out << "|" << fromNum(sct.size_top);
        //    w=width of the section[if (w < 800) w = 800]
        out << "|" << fromNum(sct.size_right - sct.size_left);
        //    h=height of the section[if (h < 600) h = 600]
        out << "|" << fromNum(sct.size_bottom - sct.size_top);
        //    b1=under water?[0=false !0=true]
        out << "|" << fromNum((int)sct.underwater);
        //    b2=is x-level wrap[0=false !0=true]
        out << "|" << fromNum(sct.wrap_h);
        //    b3=enable off screen exit[0=false !0=true]
        out << "|" << fromNum((int)sct.OffScreenEn);

        //    b4=no turn back(x)[0=no x-scrolllock 1=scrolllock left 2=scrolllock right]
        if((!sct.lock_left_scroll) && (!sct.lock_right_scroll))
            out << "|" << fromNum(0);
        else if((sct.lock_left_scroll) && (!sct.lock_right_scroll))
            out << "|" << fromNum(1);
        else
            out << "|" << fromNum(2);

        //    b5=no turn back(y)[0=no y-scrolllock 1=scrolllock up 2=scrolllock down]
        if((!sct.lock_up_scroll) && (!sct.lock_down_scroll))
            out << "|" << fromNum(0);
        else if((sct.lock_up_scroll) && (!sct.lock_down_scroll))
            out << "|" << fromNum(1);
        else
            out << "|" << fromNum(2);

        //    b6=is y-level wrap[0=false !0=true]
        out << "|" << fromNum(sct.wrap_v);
        //    music=music number[same as smbx1.3]
        out << "|" << fromNum(sct.music_id);
        //    background=background number[same as the filename in 'background2' folder]
        out << "|" << fromNum(SMBX38A_mapBGID_To(sct.background));
        //    musicfile=custom music file[***urlencode!***]
        out << "|" << PGE_URLENC(sct.music_file);
        out << "\n";
    }

    //next line: blocks
    for(i = 0; i < FileData.blocks.size(); i++)
    {
        // B|layer[,name]|id[,dx,dy]|x|y|contain|b11[,b12]|b2|[e1,e2,e3,e4]|w|h
        LevelBlock &blk = FileData.blocks[i];
        out << "B";
        //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(blk.layer);
        //  only if name != ""
        //  name=block's name
        if(!IsEmpty(blk.gfx_name))
            out << "," << PGE_URLENC(blk.gfx_name);
        //    id=block id
        out << "|" << fromNum(blk.id);
        if((blk.gfx_dx) > 0 || (blk.gfx_dy > 0))
        {
            //  dx=graphics extend x
            out << "," << fromNum(blk.gfx_dx);
            //  dy=graphics extend y
            out << "," << fromNum(blk.gfx_dy);
        }
        //    x=block position x
        out << "|" << fromNum(blk.x);
        //    y=block position y
        out << "|" << fromNum(blk.y);
        //    contain=containing npc number
        //        [1001-1000+NPCMAX] npc-id
        //        [1-999] coin number
        //        [0] nothing
        out << "|" << ((blk.npc_id == 0) ? ""
                        : fromNum(blk.npc_id <= 0 ? (-1 * blk.npc_id) : (blk.npc_id + 1000)) );
        //    b11=slippery[0=false !0=true]
        out << "|" << fromNum((int)blk.slippery);
        //    b12=wing type
        //    b2=invisible[0=false !0=true]
        out << "," << fromNum(blk.motion_ai_id);
        out << "|" << fromNum((int)blk.invisible);
        //    e1=block destory event name[***urlencode!***]
        out << "|" << PGE_URLENC(blk.event_destroy);
        //    e2=block hit event name[***urlencode!***]
        out << "," << PGE_URLENC(blk.event_hit);
        //    e3=no more object in layer event name[***urlencode!***]4
        out << "," << PGE_URLENC(blk.event_emptylayer);
        //    e4=block onscreen event name[***urlencode!***]
        out << "," << PGE_URLENC(blk.event_on_screen);
        //    w=width
        out << "|" << fromNum(blk.autoscale ? (-1 * blk.w) : blk.w);
        //    h=height
        out << "|" << fromNum(blk.h);
        out << "\n";
    }

    //next line: backgrounds
    for(i = 0; i < FileData.bgo.size(); i++)
    {
        //    T|layer|id|x|y
        LevelBGO &bgo = FileData.bgo[i];
        out << "T";
        //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(bgo.layer);
        //    id=background id
        out << "|" << fromNum(bgo.id);
        if((bgo.gfx_dx) > 0 || (bgo.gfx_dy > 0))
        {
            //  dx=graphics extend x
            out << "," << fromNum(bgo.gfx_dx);
            //  dy=graphics extend y
            out << "," << fromNum(bgo.gfx_dy);
        }
        //    x=background position x
        out << "|" << fromNum(bgo.x);
        //    y=background position y
        out << "|" << fromNum(bgo.y);
        out << "\n";
    }

    //next line: npcs
    for(i = 0; i < FileData.npc.size(); i++)
    {
        LevelNPC &npc = FileData.npc[i];
        //Pre-convert some data into SMBX-38A compatible format
        long npcID = (long)npc.id;
        long containerType = 0;
        long specialData = npc.special_data;
        switch(npcID)//Convert npcID and contents ID into container type
        {
        case 91:
            containerType = 1;
            break;
        case 96:
            containerType = 2;
            break;
        case 283:
            containerType = 3;
            break;
        case 284:
            containerType = 4;
            break;
        case 300:
            containerType = 5;
            break;
        case 347:
            containerType = 6;
            break;
        default:
            containerType = 0;
            break;
        }
        if(containerType != 0)
        {
            //Set NPC-ID of contents as main NPC-ID for this NPC
            npcID = npc.contents;
        }

        //Convert "Is Boss" flag into special ID
        switch(npc.id)
        {
        case 15:
        case 39:
        case 86:
            if(npc.is_boss)
                specialData = (long)npc.is_boss;
            break;
        default:
            break;
        }

        //Convert generator type and direction into SMBX-38A Compatible format
        long genType_1 = npc.generator_type;

        //Swap "Appear" and "Projectile" types
        switch(genType_1)
        {
        case 0:
            genType_1 = 2;
            break;
        case 2:
            genType_1 = 0;
            break;
        }

        long genType_2 = npc.generator_direct;
        long genType = (genType_2 != 0) ? ((4 * genType_1) + genType_2) : 0 ;
        //    N|layer|id|x|y|b1,b2,b3,b4|sp|e1,e2,e3,e4,e5,e6,e7|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
        out << "N";
        //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(npc.layer);
        //only if name != ""
        //name=npc's name
        if(!IsEmpty(npc.gfx_name))
            out << "," << PGE_URLENC(npc.gfx_name);
        //    id=npc id
        out << "|" << fromNum(npcID);
        if((npc.gfx_dx) > 0 || (npc.gfx_dy > 0))
        {
            //  dx=graphics extend x
            out << "," << fromNum(npc.gfx_dx);
            //  dy=graphics extend y
            out << "," << fromNum(npc.gfx_dy);
        }
        //    x=npc position x
        out << "|" << fromNum(npc.x);
        //    y=npc position y
        out << "|" << fromNum(npc.y);
        //    b1=[1]left [0]random [-1]right
        out << "|" << fromNum(-1 * npc.direct);
        //    b2=friendly npc
        out << "," << fromNum((int)npc.friendly);
        //    b3=don't move npc
        out << "," << fromNum((int)npc.nomove);
        //    b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
        out << "," << fromNum(containerType);
        //    sp=special option
        out << "|" << fromNum(specialData);
        //        [***urlencode!***]
        //        e1=death event
        out << "|" << PGE_URLENC(npc.event_die);
        //        e2=talk event
        out << "," << PGE_URLENC(npc.event_talk);
        //        e3=activate event
        out << "," << PGE_URLENC(npc.event_activate);
        //        e4=no more object in layer event
        out << "," << PGE_URLENC(npc.event_emptylayer);
        //        e5=grabed event
        out << "," << PGE_URLENC(npc.event_grab);
        //        e6=next frame event
        out << "," << PGE_URLENC(npc.event_nextframe);
        //        e7=touch event
        out << "," << PGE_URLENC(npc.event_touch);
        //        a1=layer name to attach
        out << "|" << PGE_URLENC(npc.attach_layer);
        //        a2=variable name to send
        out << "," << PGE_URLENC(npc.send_id_to_variable);
        //    c1=generator enable
        out << "|" << fromNum((int)npc.generator);

        //        [if c1!=0]
        if(npc.generator)
        {
            //        c2=generator period[1 frame]
            //Convert deciseconds into frames with rounding
            SMBX38A_RestoreOrigTime(npc.generator_period_orig, (long)npc.generator_period, PGE_FileLibrary::TimeUnit::Decisecond);
            out << "," << fromNum(npc.generator_period_orig);
            //        c3=generator effect
            //            c3-1 [1=warp][0=projective][4=no effect]
            //            c3-2 [0=center][1=up][2=left][3=down][4=right][9=up+left][10=left+down][11=down+right][12=right+up]
            //                if (c3-2)!=0
            //                c3=4*(c3-1)+(c3-2)
            //                else
            //                c3=0
            out << "," << fromNum(genType);
            //        c4=generator direction[angle][when c3=0]
            out << "," << fromNum(npc.generator_custom_angle);
            //        c5=batch[when c3=0][MAX=32]
            out << "," << fromNum(npc.generator_branches);
            //        c6=angle range[when c3=0]
            out << "," << fromNum(npc.generator_angle_range);
            //        c7=speed[when c3=0][float]
            out << "," << fromNum(npc.generator_initial_speed);
        }

        //    msg=message by this npc talkative[***urlencode!***]
        out << "|" << PGE_URLENC(npc.msg);
        out << "\n";
    }

    //next line: warps
    for(i = 0; i < FileData.doors.size(); i++)
    {
        LevelDoor &door = FileData.doors[i];

        if(((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)))
            if(!door.isSetIn) continue; // Skip broken warp entry

        if(((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)))
            if(!door.isSetOut) continue; // Skip broken warp entry

        int oDirect = door.odirect;

        switch(oDirect)//Convert from SMBX64/PGE-X into SMBX-38A compatible form
        {
        case LevelDoor::EXIT_UP:
            oDirect = 1;
            break;

        case LevelDoor::EXIT_LEFT:
            oDirect = 2;
            break;

        case LevelDoor::EXIT_DOWN:
            oDirect = 3;
            break;

        case LevelDoor::EXIT_RIGHT:
            oDirect = 4;
            break;
        }

        //    W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc,mini,size|lik|liid|noexit|wx|wy|le|we
        out << "W";
        //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(door.layer);
        //    x=entrance position x
        out << "|" << fromNum(door.ix);
        //    y=entrance postion y
        out << "|" << fromNum(door.iy);
        //    ex=exit position x
        out << "|" << fromNum(door.ox);
        //    ey=exit position y
        out << "|" << fromNum(door.oy);
        //    type=[1=pipe][2=door][0=instant]
        {
            //type%100=[0=instant][1=pipe][2=door]
            int type = door.type;
                //type/100=[0=none][1=Scroll][2=Fade]
                type += door.transition_effect * 100;
            out << "|" << fromNum(type);
        }
        //    enterd=entrance direction[1=up 2=left 3=down 4=right]
        out << "|" << fromNum(door.idirect);
        //    exitd=exit direction[1=up 2=left 3=down 4=right]
        out << "|" << fromNum(oDirect);
        //    sn=need stars for enter
        out << "|" << fromNum(door.stars);
        //    msg=a message when you have not enough stars
        out << "," << PGE_URLENC(door.stars_msg);
        //    hide=hide the star number in this warp
        out << "," << fromNum((int)door.star_num_hide);
        //    locked=locked
        out << "|" << fromNum((int)door.locked);
        //    noyoshi=no yoshi
        out << "," << fromNum((int)door.novehicles);
        //    canpick=allow npc
        out << "," << fromNum((int)door.allownpc);
        //    bomb=need a bomb
        out << "," << fromNum((int)door.need_a_bomb);
        //    hide=hide the entry scene
        out << "," << fromNum((int)door.hide_entering_scene);
        //    anpc=allow npc interlevel
        out << "," << fromNum((int)door.allownpc_interlevel);
        //    mini=Mini-Only
        out << "," << fromNum((int)door.special_state_required);
        //    size=Warp Size(pixel)
        out << "," << fromNum(door.length_i);
        if(door.two_way || door.cannon_exit)
        {
            //    ts = two-way
            out << "," << fromNum((int)door.two_way);
            //    cannon = Pipe Cannon Force
            out << "," << fromNum(door.cannon_exit ? door.cannon_exit_speed : 0.0);
        }
        //    lik=warp to level[***urlencode!***]
        out << "|" << PGE_URLENC(door.lname);
        //    liid=normal enterance / to warp[0-WARPMAX]
        out << "|" << fromNum(door.warpto);
        //    noexit=level entrance
        out << "|" << fromNum((int)door.lvl_i);
        //    wx=warp to x on world map
        out << "|" << fromNum(door.world_x);
        //    wy=warp to y on world map
        out << "|" << fromNum(door.world_y);
        //    le=level exit
        out << "|" << fromNum((int)door.lvl_o);
        //    we=warp event[***urlencode!***]
        out << "|" << PGE_URLENC(door.event_enter);
        out << "\n";
    }

    //next line: waters
    for(i = 0; i < FileData.physez.size(); i++)
    {
        LevelPhysEnv &pez = FileData.physez[i];
        /*TRIVIA: It is NOT a PEZ candy brand, just "Physical Environment Zone" :-P*/
        //    Q|layer|x|y|w|h|b1,b2,b3,b4,b5|event
        out << "Q";
        //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(pez.layer);
        //    x=position x
        out << "|" << fromNum(pez.x);
        //    y=position y
        out << "|" << fromNum(pez.y);
        //    w=width
        out << "|" << fromNum(pez.w);
        //    h=height
        out << "|" << fromNum(pez.h);
        //    b1=liquid type
        //        01-Water[friction=0.5]
        //        02-Quicksand[friction=0.1]
        //        03-Custom Water
        //        04-Gravitational Field
        //        05-Event Once
        //        06-Event Always
        //        07-NPC Event Once
        //        08-NPC Event Always
        //        09-Click Event
        //        10-Collision Script
        //        11-Click Script
        //        12-Collision Event
        //        13-Air
        out << "|" << fromNum((pez.env_type + 1));
        //    b2=friction
        out << "," << fromNum(pez.friction);
        //    b3=Acceleration Direction
        out << "," << fromNum(pez.accel_direct);
        //    b4=Acceleration
        out << "," << fromNum(pez.accel);
        //    b5=Maximum Velocity
        out << "," << fromNum(pez.max_velocity);
        //    event=touch event
        out << "|" << PGE_URLENC(pez.touch_event);
        out << "\n";
    }

    for(i = 0; i < FileData.layers.size(); i++)
    {
        LevelLayer &lyr = FileData.layers[i];
        //next line: layers
        //    L|name|status
        out << "L";
        //    name=layer name[***urlencode!***]
        out << "|" << PGE_URLENC(lyr.name);
        //    status=is vizible layer
        out << "|" << fromNum((int)(!lyr.hidden));
        out << "\n";
    }

    //next line: events
    for(i = 0; i < FileData.events.size(); i++)
    {
        LevelSMBX64Event &evt = FileData.events[i];
        //    E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
        out << "E";
        //    name=event name[***urlencode!***]
        out << "|" << PGE_URLENC(evt.name);
        //    msg=show message after start event[***urlencode!***]
        out << "|" << PGE_URLENC(evt.msg);
        //    ea=val,syntax
        //        val=[0=not auto start][1=auto start when level start][2=auto start when match all condition][3=start when called and match all condidtion]
        out << "|" << fromNum(evt.autostart);
        //        syntax=condidtion expression[***urlencode!***]
        out << "," << PGE_URLENC(evt.autostart_condition);
        //    el=b/s1,s2...sn/h1,h2...hn/t1,t2...tn
        //        b=no smoke[0=false !0=true]
        out << "|" << fromNum((int)evt.nosmoke);
        //        [***urlencode!***]
        out << "/";

        //        s(n)=show layer
        for(pge_size_t j = 0; j < evt.layers_show.size(); j++)
        {
            if(j > 0) out << ",";

            out << PGE_URLENC(evt.layers_show[j]);
        }

        out << "/";

        //        l(n)=hide layer
        for(pge_size_t j = 0; j < evt.layers_hide.size(); j++)
        {
            if(j > 0) out << ",";

            out << PGE_URLENC(evt.layers_hide[j]);
        }

        out << "/";

        //        t(n)=toggle layer
        for(pge_size_t j = 0; j < evt.layers_toggle.size(); j++)
        {
            if(j > 0) out << ",";

            out << PGE_URLENC(evt.layers_toggle[j]);
        }

        out << "|";

        //    elm=elm1/elm2...elmn
        for(pge_size_t j = 0; j < evt.moving_layers.size(); j++)
        {
            if(j > 0) out << "/";

            //        elm(n)=layername,horizontal syntax,vertical syntax,way
            //        layername=layer name for movement[***urlencode!***]
            LevelEvent_MoveLayer &mvl = evt.moving_layers[j];
            //Convert all floats into strings if expression fields are empty
            PGESTRING expression_x = mvl.expression_x;
            PGESTRING expression_y = mvl.expression_y;
            SMBX38A_Num2Exp_URLEN(mvl.speed_x, expression_x);
            SMBX38A_Num2Exp_URLEN(mvl.speed_y, expression_y);
            out << PGE_URLENC(mvl.name);
            //        horizontal syntax,vertical syntax[***urlencode!***][syntax]
            out << "," << expression_x;
            out << "," << expression_y;
            //        way=[0=by speed][1=by Coordinate]
            out << "," << fromNum(mvl.way);
        }

        out << "|";
        //    epy=b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12
        //        b1=enable player controls
        out << fromNum(evt.ctrls_enable);
        //        b2=drop
        out << "," << fromNum(evt.ctrl_drop);
        //        b3=alt run
        out << "," << fromNum(evt.ctrl_altrun);
        //        b4=run
        out << "," << fromNum(evt.ctrl_run);
        //        b5=jump
        out << "," << fromNum(evt.ctrl_jump);
        //        b6=alt jump
        out << "," << fromNum(evt.ctrl_altjump);
        //        b7=up
        out << "," << fromNum(evt.ctrl_up);
        //        b8=down
        out << "," << fromNum(evt.ctrl_down);
        //        b9=left
        out << "," << fromNum(evt.ctrl_left);
        //        b10=right
        out << "," << fromNum(evt.ctrl_right);
        //        b11=start
        out << "," << fromNum(evt.ctrl_start);
        //        b12=lock keyboard
        out << "," << fromNum(evt.ctrl_lock_keyboard);
        out << "|";
        //    eps=esection/ebackground/emusic
        //        esection=es1:es2...esn
        //        ebackground=eb1:eb2...ebn
        //        emusic=em1:em2...emn
        bool size_set_added = false;

        for(pge_size_t j = 0; j < evt.sets.size(); j++)
        {
            long section_pos = evt.sets[j].position_left;
            switch(section_pos)
            {
            case -1:
                continue;
            case -2:
                section_pos = 1;
                break;
            default:
                section_pos = 2;
                break;
            }
            //Convert floats into expressions if there are empty
            PGESTRING expression_x = evt.sets[j].expression_pos_x;
            PGESTRING expression_y = evt.sets[j].expression_pos_y;
            PGESTRING expression_w = evt.sets[j].expression_pos_w;
            PGESTRING expression_h = evt.sets[j].expression_pos_h;
            PGESTRING expression_as_x = evt.sets[j].expression_autoscrool_x;
            PGESTRING expression_as_y = evt.sets[j].expression_autoscrool_y;

            if(section_pos >= 2)
            {
                SMBX38A_Num2Exp_URLEN(evt.sets[j].position_left,   expression_x);
                SMBX38A_Num2Exp_URLEN(evt.sets[j].position_top,    expression_y);
                SMBX38A_Num2Exp_URLEN(evt.sets[j].position_right - evt.sets[j].position_left, expression_w);
                SMBX38A_Num2Exp_URLEN(evt.sets[j].position_bottom - evt.sets[j].position_top, expression_h);
            }

            if(evt.sets[j].autoscrol)
            {
                SMBX38A_Num2Exp_URLEN(evt.sets[j].autoscrol_x, expression_as_x);
                SMBX38A_Num2Exp_URLEN(evt.sets[j].autoscrol_y, expression_as_y);
            }
            else
            {
                expression_as_x = PGE_URLENC(expression_as_x);
                expression_as_y = PGE_URLENC(expression_as_y);
            }

            //            es=id,x,y,w,h,auto,sx,sy
            if(size_set_added) out << ":";

            size_set_added = true;
            //                id=section id
            out        << fromNum(evt.sets[j].id + 1);
            //                stype=[0=don't change][1=default][2=custom]
            out << "," << fromNum(section_pos);
            //                x=left x coordinates for section [id][***urlencode!***][syntax]
            out << "," << expression_x;
            //                y=top y coordinates for section [id][***urlencode!***][syntax]
            out << "," << expression_y;
            //                w=width for section [id][***urlencode!***][syntax]
            out << "," << expression_w;
            //                h=height for section [id][***urlencode!***][syntax]
            out << "," << expression_h;
            //                auto=enable autoscroll controls[0=false !0=tru
            out << "," << fromNum((int)evt.sets[j].autoscrol);
            //                sx=move screen horizontal syntax[***urlencode!***][syntax]
            out << "," << expression_as_x;
            //                sy=move screen vertical syntax[***urlencode!***][syntax]
            out << "," << expression_as_y;
        }

        out << "/";
        bool bg_set_added = false;

        for(pge_size_t j = 0; j < evt.sets.size(); j++)
        {
            long section_bg = evt.sets[j].background_id;
            switch(section_bg)
            {
            case -1:
                continue;
            case -2:
                section_bg = 1;
                break;
            default:
                section_bg = 2;
                break;
            }
            //            eb=id,btype,backgroundid
            if(bg_set_added) out << ":";

            bg_set_added = true;
            //                id=section id
            out        << fromNum(evt.sets[j].id + 1);
            //                btype=[0=don't change][1=default][2=custom]
            out << "," << fromNum(section_bg);
            //                backgroundid=[when btype=2]custom background id
            out << "," << fromNum(evt.sets[j].background_id >= 0 ? SMBX38A_mapBGID_To(evt.sets[j].background_id) : 0);
        }

        out << "/";
        bool muz_set_added = false;
        for(pge_size_t j = 0; j < evt.sets.size(); j++)
        {
            long section_muz = evt.sets[j].music_id;
            switch(section_muz)
            {
            case -1:
                continue;
            case -2:
                section_muz = 1;
                break;
            default:
                section_muz = 2;
                break;
            }
            //            em=id,mtype,musicid,customfile
            if(muz_set_added)
                out << ":";
            muz_set_added = true;
            //                id=section id
            out        << fromNum(evt.sets[j].id + 1);
            //                mtype=[0=don't change][1=default][2=custom]
            out << "," << fromNum(section_muz);
            //                musicid=[when mtype=2]custom music id
            out << "," << fromNum(evt.sets[j].music_id >= 0 ? evt.sets[j].music_id : 0);
            //                customfile=[when mtype=3]custom music file name[***urlencode!***]
            out << "," << PGE_URLENC(evt.sets[j].music_file);
        }

        out << "|";
        //    eef=sound/endgame/ce1/ce2...cen
        //        sound=play sound number
        out << fromNum(evt.sound_id);
        //        endgame=[0=none][1=bowser defeat]
        out << "/" << fromNum(evt.end_game);

        for(pge_size_t j = 0; j < evt.spawn_effects.size(); j++)
        {
            LevelEvent_SpawnEffect &eff = evt.spawn_effects[j];
            //if(j<(evt.spawn_effects.size()-1))
            out << "/";
            //Convert floats into expressions if there are empty
            PGESTRING expression_x = eff.expression_x;
            PGESTRING expression_y = eff.expression_y;
            PGESTRING expression_sx = eff.expression_sx;
            PGESTRING expression_sy = eff.expression_sy;
            SMBX38A_Num2Exp_URLEN(eff.x, expression_x);
            SMBX38A_Num2Exp_URLEN(eff.y, expression_y);
            SMBX38A_Num2Exp_URLEN(eff.speed_x, expression_sx);
            SMBX38A_Num2Exp_URLEN(eff.speed_y, expression_sy);
            //        ce(n)=id,x,y,sx,sy,grv,fsp,life
            //            id=effect id
            out        << fromNum(eff.id);
            //            x=effect position x[***urlencode!***][syntax]
            out << "," << expression_x;
            //            y=effect position y[***urlencode!***][syntax]
            out << "," << expression_y;
            //            sx=effect horizontal speed[***urlencode!***][syntax]
            out << "," << expression_sx;
            //            sy=effect vertical speed[***urlencode!***][syntax]
            out << "," << expression_sy;
            //            grv=to decide whether the effects are affected by gravity[0=false !0=true]
            out << "," << fromNum((int)eff.gravity);
            //            fsp=frame speed of effect generated
            out << "," << fromNum(eff.fps);
            //            life=effect existed over this time will be destroyed.
            out << "," << fromNum(eff.max_life_time);
        }

        out << "|";

        //    ecn=cn1/cn2...cnn
        for(pge_size_t j = 0; j < evt.spawn_npc.size(); j++)
        {
            LevelEvent_SpawnNPC &snpc = evt.spawn_npc[j];
            //Convert floats into expressions if there are empty
            PGESTRING expression_x = snpc.expression_x;
            PGESTRING expression_y = snpc.expression_y;
            PGESTRING expression_sx = snpc.expression_sx;
            PGESTRING expression_sy = snpc.expression_sy;
            SMBX38A_Num2Exp_URLEN(snpc.x, expression_x);
            SMBX38A_Num2Exp_URLEN(snpc.y, expression_y);
            SMBX38A_Num2Exp_URLEN(snpc.speed_x, expression_sx);
            SMBX38A_Num2Exp_URLEN(snpc.speed_y, expression_sy);
            //        cn(n)=id,x,y,sx,sy,sp
            if(j > 0) out << "/";
            //            id=npc id
            out        << fromNum(snpc.id);
            //            x=npc position x[***urlencode!***][syntax]
            out << "," << expression_x;
            //            y=npc position y[***urlencode!***][syntax]
            out << "," << expression_y;
            //            sx=npc horizontal speed[***urlencode!***][syntax]
            out << "," << expression_sx;
            //            sy=npc vertical speed[***urlencode!***][syntax]
            out << "," << expression_sy;
            //            sp=advanced settings of generated npc
            out << "," << fromNum(snpc.special);
        }

        out << "|";

        //    evc=vc1/vc2...vcn
        for(pge_size_t j = 0; j < evt.update_variable.size(); j++)
        {
            LevelEvent_UpdateVariable &uvar = evt.update_variable[j];

            if(j > 0)
                out << "/";

            //        vc(n)=name,newvalue
            out        << PGE_URLENC(uvar.name);
            //            name=variable name[***urlencode!***]
            out << "," << PGE_URLENC(uvar.newval);
            //            newvalue=new value[***urlencode!***][syntax]
        }

        out << "|";
        //    ene=nextevent/timer/apievent/scriptname
        //        nextevent=name,delay
        //            name=trigger event name[***urlencode!***]
        out        << PGE_URLENC(evt.trigger);
        //            delay=trigger delay[1 frame]
        SMBX38A_RestoreOrigTime(evt.trigger_timer_orig, evt.trigger_timer, PGE_FileLibrary::TimeUnit::Decisecond);
        out << "," << fromNum(evt.trigger_timer_orig);
        //        timer=enable,count,interval,type,show
        //            enable=enable the game timer controlling[0=false !0=true]
        out << "/" << fromNum((int)evt.timer_def.enable);
        //            count=set the time left of the game timer
        out << "," << fromNum(evt.timer_def.count);
        //            interval=set the time count interval of the game timer
        out << "," << fromNum(PGE_FileLibrary::TimeUnitsCVT(evt.timer_def.interval,
                              PGE_FileLibrary::TimeUnit::Millisecond,
                              PGE_FileLibrary::TimeUnit::FrameOneOf65sec));
        //            type=to choose the way timer counts[0=counting down][1=counting up]
        out << "," << fromNum(evt.timer_def.count_dir);
        //            show=to choose whether the game timer is showed in hud[0=false !0=true]
        out << "," << fromNum(evt.timer_def.show);
        //        apievent=the id of apievent
        out << "/" << fromNum(evt.trigger_api_id);
        //        scriptname=script name[***urlencode!***]
        out << "/" << PGE_URLENC(evt.trigger_script);
        out << "\n";
    }

    //next line: variables
    for(LevelVariable &var : FileData.variables)
    {
        //    V|name|value
        out << "V";
        //    name=variable name[***urlencode!***]
        out << "|" << PGE_URLENC(var.name);

        //    value=initial value of the variable
        if(!SMBX64::IsSInt(var.value))//if is not signed integer, set value as zero
            out << "|" << fromNum(0);
        else
            out << "|" << var.value;

        out << "\n";
    }

    //next line: scripts
    for(LevelScript &script : FileData.scripts)
    {
        //    S|name|script
        out << "S";
        //    Su|name|scriptu
        //    name=name of script[***urlencode!***]
        out << "|" << PGE_URLENC(script.name);
        //    script=script[***base64encode!***][utf-8]
        PGESTRING scriptT = script.script;

        /* ********** Disabled appending "\n" until 38A will fix an inability to ignore empty lines in scripts ********** */
        //if(scriptT.size() > 0 && (PGEGetChar(scriptT[scriptT.size() - 1]) != '\n'))
        //    scriptT.append("\n");

        //Convert into CRLF
        PGE_ReplSTRING(scriptT, "\n", "\r\n");
        out << "|" << PGE_BASE64ENC_nopad(scriptT);//"=" ending makes SMBX-38A fail to interpret scripts
        //    scriptu=script[***base64encode!***][ASCII]
        out << "\n";
    }

    //next line: Custom block
    for(LevelItemSetup38A &is : FileData.custom38A_configs)
    {
        //    V|name|value
        switch(is.type)
        {
        case LevelItemSetup38A::BLOCK:
            out << "CB";
            break;
        case LevelItemSetup38A::BGO:
            out << "CT";
            break;
        case LevelItemSetup38A::EFFECT:
            out << "CE";
            break;
        case LevelItemSetup38A::UNKNOWN:
        default:
            out << "CUnk";
            break;
        }
        //    id = object id
        out << "|" << fromNum(is.id);

        for(pge_size_t j = 0; j < is.data.size(); j++)
        {
            LevelItemSetup38A::Entry &e = is.data[j];
            out << ((j == 0) ? "|" : ",");
            out << SMBX38A_CC_encode(e.key, e.value);
        }
        out << "\n";
    }

    if(FileData.sound_overrides.size() > 0)
    {
        out << "CW";
        for(LevelData::MusicOverrider &mo : FileData.sound_overrides)
        {
            out << "|" << fromNum(mo.id) << "," << PGE_URLENC(mo.fileName);
        }
        out << "\n";
    }

    return true;
}
