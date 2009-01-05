/*
 @file Logger.cpp
 Contains definition for the %Logger class
 */
#include "common.h"
#include <time.h>
#include <stdarg.h>
#include "zthread/ZThread.h"

Logger::Logger():m_ostream(0){}

void Logger::Init(eLogLevel level, std::string fileName)
{
	//prevents leak on reinitialize
	if(m_ostream && m_ostream != &std::cout){
		delete m_ostream;
	}
	if(fileName != ""){
		m_ostream = new std::ofstream(fileName.c_str());
		if(m_ostream->bad())
			throw "Can't open seeq.log";
	} else {
		m_ostream = &std::cout;
	}
	
	m_logLevel = level;
}

Logger::~Logger(void)
{
	if(m_ostream != &std::cout){
		delete m_ostream;
	}
}

void Logger::Log(eLogLevel level, const char* message, ...){
	if(!m_ostream) return;
	if(level >= m_logLevel){
		//guarding output for multithreading
		ZThread::Guard<ZThread::FastMutex> gvrd(m_lock);
		std::string outStrPref("[");
		char buff[16*1024];
		_strtime_s(buff,1024);
		outStrPref += buff;
		switch(level){
			case LVL_DEBUG:
				outStrPref += "][DEBUG] ";
				SetConColor(GREEN);
				break;
			case LVL_NOTICE:
				outStrPref += "][NOTICE] ";
				SetConColor(GREEN,true);
				break;
			case LVL_WARNING:
				outStrPref += "][WARNING] ";
				SetConColor(YELLOW);
				break;
			case LVL_ERROR:
				outStrPref += "][ERROR] ";
				SetConColor(RED,true);
				break;
		}
		va_list args;

		va_start(args, message);
		vsprintf_s(buff, 16*1024, message, args);
		(*m_ostream)<<outStrPref;
		SetConColor(WHITE);
		(*m_ostream)<<buff<<std::endl;
	}
}