#pragma once
#include "zthread/Runnable.h"
#include "seeqd/WorldMessage.h"

/**
 World handler for handling WorldMessage::TYPE_TALK packets.
 There is just a phrase, that player says in a packet(message) body.
*/
class TalkHandler: public ZThread::Runnable{
private:
	TalkHandler(){}//prevent empty creation
	std::tr1::shared_ptr<WorldMessage> m_msg;
public:
	void run();
	/**
	 Handler ctor.
	 @warning shared_ptr WILL nullify passed pointer. so do NOT use ur WorldMessage * after u pass it here.
	 */
	TalkHandler(const std::tr1::shared_ptr<WorldMessage> &msg):m_msg(msg){}
	//TalkHandler(WorldMessage *msg):m_msg(msg){}
};
