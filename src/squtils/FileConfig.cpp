#include "common.h"


FileConfig::FileConfig(std::string fileName):Config(){
	m_fileName = fileName;
}

FileConfig::~FileConfig(void){}
/**
 There is a function, that walks thrue a stream and parses it into params with values.
 File rules are simple: [param] = [value] and string starting with '#' is comment.
 */
void FileConfig::Load(){
	std::fstream fileStream(m_fileName.c_str(),std::ios::in);
	if(fileStream.bad()){
		sLog->Log(LVL_WARNING,"File stream is bad, loading defaults.");
		LoadDefaults();
	} else {
		//loading itself.
		std::fstream::char_type curLine[1024];
		long lineNum = 1;//< keeps track of line number
		while(!fileStream.eof()){
			fileStream.getline(curLine,1024);
			if(fileStream.fail()){
				sLog->Log(LVL_WARNING,"Error at config line %ld. Line too long?",lineNum);
				//skipping till next line
				while(fileStream.fail() && !fileStream.eof()){
					fileStream.clear(fileStream.rdstate()^std::ios::failbit);
					fileStream.getline(curLine,1024);
				}
			} else {//getline successfull
				std::string line(curLine);
				trim(line);
				if(line[0]=='#') continue;
				//split by '='
				std::string::size_type pos = line.find('=');
				std::string value = line.substr(pos + 1);
				line.erase(pos);
				m_params[trim(line)] = trim(value);
				lineNum++;
			}
		}
	}
}

void FileConfig::Save(){
	std::fstream fileStream(m_fileName.c_str(), std::ios::out);
	if(fileStream.bad()){
		sLog->Log(LVL_ERROR,"Config wasn't saved: fileStream is bad.");
		return;
	}
	std::tr1::unordered_map<std::string, std::string>::iterator it;
	for(it = m_params.begin(); it != m_params.end(); it++){
		fileStream<< it->first << " = " << it->second <<std::endl;
	}
}