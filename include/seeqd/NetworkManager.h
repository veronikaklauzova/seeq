#pragma once
#include "seeqd/NetworkSession.h"
#include "zthread/Singleton.h"
#include "zthread/Runnable.h"
#include "zthread/BlockingQueue.h"
#include "zthread/FastMutex.h"
#include <unordered_map>
#include <winsock.h>

/**
 Class responsible for all network handling.
 Has its own message queue, and puts a requets into #World MQ.
 Designed to be "run" in separate thread.
 */
class NetworkManager: public ZThread::Singleton<NetworkManager>, public ZThread::Runnable, public ZThread::Cancelable{
private:
	std::tr1::unordered_map<SOCKET, NetworkSession> m_sessions;
	bool m_canceled;
public:
	void Init();
	void run();
	void cancel(){m_canceled = true;}
	bool isCanceled(){return m_canceled;}

};

#define sNetMan NetworkManager::instance()