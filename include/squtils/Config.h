#pragma once
#include <unordered_map>
#include <string>
#include "zthread/Singleton.h"
/**
 Basic class for game configuration purposes.
 Mostly virtual.
 @sa %FileConfig
 @todo make %RegistryConfig
 @version 0.2
 @warning NOT thread-safe. Use with caution.
 */
class Config {
protected:
	Config(){}///<ctor is protected: prohibits class creation. Use child class instead.
public:
	//virtual void Init() = 0;///<initializes config
	virtual ~Config(){}
	virtual void Load() = 0; ///< load stored configuration.
	virtual void Save() = 0; ///< save stored configuration.
	/**
	 Get parametr value.
	 @param paramName name of configuration parametr. not a reference type for easyness of using.
	 @return Return value is a std::string with string representation of value.
	 It isn't typificated coz %Config in general can't know the type of configured parametr.
	 @todo Typificate it somehow.
	 may be add a param to param ( :D ), deciding the type and converting on first call.
	 */
	std::string GetParam(std::string paramName);
	long GetLong(std::string paramName);///<Reads param and converts it to long
	void SetParam(std::string paramName, std::string value);///< simply sets param into value.
	void LoadDefaults();///< load default settings


protected:
	std::tr1::unordered_map<std::string, std::string> m_params;
};