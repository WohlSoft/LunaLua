#include "MiscFuncs.h"
#include "Globals.h"
#include "PlayerMOB.h"
#include "Layer.h"
#include <windows.h>
#include <math.h>

void NumpadLayerControl1(LayerControl* sought_layer) {		
	gNumpad4 = GetKeyState(VK_NUMPAD4);
	gNumpad8 = GetKeyState(VK_NUMPAD8);
	gNumpad2 = GetKeyState(VK_NUMPAD2);
	gNumpad6 = GetKeyState(VK_NUMPAD6);	

	Layer::Stop(sought_layer);

	if(gNumpad4 & 0x80) {
		Layer::SetXSpeed(sought_layer, -2);
	}
	if(gNumpad8 & 0x80) {
		Layer::SetYSpeed(sought_layer, -2);
	}
	if(gNumpad6 & 0x80) {
		Layer::SetXSpeed(sought_layer, 2);
	}
	if(gNumpad2 & 0x80) {
		Layer::SetYSpeed(sought_layer, 2);
	} 		
}

void InfiniteFlying(int player) {
	PlayerMOB* demo = Player::Get(1);

	if(demo != 0)
		demo->FlightTimeRemaining = (short)50;
}


std::wstring removeExtension(const std::wstring filename) {
    size_t lastdot = filename.find_last_of(L".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot); 
}

BOOL FileExists(LPCTSTR szPath) {
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
};

BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
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
		}
		break;
				  }					
	}

	return false;
}

double GetMem(int addr, FIELDTYPE ftype) {
	switch(ftype) {
	case FT_BYTE:
		return (double)*((byte*)addr);
	case FT_WORD:
		return (double)*((short*)addr);
	case FT_DWORD:
		return (double)*((int*)addr);
	case FT_FLOAT:
		return (double)*((float*)addr);
	case FT_DFLOAT:
		return *((double*)addr);
	}
	return 0;
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