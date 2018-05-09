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

#ifdef PGE_FILES_QT
#include <QFileInfo>
#include <QDir>
#else
#include <stdlib.h>
#include <limits.h> /* PATH_MAX */
#endif

#include "file_formats.h"
#include "file_strlist.h"
#include "save_filedata.h"
#include "smbx64.h"
#include "smbx64_macro.h"
#include "CSVUtils.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX64SavFileF(PGESTRING  filePath, GamesaveData &FileData)
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
    return ReadSMBX64SavFile(file, FileData);
}

bool FileFormats::ReadSMBX64SavFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  GamesaveData &FileData)
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
    return ReadSMBX64SavFile(file, FileData);
}

bool FileFormats::ReadSMBX64SavFile(PGE_FileFormats_misc::TextInput &in, GamesaveData &FileData)
{
    SMBX64_FileBegin();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    //SMBX64_File( RawData );

    int i;                  //counters
    int arrayIdCounter = 0;
    //GamesaveData FileData;
    FileData = CreateGameSaveData();

    FileData.meta.untitled = false;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.meta.filename = in_1.basename();
        FileData.meta.path = in_1.dirpath();
    }

    //Enable strict mode for SMBX LVL file format
    FileData.meta.smbx64strict = true;

    try
    {
        ///////////////////////////////////////Begin file///////////////////////////////////////
        nextLine();
        SMBX64::ReadUInt(&file_format, line);//File format number
        FileData.meta.RecentFormatVersion = file_format;
        nextLine();
        SMBX64::ReadUInt(&FileData.lives, line); //Number of lives
        nextLine();
        SMBX64::ReadUInt(&FileData.coins, line); //Number of coins
        nextLine();
        SMBX64::ReadSInt(&FileData.worldPosX, line);  //World map pos X
        nextLine();
        SMBX64::ReadSInt(&FileData.worldPosY, line);  //World map pos Y

        for(i = 0; i < (ge(56) ? 5 : 2) ; i++)
        {
            saveCharState charState;
            charState = CreateSavCharacterState();
            nextLine();
            SMBX64::ReadUInt(&charState.state, line);//Character's power up state
            nextLine();
            SMBX64::ReadUInt(&charState.itemID, line); //ID of item in the slot
            if(ge(10))
            {
                nextLine();    //Type of mount
                SMBX64::ReadUInt(&charState.mountType, line);
            }
            nextLine();
            SMBX64::ReadUInt(&charState.mountID, line); //ID of mount
            if(lt(10))
            {
                if(charState.mountID > 0) charState.mountType = 1;
            }
            if(ge(56))
            {
                nextLine();    //ID of mount
                SMBX64::ReadUInt(&charState.health, line);
            }
            FileData.characterStates.push_back(charState);
        }

        nextLine();
        SMBX64::ReadUInt(&FileData.musicID, line);//ID of music
        nextLine();
        if(line == "" || in.eof())
            goto successful;

        if(ge(56))
        {
            SMBX64::ReadCSVBool(&FileData.gameCompleted, line);   //Game was complited
        }

        arrayIdCounter = 1;

        nextLine();
        while((line != "next") && (!in.eof()))
        {
            visibleItem level;
            level.first = (unsigned int)arrayIdCounter;
            level.second = false;
            SMBX64::ReadCSVBool(&level.second, line); //Is level shown

            FileData.visibleLevels.push_back(level);
            arrayIdCounter++;
            nextLine();
        }

        arrayIdCounter = 1;
        nextLine();
        while((line != "next") && (!in.eof()))
        {
            visibleItem level;
            level.first = (unsigned int)arrayIdCounter;
            level.second = false;
            SMBX64::ReadCSVBool(&level.second, line); //Is path shown

            FileData.visiblePaths.push_back(level);
            arrayIdCounter++;
            nextLine();
        }

        arrayIdCounter = 1;
        nextLine();
        while((line != "next") && (!in.eof()))
        {
            visibleItem level;
            level.first = (unsigned int)arrayIdCounter;
            level.second = false;
            SMBX64::ReadCSVBool(&level.second, line); //Is Scenery shown

            FileData.visibleScenery.push_back(level);
            arrayIdCounter++;
            nextLine();
        }

        if(ge(7))
        {
            nextLine();
            while((line != "next") && (!IsNULL(line)))
            {
                starOnLevel gottenStar;
                gottenStar.first = "";
                gottenStar.second = 0;

                SMBX64::ReadStr(&gottenStar.first, line);//Level file
                if(ge(16))
                {
                    nextLine();    //Section ID
                    SMBX64::ReadUInt(&gottenStar.second, line);
                }

                FileData.gottenStars.push_back(gottenStar);
                nextLine();
            }
        }

        if(ge(21))
        {
            nextLine();
            if(line == "" || in.eof())
                goto successful;
            SMBX64::ReadUInt(&FileData.totalStars, line);//Total Number of stars
        }

successful:
        ///////////////////////////////////////EndFile///////////////////////////////////////
        FileData.meta.ReadFileValid = true;
        return true;
    }
    catch(const std::exception &err)
    {
        if(file_format > 0)
            FileData.meta.ERROR_info = "Detected file format: SMBX-" + fromNum(file_format) + " is invalid\n";
        else
            FileData.meta.ERROR_info = "It is not an SMBX game save file\n";
        #ifdef PGE_FILES_QT
        FileData.meta.ERROR_info += QString::fromStdString(exception_to_pretty_string(err));
        #else
        FileData.meta.ERROR_info += exception_to_pretty_string(err);
        #endif
        FileData.meta.ERROR_linenum = in.getCurrentLineNumber();
        FileData.meta.ERROR_linedata = line;
        FileData.meta.ReadFileValid = false;
        return false;
    }
}

