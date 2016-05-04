#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#ifdef WIN32

#elif defined FREERTOS
extern "C" 
{
	#include "FreeRTOS.h"
	#include "semphr.h"
}
#endif

#include <cstdint>

namespace infra
{
    class Semaphore
    {
    public:
        Semaphore();
        ~Semaphore();

        bool Obtain(uint32_t timeout);
        void Obtain();
        void Release();
    private:
#ifdef WIN32
        void* mHandle;
#elif defined FREERTOS
        xSemaphoreHandle mHandle;
#endif
    };
}
#endif
