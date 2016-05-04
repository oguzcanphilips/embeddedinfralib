#include "infra\threading\public\Event.hpp"

namespace infra
{
	Event::Event()
	{
		mQueue = xQueueCreate(1, 0);
	}

	Event::~Event()
	{
		vQueueDelete(mQueue);
	}

	bool Event::Wait(unsigned long timeout)
	{
		return pdTRUE == xQueuePeek(mQueue, 0, timeout);
	}

	void Event::Wait()
	{
		xQueuePeek(mQueue, 0, portMAX_DELAY);
	}

	void Event::Set()
	{
		xQueueSend(mQueue, 0, 0);
	}

	void Event::Reset()
	{
		xQueueReceive(mQueue, 0, 0);
	}
}
