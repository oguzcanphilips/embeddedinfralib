#ifndef LOCK_HPP
#define LOCK_HPP

#ifdef WIN32

#elif defined FREERTOS
extern "C" 
{
	#include "FreeRTOS.h"
	#include "semphr.h"
}
#endif

namespace infra
{
    class Lock
    {
    public:
        Lock();
        ~Lock();
        void Get();
        void Release();
        class ScopedLock
        {
        public:
            ScopedLock(Lock& lock);
            ~ScopedLock();
        private:
            Lock& mLock;
            ScopedLock(const ScopedLock&) = delete;
            const ScopedLock& operator=(const ScopedLock&) = delete;
        };
    private:
        // define only
        Lock(const Lock&);
        const Lock& operator=(const Lock&);
        friend class Lock::ScopedLock;

#ifdef WIN32
        void* mCriticalSection;
#elif defined FREERTOS
        xSemaphoreHandle mMutex;
#endif
    };
}
#endif 
