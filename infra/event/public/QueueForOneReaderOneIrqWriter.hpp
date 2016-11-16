#ifndef INFRA_QUEUE_FOR_ONE_READER_ONE_IRQ_WRITER_HPP
#define INFRA_QUEUE_FOR_ONE_READER_ONE_IRQ_WRITER_HPP

#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/Function.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <array>
#include <atomic>
#include <cstdint>

namespace infra
{
    class QueueForOneReaderOneIrqWriter
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<QueueForOneReaderOneIrqWriter, std::array<uint8_t, Size>>;

        QueueForOneReaderOneIrqWriter(const infra::ByteRange& aBuffer, const infra::Function<void()>& aOnDataAvailable);

        void AddFromInterrupt(uint8_t element);

        bool Empty() const;
        bool Full() const;
        uint8_t Get();

    private:
        void NotifyDataAvailable();
        void DataAvailable();

    private:
        infra::ByteRange buffer;
        std::atomic<uint8_t*> contentsBegin;
        std::atomic<uint8_t*> contentsEnd;

        infra::Function<void()> onDataAvailable;
        std::atomic_bool notificationScheduled;
    };
}

#endif
