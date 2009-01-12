#pragma once
#include "zthread/ZThread.h"
#include "shared/NetworkPackets.h"
#include <winsock.h>
#include <string>

class NetworkLayer: public ZThread::Singleton<NetworkLayer>, 
	public ZThread::Runnable, public ZThread::Cancelable{
private:
	SOCKET m_connection_s;
	bool m_canceled;
	fd_set m_read_set;
public:
	void Init();
	~NetworkLayer();
	void run();
	bool isCanceled(){ return m_canceled;}
	void cancel(){ m_canceled = true;}
	void SendMsg(PacketTypes::ePacketType type, const std::string &msg);
};

#define sNetLay NetworkLayer::instance()