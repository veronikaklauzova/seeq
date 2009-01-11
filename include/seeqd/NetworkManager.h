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
	typedef std::tr1::unordered_map<SOCKET, NetworkSession> sessions_map;
	/**
	 map of sessions.
	 it is important that they will be copied from container: while one thread processing smth,
	 NetMan can close connection and delete linked session, which will cause
	 segfault if there is one shared copy of them.
	 */
	sessions_map m_sessions;
	SOCKET m_listen_s; ///< socket used for new connections listening
	fd_set m_read_set; ///< fd_set used for select() calls in NetworkManager::run()
	static unsigned long next_session_id;///< static next-id var
	/**
	 List of killed, but yet not erased sessions.
	 Whole purpose of this list is not modyfying
	 m_sessions while still traversing them for preserving
	 iterators. after traversing is done,
	 we can safely clear dead sessions from m_sessions.
	 */
	std::list<SOCKET> m_dead_sessions;

	bool m_canceled;
	ZThread::FastMutex m_lock_sessions;

	void AcceptConnection();///< accept pending connection. called after FD_ISSET check in NetworkManager::run()
	void AddNewSession(SOCKET sock, sockaddr_in addr_info);///< adds new session (and rebuilding indexes?)
	void KillSession(SOCKET sock);///< destroyes session
	unsigned long GetNextSessId(){ return next_session_id++;}
	void _fd_add_helper(const sessions_map::value_type &v);///< helper for checking select() results
	/**
	 initiates data receiveing from socket.
	 @pre there MUST be data pending. so its called after succesful select() and checks FD_ISSET by itself
	 @post all pending data read to session buffer for constructing complete packet and integrity checks
	 */
	void FdRecvFrom(sessions_map::value_type &v);
	/**
	 Makes integrity checks on session recv_buffer,
	 forms complete network packets, forms #WorldMessage's from them
	 and puts em in #World MQ.
	 */
	void FormWorldMessage(sessions_map::value_type &v);
public:
	void Init();
	void run();
	void cancel(){m_canceled = true;}
	bool isCanceled(){return m_canceled;}
	~NetworkManager();
};

#define sNetMan NetworkManager::instance()