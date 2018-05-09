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
#include "smbx64.h"

#include <functional>

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadNpcTXTFileF(PGESTRING filePath, NPCConfigFile &FileData, bool IgnoreBad)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, false))
    {
        errorString = "Failed to open file for read";
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadNpcTXTFile(file, FileData, IgnoreBad);
}

bool FileFormats::ReadNpcTXTFileRAW(PGESTRING &rawdata, NPCConfigFile &FileData, bool IgnoreBad)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;
    if(!file.open(&rawdata))
    {
        errorString = "Failed to open raw string for read";
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadNpcTXTFile(file, FileData, IgnoreBad);
}



static inline PGESTRING invalidLine_SINT(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be signed intger number!>\n";
}

static inline PGESTRING invalidLine_UINT(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be unsigned intger number!>\n";
}

static inline PGESTRING invalidLine_FLT(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be floating point number!>\n";
}

static inline PGESTRING invalidLine_BOOL(long line, PGESTRING data)
{
    return fromNum(line) + ": " + data + " <Should be 1 or 0!>\n";
}

bool FileFormats::ReadNpcTXTFile(PGE_FileFormats_misc::TextInput &inf, NPCConfigFile &FileData, bool IgnoreBad)
{
    PGESTRING line;           //Current Line data
    PGESTRINGList Params;
    FileData = CreateEmpytNpcTXT();
    bool doLog = !IgnoreBad;

    auto handlerSInt = [&](bool &dest_en, int32_t &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsSInt(input))
        {
            if(doLog)
                FileData.unknownLines += invalidLine_SINT(inf.getCurrentLineNumber(), line);
        }
        else
        {
            if(input.size() > 9)
                dest = 0;
            else
            {
                dest = toInt(input);
                dest_en = true;
            }
        }
    };

    auto handlerUInt = [&](bool &dest_en, uint32_t &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsUInt(input))
        {
            if(doLog)
                FileData.unknownLines += invalidLine_UINT(inf.getCurrentLineNumber(), line);
        }
        else
        {
            if(input.size() > 9)
                dest = 0;
            else
            {
                dest = toUInt(input);
                dest_en = true;
            }
        }
    };

    auto handlerBool = [&](bool &dest_en, bool &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsBool(input))
        {
            if(doLog)
                FileData.unknownLines += invalidLine_BOOL(inf.getCurrentLineNumber(), line);
        }
        else
        {
            if(input.size() > 9)
                dest = 0;
            else
            {
                dest = static_cast<bool>(toInt(input));
                dest_en = true;
            }
        }
    };

    auto handlerDouble = [&](bool &dest_en, double &dest, PGESTRING &input)
    {
        input = PGESTR_Simpl(input);
        input = PGE_RemSubSTRING(input, " ");//Delete spaces
        if(!SMBX64::IsFloat(input))
        {
            if(doLog)
                FileData.unknownLines += invalidLine_FLT(inf.getCurrentLineNumber(), line);
        }
        else
        {
            dest = toDouble(input);
            dest_en = true;
        }
    };

    auto handlerString = [&](bool &dest_en, PGESTRING &dest, PGESTRING &input)
    {
        dest    = removeQuotes(input);
        dest_en = !IsEmpty(input);
    };

    typedef PGEHASH<PGESTRING, std::function<void(PGESTRING&)>> NpcCfgHandlerMap;
    #define SINT_ENTRY(param_name) { #param_name, [&](PGESTRING &param) { handlerSInt(FileData.en_##param_name, FileData.param_name, param);} }
    #define UINT_ENTRY(param_name) { #param_name, [&](PGESTRING &param) { handlerUInt(FileData.en_##param_name, FileData.param_name, param);} }
    #define BOOL_ENTRY(param_name) { #param_name, [&](PGESTRING &param) { handlerBool(FileData.en_##param_name, FileData.param_name, param);} }
    #define STR_ENTRY(param_name)  { #param_name, [&](PGESTRING &param) { handlerString(FileData.en_##param_name, FileData.param_name, param);} }
    #define FLT_ENTRY(param_name)  { #param_name, [&](PGESTRING &param) { handlerDouble(FileData.en_##param_name, FileData.param_name, param);} }

    NpcCfgHandlerMap paramsHandler =
    {
        SINT_ENTRY(gfxoffsetx),
        SINT_ENTRY(gfxoffsety),
        UINT_ENTRY(width),
        UINT_ENTRY(height),
        UINT_ENTRY(gfxwidth),
        UINT_ENTRY(gfxheight),
        UINT_ENTRY(score),
        UINT_ENTRY(health),
        BOOL_ENTRY(playerblock),
        BOOL_ENTRY(playerblocktop),
        BOOL_ENTRY(npcblock),
        BOOL_ENTRY(npcblocktop),
        BOOL_ENTRY(grabside),
        BOOL_ENTRY(grabtop),
        BOOL_ENTRY(jumphurt),
        BOOL_ENTRY(nohurt),
        BOOL_ENTRY(noblockcollision),
        BOOL_ENTRY(cliffturn),
        BOOL_ENTRY(noyoshi),
        BOOL_ENTRY(foreground),
        FLT_ENTRY(speed),
        BOOL_ENTRY(nofireball),
        BOOL_ENTRY(nogravity),
        UINT_ENTRY(frames),
        UINT_ENTRY(framespeed),
        UINT_ENTRY(framestyle),
        BOOL_ENTRY(noiceball),
        // Non-SMBX64 parameters (not working in SMBX <=1.3)
        BOOL_ENTRY(nohammer),
        BOOL_ENTRY(noshell),
        STR_ENTRY(name),
        STR_ENTRY(description),
        STR_ENTRY(image),
        STR_ENTRY(icon),
        STR_ENTRY(script),
        STR_ENTRY(group),
        STR_ENTRY(category),
        UINT_ENTRY(grid),
        SINT_ENTRY(gridoffsetx),
        SINT_ENTRY(gridoffsety),
        UINT_ENTRY(gridalign),
    };

    //Read NPC.TXT File config
#define NextLine(line) line = inf.readCVSLine();

    do
    {
        NextLine(line)
        if(IsEmpty(PGE_RemSubSTRING(line, " ")))
            continue;//Skip empty strings

        #ifdef PGE_FILES_QT
        Params = line.split("=", QString::SkipEmptyParts); // split the Parameter and value (example: chicken=2)
        #else
        PGE_SPLITSTRING(Params, line, "=");
        #endif

        if(Params.size() != 2) // If string does not contain strings with "=" as separator
        {
            if(doLog)
                FileData.unknownLines += fromNum(inf.getCurrentLineNumber()) + ": " + line + " <wrong syntax!>\n";
            if(doLog || (Params.size() < 2))
                continue;
        }

        Params[0] = PGESTR_Simpl(Params[0]);
        Params[0] = PGE_RemSubSTRING(Params[0], " "); //Delete spaces
        Params[0] = PGESTR_toLower(Params[0]);//To lower case

        NpcCfgHandlerMap::iterator hand = paramsHandler.find(Params[0]);
        if(hand != paramsHandler.end())
        {
            #ifdef PGE_FILES_QT
            (*hand)(Params[1]);
            #else
            (hand->second)(Params[1]);
            #endif
        }
        else if(doLog)//Store unknown value into warnings list
            FileData.unknownLines += fromNum(inf.getCurrentLineNumber()) + ": " + line + "\n";
    }
    while(!inf.eof());

    FileData.ReadFileValid = true;
    return true;
}




//*********************************************************
//****************WRITE FILE*******************************
//*********************************************************
bool FileFormats::WriteNPCTxtFileF(PGESTRING filePath, NPCConfigFile &FileData)
{
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open file for write";
        return false;
    }
    return WriteNPCTxtFile(file, FileData);
}

bool FileFormats::WriteNPCTxtFileRaw(NPCConfigFile &FileData, PGESTRING &rawdata)
{
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString = "Failed to open target raw string for write";
        return false;
    }
    return WriteNPCTxtFile(file, FileData);
}

//Convert NPC Options structore to text for saving
bool FileFormats::WriteNPCTxtFile(PGE_FileFormats_misc::TextOutput &out, NPCConfigFile &FileData)
{
    if(FileData.en_gfxoffsetx)
        out << "gfxoffsetx=" + fromNum(FileData.gfxoffsetx) + "\n";
    if(FileData.en_gfxoffsety)
        out << "gfxoffsety=" + fromNum(FileData.gfxoffsety) + "\n";
    if(FileData.en_gfxwidth)
        out << "gfxwidth=" + fromNum(FileData.gfxwidth) + "\n";
    if(FileData.en_gfxheight)
        out << "gfxheight=" + fromNum(FileData.gfxheight) + "\n";
    if(FileData.en_foreground)
        out << "foreground=" + fromBoolToNum(FileData.foreground) + "\n";
    if(FileData.en_width)
        out << "width=" + fromNum(FileData.width) + "\n";
    if(FileData.en_height)
        out << "height=" + fromNum(FileData.height) + "\n";

    if(FileData.en_score)
        out << "score=" + fromNum(FileData.score) + "\n";
    if(FileData.en_health)
        out << "health=" + fromNum(FileData.health) + "\n";

    if(FileData.en_playerblock)
        out << "playerblock=" + fromBoolToNum(FileData.playerblock) + "\n";

    if(FileData.en_playerblocktop)
        out << "playerblocktop=" + fromBoolToNum(FileData.playerblocktop) + "\n";

    if(FileData.en_npcblock)
        out << "npcblock=" + fromBoolToNum(FileData.npcblock) + "\n";

    if(FileData.en_npcblocktop)
        out << "npcblocktop=" + fromBoolToNum(FileData.npcblocktop) + "\n";
    if(FileData.en_grabside)
        out << "grabside=" + fromBoolToNum(FileData.grabside) + "\n";
    if(FileData.en_grabtop)
        out << "grabtop=" + fromBoolToNum(FileData.grabtop) + "\n";
    if(FileData.en_jumphurt)
        out << "jumphurt=" + fromBoolToNum(FileData.jumphurt) + "\n";
    if(FileData.en_nohurt)
        out << "nohurt=" + fromBoolToNum(FileData.nohurt) + "\n";
    if(FileData.en_speed)
        out << "speed=" + fromNum(FileData.speed) + "\n";
    if(FileData.en_noblockcollision)
        out << "noblockcollision=" + fromBoolToNum(FileData.noblockcollision) + "\n";
    if(FileData.en_cliffturn)
        out << "cliffturn=" + fromBoolToNum(FileData.cliffturn) + "\n";
    if(FileData.en_noyoshi)
        out << "noyoshi=" + fromBoolToNum(FileData.noyoshi) + "\n";
    if(FileData.en_nofireball)
        out << "nofireball=" + fromBoolToNum(FileData.nofireball) + "\n";
    if(FileData.en_nogravity)
        out << "nogravity=" + fromBoolToNum(FileData.nogravity) + "\n";
    if(FileData.en_noiceball)
        out << "noiceball=" + fromBoolToNum(FileData.noiceball) + "\n";
    if(FileData.en_frames)
        out << "frames=" + fromNum(FileData.frames) + "\n";
    if(FileData.en_framespeed)
        out << "framespeed=" + fromNum(FileData.framespeed) + "\n";
    if(FileData.en_framestyle)
        out << "framestyle=" + fromNum(FileData.framestyle) + "\n";

    //Extended
    if(FileData.en_nohammer)
        out << "nohammer=" + fromBoolToNum(FileData.nohammer) + "\n";
    if(FileData.en_noshell)
        out << "noshell=" + fromBoolToNum(FileData.noshell) + "\n";
    if(FileData.en_name && !IsEmpty(FileData.name))
        out << "name=" + SMBX64::WriteStr(FileData.name);
    if(FileData.en_description && !IsEmpty(FileData.description))
        out << "description=" + SMBX64::WriteStr(FileData.description);
    if(FileData.en_image && !IsEmpty(FileData.image))
        out << "image=" + SMBX64::WriteStr(FileData.image);
    if(FileData.en_icon && !IsEmpty(FileData.icon))
        out << "icon=" + SMBX64::WriteStr(FileData.icon);
    if(FileData.en_script && !IsEmpty(FileData.script))
        out << "script=" + SMBX64::WriteStr(FileData.script);
    if(FileData.en_group && !IsEmpty(FileData.group))
        out << "group=" + SMBX64::WriteStr(FileData.group);
    if(FileData.en_category && !IsEmpty(FileData.category))
        out << "category=" + SMBX64::WriteStr(FileData.category);
    if(FileData.en_grid)
        out << "grid=" + fromNum(FileData.grid) + "\n";
    if(FileData.en_gridoffsetx)
        out << "gridoffsetx=" + fromNum(FileData.gridoffsetx) + "\n";
    if(FileData.en_gridoffsety)
        out << "gridoffsety=" + fromNum(FileData.gridoffsety) + "\n";
    if(FileData.en_gridalign)
        out << "gridalign=" + fromNum(FileData.gridalign) + "\n";

    return true;
}
