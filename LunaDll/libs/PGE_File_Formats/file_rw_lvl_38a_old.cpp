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

void old38a_SplitCSVStr(PGESTRINGList &dst, PGESTRING &Src)
{
    dst.clear();
    for(pge_size_t i = 0; i < Src.size();)
    {
        PGESTRING Buffer = "";
        PGEChar cur = ' ';
        do
        {
            cur = Src[static_cast<pge_size_t>(i++)];
            if(cur != ',') Buffer.push_back(cur);
        }
        while((i < Src.size()) && (cur != ','));

        dst.push_back(Buffer);
    }
}

#define SMBX38A_SplitSubLine(dst, src) SMBX38A_SplitLine(dst, src, '/')
void SMBX38A_SplitLine(PGESTRINGList &dst, PGESTRING &Src, char sep = '|')
{
    dst.clear();

    for(pge_size_t i = 0; i < Src.size();)
    {
        PGESTRING Buffer = "";
        PGEChar cur = ' ';
        do
        {
            cur = Src[i++];

            if(cur != sep) Buffer.push_back(cur);
        }
        while((i < Src.size()) && (cur != sep));
        dst.push_back(Buffer);
    }
}

bool FileFormats::ReadSMBX38ALvlFile_OLD(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    SMBX38A_FileBegin();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    CreateLevelData(FileData);
    FileData.meta.RecentFormat = LevelData::SMBX38A;
    FileData.LevelName = "";
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
    LevelPhysEnv waters;
    LevelLayer layerdata;
    LevelSMBX64Event eventdata;
    //LevelEvent_Sets event_sets;
    LevelVariable vardata;
    LevelScript scriptdata;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    PGESTRINGList currentLine;
    nextLine();   //Read first line

    if(!PGE_StartsWith(line, "SMBXFile"))   //File format number
        goto badfile;
    else file_format = 65;

    while(!in.eof())
    {
        nextLine();   //Read second Line
        SMBX38A_SplitLine(currentLine, line);

        if(currentLine.size() == 0)
            continue;

        if(currentLine[0] == "A") //Level settings
        {
            for(int i = 1; i < static_cast<signed>(currentLine.size()); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                case 1://Number of stars
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else FileData.stars = toInt(cLine);
                }
                break;

                case 2://Level title (URL Encoded!)
                {
                    FileData.LevelName = PGE_URLDEC(cLine);
                }
                break;

                //param3=a filename, when player died, the player will be sent to this level.
                case 3:
                {
                    FileData.open_level_on_fail = PGE_URLDEC(cLine);
                }
                break;

                //param4=normal entrance / to warp [0-WARPMAX]
                case 4:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else
                        FileData.open_level_on_fail_warpID = toUInt(cLine);
                }
                break;
                }
            }
        }
        else if(currentLine[0] == "P1") //Player 1 point
        {
            playerdata = CreateLvlPlayerPoint(1);

            for(int i = 1; i < static_cast<signed>(currentLine.size()); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                case 1://Pos X
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else
                        playerdata.x = static_cast<long>(toFloat(cLine));
                }
                break;

                case 2://Pos Y
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else playerdata.y = static_cast<long>(toFloat(cLine));
                }
                break;
                }
            }

            FileData.players.push_back(playerdata);
        }
        else if(currentLine[0] == "P2") //Player 2 point
        {
            playerdata = CreateLvlPlayerPoint(2);

            for(int i = 1; i < static_cast<signed>(currentLine.size()); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                case 1://Pos X
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else
                        playerdata.x = static_cast<long>(toFloat(cLine));
                }
                break;

                case 2://Pos Y
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else
                        playerdata.y = static_cast<long>(toFloat(cLine));
                }
                break;
                }
            }

            FileData.players.push_back(playerdata);
        }
        else if(currentLine[0] == "M") //Section
        {
            //M|id|x|y|w|h|b1|b2|b3|b4|b5|b6|music|background|musicfile
            section = CreateLvlSection();
            double x = 0.0, y = 0.0, w = 0.0, h = 0.0;

            for(int i = 1; i < static_cast<signed>(currentLine.size()); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //"id=[1-SectionMAX]
                case 1:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else section.id = (toInt(cLine) - 1);

                    if(section.id < 0) section.id = 0;
                }
                break;

                //"x=Left size[-left/+right]
                case 2:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else x = toDouble(cLine);
                }
                break;

                //"y=Top size[-down/+up]
                case 3:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else y = toDouble(cLine);
                }
                break;

                //"w=width of the section[if (w < 800) w = 800]
                case 4:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else w = toDouble(cLine);

                    if((w < 800.0) && (w != 0.0)) w = 800.0;
                }
                break;

                //"h=height of the section[if (h < 600) h = 600]
                case 5:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else h = toDouble(cLine);

                    if((h < 600.0) && (h != 0.0)) h = 600.0;
                }
                break;

                //"b1=under water?[0=false !0=true]
                case 6:
                {
                    section.underwater = (cLine != "0");
                }
                break;

                //"b2=is x-level wrap[0=false !0=true]
                case 7:
                {
                    section.wrap_h = (cLine != "0");
                }
                break;

                //"b3=enable off screen exit[0=false !0=true]
                case 8:
                {
                    section.OffScreenEn = (cLine != "0");
                }
                break;

                //"b4=no turn back(x)[0=no x-scrolllock 1=scrolllock left 2=scrolllock right]
                case 9:
                {
                    section.lock_left_scroll  = (cLine == "1");
                    section.lock_right_scroll = (cLine == "2");
                }
                break;

                //"b5=no turn back(y)[0=no y-scrolllock 1=scrolllock up 2=scrolllock down]
                case 10:
                {
                    section.lock_up_scroll = (cLine == "1");
                    section.lock_down_scroll = (cLine == "2");
                }
                break;

                //"b6=is y-level wrap[0=false !0=true]
                case 11:
                {
                    section.wrap_v = (cLine != "0");
                }
                break;

                //"music=music number[same as smbx1.3]
                case 12:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else section.music_id = static_cast<unsigned int>(toDouble(cLine));
                }
                break;

                //"background=background number[same as the filename in 'background2' folder]
                case 13:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else section.background = static_cast<unsigned int>(toFloat(cLine));
                }
                break;

                //"musicfile=custom music file[***urlencode!***]
                case 14:
                {
                    section.music_file = PGE_URLDEC(cLine);
                }
                break;
                }
            }

            SMBX38A_mapBGID_From(section.background);//Convert into SMBX64 ID set
            section.size_left   = static_cast<long>(round(x));
            section.size_top    = static_cast<long>(round(y));
            section.size_right  = static_cast<long>(round(x + w));
            section.size_bottom = static_cast<long>(round(y + h));
            //Very important data! I'ts a camera position in the editor!
            section.PositionX = section.size_left - 10;
            section.PositionY = section.size_top - 10;

            if(section.id < static_cast<signed>(FileData.sections.size()))
                FileData.sections[section.id] = section; //Replace if already exists
            else
                FileData.sections.push_back(section); //Add Section in main array
        }
        else if(currentLine[0] == "B") //Blocks
        {
            //B|layer|id|x|y|contain|b1|b2|e1,e2,e3|w|h
            blockdata = CreateLvlBlock();

            for(int i = 1; i < static_cast<signed>(currentLine.size()); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                {
                    blockdata.layer = (IsEmpty(cLine) ? "Default" : PGE_URLDEC(cLine));
                }
                break;

                //    id=block id
                case 2:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else blockdata.id = toULong(cLine);
                }
                break;

                //    x=block position x
                case 3:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else blockdata.x = static_cast<long>(round(toDouble(cLine)));
                }
                break;

                //    y=block position y
                case 4:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else blockdata.y = static_cast<long>(round(toDouble(cLine)));
                }
                break;

                //    contain=containing npc number
                //        [1001-1000+NPCMAX] npc-id
                //        [1-999] coin number
                //        [0] nothing
                case 5:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else
                    {
                        long npcid = toInt(cLine);
                        blockdata.npc_id = ((npcid < 1000) ? -1 * npcid : npcid - 1000);
                    }
                }
                break;

                //    b1=slippery[0=false !0=true]
                case 6:
                {
                    blockdata.slippery = (cLine != "0");
                }
                break;

                //    b2=invisible[0=false !0=true]
                case 7:
                {
                    blockdata.invisible = (cLine != "0");
                }
                break;

                case 8:
                {
                    PGESTRINGList bevents;
                    old38a_SplitCSVStr(bevents, cLine);

                    for(int j = 0; j < static_cast<signed>(bevents.size()); j++)
                    {
                        PGESTRING &dLine = bevents[j];

                        switch(j)
                        {
                        //    e1=block destory event name[***urlencode!***]
                        case 0:
                            blockdata.event_destroy = PGE_URLDEC(dLine);
                            break;

                        //    e2=block hit event name[***urlencode!***]
                        case 1:
                            blockdata.event_hit = PGE_URLDEC(dLine);
                            break;

                        //    e3=no more object in layer event name[***urlencode!***]4
                        case 2:
                            blockdata.event_emptylayer = PGE_URLDEC(dLine);
                            break;
                        }
                    }
                }
                break;

                //    w=width
                case 9:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else blockdata.w = (long)round(toDouble(cLine));
                }
                break;

                //    h=height
                case 10:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else blockdata.h = (long)round(toDouble(cLine));
                }
                break;
                }
            }

            blockdata.meta.array_id = FileData.blocks_array_id++;
            FileData.blocks.push_back(blockdata);
        }
        else if(currentLine[0] == "T") //BGOs
        {
            //T|layer|id|x|y
            bgodata = CreateLvlBgo();

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                {
                    bgodata.layer = (cLine == "" ? "Default" : PGE_URLDEC(cLine));
                }
                break;

                //    id=background id
                case 2:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else bgodata.id = toULong(cLine);
                }
                break;

                //    x=background position x
                case 3:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else bgodata.x = (long)round(toDouble(cLine));
                }
                break;

                //    y=background position y
                case 4:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else bgodata.y = (long)round(toDouble(cLine));
                }
                break;
                }
            }

            bgodata.meta.array_id = FileData.bgo_array_id++;
            FileData.bgo.push_back(bgodata);
        }
        else if(currentLine[0] == "N") //NPC
        {
            npcdata = CreateLvlNpc();

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                //next line: npcs
                //N|layer|id|x|y|b1,b2,b3,b4|sp|e1,e2,e3,e4,e5,e6,e7|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                {
                    npcdata.layer = (cLine == "" ? "Default" : PGE_URLDEC(cLine));
                }
                break;

                //id=npc id
                case 2:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else npcdata.id = static_cast<uint64_t>(toInt(cLine));
                }
                break;

                //x=npc position x
                case 3:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else npcdata.x = static_cast<long>(std::round(toFloat(cLine)));
                }
                break;

                //y=npc position y
                case 4:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else npcdata.y = static_cast<long>(std::round(toFloat(cLine)));
                }
                break;

                //b1=[-1]left [0]random [1]right
                //b2=friendly npc
                //b3=don't move npc
                //b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
                case 5:
                {
                    PGESTRINGList physparams;
                    old38a_SplitCSVStr(physparams, cLine);

                    for(int j = 0; j < (signed)physparams.size(); j++)
                    {
                        PGESTRING &dLine = physparams[j];

                        switch(j)
                        {
                        //b1=[1]left [0]random [-1]right
                        case 0:
                            if(!SMBX64::IsSInt(dLine))
                                goto badfile;
                            else npcdata.direct = -1 * toInt(dLine);//Convert into SMBX64/PGE-X Compatible form

                            break;

                        //b2=friendly npc
                        case 1:
                            npcdata.friendly = ((dLine != "") && (dLine != "0"));
                            break;

                        //b3=don't move npc
                        case 2:
                            npcdata.nomove = ((dLine != "") && (dLine != "0"));
                            break;

                        //b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
                        case 3:

                            //CONTAINER with packed NPC
                            if(!SMBX64::IsSInt(dLine))
                                goto badfile;

                            int contID = toInt(dLine);

                            if(contID == 0)
                                break;

                            npcdata.contents = static_cast<long>(npcdata.id);

                            switch(contID)
                            {
                            case 1:
                                npcdata.id = 91;
                                break;

                            case 2:
                                npcdata.id = 96;
                                break;

                            case 3:
                                npcdata.id = 283;
                                break;

                            case 4:
                                npcdata.id = 284;
                                break;

                            case 5:
                                npcdata.id = 300;
                                break;
                            }

                            break;
                        }
                    }
                }
                break;

                //sp=special option
                case 6:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else
                    {
                        npcdata.special_data = (long)round(toDouble(cLine));

                        switch(npcdata.id)
                        {
                        case 15:
                        case 39:
                        case 86: //Bind "Is Boss" flag for supported NPC's
                            npcdata.is_boss = (bool)npcdata.special_data;
                            npcdata.special_data = 0;

                        default:
                            break;
                        }
                    }
                }
                break;

                //Event slots
                case 7:
                {
                    PGESTRINGList nevents;
                    old38a_SplitCSVStr(nevents, cLine);

                    for(int j = 0; j < (signed)nevents.size(); j++)
                    {
                        PGESTRING &dLine = nevents[j];

                        switch(j) //    [***urlencode!***]
                        {
                        //    e1=death event
                        case 0:
                            npcdata.event_die = PGE_URLDEC(dLine);
                            break;

                        //    e2=talk event
                        case 1:
                            npcdata.event_talk = PGE_URLDEC(dLine);
                            break;

                        //    e3=activate event
                        case 2:
                            npcdata.event_activate = PGE_URLDEC(dLine);
                            break;

                        //    e4=no more object in layer event
                        case 3:
                            npcdata.event_emptylayer = PGE_URLDEC(dLine);
                            break;

                        //    e5=grabed event
                        case 4:
                            npcdata.event_grab = PGE_URLDEC(dLine);
                            break;

                        //    e6=next frame event
                        case 5:
                            npcdata.event_nextframe = PGE_URLDEC(dLine);
                            break;

                        //    e7=touch event
                        case 6:
                            npcdata.event_touch = PGE_URLDEC(dLine);
                            break;
                        }
                    }
                }
                break;

                //Attach layer / Updated variable with
                case 8:
                {
                    PGESTRINGList nelayers;
                    old38a_SplitCSVStr(nelayers, cLine);

                    for(int j = 0; j < (signed)nelayers.size(); j++)
                    {
                        PGESTRING &dLine = nelayers[j];

                        switch(j) //    [***urlencode!***]
                        {
                        //    a1=layer name to attach
                        case 0:
                            npcdata.attach_layer = PGE_URLDEC(dLine);
                            break;

                        //    a2=variable name to send
                        case 1:
                            npcdata.send_id_to_variable = PGE_URLDEC(dLine);
                            break;
                        }
                    }
                }
                break;

                //Generators
                case 9:
                {
                    PGESTRINGList nevents;
                    old38a_SplitCSVStr(nevents, cLine);

                    for(int j = 0; j < (signed)nevents.size(); j++)
                    {
                        PGESTRING &dLine = nevents[j];

                        if((j > 0) && (!npcdata.generator)) break;

                        switch(j)
                        {
                        //c1=generator enable
                        case 0:
                            npcdata.generator = ((dLine != "") && (dLine != "0"));
                            break;

                        //[if c1!=0]
                        //    c2=generator period[1 frame]
                        case 1:
                            if(!SMBX64::IsSInt(dLine))
                                goto badfile;
                            else npcdata.generator_period = (int)round((toDouble(dLine) * 10.0) / 65.0); //Convert into deci-seconds

                            break;

                        //    c3=generator effect
                        //        c3-1[1=warp][0=projective][4=no effect]
                        //        c3-2[0=center][1=up][2=left][3=down][4=right]
                        //            [9=up+left][10=left+down][11=down+right][12=right+up]
                        //            if (c3-2)!=0
                        //              c3=4*(c3-1)+(c3-2)
                        //            else
                        //              c3=0
                        case 2:
                            if(!SMBX64::IsSInt(dLine))
                                goto badfile;
                            else
                            {
                                int gentype = toInt(dLine);

                                switch(gentype)
                                {
                                case 0:
                                    npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                                    npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                                    break;

                                default:
                                    if(gentype < 29)
                                    {
                                        npcdata.generator_type   = SMBX38A_NpcGeneratorTypes[gentype];
                                        npcdata.generator_direct = SMBX38A_NpcGeneratorDirections[gentype];
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
                                    npcdata.generator_type   = LevelNPC::NPC_GENERATPR_PROJECTILE;
                                    break;

                                case 1:
                                    npcdata.generator_type   = LevelNPC::NPC_GENERATOR_WARP;
                                    break;

                                case 4:
                                    npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                                    break;
                                }
                            }

                            break;

                        //    c4=generator direction[angle][when c3=0]
                        case 3:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else npcdata.generator_custom_angle = toFloat(dLine);
                        }
                        break;

                        //    c5=batch[when c3=0][MAX=32]
                        case 4:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else npcdata.generator_branches = (long)fabs(round(toFloat(dLine)));
                        }
                        break;

                        //    c6=angle range[when c3=0]
                        case 5:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else npcdata.generator_angle_range = fabs(toFloat(dLine));
                        }
                        break;

                        //    c7=speed[when c3=0][float]
                        case 6:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else npcdata.generator_initial_speed = toFloat(dLine);
                        }
                        break;
                        }
                    }
                }
                break;

                //msg=message by this npc talkative[***urlencode!***]
                case 10:
                {
                    npcdata.msg = PGE_URLDEC(cLine);
                }
                break;
                }
            }

            npcdata.meta.array_id = FileData.npc_array_id++;
            FileData.npc.push_back(npcdata);
        }
        else if(currentLine[0] == "Q") //next line: waters
        {
            //Q|layer|x|y|w|h|b1,b2,b3,b4,b5|event
            waters = CreateLvlPhysEnv();

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                {
                    waters.layer = (cLine == "" ? "Default" : PGE_URLDEC(cLine));
                }
                break;

                //x=position x
                case 2:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else waters.x = (long)round(toFloat(cLine));
                }
                break;

                //y=position y
                case 3:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else waters.y = (long)round(toFloat(cLine));
                }
                break;

                //w=width
                case 4:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else waters.w = (long)round(toFloat(cLine));
                }
                break;

                //h=height
                case 5:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else waters.h = (long)round(toFloat(cLine));
                }
                break;

                case 6:
                {
                    PGESTRINGList nevents;
                    old38a_SplitCSVStr(nevents, cLine);

                    for(int j = 0; j < (signed)nevents.size(); j++)
                    {
                        PGESTRING &dLine = nevents[j];

                        switch(j)
                        {
                        //b1=liquid type
                        //    01-Water[friction=0.5]
                        //    02-Quicksand[friction=0.1]
                        //    03-Custom Water
                        //    04-Gravitational Field
                        //    05-Event Once
                        //    06-Event Always
                        //    07-NPC Event Once
                        //    08-NPC Event Always
                        //    09-Click Event
                        //    10-Collision Script
                        //    11-Click Script
                        //    12-Collision Event
                        //    13-Air chamber
                        case 0:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else waters.env_type = (int)round(toFloat(dLine)) - 1;
                        }
                        break;

                        //b2=friction
                        case 1:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else waters.friction = toFloat(dLine);
                        }
                        break;

                        //b3=Acceleration Direction
                        case 2:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else waters.accel_direct = toFloat(dLine);
                        }
                        break;

                        //b4=Acceleration
                        case 3:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else waters.accel = toFloat(dLine);
                        }
                        break;

                        //b5=Maximum Velocity
                        case 4:
                        {
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;
                            else waters.accel = toFloat(dLine);
                        }
                        break;
                        }
                    }
                }
                break;

                //event=touch event
                case 7:
                {
                    waters.touch_event = PGE_URLDEC(cLine);
                }
                break;
                }
            }

            waters.meta.array_id = FileData.physenv_array_id++;
            FileData.physez.push_back(waters);
        }
        else if(currentLine[0] == "W") //next line: warps
        {
            //W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc|lik|liid|noexit|wx|wy|le|we
            doordata = CreateLvlWarp();

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                case 1:
                {
                    doordata.layer = (cLine == "" ? "Default" : PGE_URLDEC(cLine));
                }
                break;

                //x=entrance position x
                case 2:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else doordata.ix = (long)round(toFloat(cLine));
                }
                break;

                //y=entrance postion y
                case 3:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else doordata.iy = (long)round(toFloat(cLine));
                }
                break;

                //ex=exit position x
                case 4:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else doordata.ox = (long)round(toFloat(cLine));
                }
                break;

                //ey=exit position y
                case 5:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else doordata.oy = (long)round(toFloat(cLine));
                }
                break;

                //type=[1=pipe][2=door][0=instant]
                case 6:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else doordata.type = toInt(cLine);
                }
                break;

                //enterd=entrance direction[1=up 2=left 3=down 4=right]
                case 7:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else doordata.idirect = toInt(cLine);
                }
                break;

                //exitd=exit direction[1=up 2=left 3=down 4=right]
                case 8:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else switch(toInt(cLine))//Convert into SMBX64/PGE-X Compatible form
                        {
                        case 1:
                            doordata.odirect = LevelDoor::EXIT_UP;
                            break;

                        case 2:
                            doordata.odirect = LevelDoor::EXIT_LEFT;
                            break;

                        case 3:
                            doordata.odirect = LevelDoor::EXIT_DOWN;
                            break;

                        case 4:
                            doordata.odirect = LevelDoor::EXIT_RIGHT;
                            break;
                        }
                }
                break;

                case 9:
                {
                    PGESTRINGList bevents;
                    old38a_SplitCSVStr(bevents, cLine);

                    for(int j = 0; j < (signed)bevents.size(); j++)
                    {
                        PGESTRING &dLine = bevents[j];

                        switch(j)
                        {
                        //sn=need stars for enter
                        case 0:
                        {
                            if(!SMBX64::IsSInt(dLine))
                                goto badfile;
                            else doordata.stars = toInt(dLine);
                        }
                        break;

                        //msg=a message when you have not enough stars
                        case 1:
                        {
                            doordata.stars_msg = PGE_URLDEC(dLine);
                        }
                        break;

                        //hide=hide the star number in this warp
                        case 2:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.star_num_hide = (dLine != "0");
                        }
                        break;
                        }
                    }
                }
                break;

                case 10:
                {
                    PGESTRINGList bevents;
                    old38a_SplitCSVStr(bevents, cLine);

                    for(int j = 0; j < (signed)bevents.size(); j++)
                    {
                        PGESTRING &dLine = bevents[j];

                        switch(j)
                        {
                        //locked=locked
                        case 0:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.locked = (bool)toInt(dLine);
                        }
                        break;

                        //noyoshi=no yoshi
                        case 1:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.novehicles = (bool)toInt(dLine);
                        }
                        break;

                        //canpick=allow npc
                        case 2:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.allownpc = (bool)toInt(dLine);
                        }
                        break;

                        //bomb=need a bomb
                        case 3:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.need_a_bomb = (bool)toInt(dLine);
                        }
                        break;

                        //hide=hide the entry scene
                        case 4:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.hide_entering_scene = (bool)toInt(dLine);
                        }
                        break;

                        //anpc=allow npc interlevel
                        case 5:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.allownpc_interlevel = (bool)toInt(dLine);
                        }
                        break;

                        //Since SMBX-66-38A: {
                        //mini=Mini-Only
                        case 6:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.special_state_required = (bool)toInt(dLine);
                        }
                        break;

                        //size=Warp Size(pixel)
                        case 7:
                        {
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;
                            else doordata.length_i = toInt(dLine);

                            doordata.length_o = doordata.length_i;
                        }
                        break;
                        }

                        // }//Since SMBX-66-38A
                    }
                }
                break;

                //lik=warp to level[***urlencode!***]
                case 11:
                {
                    doordata.lname = PGE_URLDEC(cLine);
                }
                break;

                //liid=normal enterance / to warp[0-WARPMAX]
                case 12:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else doordata.warpto = toInt(cLine);
                }
                break;

                //noexit=level entrance
                case 13:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else doordata.lvl_i = (bool)toInt(cLine);

                    doordata.isSetIn = ((doordata.lvl_i) ? false : true);
                }
                break;

                //wx=warp to x on world map
                case 14:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else doordata.world_x = (long)round(toFloat(cLine));
                }
                break;

                //wy=warp to y on world map
                case 15:
                {
                    if(!SMBX64::IsFloat(cLine))
                        goto badfile;
                    else doordata.world_y = (long)round(toFloat(cLine));
                }
                break;

                //le=level exit
                case 16:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else doordata.lvl_o = (bool)toInt(cLine);

                    doordata.isSetOut = (((doordata.lvl_o) ? false : true) || (doordata.lvl_i));
                }
                break;

                //we=warp event[***urlencode!***]
                case 17:
                {
                    doordata.event_enter = PGE_URLDEC(cLine);
                }
                break;
                }
            }

            doordata.meta.array_id = FileData.doors_array_id++;
            FileData.doors.push_back(doordata);
        }
        else if(currentLine[0] == "L") //Layers
        {
            //L|name|status
            layerdata = CreateLvlLayer();

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                {
                    layerdata.name = PGE_URLDEC(cLine);
                }
                break;

                //    status=is vizible layer
                case 2:
                {
                    layerdata.hidden = (cLine == "0");
                }
                break;
                }
            }

            layerdata.meta.array_id = FileData.layers_array_id++;
            FileData.layers.push_back(layerdata);
        }
        else if(currentLine[0] == "E") //next line: events
        {
            //E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
            eventdata = CreateLvlEvent();

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //    name=event name[***urlencode!***]
                case 1:
                {
                    eventdata.name = PGE_URLDEC(cLine);
                }
                break;

                //    msg=spawn message
                case 2:
                {
                    eventdata.msg = PGE_URLDEC(cLine);
                }
                break;

                //ea=val,syntax
                case 3:
                {
                    PGESTRINGList autorun;
                    old38a_SplitCSVStr(autorun, cLine);

                    for(int j = 0; j < (signed)autorun.size(); j++)
                    {
                        PGESTRING &dLine = autorun[j];

                        switch(j)
                        {
                        //    val=[0=not auto start][1=auto start when level start][2=auto start when match all condition][3=start when called and match all condidtion]
                        case 0:
                            if(!SMBX64::IsFloat(dLine))
                                goto badfile;

                            eventdata.autostart = (int)round(toFloat(dLine));
                            break;

                        //    syntax=condidtion expression[***urlencode!***]
                        case 1:
                            eventdata.autostart_condition = PGE_URLDEC(dLine);
                            break;
                        }
                    }
                }
                break;

                //el=b/s1,s2...sn/h1,h2...hn/t1,t2...tn
                case 4:
                {
                    PGESTRINGList EvPref;
                    SMBX38A_SplitSubLine(EvPref, cLine);

                    for(int j = 0; j < (signed)EvPref.size(); j++)
                    {
                        PGESTRING &dLine = EvPref[j];

                        switch(j)
                        {
                        //    b=no smoke[0=false !0=true]
                        case 0:
                            eventdata.nosmoke = (dLine != "0");
                            break;

                        //    [***urlencode!***]
                        //    s(n)=show layer
                        case 1:
                        {
                            PGESTRINGList showlayers;
                            old38a_SplitCSVStr(showlayers, dLine);

                            for(int k = 0; k < (signed)showlayers.size(); k++)
                                eventdata.layers_show.push_back(PGE_URLDEC(showlayers[k]));
                        }
                        break;

                        //    l(n)=hide layer
                        case 2:
                        {
                            PGESTRINGList hidelayers;
                            old38a_SplitCSVStr(hidelayers, dLine);

                            for(int k = 0; k < (signed)hidelayers.size(); k++)
                                eventdata.layers_hide.push_back(PGE_URLDEC(hidelayers[k]));
                        }
                        break;

                        //    t(n)=toggle layer
                        case 3:
                        {
                            PGESTRINGList togglelayers;
                            old38a_SplitCSVStr(togglelayers, dLine);

                            for(int k = 0; k < (signed)togglelayers.size(); k++)
                                eventdata.layers_toggle.push_back(PGE_URLDEC(togglelayers[k]));
                        }
                        break;
                        }
                    }
                }
                break;

                //elm=elm1/elm2...elmn
                case 5:
                {
                    //    elm(n)=layername,horizontal syntax,vertical syntax,way
                    //    layername=layer name for movement[***urlencode!***]
                    //    horizontal syntax, vertical syntax[***urlencode!***][syntax]
                    //    way=[0=by speed][1=by Coordinate]
                    PGESTRINGList EvMvLayers;
                    SMBX38A_SplitSubLine(EvMvLayers, cLine);

                    for(int j = 0; j < (signed)EvMvLayers.size(); j++)
                    {
                        LevelEvent_MoveLayer ml;
                        PGESTRING &dLine = EvMvLayers[j];
                        PGESTRINGList movelayers;
                        old38a_SplitCSVStr(movelayers, dLine);

                        for(int k = 0; k < (signed)movelayers.size(); k++)
                        {
                            PGESTRING &eLine = movelayers[k];

                            switch(k)
                            {
                            case 0:
                                ml.name = PGE_URLDEC(eLine);
                                eventdata.movelayer = ml.name;
                                break;

                            case 1:
                                ml.expression_x = PGE_URLDEC(eLine);
                                SMBX38A_Exp2Double(ml.expression_x, ml.speed_x);
                                eventdata.layer_speed_x = ml.speed_x;
                                break;

                            case 2:
                                ml.expression_y = PGE_URLDEC(eLine);
                                SMBX38A_Exp2Double(ml.expression_y, ml.speed_y);
                                eventdata.layer_speed_y = ml.speed_y;
                                break;

                            case 3:
                                if(!SMBX64::IsUInt(eLine))
                                    goto badfile;

                                ml.way = toInt(eLine);
                                break;
                            }
                        }

                        eventdata.moving_layers.push_back(ml);
                    }
                }
                break;

                //1,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 1
                //epy=b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12
                case 6:
                {
                    PGESTRINGList PlrCtrls;
                    old38a_SplitCSVStr(PlrCtrls, cLine);

                    for(int j = 0; j < (signed)PlrCtrls.size(); j++)
                    {
                        PGESTRING &dLine = PlrCtrls[j];

                        switch(j)
                        {
                        //    b1=enable player controls
                        case 0:
                            eventdata.ctrls_enable = (dLine != "0");
                            break;

                        //    b2=drop
                        case 1:
                            eventdata.ctrl_drop = (dLine != "0");
                            break;

                        //    b3=alt run
                        case 2:
                            eventdata.ctrl_altrun = (dLine != "0");
                            break;

                        //    b4=run
                        case 3:
                            eventdata.ctrl_run = (dLine != "0");
                            break;

                        //    b5=jump
                        case 4:
                            eventdata.ctrl_jump = (dLine != "0");
                            break;

                        //    b6=alt jump
                        case 5:
                            eventdata.ctrl_altjump = (dLine != "0");
                            break;

                        //    b7=up
                        case 6:
                            eventdata.ctrl_up = (dLine != "0");
                            break;

                        //    b8=down
                        case 7:
                            eventdata.ctrl_down = (dLine != "0");
                            break;

                        //    b9=left
                        case 8:
                            eventdata.ctrl_left = (dLine != "0");
                            break;

                        //    b10=right
                        case 9:
                            eventdata.ctrl_right = (dLine != "0");
                            break;

                        //    b11=start
                        case 10:
                            eventdata.ctrl_start = (dLine != "0");
                            break;

                        //    b12=lock keyboard
                        case 11:
                            eventdata.ctrl_lock_keyboard = (dLine != "0");
                            break;
                        }
                    }
                }
                break;

                //eps=esection/ebackground/emusic
                case 7:
                {
                    //    esection=es1:es2...esn
                    //    ebackground=eb1:eb2...ebn
                    //    emusic=em1:em2...emn
                    PGESTRINGList EvSets;
                    SMBX38A_SplitSubLine(EvSets, cLine);
                    PGESTRINGList ev_sections;
                    PGESTRINGList ev_bgs;
                    PGESTRINGList ev_musics;

                    //Collect entries
                    for(int j = 0; j < (signed)EvSets.size(); j++)
                    {
                        PGESTRING &dLine = EvSets[j];

                        switch(j)
                        {
                        case 0:
                            SMBX38A_SplitLine(ev_sections, dLine, ':');
                            break;

                        case 1:
                            SMBX38A_SplitLine(ev_bgs, dLine, ':');
                            break;

                        case 2:
                            SMBX38A_SplitLine(ev_musics, dLine, ':');
                            break;
                        }
                    }

                    //fill entries
                    eventdata.sets.clear();

                    for(int q = 0; q < (signed)FileData.sections.size(); q++)
                    {
                        LevelEvent_Sets set;
                        set.id = q;
                        eventdata.sets.push_back(set);
                    }

                    int evSetsSize = ev_sections.size();

                    if(evSetsSize < (signed)ev_bgs.size())
                        evSetsSize = ev_bgs.size();

                    if(evSetsSize < (signed)ev_musics.size())
                        evSetsSize = ev_musics.size();

                    for(int j = 0; j < evSetsSize; j++)
                    {
                        //SECTIONS
                        if(j < (signed)ev_sections.size())
                        {
                            PGESTRINGList params;
                            old38a_SplitCSVStr(params, ev_sections[j]);
                            //        es=id,stype,x,y,w,h,auto,sx,sy
                            int id = -1;
                            bool customSizes = false;
                            bool autoscroll = false;

                            for(int k = 0; k < (signed)params.size(); k++)
                            {
                                if((k > 0) &&
                                   ((id < 0) || (id >= (signed)eventdata.sets.size()))
                                  )//Append sections
                                {
                                    if(id < 0) goto badfile; //Missmatched section ID!

                                    int last = eventdata.sets.size() - 1;

                                    while(id >= (signed)eventdata.sets.size())
                                    {
                                        LevelEvent_Sets set;
                                        set.id = last;
                                        eventdata.sets.push_back(set);
                                        last++;
                                    }
                                }

                                PGESTRING &eLine = params[k];

                                switch(k)
                                {
                                //id=section id
                                case 0:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    id = toInt(eLine) - 1;
                                    break;

                                //stype=[0=don't change][1=default][2=custom]
                                case 1:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    switch(toInt(eLine))
                                    {
                                    case 0:
                                        eventdata.sets[id].position_left = LevelEvent_Sets::LESet_Nothing;
                                        break;

                                    case 1:
                                        eventdata.sets[id].position_left = LevelEvent_Sets::LESet_ResetDefault;
                                        break;

                                    case 2:
                                        customSizes = true;
                                        break;
                                    }

                                    break;

                                //x=left x coordinates for section [id][***urlencode!***][syntax]
                                case 2:
                                    if(customSizes)
                                    {
                                        eventdata.sets[id].expression_pos_x = PGE_URLDEC(eLine);
                                        SMBX38A_Exp2Int(eventdata.sets[id].expression_pos_x, eventdata.sets[id].position_left);
                                    }

                                    break;

                                //y=top y coordinates for section [id][***urlencode!***][syntax]
                                case 3:
                                    if(customSizes)
                                    {
                                        eventdata.sets[id].expression_pos_y = PGE_URLDEC(eLine);
                                        SMBX38A_Exp2Int(eventdata.sets[id].expression_pos_y, eventdata.sets[id].position_top);
                                    }
                                    else
                                        eventdata.sets[id].position_top = -1;

                                    break;

                                //w=width for section [id][***urlencode!***][syntax]
                                case 4:
                                    if(customSizes)
                                    {
                                        eventdata.sets[id].expression_pos_w = PGE_URLDEC(eLine);
                                        SMBX38A_Exp2Int(eventdata.sets[id].expression_pos_w, eventdata.sets[id].position_right);

                                        if(IsEmpty(eventdata.sets[id].expression_pos_w))
                                            eventdata.sets[id].position_right += eventdata.sets[id].position_left;
                                    }
                                    else
                                        eventdata.sets[id].position_right = -1;

                                    break;

                                //h=height for section [id][***urlencode!***][syntax]
                                case 5:
                                    if(customSizes)
                                    {
                                        eventdata.sets[id].expression_pos_h = PGE_URLDEC(eLine);
                                        SMBX38A_Exp2Int(eventdata.sets[id].expression_pos_h, eventdata.sets[id].position_bottom);

                                        if(IsEmpty(eventdata.sets[id].expression_pos_h))
                                            eventdata.sets[id].position_bottom += eventdata.sets[id].position_top;
                                    }
                                    else
                                        eventdata.sets[id].position_bottom = -1;

                                    break;

                                //auto=enable autoscroll controls[0=false !0=true]
                                case 6:
                                    autoscroll = (eLine != "0");
                                    eventdata.sets[id].autoscrol = autoscroll;
                                    break;

                                //sx=move screen horizontal syntax[***urlencode!***][syntax]
                                case 7:
                                    if(autoscroll)
                                    {
                                        eventdata.sets[id].expression_autoscrool_x = PGE_URLDEC(eLine);
                                        SMBX38A_Exp2Float(eventdata.sets[id].expression_autoscrool_x, eventdata.sets[id].autoscrol_x);
                                        eventdata.scroll_section = id;
                                        eventdata.move_camera_x = eventdata.sets[id].autoscrol_x;
                                    }
                                    else
                                    {
                                        eventdata.sets[id].autoscrol_x = 0.f;
                                        eventdata.scroll_section = id;
                                        eventdata.move_camera_x = 0.f;
                                    }

                                    break;

                                //sy=move screen vertical syntax[***urlencode!***][syntax]
                                case 8:
                                    if(autoscroll)
                                    {
                                        eventdata.sets[id].expression_autoscrool_y = PGE_URLDEC(eLine);
                                        SMBX38A_Exp2Float(eventdata.sets[id].expression_autoscrool_y, eventdata.sets[id].autoscrol_y);
                                        eventdata.scroll_section = id;
                                        eventdata.move_camera_y = eventdata.sets[id].autoscrol_y;
                                    }
                                    else
                                    {
                                        eventdata.sets[id].autoscrol_y = 0.f;
                                        eventdata.scroll_section = id;
                                        eventdata.move_camera_y = 0.f;
                                    }

                                    break;
                                }
                            }
                        }

                        //BACKGROUNDS
                        if(j < (signed)ev_bgs.size())
                        {
                            PGESTRINGList params;
                            old38a_SplitCSVStr(params, ev_bgs[j]);
                            //eb=id,btype,backgroundid
                            int id = -1;
                            bool customBg = false;

                            for(int k = 0; k < (signed)params.size(); k++)
                            {
                                if((k > 0) &&
                                   ((id < 0) || (id >= (signed)eventdata.sets.size()))
                                  )//Append sections
                                {
                                    if(id < 0) goto badfile; //Missmatched section ID!

                                    int last = eventdata.sets.size() - 1;

                                    while(id >= (signed)eventdata.sets.size())
                                    {
                                        LevelEvent_Sets set;
                                        set.id = last;
                                        eventdata.sets.push_back(set);
                                        last++;
                                    }
                                }

                                PGESTRING &eLine = params[k];

                                switch(k)
                                {
                                //id=section id
                                case 0:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    id = toInt(eLine) - 1;
                                    break;

                                //btype=[0=don't change][1=default][2=custom]
                                case 1:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    switch(toInt(eLine))
                                    {
                                    case 0:
                                        eventdata.sets[id].background_id = LevelEvent_Sets::LESet_Nothing;
                                        break;

                                    case 1:
                                        eventdata.sets[id].background_id = LevelEvent_Sets::LESet_ResetDefault;
                                        break;

                                    case 2:
                                        customBg = true;
                                        eventdata.sets[id].background_id = 0;
                                        break;
                                    }

                                    break;

                                //backgroundid=[when btype=2]custom background id
                                case 2:
                                    if(customBg)
                                    {
                                        if(!SMBX64::IsFloat(eLine))
                                            goto badfile;

                                        eventdata.sets[id].background_id = (long)round(toFloat(eLine));
                                        SMBX38A_mapBGID_From(eventdata.sets[id].background_id);//Convert into SMBX64 ID set
                                    }

                                    break;
                                }
                            }
                        }

                        //em=id,mtype,musicid,customfile
                        //  id=section id
                        //  mtype=[0=don't change][1=default][2=custom]
                        //  musicid=[when mtype=2]custom music id
                        //  customfile=[when mtype=3]custom music file name[***urlencode!***]
                        //MUSICS
                        if(j < (signed)ev_musics.size())
                        {
                            PGESTRINGList params;
                            old38a_SplitCSVStr(params, ev_musics[j]);
                            //em=id,mtype,musicid,customfile
                            int id = -1;
                            bool customMusics = false;

                            for(int k = 0; k < (signed)params.size(); k++)
                            {
                                if((k > 0) &&
                                   ((id < 0) || (id >= (signed)eventdata.sets.size()))
                                  )//Append sections
                                {
                                    if(id < 0) goto badfile; //Missmatched section ID!

                                    int last = eventdata.sets.size() - 1;

                                    while(id >= (signed)eventdata.sets.size())
                                    {
                                        LevelEvent_Sets set;
                                        set.id = last;
                                        eventdata.sets.push_back(set);
                                        last++;
                                    }
                                }

                                PGESTRING &eLine = params[k];

                                switch(k)
                                {
                                //id=section id
                                case 0:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    id = toInt(eLine) - 1;
                                    break;

                                //mtype=[0=don't change][1=default][2=custom]
                                case 1:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    switch(toInt(eLine))
                                    {
                                    case 0:
                                        eventdata.sets[id].music_id = LevelEvent_Sets::LESet_Nothing;
                                        break;

                                    case 1:
                                        eventdata.sets[id].music_id = LevelEvent_Sets::LESet_ResetDefault;
                                        break;

                                    case 2:
                                    default:
                                        customMusics = true;
                                        eventdata.sets[id].music_id = 0;
                                        break;
                                    }

                                    break;

                                //musicid=[when mtype=2]custom music id
                                case 2:
                                    if(customMusics)
                                    {
                                        if(!SMBX64::IsFloat(eLine))
                                            goto badfile;

                                        eventdata.sets[id].music_id = (long)round(toDouble(eLine));
                                    }

                                    break;

                                case 3:
                                    if(customMusics)
                                    {
                                        eventdata.sets[id].music_file = PGE_URLDEC(eLine);

                                        if(eventdata.sets[id].music_file == "0")
                                            eventdata.sets[id].music_file.clear();
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
                break;

                //eef=sound/endgame/ce1/ce2...cen
                case 8:
                {
                    PGESTRINGList Effects;
                    SMBX38A_SplitSubLine(Effects, cLine);

                    for(int j = 0; j < (signed)Effects.size(); j++)
                    {
                        PGESTRING &dLine = Effects[j];

                        switch(j)
                        {
                        //sound=play sound number
                        case 0:
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;

                            eventdata.sound_id = toInt(dLine);
                            break;

                        //    endgame=[0=none][1=bowser defeat]
                        case 1:
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;

                            eventdata.end_game = toInt(dLine);
                            break;

                        default:
                        {
                            LevelEvent_SpawnEffect effect;
                            PGESTRINGList EffectsToSpawn;
                            old38a_SplitCSVStr(EffectsToSpawn, dLine);

                            for(int k = 0; k < (signed)EffectsToSpawn.size(); k++)
                            {
                                //ce(n)=id,x,y,sx,sy,grv,fsp,life
                                PGESTRING &eLine = EffectsToSpawn[k];

                                switch(k)
                                {
                                //        id=effect id
                                case 0:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    effect.id = toInt(eLine);
                                    break;

                                //        x=effect position x[***urlencode!***][syntax]
                                case 1:
                                    effect.expression_x = PGE_URLDEC(eLine);
                                    SMBX38A_Exp2Int(effect.expression_x, effect.x);
                                    break;

                                //        y=effect position y[***urlencode!***][syntax]
                                case 2:
                                    effect.expression_y = PGE_URLDEC(eLine);
                                    SMBX38A_Exp2Int(effect.expression_y, effect.y);
                                    break;

                                //        sx=effect horizontal speed[***urlencode!***][syntax]
                                case 3:
                                    effect.expression_sx = PGE_URLDEC(eLine);
                                    SMBX38A_Exp2Double(effect.expression_sx, effect.speed_x);
                                    break;

                                //        sy=effect vertical speed[***urlencode!***][syntax]
                                case 4:
                                    effect.expression_sy = PGE_URLDEC(eLine);
                                    SMBX38A_Exp2Double(effect.expression_sy, effect.speed_y);
                                    break;

                                //        grv=to decide whether the effects are affected by gravity[0=false !0=true]
                                case 5:
                                    effect.gravity = (eLine != "0");
                                    break;

                                //        fsp=frame speed of effect generated
                                case 6:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    effect.fps = toInt(eLine);
                                    break;

                                //        life=effect existed over this time will be destroyed.
                                case 7:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    effect.max_life_time = toInt(eLine);
                                    break;
                                }
                            }

                            eventdata.spawn_effects.push_back(effect);
                        }
                        break;
                        }
                    }
                }
                break;

                //ecn=cn1/cn2...cnn
                case 9:
                {
                    PGESTRINGList SpawnNPCs;
                    SMBX38A_SplitSubLine(SpawnNPCs, cLine);

                    //cn(n)=id,x,y,sx,sy,sp
                    for(int j = 0; j < (signed)SpawnNPCs.size(); j++)
                    {
                        LevelEvent_SpawnNPC spawnnpc;
                        PGESTRING &dLine = SpawnNPCs[j];
                        PGESTRINGList SpawnNPC;
                        old38a_SplitCSVStr(SpawnNPC, dLine);

                        for(int k = 0; k < (signed)SpawnNPC.size(); k++)
                        {
                            PGESTRING &eLine = SpawnNPC[k];

                            switch(k)
                            {
                            //id=npc id
                            case 0:
                                if(!SMBX64::IsUInt(eLine))
                                    goto badfile;

                                spawnnpc.id = toInt(eLine);
                                break;

                            //x=npc position x[***urlencode!***][syntax]
                            case 1:
                                spawnnpc.expression_x = PGE_URLDEC(eLine);
                                SMBX38A_Exp2Int(spawnnpc.expression_x, spawnnpc.x);
                                break;

                            //y=npc position y[***urlencode!***][syntax]
                            case 2:
                                spawnnpc.expression_y = PGE_URLDEC(eLine);
                                SMBX38A_Exp2Int(spawnnpc.expression_y, spawnnpc.y);
                                break;

                            //sx=npc horizontal speed[***urlencode!***][syntax]
                            case 3:
                                spawnnpc.expression_sx = PGE_URLDEC(eLine);
                                SMBX38A_Exp2Double(spawnnpc.expression_sx, spawnnpc.speed_x);
                                break;

                            //sy=npc vertical speed[***urlencode!***][syntax]
                            case 4:
                                spawnnpc.expression_sy = PGE_URLDEC(eLine);
                                SMBX38A_Exp2Double(spawnnpc.expression_sy, spawnnpc.speed_y);
                                break;

                            //sp=advanced settings of generated npc
                            case 5:
                                if(!SMBX64::IsSInt(eLine))
                                    goto badfile;

                                spawnnpc.special = toInt(eLine);
                                break;
                            }
                        }

                        eventdata.spawn_npc.push_back(spawnnpc);
                    }
                }
                break;

                //evc=vc1/vc2...vcn
                case 10:
                {
                    LevelEvent_UpdateVariable updVar;
                    PGESTRINGList updVars;
                    old38a_SplitCSVStr(updVars, cLine);

                    //    vc(n)=name,newvalue
                    for(int j = 0; j < (signed)updVars.size(); j++)
                    {
                        PGESTRING &dLine = updVars[j];

                        switch(j)
                        {
                        //name=variable name[***urlencode!***]
                        case 0:
                            updVar.name = PGE_URLDEC(dLine);
                            break;

                        //newvalue=new value[***urlencode!***][syntax]
                        case 1:
                            updVar.newval = PGE_URLDEC(dLine);
                            break;
                        }
                    }

                    if(!IsEmpty(updVar.name))
                        eventdata.update_variable.push_back(updVar);
                }
                break;

                //ene=nextevent/timer/apievent/scriptname
                case 11:
                {
                    PGESTRINGList extraProps;
                    SMBX38A_SplitSubLine(extraProps, cLine);

                    for(int j = 0; j < (signed)extraProps.size(); j++)
                    {
                        PGESTRING &dLine = extraProps[j];

                        switch(j)
                        {
                        //nextevent=name,delay
                        case 0:
                        {
                            PGESTRINGList TriggerEvent;
                            old38a_SplitCSVStr(TriggerEvent, dLine);

                            for(int k = 0; k < (signed)TriggerEvent.size(); k++)
                            {
                                PGESTRING &eLine = TriggerEvent[k];

                                switch(k)
                                {
                                //name=trigger event name[***urlencode!***]
                                case 0:
                                    eventdata.trigger = PGE_URLDEC(eLine);
                                    break;

                                //delay=trigger delay[1 frame]
                                case 1:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    //Convert 1/65 seconds into 1/10 seconds for SMBX-64 Standard
                                    eventdata.trigger_timer = (long)round(SMBX64::t65_to_ms(toFloat(eLine)) / 100.0);
                                    break;
                                }
                            }
                        }
                        break;

                        //timer=enable,count,interval,type,show
                        case 1:
                        {
                            PGESTRINGList Timer;
                            old38a_SplitCSVStr(Timer, dLine);

                            for(int k = 0; k < (signed)Timer.size(); k++)
                            {
                                PGESTRING &eLine = Timer[k];

                                switch(k)
                                {
                                //enable=enable the game timer controlling[0=false !0=true]
                                case 0:
                                    eventdata.timer_def.enable = (eLine != "0");
                                    break;

                                //count=set the time left of the game timer
                                case 1:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    //Convert 1/65 seconds into milliseconds units
                                    eventdata.timer_def.count = toInt(eLine);
                                    break;

                                //interval=set the time count interval of the game timer
                                case 2:
                                    if(!SMBX64::IsFloat(eLine))
                                        goto badfile;

                                    //Convert 1/65 seconds into milliseconds units
                                    eventdata.timer_def.interval = SMBX64::t65_to_ms(toDouble(eLine));
                                    break;

                                //type=to choose the way timer counts[0=counting down][1=counting up]
                                case 3:
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;

                                    eventdata.timer_def.count_dir = toInt(eLine);
                                    break;

                                //show=to choose whether the game timer is showed in hud[0=false !0=true]
                                case 4:
                                    eventdata.timer_def.show = (eLine != "0");
                                    break;
                                }
                            }
                        }
                        break;

                        //    apievent=the id of apievent
                        case 2:
                            if(!SMBX64::IsUInt(dLine))
                                goto badfile;

                            eventdata.trigger_api_id = toInt(dLine);
                            break;

                        //    scriptname=script name[***urlencode!***]
                        case 3:
                            eventdata.trigger_script = PGE_URLDEC(dLine);
                            break;
                        }
                    }
                }
                break;
                }
            }

            eventdata.meta.array_id = FileData.events_array_id++;
            FileData.events.push_back(eventdata);
        }
        else if(currentLine[0] == "V") //next line: variables
        {
            //V|name|value
            vardata = CreateLvlVariable("var");

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //name=variable name[***urlencode!***]
                case 1:
                {
                    vardata.name = PGE_URLDEC(cLine);
                }
                break;

                //value=initial value of the variable
                case 2:
                {
                    if(!SMBX64::IsUInt(cLine))
                        goto badfile;
                    else vardata.value = cLine; /*save variable value as string

                          because in PGE is planned to have
                          variables to be universal*/
                }
                break;
                }
            }

            FileData.variables.push_back(vardata);
        }
        else if(currentLine[0] == "S") //next line: scripts
        {
            //S|name|script
            scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //name=name of script[***urlencode!***]
                case 1:
                {
                    scriptdata.name = PGE_URLDEC(cLine);
                }
                break;

                //script=script[***base64encode!***][utf-8]
                case 2:
                {
                    scriptdata.script = PGE_BASE64DEC(cLine);
                }
                break;
                }
            }

            FileData.variables.push_back(vardata);
        }
        else if(currentLine[0] == "Su") //next line: scripts (saved as ASCII)
        {
            //Su|name|scriptu
            scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);

            for(int i = 1; i < (signed)currentLine.size(); i++)
            {
                PGESTRING &cLine = currentLine[i];

                switch(i)
                {
                //name=name of script[***urlencode!***]
                case 1:
                {
                    scriptdata.name = PGE_URLDEC(cLine);
                }
                break;

                //scriptu=script[***base64encode!***][ASCII]
                case 2:
                {
                    scriptdata.script = PGE_BASE64DEC_A(cLine);
                    PGE_ReplSTRING(scriptdata.script, "\r\n", "\n");
                }
                break;
                }
            }

            FileData.scripts.push_back(scriptdata);
        }
    }//while is not EOF

    LevelAddInternalEvents(FileData);
    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(file_format) + "format";
    FileData.meta.ERROR_linenum = in.getCurrentLineNumber();
    FileData.meta.ERROR_linedata = line;
    return false;
}
