#include "common.h"


Config* g_Config = 0; ///<global var for current config

Config::Config(void){
	g_Config = this;
}
Config::~Config(void){
	if(g_Config == this){
		g_Config = 0;
	}
}

std::string Config::GetParam(std::string paramName){
	return m_params[paramName];
}

void Config::SetParam(std::string paramName, std::string value){
	m_params[paramName] = value;
}

void Config::LoadDefaults(){
	m_params.clear();
	m_params["port"] = "7334";
}

Config* Config::Get(){
	if(!g_Config){
		LOG("config is not initialized",ERROR);
	}
	return g_Config;
}