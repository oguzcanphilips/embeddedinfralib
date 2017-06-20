#include "infra/event/EventDispatcher.hpp"
#include "infra/event/QueueForOneReaderOneIrqWriter.hpp"
#include <cassert>

namespace infra
{
    QueueForOneReaderOneIrqWriter::QueueForOneReaderOneIrqWriter(const infra::ByteRange& buffer, const infra::Function<void()>& onDataAvailable)
        : buffer(buffer)
        , contentsBegin(buffer.begin())
        , contentsEnd(buffer.begin())
        , onDataAvailable(onDataAvailable)
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

    void QueueForOneReaderOneIrqWriter::AddFromInterrupt(infra::ConstByteRange data)
    {
        std::size_t copySize = std::min<std::size_t>(data.size(), buffer.end() - contentsEnd);
        uint8_t* begin = contentsBegin.load();
        assert(begin <= contentsEnd.load() || begin > contentsEnd.load() + copySize);
        std::copy(data.begin(), data.begin() + copySize, contentsEnd.load());

        if (contentsEnd == buffer.end() - copySize)
            contentsEnd = buffer.begin();
        else
            contentsEnd += copySize;

        data.pop_front(copySize);

        assert(begin <= contentsEnd.load() || begin > contentsEnd.load() + data.size());
        std::copy(data.begin(), data.end(), contentsEnd.load());
        contentsEnd += data.size();

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

    infra::ConstByteRange QueueForOneReaderOneIrqWriter::ContiguousRange() const
    {
        const uint8_t* end = contentsEnd.load();

        if (end < contentsBegin)
            return infra::ConstByteRange(contentsBegin, buffer.end());
        else
            return infra::ConstByteRange(contentsBegin, end);
    }

    void QueueForOneReaderOneIrqWriter::Consume(uint32_t amount)
    {
        if (contentsBegin + amount >= buffer.end())
        {
            amount -= buffer.end() - contentsBegin;
            contentsBegin = buffer.begin();
        }

        contentsBegin += amount;
    }

    void QueueForOneReaderOneIrqWriter::NotifyDataAvailable()
    {
        if (!Empty() && !notificationScheduled.exchange(true))
            infra::EventDispatcher::Instance().Schedule([this]() { DataAvailable(); });
    }

    void QueueForOneReaderOneIrqWriter::DataAvailable()
    {
        onDataAvailable();

        notificationScheduled = false;

        NotifyDataAvailable();
    }
}
