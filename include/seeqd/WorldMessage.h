#pragma once
#include <vector>
#include "seeqd/NetworkSession.h"

///Message for World::MessageQueue
struct WorldMessage {
	enum eWorldMessageType{
		TYPE_TALK = 1
	} type;
	NetworkSession sender;///<a copy of sender session.
	std::vector<unsigned char> data;
};