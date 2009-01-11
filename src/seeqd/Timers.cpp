#include "zthread/Guard.h"
#include "zthread/RecursiveMutex.h"
#include "seeqd/Timers.h"
#include <windows.h> //GetTickCount()
#include "squtils/Logger.h"
#include <algorithm>


void TimersQueue::AddTimer(const Timer &timer, unsigned long startTime){
	ZThread::Guard<ZThread::RecursiveMutex> g(m_lock);
	unsigned long shootingTime, tickCount = GetTickCount();
	if(startTime){
		tickCount = startTime;
	}
	shootingTime = tickCount + timer.GetInterval();
	if(shootingTime < tickCount){//overflow occured
		/**
		 @bug overflow shurely happens if the system runs longer, than about 50 days.
		 and this is realy bad, coz all the timers will stuck now, and this 
		 one will fire for 50 days. Solution: don't count by GetTickCount(), use diffs every time.
		 Temporary solution is to requeue all the timers.
		 Bad thing about it is few (or if interval of current timer is waay too big
		 then many) skipped shoots.
		 */
		sLog->Log(LVL_ERROR,"Overflow in TimersQueue. GetTickCount() problem.");
	}
	m_timers.insert( std::pair<unsigned long, Timer>(shootingTime,timer));
}

unsigned long TimersQueue::ShootTimers(unsigned long tillTime){
	ZThread::Guard<ZThread::RecursiveMutex> g(m_lock);
	std::multimap<unsigned long, Timer>::iterator it;
	if(!m_timers.empty()){
		while(1){
			it = m_timers.begin();
			//start pop'ing elements from begining
			//(thay are sorted by key, since its a multimap)
			if(it->first <= tillTime){//do we need to shoot em?
				it->second.execute();
				if(it->second.GetType() == REPEATED){
					//requeue it
					AddTimer(it->second, it->first);
				} else if(it->second.GetType() == ONCE){
					it->second.Free();
				}
				unsigned long executionTime = it->first;
				m_timers.erase(it);
				if(tillTime - executionTime > 10000L){//10 seconds is TOO MASSIVE lag
					//something wrong
					sLog->Log(LVL_WARNING, "There is a 10s late for a timer (%ld vs %ld).", tillTime, executionTime);
					break;
				}
			} else {
				break;
			}
		}

		it = m_timers.begin();
		if(it->first < tillTime){
			sLog->Log(LVL_ERROR, "Time paradox in ShootTimers()");
		} else {
			return it->first - tillTime;
		}
	}
	//default value. Let it be 1sec.
	return 1000L;
}
TimersQueue::~TimersQueue(){
	std::for_each(m_timers.begin(),m_timers.end(),
		std::tr1::bind(
			&Timer::Free,
			std::tr1::bind(
				&std::multimap<unsigned long, Timer>::value_type::second,
				std::tr1::placeholders::_1
				)
			)
		);

	//i've been fucking my brains out for 10 minutes trying to use all that neat binders
	//BUT WTF
	//IT USED TO BE SIMPLIER, THAT
	/*std::multimap<unsigned long, Timer>::iterator it;
	for(it = m_timers.begin(); it!= m_timers.end(); it++)
		it->second.Free();*/
}