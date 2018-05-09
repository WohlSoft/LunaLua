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
#include "pge_x.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadNonSMBX64MetaDataF(PGESTRING filePath, MetaData &FileData)
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

    return ReadNonSMBX64MetaDataFile(file, FileData);
}

bool FileFormats::ReadNonSMBX64MetaDataRaw(PGESTRING &rawdata, PGESTRING filePath, MetaData &FileData)
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

    return ReadNonSMBX64MetaDataFile(file, FileData);
}

bool FileFormats::ReadNonSMBX64MetaDataFile(PGE_FileFormats_misc::TextInput &in, MetaData &FileData)
{
    PGESTRING errorString;
    int str_count = 0;      //Line Counter
    PGESTRING line;           //Current Line data
    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEFile pgeX_Data(in.readAll());

    if(!pgeX_Data.buildTreeFromRaw())
    {
        errorString = pgeX_Data.lastError();
        goto badfile;
    }

    for(pge_size_t section = 0; section < pgeX_Data.dataTree.size(); section++) //look sections
    {
        PGEFile::PGEX_Entry &f_section = pgeX_Data.dataTree[section];

        if(f_section.name == "META_BOOKMARKS")
        {
            if(f_section.type != PGEFile::PGEX_Struct)
            {
                errorString = PGESTRING("Wrong section data syntax:\nSection [") + f_section.name + "%1]";
                goto badfile;
            }

            for(pge_size_t sdata = 0; sdata < f_section.data.size(); sdata++)
            {
                if(f_section.data[sdata].type != PGEFile::PGEX_Struct)
                {
                    errorString = PGESTRING("Wrong data item syntax:\nSection [") +
                                  f_section.name + "]\nData line " +
                                  fromNum(sdata) + ")";
                    goto badfile;
                }

                PGEFile::PGEX_Item x = f_section.data[sdata];
                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName = "";
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;

                for(pge_size_t sval = 0; sval < x.values.size(); sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString = PGESTRING("Wrong value syntax\nSection [") +
                                  f_section.name + "]\nData line " +
                                  fromNum(sdata) + "\nMarker " +
                                  v.marker + "\nValue " +
                                  v.value;

                    if(v.marker == "BM") //Bookmark name
                    {
                        if(PGEFile::IsQoutedString(v.value))
                            meta_bookmark.bookmarkName = PGEFile::X2STRING(v.value);
                        else
                            goto badfile;
                    }
                    else if(v.marker == "X") // Position X
                    {
                        if(PGEFile::IsIntS(v.value))
                            meta_bookmark.x = toInt(v.value);
                        else
                            goto badfile;
                    }
                    else if(v.marker == "Y") //Position Y
                    {
                        if(PGEFile::IsIntS(v.value))
                            meta_bookmark.y = toInt(v.value);
                        else
                            goto badfile;
                    }
                }

                FileData.bookmarks.push_back(meta_bookmark);
            }
        }
    }

    ///////////////////////////////////////EndFile///////////////////////////////////////
    errorString.clear(); //If no errors, clear string;
    FileData.meta.ReadFileValid = true;
    return true;
badfile:    //If file format is not correct
    //BadFileMsg(filePath+"\nError message: "+errorString, str_count, line);
    FileData.meta.ERROR_info = errorString;
    FileData.meta.ERROR_linenum = str_count;
    FileData.meta.ERROR_linedata = line;
    FileData.meta.ReadFileValid = false;
    FileData.bookmarks.clear();
    return false;
}






//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteNonSMBX64MetaDataF(PGESTRING filePath, MetaData &metaData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;

    if(!file.open(filePath, true, false, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }

    return WriteNonSMBX64MetaData(file, metaData);
}

bool FileFormats::WriteNonSMBX64MetaDataRaw(MetaData &metaData, PGESTRING &rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;

    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open raw string for write";
        return false;
    }

    return WriteNonSMBX64MetaData(file, metaData);
}

bool FileFormats::WriteNonSMBX64MetaData(PGE_FileFormats_misc::TextOutput &out, MetaData &metaData)
{
    pge_size_t i;

    //Bookmarks
    if(!metaData.bookmarks.empty())
    {
        out << "META_BOOKMARKS\n";

        for(i = 0; i < metaData.bookmarks.size(); i++)
        {
            Bookmark &bm = metaData.bookmarks[i];
            //Bookmark name
            out << PGEFile::value("BM", PGEFile::WriteStr(bm.bookmarkName));
            out << PGEFile::value("X", PGEFile::WriteRoundFloat(bm.x));
            out << PGEFile::value("Y", PGEFile::WriteRoundFloat(bm.y));
            out << "\n";
        }

        out << "META_BOOKMARKS_END\n";
    }

    return true;
}
