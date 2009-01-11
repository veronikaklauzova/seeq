#include "common.h"

std::string Config::GetParam(std::string paramName){
	return m_params[paramName];
}

long Config::GetLong(std::string paramName){
	return atol(m_params[paramName].c_str());
}

void Config::SetParam(std::string paramName, std::string value){
	m_params[paramName] = value;
}

void Config::LoadDefaults(){
	m_params.clear();
	m_params["serverIP"] = "127.0.0.1";
	m_params["port"] = "7334";
	m_params["threadPoolSize"] = "1";
}