#ifndef INFRA_BYTE_INPUT_STREAM_HPP
#define INFRA_BYTE_INPUT_STREAM_HPP

// With a ByteInputStream, you can easily place all sorts of objects into a block of memory.
// A ByteInputStream is created with a ByteRange as argument, objects streamed into the
// ByteInputStream are placed into that range, objects streamed out of the ByteInputStream are
// taken from that range.
// Most of the times, you will want to use a ByteStream, available via ByteRange.hpp.
//
// Example:
#include "infra/stream/public/InputStream.hpp"
#include "infra/stream/public/InputStreamHelpers.hpp"
#include "infra/util/public/ByteRange.hpp"
#include "infra/util/public/WithStorage.hpp"
#include <algorithm>
#include <cassert>

namespace infra
{
    class ByteInputStream
        : public DataInputStreamHelper<uint8_t>
    {
    public:
        explicit ByteInputStream(ConstByteRange aRange);

        ConstByteRange Processed() const;   // Invariant: Processed() ++ Remaining() == range
        ConstByteRange Remaining() const;

        template<std::size_t Size>
            using WithStorage = infra::WithStorage<ByteInputStream, std::array<uint8_t, Size>>;

    public:
        void Extract(ByteRange range) override;
        void Extract(uint8_t& element) override;
        void Peek(uint8_t& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;

    private:
        ConstByteRange range;
        std::size_t offset = 0;
    };

    ////    Implementation    ////

    ByteInputStream::ByteInputStream(ConstByteRange aRange)
        : DataInputStreamHelper<uint8_t>(static_cast<InputStream&>(*this))
        , range(aRange)
    {}

    ConstByteRange ByteInputStream::Processed() const
    {
        return MakeRange(range.begin(), range.begin() + offset);
    }

    ConstByteRange ByteInputStream::Remaining() const
    {
        return MakeRange(range.begin() + offset, range.end());
    }

    void ByteInputStream::Extract(ByteRange dataRange)
    {
        assert(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        offset += dataRange.size();
    }

    void ByteInputStream::Extract(uint8_t& element)
    {
        Extract(MakeByteRange(element));
    }

    void ByteInputStream::Peek(uint8_t& element)
    {
        ByteRange dataRange(MakeByteRange(element));
        assert(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
    }

    void ByteInputStream::Forward(std::size_t amount)
    {
        assert(amount <= range.size() - offset);
        offset += amount;
    }

    bool ByteInputStream::Empty() const
    {
        return offset == range.size();
    }
}

#endif
