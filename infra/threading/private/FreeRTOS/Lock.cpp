#include "infra\threading\public\Lock.hpp"

namespace infra
{
	Lock::Lock()
	: mMutex(xSemaphoreCreateMutex())
	{
	}

	Lock::~Lock()
	{
		vSemaphoreDelete(mMutex);
	}

	void Lock::Get()
	{
		while(pdTRUE != xSemaphoreTakeRecursive(mMutex, 1000))
		{
		}
	}

	void Lock::Release()
	{
		xSemaphoreGiveRecursive(mMutex);
	}

	Lock::ScopedLock::ScopedLock(Lock& lock)
	: mLock(lock)
	{
		mLock.Get();
	}

	Lock::ScopedLock::~ScopedLock()
	{
		mLock.Release();
	}
}