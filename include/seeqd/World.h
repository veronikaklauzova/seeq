#pragma once
#include "zthread/ZThread.h"
#include "seeqd/WorldMessage.h"
#include "seeqd/Timers.h"
#include "seeqd/WorldMap.h"

/**
 A main world-processing server class.
 Consists of:
	- MessageQueue for incoming client actions. #NetworkManager will queue valid packets here.
	- Numerous handlers for each message type.
	- Ordered list of "timers".
 */
class World : public ZThread::Singleton<World>, public ZThread::Runnable, public ZThread::Cancelable {
private:
	ZThread::BlockingQueue<std::tr1::shared_ptr<WorldMessage> , ZThread::FastMutex> m_messageQueue;
	TimersQueue m_timersQueue;
	bool m_canceled;
public:
	//@todo move me to private
	WorldMap map;
	/**
	 Puts message to a world queue.
	 Any message lying there will be extracted in main world loop,
	 then redirected to handler (wich is defined by WorldMessage.type) in pooled thread.
	 @param message The message itself. Passed by shared_ptr.
	*/
	void PutMessage(const std::tr1::shared_ptr<WorldMessage> &message);
	void Init();
	void run();

	bool isCanceled(){return m_canceled;}
	void cancel(){m_canceled = true;}
};

///Shortcut definition
#define sWorld World::instance()