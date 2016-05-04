#include "infra\threading\public\Event.hpp"
#include <windows.h>
#include <cassert>

namespace infra
{
    Event::Event()
    {
        m_Event = ::CreateEvent(0, true, false, 0);
    }

    Event::~Event()
    {
        ::CloseHandle(m_Event);
    }

    bool Event::Wait(unsigned long timeout)
    {
        return (WAIT_OBJECT_0 == ::WaitForSingleObject(m_Event, timeout));
    }

    void Event::Wait()
    {
        ::WaitForSingleObject(m_Event, INFINITE);
    }

    void Event::Set()
    {
        if (!::SetEvent(m_Event))
        {
            assert(false);
        }
    }

    void Event::Reset()
    {
        if (!::ResetEvent(m_Event))
        {
            assert(false);
        }
    }
}