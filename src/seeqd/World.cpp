#pragma once
#include "seeqd/World.h"
#include "seeqd/WorldHandlers.h"
#include "zthread/Runnable.h"
#include "zthread/Thread.h"
#include "squtils/Logger.h"
#include "squtils/FileConfig.h"
#include <windows.h>

#include <string> //kill meeeeeeeee

void World::PutMessage(const WorldMessage &message){
	m_messageQueue.add(new WorldMessage(message));//creating new copy of message
}

class GoofyMessanger: public ZThread::Runnable{
public:
	std::string m_msg;
	GoofyMessanger(const char * msg):m_msg(msg){}
	void run(){
		//sLog->Log(LVL_DEBUG,m_msg.c_str());
		WorldMessage msg;
		msg.sender.id = 1;
		msg.data.assign(m_msg.begin(), m_msg.end());
		msg.type = WorldMessage::TYPE_TALK;
		sWorld->PutMessage(msg);
	}
};

void World::Init(){
	m_canceled = false;
	m_timersQueue.AddTimer(Timer(REPEATED,3000, new GoofyMessanger("Hi! Im goofy 1!")));
	m_timersQueue.AddTimer(Timer(REPEATED,5000, new GoofyMessanger("Oh!Im goofy 2!")));
}

void World::run(){
	ZThread::PoolExecutor poolExecutor(sConfig->GetLong("threadPoolSize"));
	LOG(LVL_DEBUG,"World is running");
	while(!isCanceled()){
		try{
			unsigned long delay = m_timersQueue.ShootTimers(GetTickCount());
			WorldMessage *wMsg = m_messageQueue.next(delay);
			switch(wMsg->type){
				case WorldMessage::TYPE_TALK:
					poolExecutor.execute(ZThread::Task(new TalkHandler(wMsg)));
					break;
				default:
					LOG(LVL_WARNING, "Unknown message to World. Type: %d.", wMsg->type);
			}
		}
		catch (ZThread::Timeout_Exception toExcept){}//no msg added while waiting
		catch (...){LOG(LVL_ERROR,"World exception");}//other excpt, like Interrupted or Canceled
	}
	poolExecutor.wait();
}