/**
 @file seeqd.cpp
 The main seeq daemon file, has entry point.
 */

#include <tchar.h>
#include <signal.h>
#include "squtils/Logger.h"
#include "squtils/FileConfig.h"
#include "zthread/ZThread.h"
#include "seeqd/World.h"



/*class Flooder : public ZThread::Runnable{
public:
	Flooder(int num, int seed):m_num(num),m_seed(seed){}
	void run(){
		try{
			srand((unsigned)m_seed);
			while(!ZThread::Thread::interrupted()){
				sLog->Log(LVL_NOTICE,"Hi! Im thread #%d!",m_num);
				ZThread::Thread::sleep(rand()%5000);
			}
		} catch (...){}
	}
private:
	int m_num, m_seed;
};*/

void SignalHandler(int signal)
{
	LOG(LVL_NOTICE, "Signal handling: shutting down.(SIG: %d)",signal);
	sWorld->cancel();
}


int _tmain(int argc, _TCHAR* argv[])
{
	sLog->Init(LVL_DEBUG);
	sConfig->Init("config.cfg");
	/*sLog->Log(LVL_DEBUG,"-= Console testing =-");
	sLog->Log(LVL_NOTICE,"-= Notice =-");
	sLog->Log(LVL_WARNING,"-= Warning =-");
	sLog->Log(LVL_ERROR,"-= Error =-");*/
	sWorld->Init();

	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);

	sWorld->Run();
	
	/*long poolSize = sConfig->GetLong("threadPoolSize");
	ZThread::PoolExecutor executor(poolSize);
	for(int i=0;i<poolSize;i++){
		executor.execute(new Flooder(i+1,rand()));
	}
	std::cin.get();
	executor.interrupt();
	executor.wait();
	//executor.cancel();*/
	return 0;
}