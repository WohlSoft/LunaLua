#include <windows.h>
#include <math.h>
#include <string>
#include <cctype>
#include <clocale>
#include <fstream>
#include "MiscFuncs.h"
#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Layer.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../GlobalFuncs.h"

#ifdef __MINGW32__
std::wstring std::to_wstring(long long src)
{
    wstringstream tg;
    tg<<src;
    return tg.str();
}


std::string std::to_string(long long src)
{
    stringstream tg;
    tg << src;
    return tg.str();
}
#endif

void InfiniteFlying(int player) {
    PlayerMOB* demo = Player::Get(1);

    if(demo != 0)
        demo->FlightTimeRemaining = (short)50;
}


BOOL FileExists(LPCTSTR szPath) {
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool fileExists(const std::wstring &szPath)
{
    return (FileExists(szPath.c_str()) != FALSE);
}

bool fileExists(std::string szPath)
{
    const std::wstring &szPathFinal = Str2WStr(szPath);
    return (FileExists(szPathFinal.c_str()) != FALSE);
}

bool directoryExists(const std::wstring &szPath)
{
    return (DirectoryExists(szPath.c_str()) != FALSE);
}


bool hasSuffix(const std::string &path, const std::string &suffix)
{
    if(suffix.size() > path.size())
        return false;

    std::string f = path.substr(path.size() - suffix.size(), suffix.size());
    for(char &c : f)
        c = tolower(c);
    return (f.compare(suffix) == 0);
}

bool hasSuffix(const std::wstring &path, const std::wstring &suffix)
{
    if(suffix.size() > path.size())
        return false;

    std::wstring f = path.substr(path.size() - suffix.size(), suffix.size());
    for(wchar_t &c : f)
        c = towlower(c);
    return (f.compare(suffix) == 0);
}


FIELDTYPE StrToFieldtype(std::wstring string) {
    string.erase(string.find_last_not_of(L" \n\r\t")+1);
    if(string == L"b") {
        return FT_BYTE;
    }
    else if(string == L"s") {
        return FT_WORD;
    }
    else if(string == L"w") {
        return FT_WORD;
    }
    else if(string == L"dw") {
        return FT_DWORD;
    }
    else if(string == L"f") {
        return FT_FLOAT;
    }
    else if(string == L"df") {
        return FT_DFLOAT;
    }
    return FT_BYTE;
}

void MemAssign(int Address, double value, OPTYPE operation, FIELDTYPE ftype) {
    char* dbg =  "MemAssignDbg";
    if(ftype == FT_INVALID)
        return;

    if(operation == OP_Div && value == 0)
        return;

    switch(operation) {
    case OP_Assign: {
        switch(ftype) {
        case FT_BYTE: {
            *((byte*)Address) = (byte)value;
            break;
                        }
        case FT_WORD: {
            *((short*)Address) = (short)value;
            break;
                        }
        case FT_DWORD: {
            *((int*)Address) = (int)value;
            break;
                        }
        case FT_FLOAT: {
            *((float*)Address) = (float)value;
            break;
                        }
        case FT_DFLOAT: {
            *((double*)Address) = value;
            break;
                        }
        default:
            break;
        }
                    }//OP Assign
                    break;

    case OP_Add: {
        switch(ftype) {
        case FT_BYTE: {
            *((byte*)Address) += (byte)value;
            break;
                        }
        case FT_WORD: {
            *((short*)Address) += (short)value;
            break;
                        }
        case FT_DWORD: {
            *((int*)Address) += (int)value;
            break;
                        }
        case FT_FLOAT: {
            *((float*)Address) += (float)value;
            break;
                        }
        case FT_DFLOAT: {
            *((double*)Address) += value;
            break;
                        }
        default:
            break;
        }
                 }//OP Add
                 break;

    case OP_Sub: {
        switch(ftype) {
        case FT_BYTE: {
            *((byte*)Address) -= (byte)value;
            break;
                        }
        case FT_WORD: {
            *((short*)Address) -= (short)value;
            break;
                        }
        case FT_DWORD: {
            *((int*)Address) -= (int)value;
            break;
                        }
        case FT_FLOAT: {
            *((float*)Address) -= (float)value;
            break;
                        }
        case FT_DFLOAT: {
            *((double*)Address) -= value;
            break;
                        }
        default:
            break;
        }
                    }//OP Sub
                 break;

    case OP_Mult: {
        switch(ftype) {
        case FT_BYTE: {
            *((byte*)Address) *= (byte)value;
            break;
                        }
        case FT_WORD: {
            *((short*)Address) *= (short)value;
            break;
                        }
        case FT_DWORD: {
            *((int*)Address) *= (int)value;
            break;
                        }
        case FT_FLOAT: {
            *((float*)Address) *= (float)value;
            break;
                        }
        case FT_DFLOAT: {
            *((double*)Address) *= value;
            break;
                        }
        default:
            break;
        }
                    }//OP Mult
                  break;

    case OP_Div: {
        switch(ftype) {
        case FT_BYTE: {
            *((byte*)Address) /= (byte)value;
            break;
                        }
        case FT_WORD: {
            *((short*)Address) /= (short)value;
            break;
                        }
        case FT_DWORD: {
            *((int*)Address) /= (int)value;
            break;
                        }
        case FT_FLOAT: {
            *((float*)Address) = roundf(*((float*)Address) / (float)value);
            break;
                        }
        case FT_DFLOAT: {
            *((double*)Address) = round(*((float*)Address) / value);
            break;
                        }
        default:
            break;
        }
                    }//OP Div
                 break;

    case OP_XOR: {
        switch(ftype) {
        case FT_BYTE: {
            *((byte*)Address) ^= (byte)value;
            break;
                        }
        case FT_WORD: {
            *((short*)Address) ^= (short)value;
            break;
                        }
        case FT_DWORD: {
            *((int*)Address) ^= (int)value;
            break;
                        }
        default:
            break;
        }
                    }//OP XOR
                 break;

    default:
        break;
                 }// switch on op
}

bool CheckMem(int address, double value, COMPARETYPE ctype, FIELDTYPE ftype) {
    char* dbg =  "CHECKMEM DBG";
    if(ftype == FT_INVALID)
        return false;

    switch(ftype) {
    case FT_BYTE: {
        switch(ctype) {
        case CMPT_EQUALS: {
            return *((byte*)address) == (byte)value;
            break;
                          }

        case CMPT_GREATER: {
            return *((byte*)address) > (byte)value;
            break;
                          }

        case CMPT_LESS: {
            return *((byte*)address) < (byte)value;
            break;
                          }

        case CMPT_NOTEQ: {
            return *((byte*)address) != (byte)value;
            break;
                          }
        }
        break;
                  }


    case FT_WORD: {
        switch(ctype) {
        case CMPT_EQUALS: {
            return *((short*)address) == (short)value;
            break;
                          }

        case CMPT_GREATER: {
            return *((short*)address) > (short)value;
            break;
                          }

        case CMPT_LESS: {
            return *((short*)address) < (short)value;
            break;
                          }
        default:
            break;
        }
        break;
                  }

    case FT_DWORD: {
        switch(ctype) {
        case CMPT_EQUALS: {
            return *((int*)address) == (int)value;
            break;
                          }

        case CMPT_GREATER: {
            return *((int*)address) > (int)value;
            break;
                          }

        case CMPT_LESS: {
            return *((int*)address) < (int)value;
            break;
                          }
        default:
            break;
        }
        break;
                  }

    case FT_FLOAT: {
        switch(ctype) {
        case CMPT_EQUALS: {
            return *((float*)address) == (float)value;
            break;
                          }

        case CMPT_GREATER: {
            return *((float*)address) > (float)value;
            break;
                          }

        case CMPT_LESS: {
            return *((float*)address) < (float)value;
            break;
                          }
        default:
            break;
        }
        break;
                  }

    case FT_DFLOAT: {
        switch(ctype) {
        case CMPT_EQUALS: {
            return *((double*)address) == value;
            break;
                          }

        case CMPT_GREATER: {
            return *((double*)address) > value;
            break;
                          }

        case CMPT_LESS: {
            return *((double*)address) < value;
            break;
                          }
        default:
            break;
        }
        break;
                  }
    default:
        break;
    }

    return false;
}

double GetMem(int addr, FIELDTYPE ftype) {
    switch(ftype) {
    case FT_BYTE:
        return (double)*((byte*)addr);
    case FT_WORD:
        return (double)*((short*)addr);
    case 6: //FIELD_STRING
    case FT_DWORD:
        return (double)*((int*)addr);
    case FT_FLOAT:
        return (double)*((float*)addr);
    case FT_DFLOAT:
        return *((double*)addr);
    default:
        break;
    }
    return 0;
}

void InitIfMissing(std::map<std::wstring, double>* pMap, std::wstring sought_key, double init_val) {
    if(pMap->find(sought_key) == pMap->end()) {
        (*pMap)[sought_key] = init_val;
    }
}

int ComputeLevelSection(int x, int y) {
    int x_sec = ToSection(x);
    int y_sec = ToSection(y);
    if(x_sec != y_sec)
        return -1;
    return x_sec;
}

int ToSection(int coord) {
    coord = coord / 10000;
    switch(coord) {
    case -21:
        return 1;
    case -20:
        return 1;
    case -19:
        return 1;
    case -18:
        return 2;
    case -17:
        return 2;
    case -16:
        return 3;
    case -15:
        return 3;
    case -14:
        return 4;
    case -13:
        return 4;
    case -12:
        return 5;
    case -11:
        return 5;
    case -10:
        return 6;
    case -9:
        return 6;
    case -8:
        return 7;
    case -7:
        return 7;
    case -6:
        return 8;
    case -5:
        return 8;
    case -4:
        return 9;
    case -3:
        return 9;
    case -2:
        return 10;
    case -1:
        return 10;

    case 0:
        return 11;

    case 1:
        return 12;
    case 2:
        return 12;
    case 3:
        return 13;
    case 4:
        return 13;
    case 5:
        return 14;
    case 6:
        return 14;
    case 7:
        return 15;
    case 8:
        return 15;
    case 9:
        return 16;
    case 10:
        return 16;
    case 11:
        return 17;
    case 12:
        return 17;
    case 13:
        return 18;
    case 14:
        return 18;
    case 15:
        return 19;
    case 16:
        return 19;
    case 17:
        return 20;
    case 18:
        return 20;
    case 19:
        return 21;
    case 20:
        return 21;
    case 21:
        return 21;

    default:
        return -1;
    }
}

void RandomPointInRadius(double* ox, double* oy, double cx, double cy, int radius) {
    double phase1 = rand() % 360;
    double phase2 = rand() % 360;
    double xoff = sin(phase1) * radius;
    double yoff = cos(phase2) * radius;
    *ox = cx + xoff;
    *oy = cy + yoff;
}

bool FastTestCollision(int L1, int U1, int R1, int D1, int L2, int U2, int R2, int D2) {
    bool rightcol = true;
    bool leftcol = true;
    bool upcol = true;
    bool downcol = true;

    if(R1 < L2)
        rightcol = false;
    if(L1 > R2)
        leftcol = false;
    if(U1 > D2)
        downcol = false;
    if(D1 < U2)
        upcol = false;

    if(rightcol == false || leftcol == false || upcol == false || downcol == false)
        return false;
    return true;
}

void PrintSyntaxError(std::wstring errored_line) {
        static int errors = 0;
        errors += 25;
        RenderStringOp* render_str = new RenderStringOp();
        render_str->m_FontType = 2;
        render_str->m_FramesLeft = 440;
        render_str->m_String = errored_line;
        render_str->m_String += L"- SYNTAX ERROR";
        render_str->m_X = 125;
        render_str->m_Y = (float)(errors % 600);
        Renderer::Get().AddOp(render_str);
}

bool SegmentIntersectRectangle(double a_rectangleMinX,
                                 double a_rectangleMinY,
                                 double a_rectangleMaxX,
                                 double a_rectangleMaxY,
                                 double a_p1x,
                                 double a_p1y,
                                 double a_p2x,
                                 double a_p2y)
  {
    // Find min and max X for the segment

    double minX = a_p1x;
    double maxX = a_p2x;

    if(a_p1x > a_p2x)
    {
      minX = a_p2x;
      maxX = a_p1x;
    }

    // Find the intersection of the segment's and rectangle's x-projections

    if(maxX > a_rectangleMaxX)
    {
      maxX = a_rectangleMaxX;
    }

    if(minX < a_rectangleMinX)
    {
      minX = a_rectangleMinX;
    }

    if(minX > maxX) // If their projections do not intersect return false
    {
      return false;
    }

    // Find corresponding min and max Y for min and max X we found before

    double minY = a_p1y;
    double maxY = a_p2y;

    double dx = a_p2x - a_p1x;

    if(abs(dx) > 0.0000001)
    {
      double a = (a_p2y - a_p1y) / dx;
      double b = a_p1y - a * a_p1x;
      minY = a * minX + b;
      maxY = a * maxX + b;
    }

    if(minY > maxY)
    {
      double tmp = maxY;
      maxY = minY;
      minY = tmp;
    }

    // Find the intersection of the segment's and rectangle's y-projections

    if(maxY > a_rectangleMaxY)
    {
      maxY = a_rectangleMaxY;
    }

    if(minY < a_rectangleMinY)
    {
      minY = a_rectangleMinY;
    }

    if(minY > maxY) // If Y-projections do not intersect return false
    {
      return false;
    }

    return true;
}

#define NPC_SETTING_DATA(hex, index) std::to_string((long long)(((short*)hex)[index]))
#define WRITE_NPC_DATA(hex, index) NPC_SETTING_DATA(hex, index) << ";"

void readAndWriteNPCSettings()
{
    std::ofstream settingsOutput("npcsettings.csv");
    if(!settingsOutput)
        return;


    settingsOutput << "ID:;";
    settingsOutput << "GFX Offset X:;";
    settingsOutput << "GFX Offset Y:;";
    settingsOutput << "Width:;";
    settingsOutput << "Height;";
    settingsOutput << "GFX Width:;";
    settingsOutput << "GFX Height:;";
    settingsOutput << "Unknown [0xB26DDC]:;";
    settingsOutput << "Npc Block:;";
    settingsOutput << "Npc Block Top:;";
    settingsOutput << "Is Interactable NPC:;";
    settingsOutput << "Is Coin:;";
    settingsOutput << "Is Vine:;";
    settingsOutput << "Is Collectable Goal:;";
    settingsOutput << "Unknown [0xB27E52]:;";
    settingsOutput << "Unknown [0xB280AC]:;";
    settingsOutput << "Jumphurt:;";
    settingsOutput << "No Block Collision:;";
    settingsOutput << "Score:;";
    settingsOutput << "Player Block Top:;";
    settingsOutput << "Grab Top:;";
    settingsOutput << "Cliff Turn:;";
    settingsOutput << "No hurt:;";
    settingsOutput << "Player Block:;";
    settingsOutput << "Unknown [0xB295D6]:;";
    settingsOutput << "Grab Side:;";
    settingsOutput << "Is Shoe NPC:;";
    settingsOutput << "Is Yoshi NPC:;";
    settingsOutput << "Unknown [0xB29F3E]:;";
    settingsOutput << "No Yoshi:;";
    settingsOutput << "Foreground:;";
    settingsOutput << "Unknown [0xB2A64C]:;";
    settingsOutput << "Unknown [0xB2A8A6]:;";
    settingsOutput << "Is Vegetable NPC:;";
    settingsOutput << "Speed:;";
    settingsOutput << "No Fireball:;";
    settingsOutput << "No Iceball:;";
    settingsOutput << "No Gravity:;";
    settingsOutput << "\n";
    for(int i = 0; i <= 300; ++i){
        settingsOutput << std::to_string((long long)i) << ";";
        settingsOutput << WRITE_NPC_DATA(0xB25FC0,i);
        settingsOutput << WRITE_NPC_DATA(0xB2621A,i);
        settingsOutput << WRITE_NPC_DATA(0xB26474,i);
        settingsOutput << WRITE_NPC_DATA(0xB266CE,i);
        settingsOutput << WRITE_NPC_DATA(0xB26928,i);
        settingsOutput << WRITE_NPC_DATA(0xB26B82,i);
        settingsOutput << WRITE_NPC_DATA(0xB26DDC,i);
        settingsOutput << WRITE_NPC_DATA(0xB27036,i);
        settingsOutput << WRITE_NPC_DATA(0xB27290,i);
        settingsOutput << WRITE_NPC_DATA(0xB274EA,i);
        settingsOutput << WRITE_NPC_DATA(0xB27744,i);
        settingsOutput << WRITE_NPC_DATA(0xB2799E,i);
        settingsOutput << WRITE_NPC_DATA(0xB27BF8,i);
        settingsOutput << WRITE_NPC_DATA(0xB27E52,i);
        settingsOutput << WRITE_NPC_DATA(0xB280AC,i);
        settingsOutput << WRITE_NPC_DATA(0xB28306,i);
        settingsOutput << WRITE_NPC_DATA(0xB28560,i);
        settingsOutput << WRITE_NPC_DATA(0xB287BA,i);
        settingsOutput << WRITE_NPC_DATA(0xB28A14,i);
        settingsOutput << WRITE_NPC_DATA(0xB28C6E,i);
        settingsOutput << WRITE_NPC_DATA(0xB28EC8,i);
        settingsOutput << WRITE_NPC_DATA(0xB29122,i);
        settingsOutput << WRITE_NPC_DATA(0xB2937C,i);
        settingsOutput << WRITE_NPC_DATA(0xB295D6,i);
        settingsOutput << WRITE_NPC_DATA(0xB29830,i);
        settingsOutput << WRITE_NPC_DATA(0xB29A8A,i);
        settingsOutput << WRITE_NPC_DATA(0xB29CE4,i);
        settingsOutput << WRITE_NPC_DATA(0xB29F3E,i);
        settingsOutput << WRITE_NPC_DATA(0xB2A198,i);
        settingsOutput << WRITE_NPC_DATA(0xB2A3F2,i);
        settingsOutput << WRITE_NPC_DATA(0xB2A64C,i);
        settingsOutput << WRITE_NPC_DATA(0xB2A8A6,i);
        settingsOutput << WRITE_NPC_DATA(0xB2AB00,i);
        settingsOutput << WRITE_NPC_DATA(0xB2AD5C,i);
        settingsOutput << WRITE_NPC_DATA(0xB2B210,i);
        settingsOutput << WRITE_NPC_DATA(0xB2B46A,i);
        settingsOutput << WRITE_NPC_DATA(0xB2B6C4,i);
        settingsOutput << "\n";
    }
}


const wchar_t *BoolToString(bool b)
{
    return (b ? L"TRUE" : L"FALSE");
}

