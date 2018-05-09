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
#include <QMutex>
#include <QMutexLocker>
#else
#include <regex>
#endif

#include "pge_x.h"
#include "file_strlist.h"

namespace PGEExtendedFormat
{
    static const char *heximal_valid_chars    = "0123456789ABCDEFabcdef";
    static const pge_size_t heximal_valid_chars_len = 22;

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


#ifdef PGE_FILES_QT
PGEFile::PGEFile(QObject *parent)
    : QObject(parent)
#else
PGEFile::PGEFile()
#endif
{
    m_lastError = "";
    m_rawData = "";
}

#ifdef PGE_FILES_QT
PGEFile::PGEFile(const PGEFile &pgeFile, QObject *parent)
    : QObject(parent)
#else
PGEFile::PGEFile(const PGEFile & pgeFile)
#endif
{
    m_rawData = pgeFile.m_rawData;
    m_rawDataTree = pgeFile.m_rawDataTree;
    m_lastError = pgeFile.m_lastError;
}

PGEFile::PGEFile(const PGESTRING &_rawData)
{
    m_rawData = _rawData;
    m_lastError = "";
}

PGESTRING PGEFile::removeQuotes(PGESTRING str)
{
    PGESTRING target = PGE_RemStrRng(str, 0, 1);
    target = PGE_RemStrRng(target, static_cast<int>(target.size() - 1), 1);
    return target;
}

void PGEFile::setRawData(const PGESTRING &_rawData)
{
    m_rawData = _rawData;
}

bool PGEFile::buildTreeFromRaw()
{
    PGEXSct PGEXsection;

    FileStringList in;
    in.addData(m_rawData);

    //Read raw data sections
    bool sectionOpened = false;
    while(!in.atEnd())
    {
        PGEXsection.first = in.readLine();
        PGEXsection.second.clear();

        //Skip empty parts
        PGESTRING pgex_sectionName = removeSpaces(PGEXsection.first);
        if(IsEmpty(pgex_sectionName)) continue;

        sectionOpened = true;
        PGESTRING data;
        while(!in.atEnd())
        {
            data = in.readLine();
            if(data == PGEXsection.first + "_END")
            {
                sectionOpened = false;    // Close Section
                break;
            }
            PGEXsection.second.push_back(data);
        }
        m_rawDataTree.push_back(PGEXsection);
    }

    if(sectionOpened)
    {
        m_lastError = PGESTRING("Section [" + PGEXsection.first + "] is not closed");
        return false;
    }

    //Building tree

    for(pge_size_t z = 0; z < m_rawDataTree.size(); z++)
    {
        bool valid = true;
        PGEX_Entry subTree = buildTree(m_rawDataTree[z].second, &valid);
        if(valid)
        {
            //Store like subtree
            subTree.type = PGEX_Struct;
            subTree.name = m_rawDataTree[z].first;
            dataTree.push_back(subTree);
        }
        else
        {
            //Store like plain text
            PGEX_Item dataItem;
            dataItem.type = PGEX_PlainText;
            subTree.data.clear();
            subTree.subTree.clear();
            PGEX_Val dataValue;
            dataValue.marker = "PlainText";
            for(pge_size_t i = 0; i < m_rawDataTree[z].second.size(); i++)
                dataValue.value += m_rawDataTree[z].second[i] + "\n";
            dataItem.values.push_back(dataValue);
            subTree.name = m_rawDataTree[z].first;
            subTree.type = PGEX_PlainText;
            subTree.data.push_back(dataItem);
            dataTree.push_back(subTree);
            valid = true;
        }
    }

    return true;
}


PGEFile::PGEX_Entry PGEFile::buildTree(PGESTRINGList &src_data, bool *_valid)
{
    PGEX_Entry entryData;

    bool valid = true;
    for(pge_size_t q = 0; q < src_data.size(); q++)
    {
        if(IsSectionTitle(removeSpaces(src_data[q])))
        {
            //Build and store subTree
            PGESTRING nameOfTree = removeSpaces(src_data[q]);
            PGESTRINGList rawSubTree;
            q++;
            for(; q < src_data.size() && src_data[q] != nameOfTree + "_END" ; q++)
            {
                rawSubTree.push_back(src_data[q]);
            }
            PGEX_Entry subTree = buildTree(rawSubTree, &valid);
            if(valid)
            {
                //Store like subtree
                subTree.name = nameOfTree;
                entryData.subTree.push_back(subTree);
                entryData.type = PGEX_Struct;
            }
            else
            {
                //Store like plain text
                subTree.name = nameOfTree;
                subTree.subTree.clear();
                subTree.type = PGEX_PlainText;
                subTree.data.clear();

                PGEX_Item dataItem;
                PGEX_Val dataValue;
                dataItem.type = PGEX_PlainText;

                dataValue.marker = nameOfTree;
                //foreach(PGESTRING x, rawSubTree) dataValue.value += x+"\n";
                for(pge_size_t i = 0; i < rawSubTree.size(); i++)
                    dataValue.value += rawSubTree[i] + "\n";
                dataItem.values.push_back(dataValue);
                subTree.data.push_back(dataItem);
                entryData.subTree.push_back(subTree);
                entryData.type = PGEX_Struct;
                valid = true;
            }

        }
        else
        {
            PGEX_Item dataItem;
            PGESTRING &srcData_nc = src_data[q];
            enum States
            {
                STATE_MARKER = 0,
                STATE_VALUE = 1,
                STATE_ERROR = 2
            };
            pge_size_t state = 0, size = srcData_nc.size(), tail = srcData_nc.size() - 1;
            PGEX_Val dataValue;
            int escape = 0;
            for(pge_size_t i = 0; i < size; i++)
            {
                if(state == STATE_ERROR)
                {
                    valid = false;
                    break;
                }
                PGEChar c = srcData_nc[i];
                if(escape > 0)
                {
                    escape--;
                }
                if((c == '\\') && (escape == 0))
                {
                    //Skip escape sequence
                    escape = 2;
                }
                switch(state)
                {
                case STATE_MARKER:
                    if((c == ';') && (escape == 0))
                    {
                        state = STATE_ERROR;
                        continue;
                    }
                    if((c == ':') && (escape == 0))
                    {
                        state = STATE_VALUE;
                        continue;
                    }
                    dataValue.marker.push_back(c);
                    break;
                case STATE_VALUE:
                    if((c == ':') && (escape == 0))
                    {
                        state = STATE_ERROR;
                        continue;
                    }
                    if(((c == ';') && (escape == 0)) || (i == tail))
                    {
                        //STORE DATA
                        dataItem.values.push_back(dataValue);
                        dataValue.marker.clear();
                        dataValue.value.clear();
                        state = STATE_MARKER;
                        continue;
                    }
                    dataValue.value.push_back(c);
                    break;
                    //case STATE_ERROR: //Dead code
                    //break;
                }
            }
            dataItem.type = PGEX_Struct;
            entryData.type = PGEX_Struct;
            entryData.data.push_back(dataItem);
            //            PGE_SPLITSTRING(fields, srcData_nc, ";");
            //            PGEX_Item dataItem;
            //            dataItem.type = PGEX_Struct;
            //            for(int i=0;i<(signed)fields.size(); i++)
            //            {
            //                PGESTRING fields_ns = removeSpaces(fields[i]);
            //                if(IsEmpty(fields_ns)) continue;

            //                //Store data into list
            //                PGESTRINGList value;
            //                PGE_SPLITSTRING(value, fields[i], ":");

            //                if( value.size() != 2 )
            //                {
            //                    valid = false; break;
            //                }

            //                PGEX_Val dataValue;
            //                    dataValue.marker = value[0];
            //                    dataValue.value = value[1];
            //                dataItem.values.push_back(dataValue);
            //            }
            //            entryData.type = PGEX_Struct;
            //            entryData.data.push_back(dataItem);
        }
        if(!valid) break;
    }

    if(_valid) *_valid = valid;
    return entryData;
}



PGESTRING PGEFile::lastError()
{
    return m_lastError;
}


bool PGEFile::IsSectionTitle(PGESTRING in)
{
    for(pge_size_t i = 0; i < in.size(); i++)
    {
        #ifdef PGE_FILES_QT
        char cc = in[i].toLatin1();
        #else
        char &cc = in[i];
        #endif
        if(
            ((cc < 'A') || (cc > 'Z')) &&
            ((cc < '0') || (cc > '9')) &&
            (cc != '_')
        )
            return false;
    }
    return true;
}


//validatos
bool PGEFile::IsQoutedString(PGESTRING in) // QUOTED STRING
{
    //return QRegExp("^\"(?:[^\"\\\\]|\\\\.)*\"$").exactMatch(in);
    pge_size_t i = 0;
    bool escape = false;
    for(i = 0; i < in.size(); i++)
    {
        if(i == 0)
        {
            if(in[i] != '"')
                return false;
        }
        else if(i == in.size() - 1)
        {
            if((in[i] != '"') || escape)
                return false;
        }
        else if((in[i] == '\\') && !escape)
        {
            escape = true;
            continue;
        }
        else if((in[i] == '"') && !escape)
        {
            return false;
        }
        else if((in[i] == '"') && !escape)
        {
            return false;
        }
        escape = false;
    }
    if(i == 0) return false;
    return true;
}

bool PGEFile::IsHex(PGESTRING in) // Heximal string
{
    using namespace PGEExtendedFormat;
    return isValid(in, heximal_valid_chars, heximal_valid_chars_len);
}

bool PGEFile::IsBool(PGESTRING in) // Boolean
{
    if((in.size() != 1) || (IsEmpty(in)))
        return false;
    return ((PGEGetChar(in[0]) == '1') || (PGEGetChar(in[0]) == '0'));
}

bool PGEFile::IsIntU(PGESTRING in) // Unsigned Int
{
    using namespace PGEExtendedFormat;
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

bool PGEFile::IsIntS(PGESTRING in) // Signed Int
{
    using namespace PGEExtendedFormat;

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

bool PGEFile::IsFloat(PGESTRING &in) // Float Point numeric
{
    using namespace PGEExtendedFormat;

    if(IsEmpty(in))
        return false;

    if((in.size() == 1) && (!isDegit(in[0])))
        return false;
    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-') && (PGEGetChar(in[0]) != '.') && (PGEGetChar(in[0]) != ','))
        return false;

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
                if(i == (in.size() - 1))
                    return false;
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
                    if(i == (in.size() - 1))
                        return false;
                    continue;
                }
            }
        }
        if(!isDegit(in[i])) return false;
    }

    return true;
}

bool PGEFile::IsBoolArray(PGESTRING in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return isValid(in, "01", 2);
}

bool PGEFile::IsIntArray(PGESTRING in) // Boolean array
{
    using namespace PGEExtendedFormat;
    #ifdef PGE_FILES_QT
    return QRegExp("^\\[(\\-?\\d+,?)*\\]$").exactMatch(in);
    #else
    //FIXME
    std::regex rx("^\\[(\\-?\\d+,?)*\\]$");
    return std::regex_match(in, rx);
    #endif
}

bool PGEFile::IsStringArray(PGESTRING in) // String array
{
    bool valid = true;
    pge_size_t i = 0, depth = 0, comma = 0;
    bool escape = false;
    while(i < in.size())
    {
        switch(depth)
        {
        case 0://outside array
            if(in[i] == '[')
            {
                depth = 1;
                comma++;
            }
            else valid = false;
            break;
        case 1://between entries
            switch(comma)
            {
            case 0:
                if(in[i] == ']') depth = 0; //Close array
                else if(in[i] == ',') comma++; //Close array
                else valid = false;
                break;
            case 1:
                if(in[i] == '"')     depth = 2; //Open value
                else valid = false;
            }
            break;
        case 2://Inside entry
            if((in[i] == '"') && (!escape))
            {
                depth = 1;    //Close value
                comma = 0;
            }
            else if((in[i] == '[') && (!escape))
            {
                valid = false;
            }
            else if((in[i] == ']') && (!escape))
            {
                valid = false;
            }
            else if((in[i] == ',') && (!escape))
            {
                valid = false;
            }
            else if((in[i] == '\\') && (!escape))
            {
                escape = true;
                break;
            }
            escape = false;
            break;
        }
        if(!valid) break;//Stop parsing on invalid
        i++;
    }
    return valid;
}


PGESTRINGList PGEFile::X2STRArr(PGESTRING in, bool *_valid)
{
    PGESTRINGList strArr;
    PGESTRING entry;
    bool valid = true;
    pge_size_t i = 0, depth = 0, comma = 0;
    bool escape = false;
    while(i < in.size())
    {
        switch(depth)
        {
        case 0://outside array
            if(in[i] == '[')
            {
                depth = 1;
                comma++;
            }
            else valid = false;
            break;
        case 1://between entries
            switch(comma)
            {
            case 0:
                if(in[i] == ']') depth = 0; //Close array
                else if(in[i] == ',') comma++; //Close array
                else valid = false;
                break;
            case 1:
                if(in[i] == '"')     depth = 2; //Open value
                else valid = false;
            }
            break;
        case 2://Inside entry
            if((in[i] == '"') && (!escape))
            {
                strArr.push_back(X2STRING(entry));    //Close value
                entry.clear();
                depth = 1;
                comma = 0;
            }
            else if((in[i] == '[') && (!escape))
            {
                valid = false;
                break;
            }
            else if((in[i] == ']') && (!escape))
            {
                valid = false;
                break;
            }
            else if((in[i] == ',') && (!escape))
            {
                valid = false;
                break;
            }
            else if((in[i] == '\\') && (!escape))
            {
                escape = true;
                break;
            }
            entry.push_back(in[i]);
            escape = false;
            break;
        }
        if(!valid) break;//Stop parsing on invalid
        i++;
    }
    if(_valid) *_valid = valid;
    return strArr;
}

PGELIST<bool > PGEFile::X2BollArr(PGESTRING src)
{
    PGELIST<bool > arr;
    for(pge_size_t i = 0; i < src.size(); i++)
        arr.push_back(src[i] == '1');
    return arr;
}

PGELIST<PGESTRINGList > PGEFile::splitDataLine(PGESTRING src_data, bool *_valid)
{
    PGELIST<PGESTRINGList > entryData;
    bool valid = true;
    enum States
    {
        STATE_MARKER = 0,
        STATE_VALUE = 1,
        STATE_ERROR = 2
    };
    pge_size_t state = 0, size = src_data.size(), tail = src_data.size() - 1;
    PGESTRING marker;
    PGESTRING value;
    int escape = 0;
    for(pge_size_t i = 0; i < size; i++)
    {
        if(state == STATE_ERROR)
        {
            valid = false;
            break;
        }
        PGEChar c = src_data[i];
        if(escape > 0)
        {
            escape--;
        }
        if((c == '\\') && (escape == 0))
        {
            //Skip escape sequence
            escape = 2;
        }
        switch(state)
        {
        case STATE_MARKER:
            if((c == ';') && (escape == 0))
            {
                state = STATE_ERROR;
                continue;
            }
            if((c == ':') && (escape == 0))
            {
                state = STATE_VALUE;
                continue;
            }
            marker.push_back(c);
            break;
        case STATE_VALUE:
            if((c == ':') && (escape == 0))
            {
                state = STATE_ERROR;
                continue;
            }
            if(((c == ';') && (escape == 0)) || (i == tail))
            {
                //STORE ENTRY!
                PGESTRINGList fields;
                fields.push_back(marker);
                fields.push_back(value);
                entryData.push_back(fields);
                marker.clear();
                value.clear();
                state = STATE_MARKER;
                continue;
            }
            value.push_back(c);
            break;
            //case STATE_ERROR:  //Dead code
            //break;
        }
    }
    if(_valid) * _valid = valid;
    return entryData;
}

PGESTRING PGEFile::hStrS(PGESTRING input)
{
    return PGEGetChar(input);
}

PGESTRING PGEFile::WriteStrArr(PGESTRINGList &input)
{
    PGESTRING output;
    if(IsEmpty(input))
        return PGESTRING("");
    output.append("[");
    for(pge_size_t i = 0; i < input.size(); i++)
    {
        output.append(WriteStr(input[i]));
        output.append(i < input.size() - 1 ? "," : "");
    }
    output.append("]");
    return output;
}

PGESTRING PGEFile::WriteIntArr(PGELIST<int> input)
{
    PGESTRING output;
    if(input.empty()) return PGESTRING("");
    output.append("[");
    for(pge_size_t i = 0; i < input.size(); i++)
    {
        output.append(fromNum(input[i]) + (i < input.size() - 1 ? "," : ""));
    }
    output.append("]");
    return output;
}

PGESTRING PGEFile::WriteBoolArr(PGELIST<bool> input)
{
    PGESTRING output;
    for(pge_size_t i = 0; i < input.size(); i++)
    {
        output.append(input[i] ? "1" : "0");
    }
    return output;
}

PGESTRING PGEFile::X2STRING(PGESTRING input)
{
    restoreString(input, true);
    return input;
}

void PGEFile::restoreString(PGESTRING &input, bool removeQuotes)
{
    PGESTRING &output = input;
    const pge_size_t first = 0;//For convenience to understand
    pge_size_t j = 0, size = input.size(), tail = input.size() - 1;
    for(pge_size_t i = 0; i < size; i++, j++)
    {
        if(removeQuotes && ((i == first) || (i == tail)))
        {
ReCheckQuotie:
            //Skip quotie character at begin/end
            if(input[i] == '\"')
            {
                i++;
                if(i == tail) goto ReCheckQuotie;
            }
            if(i >= size)
                break;
        }
        if(input[i] == '\\')
        {
            pge_size_t k = i + 1;
            if(k >= size)
            {
                output[j] = '\\';
                continue;
            }
            char c = PGEGetChar(input[k]);
            switch(c)
            {
            case 'n':
                output[j] = '\n';
                i++;
                break;
            case 'r':
                output[j] = '\r';
                i++;
                break;
            case '\"':
                output[j] = '\"';
                i++;
                break;
            case ';':
                output[j] = ';';
                i++;
                break;
            case ':':
                output[j] = ':';
                i++;
                break;
            case '[':
                output[j] = '[';
                i++;
                break;
            case ']':
                output[j] = ']';
                i++;
                break;
            case ',':
                output[j] = ',';
                i++;
                break;
            case '%':
                output[j] = '%';
                i++;
                break;
            case '\\':
                output[j] = '\\';
                i++;
                break;
            default:
                output[j++] = input[i];
                output[j]   = input[k];
                i++;
                break;
            }
        }
        else
        {
            output[j] = input[i];
        }
    }
    output.resize(j);
}

void PGEFile::escapeString(PGESTRING &output, const PGESTRING &input, bool addQuotes)
{
    pge_size_t j = 0, size = input.size();
    output.resize(size * 2 + (addQuotes ? 2 : 0));
    if(addQuotes)
        output[j++] = '\"';
    for(pge_size_t i = 0; i < size; i++, j++)
    {
        char c = PGEGetChar(input[i]);
        switch(c)
        {
        case '\n':
            output[j++] = '\\';
            output[j] = 'n';
            break;
        case '\r':
            output[j++] = '\\';
            output[j] = 'r';
            break;
        case '\"':
            output[j++] = '\\';
            output[j] = '\"';
            break;
        case ';':
            output[j++] = '\\';
            output[j] = ';';
            break;
        case ':':
            output[j++] = '\\';
            output[j] = ':';
            break;
        case '[':
            output[j++] = '\\';
            output[j] = '[';
            break;
        case ']':
            output[j++] = '\\';
            output[j] = ']';
            break;
        case ',':
            output[j++] = '\\';
            output[j] = ',';
            break;
        case '%':
            output[j++] = '\\';
            output[j] = '%';
            break;
        case '\\':
            output[j++] = '\\';
            output[j] = '\\';
            break;
        default:
            output[j] = input[i];
            break;
        }
    }
    if(addQuotes)
        output[j++] = '\"';
    output.resize(j);
}

