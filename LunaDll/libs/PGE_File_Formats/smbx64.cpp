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

#include "smbx64.h"

namespace smbx64Format
{
    //const char *uint_vc = "0123456789";
    //const int   uint_vc_len = 10;

    bool isDegit(PGEChar c)
    {
        return ((c >= '0') && (c <= '9'));
    }

    bool isValid(PGESTRING &s, const char *valid_chars, const pge_size_t &valid_chars_len)
    {
        if(IsEmpty(s)) return false;
        pge_size_t i, j;
        for(i = 0; i < s.size(); i++)
        {
            bool found = false;
            for(j = 0; j < valid_chars_len; j++)
            {
                if(PGEGetChar(s[i]) == valid_chars[j])
                {
                    found = true;
                    break;
                }
            }
            if(!found) return false;
        }
        return true;
    }
}

// /////////////Validators///////////////
//returns FALSE on wrong data

bool SMBX64::IsUInt(PGESTRING in) // UNSIGNED INT
{
    using namespace smbx64Format;
    #ifdef PGE_FILES_QT
    PGEChar *data = in.data();
    #else
    PGEChar *data = (char *)in.data();
    #endif
    pge_size_t strSize = in.size();
    for(pge_size_t i = 0; i < strSize; i++)
    {
        PGEChar c = *data++;
        if((c < '0') || (c > '9')) return false;
    }
    return true;
}

bool SMBX64::IsSInt(PGESTRING in) // SIGNED INT
{
    using namespace smbx64Format;
    if(IsEmpty(in)) return false;

    if((in.size() == 1) && (!isDegit(in[0])))           return false;
    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-')) return false;

    #ifdef PGE_FILES_QT
    PGEChar *data = in.data() + 1;
    #else
    PGEChar *data = (char *)in.data() + 1;
    #endif
    pge_size_t strSize = in.size();
    for(pge_size_t i = 1; i < strSize; i++)
    {
        PGEChar c = *data++;
        if((c < '0') || (c > '9')) return false;
    }

    return true;
}

bool SMBX64::IsFloat(PGESTRING &in) // SIGNED FLOAT
{
    using namespace smbx64Format;

    if(IsEmpty(in)) return true;

    if((in.size() == 1) && (!isDegit(in[0])))          return false;
    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-') && (PGEGetChar(in[0]) != '.') && (PGEGetChar(in[0]) != ',')) return false;

    bool decimal = false;
    bool pow10  = false;
    bool sign   = false;
    for(pge_size_t i = ((PGEGetChar(in[0]) == '-') ? 1 : 0); i < in.size(); i++)
    {
        if((!decimal) && (!pow10))
        {
            if((PGEGetChar(in[i]) == '.') || (PGEGetChar(in[i]) == ','))
            {
                in[i] = '.'; //replace comma with a dot
                decimal = true;
                if(i == (in.size() - 1)) return false;
                continue;
            }
        }
        if(!pow10)
        {
            if((PGEGetChar(in[i]) == 'E') || (PGEGetChar(in[i]) == 'e'))
            {
                pow10 = true;
                if(i == (in.size() - 1)) return false;
                continue;
            }
        }
        else
        {
            if(!sign)
            {
                sign = true;
                if((PGEGetChar(in[i]) == '+') || (PGEGetChar(in[i]) == '-'))
                {
                    if(i == (in.size() - 1)) return false;
                    continue;
                }
            }
        }
        if(!isDegit(in[i])) return false;
    }
    return true;
}

bool SMBX64::IsQuotedString(PGESTRING in)
{
    //This is INVERTED validator. If false - good, true - bad.
#define QStrGOOD true
#define QStrBAD false
    pge_size_t i = 0;
    for(i = 0; i < in.size(); i++)
    {
        if(i == 0)
        {
            if(in[i] != '"') return QStrBAD;
        }
        else if(i == in.size() - 1)
        {
            if(in[i] != '"') return QStrBAD;
        }
        else if(in[i] == '"') return QStrBAD;
        else if(in[i] == '"') return QStrBAD;
    }
    if(i == 0) return QStrBAD; //This is INVERTED validator. If false - good, true - bad.
    return QStrGOOD;
}

