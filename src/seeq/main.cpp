#include "squtils/Logger.h"
#include "squtils/FileConfig.h"
#include "zthread/ZThread.h"
#include "seeq/NetworkLayer.h"
#include "shared/NetworkPackets.h"
#include <istream>
#include <ostream>
#include <signal.h>
#include <map>

bool g_canceled = false;
void SignalHandler(int signal)
{
	LOG(LVL_NOTICE, "Signal handling: shutting down.(SIG: %d)",signal);
	sNetLay->cancel();
	g_canceled = true;
}

int main(){

	sLog->Init(LVL_DEBUG);
	sConfig->Init("seeq.cfg");

	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);

	LOG(LVL_NOTICE,"Connecting to %s:%ld.",
		sConfig->GetParam("serverIP").c_str(), sConfig->GetLong("port"));
	sNetLay->Init();

	ZThread::ThreadedExecutor executor;
	ZThread::Task *networkTask = new ZThread::Task(sNetLay);
	executor.execute(*networkTask);

	char buf[1024];
	//login seequence

	LOG(LVL_NOTICE,"Please enter ur username to login:");
	std::cin.getline(buf,1024);
	std::string username(buf);
	if(username.size()>=20 || username.size()<=3){
		LOG(LVL_ERROR,"Username length must be more than 3 and less than 20 symbols, \
			but yours is %d symbols long!", username.size());
		SignalHandler(0);
	} else {
		sNetLay->SendMsg(PacketTypes::AUTH_REQUEST, username);
	}

	while(!g_canceled){
		std::cin.getline(buf,1024);
		std::string msg(buf);
		if(msg.size()) sNetLay->SendMsg(PacketTypes::TALK, msg);
	}
	executor.wait();
	return 0;
}