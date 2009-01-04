/*
 @file Logger.h
 Contains header for the %Logger class
 */
#include <string>
#include <iostream>

#pragma once


///enum for defining relative danger of log enty.
enum eLogLevel{
	NOTICE,//!< log notices and above
	WARNING,//!< log warnings and above
	ERROR //!< log only critical errors
};

/**
 A simple logger class.
 Used for log events in game, mostly when/for debugging.
 @todo make logging into files.

 */
class Logger
{
public:

	/**
	 %Logger constructor.
	 @param level Sets the level of logging.
	 %Logger will log all events having this level or above.
	 i.e. #eLogLevel::WARNING will make %Logger to log all the events with danger level of WARNING and ERROR.
	 @param fileName %Logger will log into this file. If empty - will log into #std::cout.
	 */
	Logger(eLogLevel level, std::string fileName = "");
	virtual ~Logger(void);

	/**
	 Main logger function.
	 Call this when u want to log anything.
	 @param message The message u like to write
	 @param level Message danger-level. One of
			- NOTICE
			- WARNING
			- ERROR
	 */
	void Log(std::string message, eLogLevel level);
private:
	eLogLevel m_logLevel;///< level of logging
	std::ostream *m_ostream;///< stream, in which all logs will go
};
///"macro" function for logging from anywhere, if %Logger was created. Uses last created logger.
void _LOG(std::string message, eLogLevel level);

#define LOG(m,l) _LOG(std::string("<")+__FUNCTION__+"> "+m,l);
