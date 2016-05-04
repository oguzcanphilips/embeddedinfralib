#include "infra\threading\public\Lock.hpp"
#include <windows.h>

namespace infra
{
    Lock::Lock()
    {
        mCriticalSection = new CRITICAL_SECTION;
        InitializeCriticalSection((CRITICAL_SECTION*)mCriticalSection);
    }

    Lock::~Lock()
    {
        CRITICAL_SECTION* p = (CRITICAL_SECTION*)mCriticalSection;
        DeleteCriticalSection((CRITICAL_SECTION*)mCriticalSection);
        delete p;
    }

    void Lock::Get()
    {
        EnterCriticalSection((CRITICAL_SECTION*)mCriticalSection);
    }

    void Lock::Release()
    {
        LeaveCriticalSection((CRITICAL_SECTION*)mCriticalSection);
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
