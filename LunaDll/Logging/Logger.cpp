#include "Logging.h"
#include <ctime>

#ifndef __MINGW32__
#pragma warning(disable: 4996) //localtime
#endif

using namespace std;

// 'TORS
Logger::Logger() {
	if(true) {
        m_File.open(LOG_FNAME, ios::out|ios::app);
        if(m_File.is_open() == false)
        {
			m_File.close();
			return;
		}

		NewSession();
	}	
}

Logger::~Logger() {
	if(m_File != NULL) {
		if(m_File.is_open() && m_Enabled) {
			m_File << L"**** Logger ended at ";
			WriteDateTime(&m_File);
			m_File << endl << endl;
		}
		m_File.flush();
		m_File.close();
	}
}

// NEW SESSION
void Logger::NewSession() {
	if(m_Enabled) {
		if(m_File != NULL && m_File.is_open()) {
			m_File << L"**** New session started at ";
			WriteDateTime(&m_File);
			m_File << endl;
		}
	}
}

// WRITE DATE TIME
void Logger::WriteDateTime(wofstream* file) {
	if(file != NULL && file->is_open()) {
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		*file << (now->tm_year + 1900) << '-' 
			<< (now->tm_mon + 1) << '-'
			<<  now->tm_mday;
	}
}

// LOG
void Logger::Log(wstring msg, LOGOPTS options) {
	if(m_Enabled) {
		if(m_File != NULL && m_File.is_open()) {

            if((options && LOG_DateTime) || (options && LOG_STD)) {
				m_File << L"[";
				WriteDateTime(&m_File);
				m_File << L"]: ";
			}

			// Write the message
			m_File.write(msg.c_str(), msg.length());

            if((options && LOG_NewlineAfter) || (options && LOG_STD))
				m_File << endl;

            if((options && LOG_Flush) || (options && LOG_STD))
				m_File.flush();

			if(options && LOG_Space)
				m_File << " ";
		}
	}
}
