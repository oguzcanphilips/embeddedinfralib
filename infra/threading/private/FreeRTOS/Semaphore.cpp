#include "infra\threading\public\Semaphore.hpp"

namespace infra
{
	Semaphore::Semaphore()
	{
		vSemaphoreCreateBinary(mHandle);
	}

	Semaphore::~Semaphore()
	{
		vSemaphoreDelete(mHandle);
	}

	bool Semaphore::Obtain(uint32_t timeout)
	{
		return pdTRUE == xSemaphoreTake(mHandle, timeout);
	}

	void Semaphore::Obtain()
	{
		xSemaphoreTake(mHandle, portMAX_DELAY);
	}

	void Semaphore::Release()
	{
		xSemaphoreGive(mHandle);
	}
}