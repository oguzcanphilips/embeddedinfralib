#ifndef EVENT_HPP
#define EVENT_HPP

#ifdef WIN32

#elif defined FREERTOS
extern "C" 
{
  #include "FreeRTOS.h"
  #include "queue.h"
}
#endif

namespace infra
{
    class Event
    {
    public:
        Event();
        ~Event();
        bool Wait(unsigned long timeout);
        void Wait();
        void Set();
        void Reset();
    private:
#ifdef WIN32
        void* m_Event;
#elif defined FREERTOS
        xQueueHandle mQueue;
#endif
    };
}
#endif
