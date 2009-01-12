#pragma once
#include "zthread/Runnable.h"
#include "seeqd/WorldMessage.h"

//helper macro for defining
#define DECLARE_HANDLER_CLASS(n) \
class n: public ZThread::Runnable{ \
	private: \
		n(){} \
		std::tr1::shared_ptr<WorldMessage> m_msg; \
	public: \
		void run(); \
		n(const std::tr1::shared_ptr<WorldMessage> &msg):m_msg(msg){} \
	};

/**
 @class TalkHandler
 World handler for handling PacketType::TALK packets.
 There is just a phrase, that player says in a packet(message) body.
*/
DECLARE_HANDLER_CLASS(TalkHandler)

/**
 @class AuthHandler
 World handler for handling PacketType::AUTH_REQUEST packets.
 Player sends in his login in body.
 Adds player into WorldMap after successfull login.
 @todo add actual authorization
*/
DECLARE_HANDLER_CLASS(AuthHandler)

/**
 @class DisconnectHandler
 World handler for handling PacketType::DISCONNECT packets.
 They r used only inside server now (only now)
 Removes player from WorldMap and destroys all linked stuff.
*/
DECLARE_HANDLER_CLASS(DisconnectHandler)