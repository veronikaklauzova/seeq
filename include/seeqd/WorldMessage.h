#include <vector>
#pragma once

enum eWorldMessageType{
	WOM_TALK
};

///Message for World::MessageQueue
struct WorldMessage {
	eWorldMessageType type;
	std::vector<unsigned char> data;
};