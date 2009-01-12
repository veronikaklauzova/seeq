#pragma once
#include <winsock.h>
#include <deque>


/**
 Struct for storing session info.
 Used in NetworkManager primary.
 Writed only from it, but readed from all (almost) WorldHandlers.
 */
struct NetworkSession{
	enum eModyfiableField{
		FIELD_LOGIN = 1
	};
	unsigned long id;
	SOCKET sock;
	sockaddr_in addrInfo;
	std::deque<unsigned char> recv_buffer;
	std::string login;
};