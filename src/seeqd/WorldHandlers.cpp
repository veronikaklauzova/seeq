#pragma once
#include "seeqd/WorldHandlers.h"
#include "squtils/Logger.h"
//#include "seeqd/NetworkMessage.h"
#include "seeqd/NetworkManager.h"
#include "seeqd/World.h"
#include "squtils/Utils.h"


void TalkHandler::run(){
	unsigned char *dataPtr = &(m_msg->data[0]);
	//m_msg->data[m_msg->data.size()-1] = '\0';//safety reasons!
	if(!m_msg->sender.login.size()){//player wo username?!
		LOG(LVL_WARNING,"Player #%ld have no username, but tries to say smth.", m_msg->sender.id);
		return;
	}
	LOG(LVL_DEBUG,"Player #%ld (%s) says '%s'", m_msg->sender.id, m_msg->sender.login.c_str(), dataPtr);

	std::tr1::shared_ptr<NetworkMessage> outgoingMsg(new NetworkMessage);
	//get nearby players from WorldMap
	outgoingMsg->recievers = sWorld->map.GetNearbyPlayersSockets();
	
	//header, then login, then ':', and the actual message
	int packetLength =
		NET_PACKET_HEADER_SIZE +
		(m_msg->sender.login.size()-1) + //w/o terminating '\0'
		1 + //':'
		(m_msg->data.size());//'\0' is in msg already
	//filling packet
	outgoingMsg->data.reserve(packetLength);
	outgoingMsg->data.assign(packetLength,0);//filling with zeroes
	dataPtr = &(outgoingMsg->data[0]);
	pack<unsigned short>(PacketTypes::TALK, dataPtr);//header [type]
	pack<unsigned short>(packetLength, dataPtr);//header [packet length]
	pack(m_msg->sender.login, dataPtr); //username
	pack(':',dataPtr);
	pack(m_msg->data, dataPtr); //message itself
	sNetMan->PutMessage(outgoingMsg);
}


void AuthHandler::run(){
	unsigned char *dataPtr = &(m_msg->data[0]);
	m_msg->sender.login.assign((char *)dataPtr, m_msg->data.size());
	LOG(LVL_DEBUG,"Player #%ld authorising as '%s'", m_msg->sender.id, m_msg->sender.login.c_str());
	sNetMan->ModifySessionString(m_msg->sender.sock,
		NetworkSession::FIELD_LOGIN,m_msg->sender.login);
	sWorld->map.AddPlayer(m_msg->sender.sock);
	
	//@todo make authorization answer with auth result
}


void DisconnectHandler::run(){
	sWorld->map.RemovePlayer(m_msg->sender.sock);
	LOG(LVL_DEBUG,"Player #%ld (%s) destroyed.", m_msg->sender.id, m_msg->sender.login.c_str());
	
	//@todo make player resource freeing here
}