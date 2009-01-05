#include <string>
#include "Utils.h"
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