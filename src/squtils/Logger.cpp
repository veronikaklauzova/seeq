/*
 @file Logger.cpp
 Contains definition for the %Logger class
 */
#include "common.h"
#include <time.h>

Logger* g_Logger = 0; ///<global var for current logger.

Logger::Logger(eLogLevel level, std::string fileName)
{
	if(fileName != ""){
		m_ostream = new std::ofstream(fileName.c_str());
		if(m_ostream->bad())
			throw "Can't open seeq.log";
	} else {
		m_ostream = &std::cout;
	}
	//std::cout<<"Logging started for the events of level "<<level<<" and above."<<std::endl;
	m_logLevel = level;
	g_Logger = this;
}

Logger::~Logger(void)
{
	if(m_ostream != &std::cout){
		delete m_ostream;
	}
	if(g_Logger == this){
		g_Logger = 0;
	}
}

void Logger::Log(std::string message, eLogLevel level){
	if(level >= m_logLevel){
		std::string outStr("[");
		char buff[16];
		_strtime_s(buff,16);
		outStr += buff;
		switch(level){
			case NOTICE:
				outStr += "][NOTICE] ";
				break;
			case WARNING:
				outStr += "][WARNING] ";
				break;
			case ERROR:
				outStr += "][ERROR] ";
				break;
		}
		outStr += message;
		(*m_ostream)<<outStr<<std::endl;
	}
}

void _LOG(std::string message, eLogLevel level){
	if(g_Logger == 0){
		return;
	}
	g_Logger->Log(message, level);
}