#include "Timer.h"
#include <Windows.h>

using namespace std;
using namespace Network;

Timer::Timer() {
	m_TimerQueue = CreateTimerQueue();
}

Timer::~Timer() {
	DeleteTimerQueueEx(m_TimerQueue, INVALID_HANDLE_VALUE);
}

TIMER_ID Timer::addTimer(TimerCallback _Callback, unsigned int _Interval) {
	TIMER_ID timerID = INVALID_TIMER_ID;
	shared_ptr<TimerCallback> task = make_shared<TimerCallback>(_Callback);

	if (!CreateTimerQueueTimer(&timerID, m_TimerQueue, WAITORTIMERCALLBACK(_TimerCallback), task.get(), _Interval, 0, WT_EXECUTELONGFUNCTION)) {
		return INVALID_TIMER_ID;
	}
	
	m_mapTimer[timerID] = task;
	return timerID;
}

bool Timer::deleteTimer(TIMER_ID _ID) {
	if (DeleteTimerQueueTimer(m_TimerQueue, _ID, INVALID_HANDLE_VALUE)) {
		m_mapTimer.erase(_ID);
		return true;
	}
	return false;
}

void Timer::_TimerCallback(void * _Param, bool _First) {
	TimerCallback *task = reinterpret_cast<TimerCallback *>(_Param);
	if (task) {
		(*task)();
	}
}
