#ifndef INFRA_MEMORY_STREAM_HPP
#define INFRA_MEMORY_STREAM_HPP

// With a MemoryStream, you can easily place all sorts of objects into a block of memory.
// A MemoryStream is created with a MemoryRange as argument, objects streamed into the
// MemoryStream are placed into that range, objects streamed out of the MemoryStream are
// taken from that range.
// Most of the times, you will want to use a ByteStream, available via ByteRange.hpp.
//
// Example:
//
// uint16_t myData = 0x1234;
// uint16_t myChecksum = 0x5678;
//
// std::array<uint8_t, 20> memory;
// infra::ByteOutputStream writeStream(memory);
// writeStream << uint8_t(4) << myData << myChecksum;
//
// Now memory contains the bytes 0x04, 0x12, 0x34, 0x56, 0x78.

#include "infra/stream/public/InputOutputStream.hpp"
#include "infra/stream/public/InputOutputStreamHelpers.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <algorithm>
#include <cassert>

namespace infra
{
    template<class T>
    class MemoryInputStream
        : public DataInputStreamHelper<T>
    {
    public:
        explicit MemoryInputStream(MemoryRange<const T> aRange);

        MemoryRange<const T> Processed() const;   // Invariant: Processed() ++ Remaining() == range
        MemoryRange<const T> Remaining() const;

        template<std::size_t Size>
            using WithStorage = infra::WithStorage<MemoryInputStream<T>, std::array<T, Size>>;

    public:
        void Extract(MemoryRange<T> range) override;
        void Extract(T& element) override;
        void Peek(T& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;

    private:
        MemoryRange<const T> range;
        std::size_t offset = 0;
    };

    class ByteOutputStream
        : private OutputStreamWriter
        , public DataOutputStream
    {
    public:
        ByteOutputStream(ByteRange range);

        ByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ByteRange Remaining() const;

        void Reset();

        template<std::size_t Size>
            using WithStorage = infra::WithStorage<ByteOutputStream, std::array<uint8_t, Size>>;

    private:
        void Insert(ConstByteRange range) override;
        void Insert(uint8_t element) override;
        void Forward(std::size_t amount) override;

    private:
        ByteRange range;
        std::size_t offset = 0;
    };

    ////    Implementation    ////

    template<class T>
    MemoryInputStream<T>::MemoryInputStream(MemoryRange<const T> aRange)
        : DataInputStreamHelper<T>(static_cast<InputStream<T>&>(*this))
        , range(aRange)
    {}

    template<class T>
    MemoryRange<const T> MemoryInputStream<T>::Processed() const
    {
        return MakeRange(range.begin(), range.begin() + offset);
    }

    template<class T>
    MemoryRange<const T> MemoryInputStream<T>::Remaining() const
    {
        return MakeRange(range.begin() + offset, range.end());
    }

    template<class T>
    void MemoryInputStream<T>::Extract(MemoryRange<T> dataRange)
    {
        assert(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        offset += dataRange.size();
    }

    template<class T>
    void MemoryInputStream<T>::Extract(T& element)
    {
        MemoryRange<T> dataRange(MakeRange(&element, &element + 1));
        Extract(dataRange);
    }

    template<class T>
    void MemoryInputStream<T>::Peek(T& element)
    {
        MemoryRange<T> dataRange(MakeRange(&element, &element + 1));
        assert(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
    }

    template<class T>
    void MemoryInputStream<T>::Forward(std::size_t amount)
    {
        assert(amount <= range.size() - offset);
        offset += amount;
    }

    template<class T>
    bool MemoryInputStream<T>::Empty() const
    {
        return offset == range.size();
    }
}

#endif
