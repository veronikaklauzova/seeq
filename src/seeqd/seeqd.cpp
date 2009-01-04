#include <tchar.h>
#include "squtils/Logger.h"

int _tmain(int argc, _TCHAR* argv[])
{
	sLog->Init(DEBUG);
	sLog->Log(DEBUG,"Hello there!");
	std::cin.get();
	return 0;
}