#include "seeq/NetworkLayer.h"
#include "squtils/Logger.h"
#include "squtils/FileConfig.h"
#include "shared/NetworkPackets.h"
#include "squtils/Utils.h"
#include <string>

void NetworkLayer::Init(){
	m_canceled = false;
	//init winsock
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != NO_ERROR){
		LOG(LVL_ERROR,"NetLay: Error at WSAStartup().");
		m_canceled = true;
		return;
	}

	m_connection_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_connection_s == INVALID_SOCKET) {
		LOG(LVL_ERROR,"NetLay: Error at socket() for m_connection_s: %d.", WSAGetLastError());
		m_canceled = true;
		return;
	}

	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(sConfig->GetParam("serverIP").c_str());
	sa.sin_port = htons(static_cast<u_short>(sConfig->GetLong("port")));

	result = connect(m_connection_s, (sockaddr *)&sa, sizeof(sa));
	if (result == SOCKET_ERROR) {
		LOG(LVL_ERROR,"NetLay: Error at connect() for m_connection_s: %d.", WSAGetLastError());
		m_canceled = true;
		return;
	}

	LOG(LVL_NOTICE,"NetLay: m_listen_s connect()'ed to %s:%ld and ready for transmission!", 
		sConfig->GetParam("serverIP").c_str(), sConfig->GetLong("port"));
}

void NetworkLayer::run(){
	TIMEVAL timeOut;
	timeOut.tv_sec = 0;
	timeOut.tv_usec = 10000;
	while(!isCanceled()){
		try {
			FD_ZERO(&m_read_set);
			FD_SET(m_connection_s, &m_read_set);
			int result = select(0, &m_read_set, NULL, NULL, &timeOut);
			if(result > 0 ){
				char buffer[2048];
				result = recv(m_connection_s, buffer, 2048, 0);
				if(!result){
					cancel();
				} else if (result != SOCKET_ERROR){
					LOG(LVL_NOTICE, "%s", buffer+4);
				} else {
					LOG(LVL_ERROR, "Socket error while recv()'ing : %d", WSAGetLastError());
					cancel();
				}
			}
		} catch (...){}
	}
}


NetworkLayer::~NetworkLayer(){
	closesocket(m_connection_s);
	WSACleanup();
}

void NetworkLayer::SendMsg(PacketTypes::ePacketType type, const std::string &msg){
	char buffer[2048];
	unsigned char *buffPtr = (unsigned char *)buffer;
	unsigned short len = msg.size()+1;
	pack<unsigned short>(type, buffPtr);
	pack<unsigned short>(len+4, buffPtr);
	pack(msg, buffPtr);
	send(m_connection_s, buffer, len+4, 0);
}