#include <string>
#include <vector>
#include "squtils/Utils.h"
std::string& trim(std::string& str){
	std::string::size_type pos = str.find_last_not_of(' ');
	if(pos != std::string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != std::string::npos) str.erase(0, pos);
	} else {
		str.erase(str.begin(), str.end());
	}
	return str;
}

void SetConColor(eConColor clr, bool intensified){
#ifdef WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole,intensified ? clr|FOREGROUND_INTENSITY : clr);
#endif
}

template <>
void pack<unsigned short>(const unsigned short &value, unsigned char *&dest){
	//little endian
	*dest = value % 0x100; dest++;
	*dest = value / 0x100; dest++;
}

template <>
void pack<char>(const char &value, unsigned char *&dest){
	*dest = (unsigned char) value;
	dest++;
}

template <>
void pack< std::vector<unsigned char> >(const std::vector<unsigned char> &value, unsigned char *&dest){
	for(unsigned int i=0; i < value.size(); i++){
		*dest = value[i];
		dest++;
	}
}

template <>
void pack<std::string>(const std::string &value, unsigned char *&dest){
	strcpy((char *)dest, value.c_str());
	dest += value.size()-1;//terminating zero overwritation if neaded
}

template <>
unsigned short unpack<unsigned short>(const unsigned char *&source){
	unsigned short retval;
	//little endian
	retval  = *source; source++;
	retval  += *source * 0x100; source++;
	return retval;
}