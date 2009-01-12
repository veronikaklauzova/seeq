/**
 @file seeqd.cpp
 The main seeq daemon file, has entry point.
 */

#include <tchar.h>
#include <signal.h>
#include "squtils/Logger.h"
#include "squtils/FileConfig.h"
#include "zthread/ZThread.h"
#include "seeqd/NetworkManager.h"
#include "seeqd/World.h"

void SignalHandler(int signal)
{
	LOG(LVL_NOTICE, "Signal handling: shutting down.(SIG: %d)",signal);
	sWorld->cancel();
	sNetMan->cancel();
}

int _tmain(int argc, _TCHAR* argv[])
{
	sLog->Init(LVL_DEBUG);
	sConfig->Init("seeqd.cfg");
	/*sLog->Log(LVL_DEBUG,"-= Console testing =-");
	sLog->Log(LVL_NOTICE,"-= Notice =-");
	sLog->Log(LVL_WARNING,"-= Warning =-");
	sLog->Log(LVL_ERROR,"-= Error =-");*/
	sNetMan->Init();
	sWorld->Init();

	ZThread::ThreadedExecutor executor;

	//there is a hack :(
	//i will NOT delete those tasks and free memory
	ZThread::Task *worldTask = new ZThread::Task(sWorld);
	ZThread::Task *netmanTask = new ZThread::Task(sNetMan);
	//coz Task::~Task() frees Runnable deriviants(world,netman)
	//but they are singletoned, so will be freed again at exiting
	//which causes segfault :(

	executor.execute(*netmanTask);//running netman in separate thread
	executor.execute(*worldTask);//running world in separate thread

	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);


	//here must be console processing loop :)


	executor.wait();//waiting for runned threads (netman, world) to terminate.
	//they will be, either by SIGTERM or by internal logic.
	return 0;
}