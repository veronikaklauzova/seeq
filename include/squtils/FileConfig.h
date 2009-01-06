#pragma once
#include "Config.h"
#include <iostream>

/**
 Class for reading/writing config to file.
 Singletoned by ZThread. (yeah, multiple inheritance is bad, i know)
 @warning NOT thread safe. Use with caution.
 */
class FileConfig : public Config, public ZThread::Singleton<FileConfig> {
public:
	FileConfig():Config(){}//private goes public, prohibits class creation
	void Init(std::string fileName);///< Initializes config. Run this first.
	virtual ~FileConfig(void);
	virtual void Load(); ///< load stored configuration.
	virtual void Save(); ///< save stored configuration.
protected:
	std::string m_fileName; ///< file name of target for reading/writing config.
};

///macro for getting config instance
#define sConfig FileConfig::instance()