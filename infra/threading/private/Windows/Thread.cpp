#include "infra\threading\public\Thread.hpp"

#include <cassert>
#include <windows.h>

namespace infra
{
    Thread::Thread()
        : mThread(0)
    {
    }

    Thread::~Thread()
    {
    }

    unsigned long Thread::Process(void* p)
    {
        Thread* thread = (Thread*)p;
        try
        {
            thread->mStarted.Set();
            thread->Run();
        }
        catch (...)
        {
        }
        return 0;
    }

    void Thread::Start()
    {
        Lock::ScopedLock lock(mLock);
        assert(mThread == 0);
        mStarted.Reset();
        mThread = ::CreateThread(0, 0, Process, this, 0, 0);
    }

    void Thread::Join()
    {
        Lock::ScopedLock lock(mLock);
        if (mThread)
        {
            mStarted.Wait();
            ::WaitForSingleObject(mThread, INFINITE);
            mThread = 0;
        }
    }

    void Thread::Sleep(uint32_t ms)
    {
        ::Sleep(ms);
    }
}