#pragma once
#include "Config.h"
#include <iostream>

/**
 Class for reading/writing config to file.
 */
class FileConfig :
	public Config
{
public:
	FileConfig(std::string fileName);
	virtual ~FileConfig(void);
	virtual void Load(); ///< load stored configuration.
	virtual void Save(); ///< save stored configuration.
protected:
	std::string m_fileName; ///< file name of target for reading/writing config.
};
