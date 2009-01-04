#pragma once
#include <unordered_map>
#include <string>
/**
 Basic class for game configuration purposes.
 Mostly virtual.
 @sa %FileConfig
 @todo make %RegistryConfig
 */
class Config
{
protected:
	Config(void);///<ctor is protected: prohibits class creation. Use child class instead.
public:
	virtual ~Config(void);
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
	void SetParam(std::string paramName, std::string value);///< simply sets param into value.
	void LoadDefaults();///< load default settings

	Config* Get();///<returns current config (
protected:
	std::tr1::unordered_map<std::string, std::string> m_params;
};
