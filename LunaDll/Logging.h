#pragma once
#include "Defines.h"
#include <fstream>

#define LOG_FNAME L"log.txt"

typedef int LOGOPTS;
#define LOG_NewlineAfter 	0x00000001
#define LOG_Flush 			0x00000002
#define LOG_DateTime 		0x00000004
#define LOG_STD 			0x00000008 // Combines newline, flush, and datetime styles
#define LOG_Space 			0x00000010

using namespace std;

class Logger {
public:
	Logger();
	~Logger();

	void NewSession(); // print header in log
	void WriteDateTime(wofstream* file);
	void Log(std::wstring message, LOGOPTS options);

	// Members
	bool m_Enabled;
	wofstream m_File;
};