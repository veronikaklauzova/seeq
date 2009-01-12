#include "seeqd/NetworkManager.h"
#include "zthread/ZThread.h"
#include "squtils/FileConfig.h"
#include "squtils/Logger.h"
#include "shared/NetworkPackets.h"
#include "seeqd/World.h"
//dunno if it works, but FD_SESTIZE=64 is awful
#define FD_SESTIZE 512
#include <winsock.h>
#define FD_SESTIZE 512
//dunno if it works here too :)

unsigned long NetworkManager::next_session_id;

/**
 NetMan initialization function.
 Initializes internal variables and binds network socket to ip:port.
 @warning Must be called first after instantiation of singlethon.
 */
void NetworkManager::Init(){
	m_canceled = false;
	next_session_id = 1;

	//init winsock
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != NO_ERROR){
		LOG(LVL_ERROR,"NetMan: Error at WSAStartup(), canceling.");
		m_canceled = true;
		return;
	}

	m_listen_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_listen_s == INVALID_SOCKET) {
		LOG(LVL_ERROR,"NetMan: Error at socket() for m_listen_s: %d, canceling.", WSAGetLastError());
		m_canceled = true;
		return;
	}

	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(sConfig->GetParam("serverIP").c_str());
	sa.sin_port = htons(static_cast<u_short>(sConfig->GetLong("port")));

	result = bind(m_listen_s, (SOCKADDR*) &sa, sizeof(sa));
	if (result == SOCKET_ERROR) {
		LOG(LVL_ERROR,"NetMan: Error at bind() for m_listen_s: %d, canceling.", WSAGetLastError());
		m_canceled = true;
		return;
	}
	result = listen(m_listen_s, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		LOG(LVL_ERROR,"NetMan: Error at listen() for m_listen_s: %d, canceling.", WSAGetLastError());
		m_canceled = true;
		return;
	}

	LOG(LVL_NOTICE,"NetMan: m_listen_s bind()'ed to %s:%ld and listening for new connections!", 
		sConfig->GetParam("serverIP").c_str(), sConfig->GetLong("port"));
}

void NetworkManager::_fd_add_helper(const sessions_map::value_type &v){
	FD_SET(v.first,&m_read_set);
}

void NetworkManager::run(){
	LOG(LVL_DEBUG,"NetMan::run()");
	TIMEVAL timeOut;
	timeOut.tv_sec = 0;
	timeOut.tv_usec = 10000;//10 ms select delay
	int result;
	while(!isCanceled()){
		try{
			//select ready to read sockets, read em and put message to #World mq.
			//set preparations
			FD_ZERO(&m_read_set); //clear all
			FD_SET(m_listen_s, &m_read_set); //socket for new connections
			//filling with currently active sessions sockets
			std::for_each(m_sessions.begin(),m_sessions.end(),
				std::tr1::bind(&NetworkManager::_fd_add_helper, this, std::tr1::placeholders::_1));

			result = select(0,&m_read_set, NULL, NULL, &timeOut);
			if(result == SOCKET_ERROR){//oops, smth wrong
				LOG(LVL_ERROR, "NetMan: error while select()'ing: %d, canceling.", WSAGetLastError());
				this->cancel();
			} else if(result){//we have some data to read or connection is pending
				if(FD_ISSET(m_listen_s, &m_read_set)){//new connection
					this->AcceptConnection();
				}
				//check all active sockets for read-ready state and read em
				std::for_each(m_sessions.begin(),m_sessions.end(),
					std::tr1::bind(&NetworkManager::FdRecvFrom, this, std::tr1::placeholders::_1));
			}

			//erase dead sessions
			if(!m_dead_sessions.empty()){
				for(std::list<SOCKET>::const_iterator it = m_dead_sessions.begin(); it!=m_dead_sessions.end(); it++)
					m_sessions.erase(*it);
				m_dead_sessions.clear();
			}

			//send all pending data from MQ
			///@todo it is possible, that data will come SO FAST so it will spin here forever
			///but it shouldn't :)
			if(!m_messageQueue.empty()){
				while(m_messageQueue.size())
					this->SendToClients(m_messageQueue.next());
			}
		} catch(...){LOG(LVL_ERROR,"NetMan exception");}
	}
}

void csckt(SOCKET sock){closesocket(sock);}//functions that returns smth cant be for_eached?

NetworkManager::~NetworkManager(){
	//closing all sessions
	std::for_each(m_sessions.begin(), m_sessions.end(),
		std::tr1::bind(&csckt,
			std::tr1::bind(
				&sessions_map::value_type::first,
				std::tr1::placeholders::_1
				)
			)
		);
	//and listening socket
	closesocket(m_listen_s);
	WSACleanup();
}

void NetworkManager::AcceptConnection(){
	sockaddr_in sa;
	int sockaddrSize = sizeof(sa);
	SOCKET pending = accept(m_listen_s, (sockaddr *)&sa, &sockaddrSize);
	if(pending == INVALID_SOCKET){
		LOG(LVL_DEBUG,"NetMan::AcceptConnection():Invalid socket after accept():%d.",WSAGetLastError());
	} else {
		char * addr = inet_ntoa(sa.sin_addr);
		LOG(LVL_DEBUG,"NetMan: New network connection! %s:%d",
			addr ? addr : "undefined", static_cast<int>(ntohs(sa.sin_port)));

		//adding new active session for this socket
		this->AddNewSession(pending, sa);
	}
}

void NetworkManager::AddNewSession(SOCKET sock, sockaddr_in addr_info){
	ZThread::Guard<ZThread::FastMutex> g(m_lock_sessions);
	
	m_sessions[sock].id = GetNextSessId();
	m_sessions[sock].sock = sock;
	m_sessions[sock].addrInfo = addr_info;
	///@todo add other usable fields :)
}

void NetworkManager::KillSession(SOCKET sock){
	closesocket(sock);
	//m_sessions.erase(sock);//erase is BAD, destroys iterators currently in use.
	m_dead_sessions.push_back(sock);
	//form DISCONNECT message for world
	//to destroy this player related stuff
	std::tr1::shared_ptr<WorldMessage> msg(new WorldMessage);
	msg->type = PacketTypes::DISCONNECT;
	msg->sender = m_sessions[sock];
	//msg->data - nah, no need in this
	sWorld->PutMessage(msg);
}

void NetworkManager::FdRecvFrom(sessions_map::value_type &v){
	if(FD_ISSET(v.first, &m_read_set)){//yay! data pending!
		//read pending data (1k max) to session buffer
		char buffer[1024];
		int result = recv(v.first, buffer, 1024, 0);
		if(result > 0){//data readed
			for(int i=0;i<result;i++)
				v.second.recv_buffer.push_back(buffer[i]);
			this->FormWorldMessage(v);
		} else {
			char * addr = inet_ntoa(v.second.addrInfo.sin_addr);
			if (result){//error occured
				LOG(LVL_WARNING,"Error while recv: %d.",WSAGetLastError());
			}
			//if result == 0 - connection gracefuly closed. either way we need to kill the session.
			LOG(LVL_DEBUG, "Connection %s:%d closed, killing session.",
				addr ? addr : "undefined", static_cast<int>(ntohs(v.second.addrInfo.sin_port)));
			this->KillSession(v.first);
		}
	}
}

void NetworkManager::FormWorldMessage(sessions_map::value_type &v){
	if(v.second.recv_buffer.size() <= NET_PACKET_HEADER_SIZE){
		//totaly incomplete packet. cant even decide bout type or len
		return;
	}
	//first 2 bytes are packet type code, next 2 - packet size.
	/**
	 @todo there is an issue with little/big endian, so it is another
	 obstacle is a cross-platform way. fix it!
	 */
	//will use little-endian, coz its still a x86! :P
	unsigned short packetType = v.second.recv_buffer[0] + v.second.recv_buffer[1]*0x100;
	unsigned short packetSize = v.second.recv_buffer[2] + v.second.recv_buffer[3]*0x100;
	//next check for 'integrity':
	//packet type is defined
	//packet size must be in range for that type
	if( packetType >= sizeof(NetworkPacketDescriptors)/sizeof(PacketDescriptor) ||
		packetSize > NetworkPacketDescriptors[packetType].packetMaxSize ||
		packetSize < NetworkPacketDescriptors[packetType].packetMinSize ){
			LOG(LVL_WARNING,"Wrong typed/sized packet recieved [%d][%d]. Killing session.",
				(int)packetType, (int)packetSize);
			//killing session is absolutely unavoidable:
			//cant determine the size of sended data, so cant
			//determine where is the next packet is.
			this->KillSession(v.first);
			return;
	}
	//ok, packet size is in range.
	//but do we recv'ed all the packet already?
	if(v.second.recv_buffer.size() < packetSize){
		//obviously not
		//hope other part will join us next cycle
		return;
	}

	//okay! now we know that first packetSize bytes in buffer is complete packet!
	//create a message from it, and post it to #World
	std::tr1::shared_ptr<WorldMessage> msg(new WorldMessage);
	msg->type = (PacketTypes::ePacketType)packetType;//yeah, pretty bad. need to synchro them :(
	msg->data.reserve(packetSize - NET_PACKET_HEADER_SIZE);
	msg->data.assign(v.second.recv_buffer.begin() + NET_PACKET_HEADER_SIZE,
		v.second.recv_buffer.begin()+packetSize);
	//clearing packet from buffer
	v.second.recv_buffer.erase(v.second.recv_buffer.begin(),
		v.second.recv_buffer.begin()+packetSize);
	msg->sender = v.second;
	sWorld->PutMessage(msg);
}

void NetworkManager::PutMessage(const std::tr1::shared_ptr<NetworkMessage> &message){
	m_messageQueue.add(message);
}

void NetworkManager::ModifySessionString(SOCKET sessionId,
		NetworkSession::eModyfiableField fieldId,
		const std::string &value)
{
	ZThread::Guard<ZThread::FastMutex> g(m_lock_sessions);
	switch(fieldId){
		case NetworkSession::FIELD_LOGIN:
			m_sessions[sessionId].login = value;
			break;
	}
}

void NetworkManager::SendToClients(const std::tr1::shared_ptr<NetworkMessage> &message){
	///@bug DEFINETELY there must be streaming fixes, like while recv()'ing
	std::list<SOCKET>::const_iterator it;
	for(it = message->recievers.begin();it != message->recievers.end(); it++){
		unsigned int result = send(*it,(char *)&message->data[0],message->data.size(),0);
		if(result == SOCKET_ERROR){
			LOG(LVL_ERROR, "NetMan: Error while send()'ing data: %d.", WSAGetLastError());
		} else if(result < message->data.size()){
			LOG(LVL_WARNING, "NetMan: There is less data send()'ed that should be. \
							Connection can be corrupted (at list now its DEFENITELY corrupted :)).");
		}
	}
}