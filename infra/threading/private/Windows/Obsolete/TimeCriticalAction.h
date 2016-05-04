#ifndef TIMECRITICALACTION_H
#define TIMECRITICALACTION_H

#include <Windows.h>

class TimeCriticalAction
{
public:
	TimeCriticalAction()
		: m_CurrentThread(GetCurrentThread())
		, m_CurrentThreadPriority(GetThreadPriority(m_CurrentThread))
	{
		SetThreadPriority(m_CurrentThread, THREAD_PRIORITY_TIME_CRITICAL);
	}
	~TimeCriticalAction()
	{
		SetThreadPriority(m_CurrentThread, m_CurrentThreadPriority);
	}
private:
	TimeCriticalAction(const TimeCriticalAction&);
	const TimeCriticalAction& operator=(const TimeCriticalAction&);

	HANDLE m_CurrentThread;
	int m_CurrentThreadPriority;
};

#endif
