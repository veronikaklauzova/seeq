#pragma once
#include <vector>
#include <list>
#include <winsock.h>

///Message for World::MessageQueue
struct NetworkMessage {
	std::list<SOCKET> recievers;
	std::vector<unsigned char> data;
};