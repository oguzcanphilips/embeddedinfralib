#include "infra\threading\public\Semaphore.hpp"
#include <Windows.h>

namespace infra
{
    Semaphore::Semaphore()
    {
        mHandle = ::CreateSemaphore(0, 1, 1, 0);
    }


    Semaphore::~Semaphore()
    {
        if (mHandle)
        {
            ::CloseHandle(mHandle);
        }
    }

    bool Semaphore::Obtain(uint32_t timeout)
    {
        return WAIT_OBJECT_0 == ::WaitForSingleObject(mHandle, timeout);
    }

    void Semaphore::Obtain()
    {
        ::WaitForSingleObject(mHandle, INFINITE);
    }

    void Semaphore::Release()
    {
        ::ReleaseSemaphore(mHandle, 1, 0);
    }
}