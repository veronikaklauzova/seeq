#pragma once
#include "zthread/ZThread.h"
//#include "zthread/FastMutex.h"
//#include "zthread/Singleton.h"
#include "seeqd/WorldMessage.h"
#include "seeqd/Timers.h"

/**
 A main world-processing server class.
 Consists of:
	- MessageQueue for incoming client actions. #NetworkManager will queue valid packets here.
	- Numerous handlers for each message type.
	- Ordered list of "timers".
 */
class World : public ZThread::Singleton<World>, public ZThread::Runnable, public ZThread::Cancelable {
protected:
	ZThread::BlockingQueue<WorldMessage *, ZThread::FastMutex> m_messageQueue;
	TimersQueue m_timersQueue;
	bool m_canceled;
public:
	/**
	 Puts message to a world queue.
	 Any message lying there will be extracted in main world loop,
	 then redirected to handler (wich is defined by WorldMessage.type) in pooled thread.
	 @param message The message itself. Passed byref, but 
	 the copy will be created for queueing.
	*/
	void PutMessage(const WorldMessage &message);
	void Init();
	void run();

	bool isCanceled(){return m_canceled;}
	void cancel(){m_canceled = true;}
};

///Shortcut definition
#define sWorld World::instance()