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
//static constexpr uint32_t newest_file_format = 68; //TODO: Uncomment when implement 38A World writing support

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX38AWldFileHeader(PGESTRING filePath, WorldData& FileData)
{
    errorString.clear();
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = WorldData::SMBX38A;
    #if !defined(_MSC_VER) || _MSC_VER > 1800
    PGE_FileFormats_misc::TextFileInput inf;

    if(!inf.open(filePath, false))
    {
        FileData.meta.ReadFileValid = false;
        return false;
    }

    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.meta.filename = in_1.basename();
    FileData.meta.path = in_1.dirpath();
    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    uint32_t file_version = 0;

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

            if(identifier == "WS1")
            {
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        //  wn=episode name[***urlencode!***]
                                        MakeCSVPostProcessor(&FileData.EpisodeTitle, PGEUrlDecodeFunc),
                                        //  bp(n)=don't use player(n) as player's character
                                        MakeCSVSubReader(dataReader, ',',
                                                    MakeCSVOptional(&FileData.nocharacter1, false),
                                                    MakeCSVOptional(&FileData.nocharacter2, false),
                                                    MakeCSVOptional(&FileData.nocharacter3, false),
                                                    MakeCSVOptional(&FileData.nocharacter4, false),
                                                    MakeCSVOptional(&FileData.nocharacter5, false)
                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  asn=auto start level file name[***urlencode!***]
                                                        MakeCSVPostProcessor(&FileData.IntroLevel_file, PGEUrlDecodeFunc),
                                                        //  gon=game over level file name[***urlencode!***]
                                                        MakeCSVOptional(&FileData.GameOverLevel_file, "", nullptr, PGEUrlDecodeFunc)
                                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  dtp=disable two player[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictSinglePlayer, false),
                                                        //  nwm=no world map[0=false !0=true]
                                                        MakeCSVOptional(&FileData.HubStyledWorld, false),
                                                        //  rsd=restart last level on player's character death[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restartlevel, false),
                                                        //  dcp=disable change player[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictCharacterSwitch, false),
                                                        //  sc=save machine code to sav file[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictSecureGameSave, false),
                                                        //  sm=save mode
                                                        MakeCSVOptional(&FileData.saveResumePolicy, 0),
                                                        //  asg=auto save game[0=false !0=true]
                                                        MakeCSVOptional(&FileData.saveAuto, false),
                                                        //  smb3=smb3 style world map[0=false !0=true]
                                                        MakeCSVOptional(&FileData.showEverything, false),
                                                        //  dss=No Entry Scene
                                                        MakeCSVOptional(&FileData.disableEnterScreen, false)
                                                    ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  sn=star number
                                                        MakeCSVOptional(&FileData.stars, 0),
                                                        //  mis=max item number in world inventory
                                                        MakeCSVOptional(&FileData.inventoryLimit, 0)
                                                        ),
                                                        //  acm=anti cheat mode[0=don't allow in list !0=allow in list]
                                                        &FileData.cheatsPolicy,
                                                        //  sc=enable save locker[0=false !0=true]
                                                        &FileData.saveLocker
                                        );
                FileData.charactersFromS64();
            }
            else if(identifier == "WS2")
            {
                dataReader.ReadDataLine(CSVDiscard(),
                                        //  credits=[1]
                                        //  #DEFT#xxxxxx[***base64encode!***]
                                        //  xxxxxx=name1 /n name2 /n ...
                                        //  [2]
                                        //  #CUST#xxxxxx[***base64encode!***]
                                        //  xxxxxx=any string
                                        MakeCSVPostProcessor(&FileData.authors, [](PGESTRING& value)
                                        {
                                            PGESTRING prefix = PGE_SubStr(value, 0, 6);
                                            if((prefix == "#DEFT#") || (prefix == "#CUST#"))
                                                PGE_RemStrRng(value, 0, 6);
                                            value = PGE_BASE64DEC(value);
                                        })
                                        );
            }
            else if(identifier == "WS3")
            {
                PGESTRING cheatsList;
                dataReader.ReadDataLine(CSVDiscard(),
                                        //  list=xxxxxx[***base64encode!***] (list of forbidden)
                                        //          xxxxxx=string1,string2...stringn
                                        MakeCSVPostProcessor(&cheatsList, [&](PGESTRING& value)
                                        {
                                            PGESTRING list = PGE_URLDEC(value);
                                            PGE_SPLITSTRING(FileData.cheatsList, list, ",");
                                        })
                                        );
            }
            else if(identifier == "WS4")
            {
                dataReader.ReadDataLine(CSVDiscard(),
                                        //    se=save locker syntax[***urlencode!***][syntax]
                                        MakeCSVPostProcessor(&FileData.saveLockerEx, PGEUrlDecodeFunc),
                                        //    msg=message when save was locked[***urlencode!***]
                                        MakeCSVPostProcessor(&FileData.saveLockerMsg, PGEUrlDecodeFunc)
                                        );
            }
            else
            {
                break; //Abort on first non-head line to don't fetch entire file
                //dataReader.ReadDataLine();
            }
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

    FileData.CurSection = 0;
    FileData.playmusic = 0;
    return true;
    #else
    FileData.meta.ReadFileValid = false;
    FileData.meta.ERROR_info = "Unsupported on MSVC2013";
    return false;
    #endif
}

bool FileFormats::ReadSMBX38AWldFileF(PGESTRING filePath, WorldData& FileData)
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

    return ReadSMBX38AWldFile(file, FileData);
}

bool FileFormats::ReadSMBX38AWldFileRaw(PGESTRING& rawdata, PGESTRING filePath, WorldData& FileData)
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

    return ReadSMBX38AWldFile(file, FileData);
}

bool FileFormats::ReadSMBX38AWldFile(PGE_FileFormats_misc::TextInput& in, WorldData& FileData)
{
    SMBX38A_FileBeginN();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();

    CreateWorldData(FileData);

    FileData.meta.RecentFormat = WorldData::SMBX38A;

    #if !defined(_MSC_VER) || _MSC_VER > 1800
    FileData.EpisodeTitle = "" ;
    FileData.stars = 0;
    FileData.CurSection = 0;
    FileData.playmusic = 0;

    //Enable strict mode for SMBX LVL file format
    FileData.meta.smbx64strict = false;

    //Begin all ArrayID's here;
    FileData.tile_array_id = 1;
    FileData.scene_array_id = 1;
    FileData.path_array_id = 1;
    FileData.level_array_id = 1;
    FileData.musicbox_array_id = 1;

    WorldTerrainTile    tile;
    WorldScenery        scen;
    WorldPathTile       pathitem;
    WorldLevelTile      lvlitem;
    WorldMusicBox       musicbox;
    WorldAreaRect       arearect;
    WorldLayer          layer;
    WorldEvent38A       event;
    WorldItemSetup38A   customcfg;

    PGESTRING           identifier;
    uint32_t            file_version = 0;

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

            if(identifier == "WS1")
            {
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        //  wn=episode name[***urlencode!***]
                                        MakeCSVPostProcessor(&FileData.EpisodeTitle, PGEUrlDecodeFunc),
                                        //  bp(n)=don't use player(n) as player's character
                                        MakeCSVSubReader(dataReader, ',',
                                                    MakeCSVOptional(&FileData.nocharacter1, false),
                                                    MakeCSVOptional(&FileData.nocharacter2, false),
                                                    MakeCSVOptional(&FileData.nocharacter3, false),
                                                    MakeCSVOptional(&FileData.nocharacter4, false),
                                                    MakeCSVOptional(&FileData.nocharacter5, false)
                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  asn=auto start level file name[***urlencode!***]
                                                        MakeCSVPostProcessor(&FileData.IntroLevel_file, PGEUrlDecodeFunc),
                                                        //  gon=game over level file name[***urlencode!***]
                                                        MakeCSVOptional(&FileData.GameOverLevel_file, "", nullptr, PGEUrlDecodeFunc)
                                                        ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  dtp=disable two player[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictSinglePlayer, false),
                                                        //  nwm=no world map[0=false !0=true]
                                                        MakeCSVOptional(&FileData.HubStyledWorld, false),
                                                        //  rsd=restart last level on player's character death[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restartlevel, false),
                                                        //  dcp=disable change player[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictCharacterSwitch, false),
                                                        //  sc=save machine code to sav file[0=false !0=true]
                                                        MakeCSVOptional(&FileData.restrictSecureGameSave, false),
                                                        //  sm=save mode
                                                        MakeCSVOptional(&FileData.saveResumePolicy, 0),
                                                        //  asg=auto save game[0=false !0=true]
                                                        MakeCSVOptional(&FileData.saveAuto, false),
                                                        //  smb3=smb3 style world map[0=false !0=true]
                                                        MakeCSVOptional(&FileData.showEverything, false),
                                                        //  dss=No Entry Scene
                                                        MakeCSVOptional(&FileData.disableEnterScreen, false)
                                                    ),
                                        MakeCSVSubReader(dataReader, ',',
                                                        //  sn=star number
                                                        MakeCSVOptional(&FileData.stars, 0),
                                                        //  mis=max item number in world inventory
                                                        MakeCSVOptional(&FileData.inventoryLimit, 0)
                                                        ),
                                                        //  acm=anti cheat mode[0=don't allow in list !0=allow in list]
                                                        &FileData.cheatsPolicy,
                                                        //  sc=enable save locker[0=false !0=true]
                                                        &FileData.saveLocker
                                        );
                FileData.charactersFromS64();
            }
            else if(identifier == "WS2")
            {
                dataReader.ReadDataLine(CSVDiscard(),
                                        //  credits=[1]
                                        //  #DEFT#xxxxxx[***base64encode!***]
                                        //  xxxxxx=name1 /n name2 /n ...
                                        //  [2]
                                        //  #CUST#xxxxxx[***base64encode!***]
                                        //  xxxxxx=any string
                                        MakeCSVPostProcessor(&FileData.authors, [](PGESTRING& value)
                                        {
                                            PGESTRING prefix = PGE_SubStr(value, 0, 6);
                                            if((prefix == "#DEFT#") || (prefix == "#CUST#"))
                                                PGE_RemStrRng(value, 0, 6);
                                            value = PGE_BASE64DEC(value);
                                        })
                                        );
            }
            else if(identifier == "WS3")
            {
                PGESTRING cheatsList;
                dataReader.ReadDataLine(CSVDiscard(),
                                        //  list=xxxxxx[***base64encode!***] (list of forbidden)
                                        //          xxxxxx=string1,string2...stringn
                                        MakeCSVPostProcessor(&cheatsList, [&](PGESTRING& value)
                                        {
                                            PGESTRING list = PGE_URLDEC(value);
                                            PGE_SPLITSTRING(FileData.cheatsList, list, ",");
                                        })
                                        );
            }
            else if(identifier == "WS4")
            {
                dataReader.ReadDataLine(CSVDiscard(),
                                        //    se=save locker syntax[***urlencode!***][syntax]
                                        MakeCSVPostProcessor(&FileData.saveLockerEx, PGEUrlDecodeFunc),
                                        //    msg=message when save was locked[***urlencode!***]
                                        MakeCSVPostProcessor(&FileData.saveLockerMsg, PGEUrlDecodeFunc)
                                        );
            }
            else if(identifier == "T")
            {
                tile = WorldTerrainTile();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &tile.id,
                                                         MakeCSVOptional(&tile.gfx_dx, 0),
                                                         MakeCSVOptional(&tile.gfx_dy, 0)
                                                         ),
                                        &tile.x,
                                        &tile.y,
                                        MakeCSVPostProcessor(&tile.layer, PGELayerOrDefault)
                                        );
                tile.meta.array_id = FileData.tile_array_id++;
                FileData.tiles.push_back(tile);
            }
            else if(identifier == "S")
            {
                scen = WorldScenery();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &scen.id,
                                                         MakeCSVOptional(&scen.gfx_dx, 0),
                                                         MakeCSVOptional(&scen.gfx_dy, 0)
                                                         ),
                                        &scen.x,
                                        &scen.y,
                                        MakeCSVPostProcessor(&scen.layer, PGELayerOrDefault)
                                        );
                scen.meta.array_id = FileData.scene_array_id++;
                FileData.scenery.push_back(scen);
            }
            else if(identifier == "P")
            {
                pathitem = WorldPathTile();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &pathitem.id,
                                                         MakeCSVOptional(&pathitem.gfx_dx, 0),
                                                         MakeCSVOptional(&pathitem.gfx_dy, 0)
                                                         ),
                                        &pathitem.x,
                                        &pathitem.y,
                                        MakeCSVPostProcessor(&pathitem.layer, PGELayerOrDefault)
                                        );
                pathitem.meta.array_id = FileData.path_array_id++;
                FileData.paths.push_back(pathitem);
            }
            else if(identifier == "M")
            {
                musicbox = WorldMusicBox();
                arearect = WorldAreaRect();
                arearect.flags = WorldAreaRect::SETUP_CHANGE_MUSIC;
                //M|10|416|1312|    |     |32|32|1   |,0
                //M|1 |384|384 |    |     |32|32|1   |%66%61%72%74,1
                //M|id|x  |y   |name|layer|w |h |flag|te,eflag      |ie1,ie2,ie3
                dataReader.ReadDataLine(CSVDiscard(),
                                        //id=music id
                                        &arearect.music_id,
                                        //x=Area position x
                                        &arearect.x,
                                        //y=Area position y
                                        &arearect.y,
                                        //name=custom music name[***urlencode!***]
                                        MakeCSVPostProcessor(&arearect.music_file, PGEUrlDecodeFunc),
                                        //layer=layer name["" == "Default"][***urlencode!***]
                                        MakeCSVOptional(&arearect.layer, "Default", nullptr, PGELayerOrDefault),
                                        //w=width
                                        MakeCSVOptional(&arearect.w, 32),
                                        //h=height
                                        MakeCSVOptional(&arearect.h, 32),
                                        //flag=area settings[***Bitwise operation***]
                                        //    0=False !0=True
                                        //    b1=(flag & 1) World Music
                                        //    b2=(flag & 2) Set Viewport
                                        //    b3=(flag & 4) Ship Route
                                        //    b4=(flag & 8) Forced Walking
                                        //    b5=(flag & 16) Item-triggered events
                                        MakeCSVOptional(&arearect.flags, WorldAreaRect::SETUP_CHANGE_MUSIC),
                                        MakeCSVOptionalSubReader(dataReader, ',',
                                                                 //te:Touch Event[***urlencode!***]
                                                                 MakeCSVOptional(&arearect.eventTouch, "", nullptr, PGELayerOrDefault),
                                                                 //eflag:    0=Triggered every time entering
                                                                 //          1=Triggered on entrance and level completion
                                                                 //          2=Triggered only once
                                                                 MakeCSVOptional(&arearect.eventTouchPolicy, 0)
                                                                 ),
                                        MakeCSVOptionalSubReader(dataReader, ',',
                                                                 //ie1=Hammer Event[***urlencode!***]
                                                                 MakeCSVOptional(&arearect.eventBreak, "", nullptr, PGELayerOrDefault),
                                                                 //ie2=Warp Whistle Event[***urlencode!***]
                                                                 MakeCSVOptional(&arearect.eventWarp, "", nullptr, PGELayerOrDefault),
                                                                 //ie3=Anchor Event[***urlencode!***]
                                                                 MakeCSVOptional(&arearect.eventAnchor, "", nullptr, PGELayerOrDefault)
                                                                 )
                                        );
                if((arearect.flags == WorldAreaRect::SETUP_CHANGE_MUSIC) &&
                   (arearect.w == 32) && (arearect.h == 32))
                {
                    //Store as generic music-box point
                    musicbox.id         = arearect.music_id;
                    musicbox.music_file = arearect.music_file;
                    musicbox.x          = arearect.x;
                    musicbox.y          = arearect.y;
                    musicbox.layer      = arearect.layer;
                    musicbox.meta.array_id = FileData.musicbox_array_id++;
                    FileData.music.push_back(musicbox);
                }
                else
                {
                    //Store as separated "Area-rect" type
                    arearect.meta.array_id = FileData.arearect_array_id++;
                    FileData.arearects.push_back(arearect);
                }
            }
            else if(identifier == "L")
            {
                //L|id[,dx,dy]|x|y|fn|n|eu\el\ed\er|wx|wy|wlz|bg,pb,av,ls,f,nsc,otl,li,lcm|s|Layer|Lmt
                lvlitem = WorldLevelTile();
                lvlitem.left_exit_extra.exit_codes = {0, 0};
                lvlitem.top_exit_extra.exit_codes = {0, 0};
                lvlitem.right_exit_extra.exit_codes = {0, 0};
                lvlitem.bottom_exit_extra.exit_codes = {0, 0};
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVSubReader(dataReader, ',',
                                                         //id=level id
                                                         &lvlitem.id,
                                                         //dx=graphics extend x
                                                         MakeCSVOptional(&lvlitem.gfx_dx, 0),
                                                         //dx=graphics extend y
                                                         MakeCSVOptional(&lvlitem.gfx_dy, 0)
                                                         ),
                                        //x=level position x
                                        &lvlitem.x,
                                        //x=level position y
                                        &lvlitem.y,
                                        //fn=level file name[***urlencode!***]
                                        MakeCSVPostProcessor(&lvlitem.lvlfile,  PGEUrlDecodeFunc),
                                        //n=level name[***urlencode!***]
                                        MakeCSVPostProcessor(&lvlitem.title,    PGEUrlDecodeFunc),
                                        //eu\el\ed\er=e[up\left\down\right]
                                        //        e=c1,c2,c3,c4
                                        //        c1,c2,c3=level exit type
                                        //        c4=condidtion expression[***urlencode!***][syntax]
                                        //        exit = (c1 || c2 || c3) && c4
                                        MakeCSVSubReader(dataReader, '\\',
                                                        MakeCSVSubReader(dataReader, ','
                                                                         &lvlitem.top_exit,
                                                                         &lvlitem.top_exit_extra.exit_codes[0],
                                                                         &lvlitem.top_exit_extra.exit_codes[1],
                                                                         MakeCSVPostProcessor(&lvlitem.top_exit_extra.expression, PGEUrlDecodeFunc)
                                                                         ),
                                                        MakeCSVSubReader(dataReader, ','
                                                                         &lvlitem.left_exit,
                                                                         &lvlitem.left_exit_extra.exit_codes[0],
                                                                         &lvlitem.left_exit_extra.exit_codes[1],
                                                                         MakeCSVPostProcessor(&lvlitem.left_exit_extra.expression, PGEUrlDecodeFunc)
                                                                         ),
                                                        MakeCSVSubReader(dataReader, ','
                                                                         &lvlitem.bottom_exit,
                                                                         &lvlitem.bottom_exit_extra.exit_codes[0],
                                                                         &lvlitem.bottom_exit_extra.exit_codes[1],
                                                                         MakeCSVPostProcessor(&lvlitem.bottom_exit_extra.expression, PGEUrlDecodeFunc)
                                                                         ),
                                                        MakeCSVSubReader(dataReader, ','
                                                                         &lvlitem.right_exit,
                                                                         &lvlitem.right_exit_extra.exit_codes[0],
                                                                         &lvlitem.right_exit_extra.exit_codes[1],
                                                                         MakeCSVPostProcessor(&lvlitem.right_exit_extra.expression, PGEUrlDecodeFunc)
                                                                         )
                                                         ),
                                        //wx=go to world map position x
                                        &lvlitem.gotox,
                                        //wx=go to world map position y
                                        &lvlitem.gotoy,
                                        //wlz=nunber of doors to warp
                                        &lvlitem.entertowarp,
                                        MakeCSVSubReader(dataReader, ',',
                                                         //bg=big background
                                                         MakeCSVOptional(&lvlitem.bigpathbg, false),
                                                         //pb=path background
                                                         MakeCSVOptional(&lvlitem.pathbg, false),
                                                         //av=always visible
                                                         MakeCSVOptional(&lvlitem.alwaysVisible, false),
                                                         //ls=is game start point
                                                         MakeCSVOptional(&lvlitem.gamestart, false),
                                                         //f=forced
                                                         MakeCSVOptional(&lvlitem.forceStart, false),
                                                         //nsc=no star coin count
                                                         MakeCSVOptional(&lvlitem.disableStarCoinsCount, false),
                                                         //otl=destory after clear
                                                         MakeCSVOptional(&lvlitem.destroyOnCompleting, false),
                                                         //li=level ID
                                                         MakeCSVOptional(&lvlitem.levelID, 0),
                                                         //lcm=Affected by Music Box
                                                         MakeCSVOptional(&lvlitem.controlledByAreaRects, false)
                                                         ),
                                        //TODO: Implement this
                                        //s=entrance syntax
                                        //        s=ds1/ds2...dsn
                                        //        ds=ds1,ds2[***urlencode!***][syntax]
                                        //        ds1=condidtion expression
                                        //        ds2=index
                                        MakeCSVOptionalIterator(dataReader, '/', [&lvlitem](const PGESTRING & nextFieldStr)
                                        {
                                            WorldLevelTile::EnterCondition e;
                                            auto fieldReader = MakeDirectReader(nextFieldStr);
                                            auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                            fullReader.ReadDataLine(
                                                        MakeCSVPostProcessor(&e.condition,  PGEUrlDecodeFunc),
                                                        MakeCSVPostProcessor(&e.levelIndex, PGEUrlDecodeFunc)
                                                        );
                                            lvlitem.enter_cond.push_back(e);
                                        }),
                                        //layer=layer name["" == "Default"][***urlencode!***]
                                        MakeCSVOptional(&lvlitem.layer, "Default", nullptr, PGELayerOrDefault),
                                        //TODO: Implement this
                                        //Lmt=Level Movement Command
                                        //    lmt=NodeInfo\PathInfo
                                        //        NodeInfo=Node1:Node2:...:NodeN
                                        //            Node=x,y,chance
                                        //        PathInfo=Path1:Path2:...:PathN
                                        //            Path=NodeID1,NodeID2
                                        MakeCSVOptionalSubReader(dataReader, '\\',
                                                                 MakeCSVOptionalIterator(dataReader, ':', [&lvlitem](const PGESTRING & nextFieldStr)
                                                                 {
                                                                     WorldLevelTile::Movement::Node node;
                                                                     auto fieldReader = MakeDirectReader(nextFieldStr);
                                                                     auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                                                     fullReader.ReadDataLine(
                                                                                 &node.x,
                                                                                 &node.y,
                                                                                 &node.chance
                                                                                 );
                                                                     lvlitem.movement.nodes.push_back(node);
                                                                 }),
                                                                 MakeCSVOptionalIterator(dataReader, ':', [&lvlitem](const PGESTRING & nextFieldStr)
                                                                 {
                                                                     WorldLevelTile::Movement::Line line;
                                                                     auto fieldReader = MakeDirectReader(nextFieldStr);
                                                                     auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                                                     fullReader.ReadDataLine(
                                                                                 &line.node1,
                                                                                 &line.node2
                                                                                 );
                                                                     lvlitem.movement.paths.push_back(line);
                                                                 })
                                                                 )
                                        );
                lvlitem.meta.array_id = FileData.level_array_id++;
                FileData.levels.push_back(lvlitem);
            }
            else if(identifier == "WL")
            {
                layer = WorldLayer();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&layer.name, PGELayerOrDefault),
                                        &layer.hidden
                                        );
                layer.meta.array_id = FileData.layers_array_id++;
                FileData.layers.push_back(layer);
            }
            else if(identifier == "WE")
            {
                event = WorldEvent38A();
                //TODO: Implement world map events support
                //next line: events
                //    WE|name|layer|layerm|world|other
                dataReader.ReadDataLine(CSVDiscard(),
                                        //    name=event name[***urlencode!***]
                                        MakeCSVPostProcessor(&event.name, PGELayerOrDefault)
                                        //    layer=way/hidelist/showlist/togglelist
                                        //        list=name1,name2,name3...namen
                                        //            name[***urlencode!***]
                                        //        if (way % 10 == 1) nosmoke = true;
                                        //        if (way > 10) object_state = true; else layer_state = true;
                                        //    layerm=movementcommand1\movementcommand2\...\movementcommandn
                                        //        movementcommand=way,layer,hp,vp,ap
                                        //            way:0=speed,1=coordinate,2=moveto,4=spin
                                        //            layer=layer name[***urlencode!***]
                                        //            hp=Horizontal Parameter[***urlencode!***]
                                        //            vp=Vertical Parameter[***urlencode!***]
                                        //            ap=Additional Parameter[***urlencode!***]
                                        //    world=aw/cs,le,inpc,msgc,syntax,msg
                                        //        aw=AutoStart Settings
                                        //            0=Not Auto Start
                                        //            1=Triggered on loading the world the first time.
                                        //            2=Triggered every time loading the world.
                                        //            3=Triggered on level exit.
                                        //        cs=Start when match all condition[0=false !0=true]
                                        //        le:0=This is a Normal Event.
                                        //           1=This is a Level Enter/Exit Event.
                                        //        inpc=Interrupt the process if 'false' returned
                                        //        msgc=Show a message if 'false' returned
                                        //        syntax=Condition expression[***urlencode!***]
                                        //        msg=message[***urlencode!***]
                                        //    other=sd/ld/event,delay/script/msg/wwx,wwy,lockl
                                        //        sd=play sound number
                                        //        ld=lock keyboard (frames)
                                        //        event=trigger event name[***urlencode!***]
                                        //        delay=trigger delay[1 frame]
                                        //        script=script name[***urlencode!***]
                                        //        msg=show message after start event[***urlencode!***]
                                        //        wwx=Warp Whistle: Map Warp Location x
                                        //        wwy=Warp Whistle: Map Warp Location y
                                        //            if (wwx == -1 && wwy == -1) [means not moving]
                                        //        lockl=[Level ID]Affected by Anchor
                                                            );
                event.meta.array_id = FileData.events38A_array_id++;
                FileData.events38A.push_back(event);
            }
            else if((identifier == "WCT") || (identifier == "WCS") || (identifier == "WCL") )
            {
                //custom object data:
                //    WCT|id|data	:custom tile
                //    WCS|id|data	:custom scene
                //    WCL|id|data	:custom level
                //    id=object id
                //    data=[HEX]value|[HEX]value...
                //    [HEX]=0001	:gfxwidth
                //    [HEX]=0002	:gfxheight
                //    [HEX]=0003	:frames
                customcfg = WorldItemSetup38A();
                if(identifier == "WCT")
                    customcfg.type = WorldItemSetup38A::TERRAIN;
                else if(identifier == "WCS")
                    customcfg.type = WorldItemSetup38A::SCENERY;
                else
                    customcfg.type = WorldItemSetup38A::LEVEL;

                dataReader.ReadDataLine(CSVDiscard(),
                                        &customcfg.id,
                                        MakeCSVIterator(dataReader, ',', [&customcfg](const PGESTRING & nextFieldStr)
                                                        {
                                                            WorldItemSetup38A::Entry e;
                                                            SMBX38A_CC_decode(e.key, e.value, nextFieldStr);
                                                            customcfg.data.push_back(e);
                                                        })
                                       );
                FileData.custom38A_configs.push_back(customcfg);
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

bool FileFormats::WriteSMBX38AWldFileF(PGESTRING filePath, WorldData& FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }

    return WriteSMBX38AWldFile(file, FileData);
}

bool FileFormats::WriteSMBX38AWldFileRaw(WorldData& FileData, PGESTRING& rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open raw string for write";
        return false;
    }

    return WriteSMBX38AWldFile(file, FileData);
}

bool FileFormats::WriteSMBX38AWldFile(PGE_FileFormats_misc::TextOutput& out, WorldData& FileData)
{
    (void)out;
    errorString = "Not implemented yet. Comming soon!";
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linedata = "";
    FileData.meta.ERROR_linenum = -1;
    FileData.meta.ReadFileValid = false;
    return false;
}

