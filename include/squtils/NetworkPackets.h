#pragma once

//4 == sizeof(id)+sizeof(packetSize) = 2*sizeof(unsigned short)
#define NET_PACKET_HEADER_SIZE 4
//hey, magic define, protect me from segfault at NetworkPacketDescriptors[...]
#define NET_PACKET_MAX_TYPE 1

struct PacketDescriptor {
	unsigned short type;
	unsigned short packetMinSize;
	unsigned short packetMaxSize;
} NetworkPacketDescriptors[] = {
	{0,0,0},//reserved :)
	{1,NET_PACKET_HEADER_SIZE+2,1024}//talk packet. 2 is one letter message + '\0'.
};