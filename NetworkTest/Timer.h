#pragma once
#include <functional>
#include <memory>
#include <map>
#include <mutex>

#define INVALID_TIMER_ID		((void *)-1)

namespace Network {
	typedef void *					TIMER_ID;
	typedef void *					TIMER_QUEUE_ID;
	typedef std::function<void()>	TimerCallback;

	class Timer {
	public:
		Timer();
		virtual ~Timer();

	public:
		TIMER_ID addTimer(TimerCallback _Callback, unsigned int _Interval);
		bool deleteTimer(TIMER_ID _ID);

	private:
		Timer(const Timer & _Timer) {};
		static void __stdcall _TimerCallback(void * _Param, bool _First);

	private:
		TIMER_QUEUE_ID		m_TimerQueue;
		std::map<TIMER_ID, std::shared_ptr<TimerCallback>>	m_mapTimer;
	};
}