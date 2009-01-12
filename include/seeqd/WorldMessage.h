#pragma once
#include <vector>
#include "seeqd/NetworkSession.h"
#include "shared/NetworkPackets.h"

///Message for World::MessageQueue
struct WorldMessage {
	PacketTypes::ePacketType type;
	NetworkSession sender;///<a copy of sender session.
	std::vector<unsigned char> data;
};