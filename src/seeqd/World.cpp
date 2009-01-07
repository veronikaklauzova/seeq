#pragma once
#include "seeqd/World.h"
#include "zthread/Runnable.h"
#include "zthread/Thread.h"
#include "squtils/Logger.h"
#include <windows.h>

#include <string> //kill meeeeeeeee

void World::PutMessage(WorldMessage &message){

}

class GoofyMessanger: public ZThread::Runnable{
public:
	std::string m_msg;
	GoofyMessanger(const char * msg):m_msg(msg){}
	void run(){
		sLog->Log(LVL_DEBUG,m_msg.c_str());
	}
};

void World::Init(){
	m_canceled = false;
	m_timersQueue.AddTimer(Timer(REPEATED,3000, new GoofyMessanger("Hi! Im goofy 1!")));
	m_timersQueue.AddTimer(Timer(REPEATED,5000, new GoofyMessanger("Oh!Im goofy 2!")));
}

void World::Run(){
	while(!isCanceled()){
		unsigned long delay = m_timersQueue.ShootTimers(GetTickCount());
		try{
			ZThread::Thread::sleep(delay);
		} catch (...){ break;}
	}
}