#pragma once

//4 == sizeof(id)+sizeof(packetSize) = 2*sizeof(unsigned short)
#define NET_PACKET_HEADER_SIZE 4
//hey, magic define, protect me from segfault at NetworkPacketDescriptors[...]
#define NET_PACKET_MAX_TYPE 1
//max login length for AUTH*
#define MAX_LOGIN_LENGTH 20

namespace PacketTypes {
	enum ePacketType {
		TALK = 1,
		AUTH_REQUEST,
		AUTH_ANSWER,///@todo use AUTH_ANSWER
		DISCONNECT ///@todo DISCONNECT in network comms, not only in server.
	};
}

static struct PacketDescriptor {
	unsigned short type;
	unsigned short packetMinSize;
	unsigned short packetMaxSize;
} NetworkPacketDescriptors[] = {
	{0,0,0},//reserved :)
	{PacketTypes::TALK, NET_PACKET_HEADER_SIZE+2, 1024},//talk packet. 2 is one letter message + '\0'.
	//auth packet. 2 is one letter login + '\0'. max login length 20 characters.
	{PacketTypes::AUTH_REQUEST, NET_PACKET_HEADER_SIZE+1+1, NET_PACKET_HEADER_SIZE+MAX_LOGIN_LENGTH+1}
};