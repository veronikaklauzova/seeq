#pragma once
#include "zthread/Runnable.h"
#include "seeqd/WorldMessage.h"

class TalkHandler: public ZThread::Runnable{
private:
	TalkHandler(){}//prevent empty creation
	WorldMessage *m_msg;
public:
	void run();
	/**
	 Handler ctor.
	 @warning msg should NOT be deleted after passing, it frees in ::run().
	 */
	TalkHandler(WorldMessage *msg):m_msg(msg){}
};
