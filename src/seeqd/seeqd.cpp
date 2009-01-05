#include <tchar.h>
#include <time.h> //KILL MEEEEEEEEEEEEE
#include "squtils/Logger.h"
#include "zthread/ZThread.h"



class Flooder : public ZThread::Runnable{
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
};

int _tmain(int argc, _TCHAR* argv[])
{
	srand((unsigned)time(NULL));
	sLog->Init(LVL_DEBUG);
	sLog->Log(LVL_DEBUG,"-= Console testing =-");
	sLog->Log(LVL_NOTICE,"-= Notice =-");
	sLog->Log(LVL_WARNING,"-= Warning =-");
	sLog->Log(LVL_ERROR,"-= Error =-");
	ZThread::PoolExecutor executor(20);
	for(int i=0;i<20;i++){
		executor.execute(new Flooder(i+1,rand()));
	}
	std::cin.get();
	executor.interrupt();
	executor.wait();
	//executor.cancel();
	return 0;
}