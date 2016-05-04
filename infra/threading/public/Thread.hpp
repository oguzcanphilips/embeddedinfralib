#ifndef THREAD_HPP
#define THREAD_HPP

#include "Event.hpp"
#include "Lock.hpp"
#include <cstdint>

namespace infra
{
    class Thread
    {
    public:
        Thread();
        virtual ~Thread();
        static void Sleep(uint32_t ms);
        void Start();
        void Join();
    protected:
        virtual void Run() = 0;

#ifdef WIN32
    private:
        static unsigned long __stdcall Process(void* p);

        Lock mLock;
        Event mStarted;
        void* mThread;

#elif defined FREERTOS
    public:
        const static uint32_t IdlePriority;
    protected:
        virtual void GetThreadInit(uint32_t& priority, uint32_t& stackSize) = 0;
    private:
        void* mTask;
        Event mStopped;
        friend void ThreadRun(void* thread);
        friend void ThreadStop(Thread* thread);

#endif
    };
}
#endif /* THREAD_H_ */
