#ifdef WIN32
	#include <Windows.h>
#endif
/**
 Just trim leading and heading spaces from string.
 @param str target string, by ref.
 @return same string by ref.
 */
std::string& trim(std::string& str);


enum eConColor {
	RED = FOREGROUND_RED,
	GREEN = FOREGROUND_GREEN,
	BLUE = FOREGROUND_BLUE,
	YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	CYAN = FOREGROUND_RED | FOREGROUND_BLUE,
	MAGENTA = FOREGROUND_GREEN | FOREGROUND_BLUE,
	WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
};
/**
 Sets color for WINDOWS console only.
 U must clear the color after u printed ur message.
 */
void SetConColor(eConColor clr, bool intensified = false);