#include "infra\threading\public\Thread.hpp"

extern "C"
{
	#include "FreeRTOS.h"
	#include "task.h"
}

namespace infra
{
	const uint32_t Thread::IdlePriority = tskIDLE_PRIORITY;

	void ThreadStop(Thread* thread)
	{
		thread->mStopped.Set();
	}

	void ThreadRun(void* thread)
	{
		Thread* self = (Thread*)thread;
		self->Run();
		ThreadStop(self);
		vTaskDelete(0);
	}

	extern "C" void ThreadStarter(void* thread)
	{
		ThreadRun(thread);
	}

	Thread::Thread()
	: mTask(0)
	{
	}

	void Thread::Start()
	{
		if(mTask) return;

		uint32_t priority;
		uint32_t stackSize;
		GetThreadInit(priority, stackSize);
		if(pdPASS != xTaskCreate( ThreadStarter, (const signed char*)"", stackSize, this, priority, &mTask))
		{
			mTask = 0;
		}
	}

	Thread::~Thread()
	{
		vTaskDelete(mTask);
		mTask = 0;
	}

	void Thread::Sleep(uint32_t ms)
	{
		vTaskDelay(ms);
	}

	void Thread::Join()
	{
		mStopped.Wait();
	}
}
