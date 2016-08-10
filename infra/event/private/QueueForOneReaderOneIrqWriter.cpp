#include "infra/event/public/EventDispatcher.hpp"
#include "infra/event/public/QueueForOneReaderOneIrqWriter.hpp"
#include <cassert>

namespace infra
{
    QueueForOneReaderOneIrqWriter::QueueForOneReaderOneIrqWriter(const infra::ByteRange& aBuffer, const infra::Function<void()>& aOnDataAvailable)
        : buffer(aBuffer)
        , contentsBegin(buffer.begin())
        , contentsEnd(buffer.begin())
        , onDataAvailable(aOnDataAvailable)
    {
        notificationScheduled = false;
    }

    void QueueForOneReaderOneIrqWriter::AddFromInterrupt(uint8_t element)
    {
        assert(!Full());
        *contentsEnd = element;

        if (contentsEnd == buffer.end() - 1)
            contentsEnd = buffer.begin();
        else
            ++contentsEnd;

        NotifyDataAvailable();
    }

    bool QueueForOneReaderOneIrqWriter::Empty() const
    {
        return contentsBegin.load() == contentsEnd.load();
    }

    bool QueueForOneReaderOneIrqWriter::Full() const
    {
        return (contentsEnd == buffer.end() - 1 || contentsBegin == contentsEnd + 1)
            && (contentsEnd != buffer.end() - 1 || contentsBegin == buffer.begin());
    }

    uint8_t QueueForOneReaderOneIrqWriter::Get()
    {
        assert(!Empty());
        uint8_t result = *contentsBegin;

        if (contentsBegin == buffer.end() - 1)
            contentsBegin = buffer.begin();
        else
            ++contentsBegin;

        return result;
    }

    void QueueForOneReaderOneIrqWriter::NotifyDataAvailable()
    {
        if (!Empty() && !notificationScheduled.exchange(true))
            infra::EventDispatcher::Instance().Schedule([this]() { DataAvailable(); });
    }

    void QueueForOneReaderOneIrqWriter::NotifyDataAvailableFromInterrupt()
    {
        if (!notificationScheduled.exchange(true))
            infra::EventDispatcher::Instance().Schedule([this]() { DataAvailable(); });
    }

    void QueueForOneReaderOneIrqWriter::DataAvailable()
    {
        onDataAvailable();

        notificationScheduled = false;

        NotifyDataAvailable();
    }
}
