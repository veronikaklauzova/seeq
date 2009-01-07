#pragma once
#include <map>
#include "zthread/Runnable.h"
#include "zthread/FastMutex.h"

enum eTimerType{
	REPEATED,
	ONCE
};

/**
 Just a timer.
 It knows its type (which is TIMER_REPEATED or TIMER_ONCE), its interval and
 its callback. OOP callback is an issue, so I've diceded to use
 a ZThread::Runnable as an callback interface.
 */
class Timer {
private:
	Timer(){}//no empty ctor
	eTimerType m_type;
	unsigned long m_interval;
	ZThread::Runnable* m_callback;
public:
	/**
	 Timer ctor.
	 @param callback is a pointer to ZThread::Runnable derived class,
	 and that object should NOT be freed after call.
	 @warning I repeat: do NOT free callback object memory, it will be freed in Timer:Free()
	*/
	Timer(eTimerType type, unsigned long interval, ZThread::Runnable *callback):
	  m_type(type),m_interval(interval),m_callback(callback){}
	void Free(){if(m_callback) delete m_callback;}///<Frees callback object. Use this, when permanently destroying timer
	unsigned long GetInterval() const {return m_interval;}
	eTimerType GetType() const {return m_type;}
	void execute() const {m_callback->run();}
	void preventMemoryDeletion() const {const_cast<Timer *>(this)->m_callback = 0;}///<hack. do not use it :P
};

/**
 A queue of timers. Used in #World class for managing timmed events.
 Since #World is multithreaded (handlers runs in separate pooled threads),
 adding must be thread safe.
 @bug When adding a timer, that must shoot before othrs in queue,
 and #World in select state, with timeout of first timer in queue,
 that new timer will NOT shoot, until select returns, or (worst case)
 till the first timer in queue shoots.
 Possible solution: discreticize waiting for, idk, 300ms should be enought?
 */
class TimersQueue {
private:
	std::multimap<unsigned long, Timer> m_timers;
	ZThread::RecursiveMutex m_lock;
public:
	/**
	 Main queue function.
	 It'll execute() all timers, execution time of whoom is < than tillTime param.
	 Than it will rearrange the queue according to timers type and
	 intervals of each shooted timer.
	 @warning eny timer in queue will be shoot as many times, as it
	 should be shoot, not once.
	 I.e. if interval is 100ms, but this function was last time
	 called 500ms in past, timer will be shoot 5 times.
	 @param tillTime the time till what all the timers must be execute()'d.
	 GetTickCount() is most likely value.
	 @returns how many ticks left, before the next timer needs to be shoot.
	*/
	unsigned long ShootTimers(unsigned long tillTime);

	void AddTimer(const Timer &timer, unsigned long startTime = 0);
	~TimersQueue();
};