#pragma once
#include <winsock.h>


/**
 Struct for storing session info.
 Used in NetworkManager primaryly.
 Writed only from it, but readed from all (almost) WorldHandlers.
 */
struct NetworkSession{
	unsigned long id;
	SOCKET sock;
};
