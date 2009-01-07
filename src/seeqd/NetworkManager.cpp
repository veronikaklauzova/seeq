#include "seeqd/NetworkManager.h"
#include "zthread/ZThread.h"
#include "squtils/Config.h"
#include "squtils/Logger.h"

void NetworkManager::Init(){
	m_canceled = false;
}
void NetworkManager::run(){
	LOG(LVL_DEBUG,"NetMan runned");
	while(!isCanceled()){
		try{
			//select ready to read sockets, read em and put message to #World mq.
			ZThread::Thread::sleep(10000);//clear me
			LOG(LVL_DEBUG,"NetMan is alive!");
		} catch(...){LOG(LVL_ERROR,"NetMan exception");}
	}
}