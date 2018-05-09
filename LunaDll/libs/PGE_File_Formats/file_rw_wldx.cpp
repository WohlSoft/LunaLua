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
#include "file_strlist.h"
#include "wld_filedata.h"
#include "pge_x.h"
#include "pge_x_macro.h"
#include "pge_file_lib_sys.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

//WorldData FileFormats::ReadExtendedWorldFile(PGEFILE &inf)
//{
//    QTextStream in(&inf);   //Read File
//    in.setCodec("UTF-8");

//    return ReadExtendedWldFile( in.readAll(), inf.fileName() );
//}

bool FileFormats::ReadExtendedWldFileHeader(PGESTRING filePath, WorldData &FileData)
{
    CreateWorldHeader(FileData);
    FileData.meta.RecentFormat = WorldData::PGEX;
    PGE_FileFormats_misc::TextFileInput  inf;

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

    //Find level header part
    do
    {
        str_count++;
        line = inf.readLine();
    }
    while((line != "HEAD") && (!inf.eof()));

    PGESTRINGList header;
    bool closed = false;

    if(line != "HEAD")//Header not found, this world map is head-less
        goto skipHeaderParse;

    str_count++;
    line = inf.readLine();

    while((line != "HEAD_END") && (!inf.eof()))
    {
        header.push_back(line);
        str_count++;
        line = inf.readLine();

        if(line == "HEAD_END")
            closed = true;
    }

    if(!closed)
        goto badfile;

    for(pge_size_t zzz = 0; zzz < header.size(); zzz++)
    {
        PGESTRING &header_line = header[zzz];
        PGELIST<PGESTRINGList >data = PGEFile::splitDataLine(header_line, &valid);

        for(pge_size_t i = 0; i < data.size(); i++)
        {
            if(data[i].size() != 2) goto badfile;

            if(data[i][0] == "TL") //Episode Title
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.EpisodeTitle = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "DC") //Disabled characters
            {
                if(PGEFile::IsBoolArray(data[i][1]))
                    FileData.nocharacter = PGEFile::X2BollArr(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "IT") //Intro level
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.IntroLevel_file = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "HB") //Hub Styled
            {
                if(PGEFile::IsBool(data[i][1]))
                    FileData.HubStyledWorld = static_cast<bool>(toInt(data[i][1]));
                else
                    goto badfile;
            }
            else if(data[i][0] == "RL") //Restart level on fail
            {
                if(PGEFile::IsBool(data[i][1]))
                    FileData.restartlevel = static_cast<bool>(toInt(data[i][1]));
                else
                    goto badfile;
            }
            else if(data[i][0] == "SZ") //Starz number
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.stars = toUInt(data[i][1]);
                else
                    goto badfile;
            }
            else if(data[i][0] == "CD") //Credits list
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.authors = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
        }
    }

skipHeaderParse:
    FileData.CurSection = 0;
    FileData.playmusic = 0;
    FileData.meta.ReadFileValid = true;
    inf.close();
    return true;
badfile:
    inf.close();
    FileData.meta.ERROR_info = "Invalid file format";
    FileData.meta.ERROR_linenum = str_count;
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    return false;
}

bool FileFormats::ReadExtendedWldFileF(PGESTRING  filePath, WorldData &FileData)
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

    return ReadExtendedWldFile(file, FileData);
}

bool FileFormats::ReadExtendedWldFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  WorldData &FileData)
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

    return ReadExtendedWldFile(file, FileData);
}

bool FileFormats::ReadExtendedWldFile(PGE_FileFormats_misc::TextInput &in, WorldData &FileData)
{
    PGESTRING errorString;
    PGEX_FileBegin();
    PGESTRING filePath = in.getFilePath();
    CreateWorldData(FileData);
    FileData.meta.RecentFormat = WorldData::PGEX;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    FileData.meta.untitled = false;
    FileData.meta.modified = false;
    WorldTerrainTile tile;
    WorldScenery scen;
    WorldPathTile pathitem;
    WorldMusicBox musicbox;
    WorldLevelTile lvlitem;
    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEX_FileParseTree(in.readAll());
    PGEX_FetchSection() //look sections
    {
        PGEX_FetchSection_begin()
        ///////////////////HEADER//////////////////////
        PGEX_Section("HEAD")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count += 8;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("TL", FileData.EpisodeTitle)        //Episode Title
                    PGEX_BoolArrVal("DC", FileData.nocharacter)     //Disabled characters
                    PGEX_StrVal("IT", FileData.IntroLevel_file)     //Intro level
                    PGEX_BoolVal("HB", FileData.HubStyledWorld)     //Hub Styled
                    PGEX_BoolVal("RL", FileData.restartlevel)       //Restart level on fail
                    PGEX_UIntVal("SZ", FileData.stars)              //Starz number
                    PGEX_StrVal("CD", FileData.authors)     //Credits list
                }
            }
        }//head
        ///////////////////////////////MetaDATA/////////////////////////////////////////////
        PGEX_Section("META_BOOKMARKS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName = "";
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("BM", meta_bookmark.bookmarkName) //Bookmark name
                    PGEX_SIntVal("X", meta_bookmark.x) // Position X
                    PGEX_SIntVal("Y", meta_bookmark.y) // Position Y
                }
                FileData.metaData.bookmarks.push_back(meta_bookmark);
            }
        }
        ////////////////////////meta bookmarks////////////////////////
#ifdef PGE_EDITOR
        PGEX_Section("META_SYS_CRASH")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    FileData.metaData.crash.used = true;
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
        ///////////////////TILES//////////////////////
        PGEX_Section("TILES")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                tile = CreateWldTile();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", tile.id) //Tile ID
                    PGEX_SIntVal("X",  tile.x) //X Position
                    PGEX_SIntVal("Y",  tile.y) //Y Position
                    PGEX_StrVal("XTRA", tile.meta.custom_params)//Custom JSON data tree
                }
                tile.meta.array_id = FileData.tile_array_id++;
                tile.meta.index = static_cast<unsigned int>(FileData.tiles.size());
                FileData.tiles.push_back(tile);
            }
        }//TILES
        ///////////////////SCENERY//////////////////////
        PGEX_Section("SCENERY")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                scen = CreateWldScenery();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", scen.id)  //Scenery ID
                    PGEX_SIntVal("X", scen.x) //X Position
                    PGEX_SIntVal("Y", scen.y) //Y Position
                    PGEX_StrVal("XTRA", scen.meta.custom_params)//Custom JSON data tree
                }
                scen.meta.array_id = FileData.scene_array_id++;
                scen.meta.index = static_cast<unsigned int>(FileData.scenery.size());
                FileData.scenery.push_back(scen);
            }
        }//SCENERY
        ///////////////////PATHS//////////////////////
        PGEX_Section("PATHS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                pathitem = CreateWldPath();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", pathitem.id)  //Path ID
                    PGEX_SIntVal("X", pathitem.x) //X Position
                    PGEX_SIntVal("Y", pathitem.y) //Y Position
                    PGEX_StrVal("XTRA", pathitem.meta.custom_params)//Custom JSON data tree
                }
                pathitem.meta.array_id = FileData.path_array_id++;
                pathitem.meta.index =  static_cast<unsigned int>(FileData.paths.size());
                FileData.paths.push_back(pathitem);
            }
        }//PATHS
        ///////////////////MUSICBOXES//////////////////////
        PGEX_Section("MUSICBOXES")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                musicbox = CreateWldMusicbox();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", musicbox.id) //MISICBOX ID
                    PGEX_SIntVal("X", musicbox.x) //X Position
                    PGEX_SIntVal("Y", musicbox.y) //X Position
                    PGEX_StrVal("MF", musicbox.music_file)  //Custom music file
                    PGEX_StrVal("XTRA", musicbox.meta.custom_params)//Custom JSON data tree
                }
                musicbox.meta.array_id = FileData.musicbox_array_id++;
                musicbox.meta.index =  static_cast<unsigned int>(FileData.music.size());
                FileData.music.push_back(musicbox);
            }
        }//MUSICBOXES
        ///////////////////LEVELS//////////////////////
        PGEX_Section("LEVELS")
        {
            str_count++;
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                str_count++;
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                lvlitem = CreateWldLevel();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", lvlitem.id) //LEVEL IMAGE ID
                    PGEX_SIntVal("X",  lvlitem.x) //X Position
                    PGEX_SIntVal("Y",  lvlitem.y) //X Position
                    PGEX_StrVal("LF", lvlitem.lvlfile)  //Target level file
                    PGEX_StrVal("LT", lvlitem.title)   //Level title
                    PGEX_UIntVal("EI", lvlitem.entertowarp) //Entrance Warp ID (if 0 - start level from default points)
                    PGEX_SIntVal("ET", lvlitem.top_exit) //Open top path on exit type
                    PGEX_SIntVal("EL", lvlitem.left_exit) //Open left path on exit type
                    PGEX_SIntVal("ER", lvlitem.right_exit) //Open right path on exit type
                    PGEX_SIntVal("EB", lvlitem.bottom_exit) //Open bottom path on exit type
                    PGEX_SIntVal("WX", lvlitem.gotox) //Goto world map X
                    PGEX_SIntVal("WY", lvlitem.gotoy) //Goto world map Y
                    PGEX_BoolVal("AV", lvlitem.alwaysVisible) //Always visible
                    PGEX_BoolVal("SP", lvlitem.gamestart) //Is Game start point
                    PGEX_BoolVal("BP", lvlitem.pathbg) //Path background
                    PGEX_BoolVal("BG", lvlitem.bigpathbg) //Big path background
                    PGEX_StrVal("XTRA", lvlitem.meta.custom_params)//Custom JSON data tree
                }
                lvlitem.meta.array_id = FileData.level_array_id++;
                lvlitem.meta.index = static_cast<unsigned int>(FileData.levels.size());
                FileData.levels.push_back(lvlitem);
            }
        }//LEVELS
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////
    errorString.clear(); //If no errors, clear string;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:    //If file format not corrects
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linenum = str_count;
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    return false;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteExtendedWldFileF(PGESTRING filePath, WorldData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, true, false, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }

    return WriteExtendedWldFile(file, FileData);
}

bool FileFormats::WriteExtendedWldFileRaw(WorldData &FileData, PGESTRING &rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open raw string for write";
        return false;
    }

    return WriteExtendedWldFile(file, FileData);
}

bool FileFormats::WriteExtendedWldFile(PGE_FileFormats_misc::TextOutput &out, WorldData &FileData)
{
    pge_size_t i;
    bool addArray = false;
    FileData.meta.RecentFormat = WorldData::PGEX;
    addArray = false;

    for(pge_size_t z = 0; z < FileData.nocharacter.size(); z++)
    {
        bool x = FileData.nocharacter[z];
        if(x) addArray = true;
    }

    //HEAD section
    if(
        (!IsEmpty(FileData.EpisodeTitle)) ||
        (addArray) ||
        (!IsEmpty(FileData.IntroLevel_file)) ||
        (FileData.HubStyledWorld) ||
        (FileData.restartlevel) ||
        (FileData.stars > 0) ||
        (!IsEmpty(FileData.authors))
    )
    {
        out << "HEAD\n";

        if(!IsEmpty(FileData.EpisodeTitle))
            out << PGEFile::value("TL", PGEFile::WriteStr(FileData.EpisodeTitle)); // Episode title

        addArray = false;

        for(pge_size_t z = 0; z < FileData.nocharacter.size(); z++)
        {
            bool x = FileData.nocharacter[z];
            if(x) addArray = true;
        }

        if(addArray)
            out << PGEFile::value("DC", PGEFile::WriteBoolArr(FileData.nocharacter)); // Disabled characters
        if(!IsEmpty(FileData.IntroLevel_file))
            out << PGEFile::value("IT", PGEFile::WriteStr(FileData.IntroLevel_file)); // Intro level
        if(FileData.HubStyledWorld)
            out << PGEFile::value("HB", PGEFile::WriteBool(FileData.HubStyledWorld)); // Hub-styled episode
        if(FileData.restartlevel)
            out << PGEFile::value("RL", PGEFile::WriteBool(FileData.restartlevel)); // Restart on fail
        if(FileData.stars > 0)
            out << PGEFile::value("SZ", PGEFile::WriteInt(FileData.stars));      // Total stars number
        if(!IsEmpty(FileData.authors))
            out << PGEFile::value("CD", PGEFile::WriteStr(FileData.authors));   // Credits

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
        out << PGEFile::value("N",  PGEFile::WriteStr(FileData.metaData.crash.filename));
        out << PGEFile::value("P",  PGEFile::WriteStr(FileData.metaData.crash.path));
        out << PGEFile::value("FP", PGEFile::WriteStr(FileData.metaData.crash.fullPath));
        out << "\n";
        out << "META_SYS_CRASH_END\n";
    }

#endif
    //////////////////////////////////////MetaData///END//////////////////////////////////////////

    if(!FileData.tiles.empty())
    {
        out << "TILES\n";

        for(i = 0; i < FileData.tiles.size(); i++)
        {
            WorldTerrainTile &tt = FileData.tiles[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(tt.id));
            out << PGEFile::value("X", PGEFile::WriteInt(tt.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(tt.y));
            if(!IsEmpty(tt.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(tt.meta.custom_params));
            out << "\n";
        }

        out << "TILES_END\n";
    }

    if(!FileData.scenery.empty())
    {
        out << "SCENERY\n";

        for(i = 0; i < FileData.scenery.size(); i++)
        {
            WorldScenery &ws = FileData.scenery[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(ws.id));
            out << PGEFile::value("X", PGEFile::WriteInt(ws.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(ws.y));
            if(!IsEmpty(ws.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(ws.meta.custom_params));
            out << "\n";
        }

        out << "SCENERY_END\n";
    }

    if(!FileData.paths.empty())
    {
        out << "PATHS\n";

        for(i = 0; i < FileData.paths.size(); i++)
        {
            WorldPathTile &wp = FileData.paths[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(wp.id));
            out << PGEFile::value("X", PGEFile::WriteInt(wp.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(wp.y));
            if(!IsEmpty(wp.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(wp.meta.custom_params));
            out << "\n";
        }

        out << "PATHS_END\n";
    }

    if(!FileData.music.empty())
    {
        out << "MUSICBOXES\n";

        for(i = 0; i < FileData.music.size(); i++)
        {
            WorldMusicBox &wm = FileData.music[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(wm.id));
            out << PGEFile::value("X", PGEFile::WriteInt(wm.x));
            out << PGEFile::value("Y", PGEFile::WriteInt(wm.y));
            if(!IsEmpty(wm.music_file))
                out << PGEFile::value("MF", PGEFile::WriteStr(wm.music_file));
            if(!IsEmpty(wm.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(wm.meta.custom_params));
            out << "\n";
        }

        out << "MUSICBOXES_END\n";
    }

    if(!FileData.levels.empty())
    {
        out << "LEVELS\n";
        WorldLevelTile defLvl = CreateWldLevel();

        for(i = 0; i < FileData.levels.size(); i++)
        {
            WorldLevelTile &lt = FileData.levels[i];
            out << PGEFile::value("ID", PGEFile::WriteInt(lt.id));
            out << PGEFile::value("X",  PGEFile::WriteInt(lt.x));
            out << PGEFile::value("Y",  PGEFile::WriteInt(lt.y));
            if(!IsEmpty(lt.title))
                out << PGEFile::value("LT", PGEFile::WriteStr(lt.title));
            if(!IsEmpty(lt.lvlfile))
                out << PGEFile::value("LF", PGEFile::WriteStr(lt.lvlfile));
            if(lt.entertowarp != defLvl.entertowarp)
                out << PGEFile::value("EI", PGEFile::WriteInt(lt.entertowarp));
            if(lt.left_exit != defLvl.left_exit)
                out << PGEFile::value("EL", PGEFile::WriteInt(lt.left_exit));
            if(lt.top_exit != defLvl.top_exit)
                out << PGEFile::value("ET", PGEFile::WriteInt(lt.top_exit));
            if(lt.right_exit != defLvl.right_exit)
                out << PGEFile::value("ER", PGEFile::WriteInt(lt.right_exit));
            if(lt.bottom_exit != defLvl.bottom_exit)
                out << PGEFile::value("EB", PGEFile::WriteInt(lt.bottom_exit));
            if(lt.gotox != defLvl.gotox)
                out << PGEFile::value("WX", PGEFile::WriteInt(lt.gotox));
            if(lt.gotoy != defLvl.gotoy)
                out << PGEFile::value("WY", PGEFile::WriteInt(lt.gotoy));
            if(lt.alwaysVisible)
                out << PGEFile::value("AV", PGEFile::WriteBool(lt.alwaysVisible));
            if(lt.gamestart)
                out << PGEFile::value("SP", PGEFile::WriteBool(lt.gamestart));
            if(lt.pathbg)
                out << PGEFile::value("BP", PGEFile::WriteBool(lt.pathbg));
            if(lt.bigpathbg)
                out << PGEFile::value("BG", PGEFile::WriteBool(lt.bigpathbg));
            if(!IsEmpty(lt.meta.custom_params))
                out << PGEFile::value("XTRA", PGEFile::WriteStr(lt.meta.custom_params));
            out << "\n";
        }

        out << "LEVELS_END\n";
    }

    return true;
}
