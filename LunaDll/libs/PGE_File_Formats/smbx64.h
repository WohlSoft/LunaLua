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

/*! \file smbx64.h
 *  \brief Contains static class with SMBX64 standard file format fields validation and generation functions
 */

#pragma once
#ifndef SMBX64_H
#define SMBX64_H

#include "pge_file_lib_globs.h"

/*!
 * \brief SMBX64 Standard validation and raw data conversion functions
 */
namespace SMBX64
{
    /*******************Readers With Exception throwers********************/
    inline void ReadUInt(unsigned int*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toUInt(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to unsigned int");
        #else
        *out = static_cast<unsigned int>(std::stoul(input));
        #endif
    }

    inline void ReadUInt(unsigned long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toULong(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to unsigned long");
        #else
        *out = std::stoul(input);
        #endif
    }

    inline void ReadUInt(unsigned long long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toULongLong(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to unsigned long long");
        #else
        *out = std::stoull(input);
        #endif
    }
    inline void ReadUInt(int*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = static_cast<int>(input.toUInt(&ok));
        if(!ok) throw std::invalid_argument("Could not convert to unsigned int");
        #else
        *out = static_cast<int>(static_cast<unsigned int>(std::stoul(input)));
        #endif
    }
    inline void ReadUInt(long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = static_cast<long>(input.toULong(&ok));
        if(!ok) throw std::invalid_argument("Could not convert to unsigned long");
        #else
        *out = static_cast<long>(std::stoul(input));
        #endif
    }
    inline void ReadUInt(long long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = static_cast<long long>(input.toULongLong(&ok));
        if(!ok) throw std::invalid_argument("Could not convert to unsigned long long");
        #else
        *out = static_cast<long long>(std::stoull(input));
        #endif
    }

    inline void ReadSInt(int*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toInt(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to int");
        #else
        *out = std::stoi(input);
        #endif
    }

    inline void ReadSInt(long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toLong(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to long");
        #else
        *out = std::stol(input);
        #endif
    }
    inline void ReadSInt(long long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toLongLong(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to long long");
        #else
        *out = std::stoll(input);
        #endif
    }
    inline void ReadFloat(float*out, PGESTRING &input)
    {
        PGE_ReplSTRING(input, ",", ".");//Allow to parse floats of both comma and dot standard
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toFloat(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to Float");
        #else
        *out = std::stof(input);
        #endif
    }
    inline void ReadFloat(double*out, PGESTRING &input)
    {
        PGE_ReplSTRING(input, ",", ".");//Allow to parse floats of both comma and dot standard
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = input.toDouble(&ok);
        if(!ok) throw std::invalid_argument("Could not convert to Float");
        #else
        *out = std::stod(input);
        #endif
    }
    inline void ReadBool(bool*out, PGESTRING &input)
    {
        if(input == "0" || input == "") // FIXME: Is it correct? Or too hackish?
            *out = false;
        else if(input != "0") // FIXME: Is it correct? Or too hackish?
            *out = true;
        else
            throw std::invalid_argument(std::string("Could not convert to bool (must be empty, \"0\", \"!0\" or \"1\")"));
    }

    inline void ReadCSVBool(bool*out, PGESTRING &input)
    {
        if(input == "#FALSE#")
            *out = false;
        else if(input == "#TRUE#")
            *out = true;
        else if(input == "false")
            *out = false;
        else if(input == "true")
            *out = true;
        else if( input == "0" || input == "" )
            *out = false;
        else if( input == "!0" || input == "1" )
            *out = true;
        else
            throw std::invalid_argument(std::string("Could not convert CSV Bool (must be #TRUE# or #FALSE#)"));
    }

    inline void ReadCSVBool(int*out, PGESTRING &input)
    {
        if(input == "#FALSE#")
            *out = 0;
        else if(input == "#TRUE#")
            *out = 1;
        else if(input == "false")
            *out = 0;
        else if(input == "true")
            *out = 1;
        else if( input == "0" || input == "" )
            *out = 0;
        else if( input == "!0" || input == "1"  )
            *out = 1;
        else
            throw std::invalid_argument(std::string("Could not convert CSV Bool (must be #TRUE# or #FALSE#)"));
    }

    inline void ReadCSVBool(long*out, PGESTRING &input)
    {
        if(input == "#FALSE#")
            *out = 0;
        else if(input == "#TRUE#")
            *out = 1;
        else if(input == "false")
            *out = 0;
        else if(input == "true")
            *out = 1;
        else if( input == "0" || input == "" )
            *out = 0;
        else if( input == "!0" || input == "1" )
            *out = 1;
        else
            throw std::invalid_argument(std::string("Could not convert CSV Bool (must be #TRUE# or #FALSE#)"));
    }

    inline void ReadSIntFromFloat(int*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = qRound(input.toDouble(&ok));
        if(!ok) throw std::invalid_argument("Could not convert to Double");
        #else
        *out = static_cast<int>(std::round(std::stod(input)));
        #endif
    }

    inline void ReadSIntFromFloat(long*out, PGESTRING &input)
    {
        #ifdef PGE_FILES_QT
        bool ok=true;
        *out = static_cast<long>(std::round(input.toDouble(&ok)));
        if(!ok) throw std::invalid_argument("Could not convert to Double");
        #else
        *out = static_cast<long>(std::round(std::stod(input)));
        #endif
    }
    inline void ReadStr(PGESTRING*out, PGESTRING &input)
    {
        if(IsEmpty(input))
        {
            out->clear();
            return;
        }
        *out = input;
        PGESTRING &target = *out;
        if(target[0] == PGEChar('\"'))
            PGE_RemStrRng(target, 0, 1);
        if( (!IsEmpty(target)) && (target[target.size()-1] == PGEChar('\"')) )
            PGE_RemStrRng(target, target.size()-1, 1);
        target = PGE_ReplSTRING(target, "\"", "\'");//Correct damaged by SMBX line
    }




    /*********************Validations**********************/
    /*!
     * \brief Validate Unsigned Integer value
     * \param in raw value
     * \return true if value is valid
     */
    bool IsUInt(PGESTRING in);
    /*!
     * \brief Validate Signed Integer value
     * \param in raw value
     * \return true if value is valid
     */
    bool IsSInt(PGESTRING in);
    /*!
     * \brief Validate Floating Point value
     * \param in raw value
     * \return true if value is valid
     */
    bool IsFloat(PGESTRING &in);
    /*!
     * \brief Validate quoted string value
     * \param in raw value
     * \return true if value is valid
     */
    bool IsQuotedString(PGESTRING in);
    /*!
     * \brief Validate CSV-boolean value (#TRUE# or #FALSE#)
     * \param in raw value
     * \return true if value is INVALID
     */
    inline bool IsCSVBool(PGESTRING in) //Worded BOOL
    {
        return ( (in=="#TRUE#")||(in=="#FALSE#") );
    }

    /*!
     * \brief Validate digital boolean value (0 or 1)
     * \param in raw value
     * \return true if value is valid
     */
    inline bool IsBool(PGESTRING in) //Digital BOOL
    {
        if((in.size()!=1) || (IsEmpty(in)) )
            return false;
        return ((PGEGetChar(in[0])=='1')||(PGEGetChar(in[0])=='0'));
    }

    /*!
     * \brief Converts CVS-bool raw string into boolean value
     * \param in raw value
     * \return boolean equivalent
     */
    inline bool wBoolR(PGESTRING in)
    { return ((in=="#TRUE#")?true:false); }

    /******************RAW to Internal**********************/
    /*!
     * \brief Convert from SMBX64 string to internal with damage correction
     * \param in raw value
     * \return fixed string vale
     */
    inline PGESTRING StrToStr(PGESTRING in)
    {
        PGESTRING target = in;
        if(IsEmpty(target))
            return target;
        if(target[0]==PGEChar('\"'))
            PGE_RemStrRng(target, 0, 1);
        if((!IsEmpty(target)) && (target[target.size()-1]==PGEChar('\"')))
            PGE_RemStrRng(target, target.size()-1, 1);
        target=PGE_ReplSTRING(target, "\"", "\'");//Correct damaged by SMBX line
        return target;
    }

    /******************Internal to RAW**********************/
    /*!
     * \brief Generate raw string from integer value
     * \param input Source signed integer value
     * \return ASCII encoded signed integer value
     */
    template<typename T>
    inline PGESTRING WriteSInt(T input)
    {  return fromNum(static_cast<long long>(input))+"\n"; }

    /*!
     * \brief Generate raw string from unsigned integer value
     * \param input Source unsigned integer value
     * \return ASCII encoded unsigned integer value
     */
    template<typename T>
    inline PGESTRING WriteUInt(T input)
    {  return fromNum(static_cast<unsigned long long>(input))+"\n"; }

    /*!
     * \brief Generate raw CVS-bool string from boolean value
     * \param input Source boolean value
     * \return ASCII encoded CVS-bool value
     */
    inline PGESTRING WriteCSVBool(bool input)
    {  return PGESTRING( (input)?"#TRUE#":"#FALSE#" )+"\n"; }

    /*!
     * \brief Convert string into valid CVS string line (line feeds are will be removed)
     * \param input Source string
     * \return Valid CVS string
     */
    inline PGESTRING WriteStr(PGESTRING input)
    {
        input = PGE_RemSubSTRING(input, "\n");
        input = PGE_RemSubSTRING(input, "\r");
        input = PGE_RemSubSTRING(input, "\t");
        input = PGE_RemSubSTRING(input, "\"");
        return PGESTRING("\"")+input+PGESTRING("\"\n");
    }

    /*!
     * \brief Convert string into valid CVS string line (with line feeds keeping)
     * \param input Source string
     * \return Valid CVS string
     */
    inline PGESTRING WriteStr_multiline(PGESTRING input)
    {
        input = PGE_RemSubSTRING(input, "\t");
        input = PGE_ReplSTRING(input, "\"", "'");
        return PGESTRING("\"")+input+PGESTRING("\"\n");
    }

    /*!
     * \brief Generate raw string from floating point value
     * \param input Source floating point vale
     * \return ASCII encoded floating point value
     */
    inline PGESTRING WriteFloat(float input)
    {  return fromNum(input)+"\n"; }

    /*!
     * \brief Generate raw string from double floating point value
     * \param input Source floating point vale
     * \return ASCII encoded floating point value
     */
    inline PGESTRING WriteFloat(double input)
    {  return fromNum(input)+"\n"; }


    /******************Units converters**********************/
    /*!
     * \brief Convert 1/65 seconds into milliseconds
     * \param t65 1/65 time value
     * \return millisecond time equivalent
     */
    inline double t65_to_ms(double t65)
    { return t65 * (1000.0/65.0); }

    /*!
     * \brief Convert milliseconds into 1/65 seconds
     * \param ms time in milliseconds
     * \return 1/65 second time equivalent
     */
    inline double ms_to_65(double ms)
    { return ms * (65.0/1000.0); }

}


#endif // SMBX64_H

