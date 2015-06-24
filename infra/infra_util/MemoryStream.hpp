#ifndef INFRA_MEMORY_STREAM_H
#define INFRA_MEMORY_STREAM_H

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
// infra::ByteStream writeStream(memory);
// writeStream << uint8_t(4) << myData << myChecksum;
//
// Now memory contains the bytes 0x04, 0x12, 0x34, 0x56, 0x78.

// With MemoryStreamManipulators, special classes provide additional functionality instead
// of having their contents streamed. For example, ForwardStream skips over elements
// in the stream.
//
// Example:
//
// infra::ByteStream readStream(memory);
// readStream >> infra::ForwardStream(1) >> myData;

#include "infra_util/MemoryRange.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <type_traits>

namespace infra
{

    template<class T>
    class MemoryStream
    {
    public:
        MemoryStream();
        explicit MemoryStream(MemoryRange<T> aRange);

        template<class Data>
            MemoryStream& operator<<(const Data& data);
        template<class Data>
            MemoryStream& operator<<(MemoryRange<Data> data);
        template<class Data>
            MemoryStream& operator>>(Data& data);
        template<class Data>
            MemoryStream& operator>>(MemoryRange<Data> data);

        MemoryRange<T> Processed() const;   // Invariant: Processed() ++ Remaining() == range
        MemoryRange<T> Remaining() const;

        // Consume indicates that a number of elements from Remaining have been used externally, probably by a manipulator
        void Consume(std::size_t size);

    private:
        template<class Data>
            void Insert(const Data& data, std::false_type);
        template<class Manipulator>
            void Insert(const Manipulator& manipulator, std::true_type);

        template<class Data>
            void Extract(Data& data, std::false_type);
        template<class Manipulator>
            void Extract(Manipulator& manipulator, std::true_type);

    private:
        MemoryRange<T> range;
        std::size_t offset;
    };

    struct MemoryStreamManipulator
    {
        // Instead of having their contents streamed into the MemoryStream,
        // descendants from MemoryStreamManipulator provide functions that
        // are called by the MemoryStream upon insertion or extraction.
        
        // A manipulator that can be used like this: stream << manipulator must provide an Insert function:
        // void Insert(MemoryStream<T>& stream) const;

        // A manipulator that can be used like this: stream >> manipulator must provide an Extract function:
        // void Extract(MemoryStream<T>& stream) const;
    };

    struct ForwardStream
        : MemoryStreamManipulator
    {
        explicit ForwardStream(std::size_t size);

        template<class T>
            void Insert(MemoryStream<T>& stream) const;
        template<class T>
            void Extract(MemoryStream<T>& stream) const;

        std::size_t size;
    };

    ////    Implementation    ////

    template<class T>
    MemoryStream<T>::MemoryStream()
        : offset(0)
    {}

    template<class T>
    MemoryStream<T>::MemoryStream(MemoryRange<T> aRange)
        : range(aRange)
        , offset(0)
    {}

    template<class T>
    template<class Data>
    MemoryStream<T>& MemoryStream<T>::operator<<(const Data& data)
    {
        Insert(data, typename std::is_base_of<MemoryStreamManipulator, Data>::type());
        return *this;
    }

    template<class T>
    template<class Data>
    MemoryStream<T>& MemoryStream<T>::operator<<(MemoryRange<Data> data)
    {
        MemoryRange<const T> dataRange(ReinterpretCastMemoryRange<const T>(data));
        assert(dataRange.size() <= range.size() - offset);
        std::copy(dataRange.begin(), dataRange.begin() + dataRange.size(), range.begin() + offset);
        offset += dataRange.size();
        return *this;
    }

    template<class T>
    template<class Data>
    MemoryStream<T>& MemoryStream<T>::operator>>(Data& data)
    {
        Extract(data, typename std::is_base_of<MemoryStreamManipulator, Data>::type());
        return *this;
    }

    template<class T>
    template<class Data>
    MemoryStream<T>& MemoryStream<T>::operator>>(MemoryRange<Data> data)
    {
        MemoryRange<typename std::remove_const<T>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<T>::type>(data));
        assert(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        offset += dataRange.size();
        return *this;
    }

    template<class T>
    template<class Data>
    void MemoryStream<T>::Insert(const Data& data, std::false_type)
    {
        MemoryRange<const T> dataRange(ReinterpretCastMemoryRange<const T>(MakeRange(&data, &data + 1)));
        assert(dataRange.size() <= range.size() - offset);
        std::copy(dataRange.begin(), dataRange.begin() + dataRange.size(), range.begin() + offset);
        offset += dataRange.size();
    }

    template<class T>
    template<class Manipulator>
    void MemoryStream<T>::Insert(const Manipulator& manipulator, std::true_type)
    {
        manipulator.Insert(*this);
    }

    template<class T>
    template<class Data>
    void MemoryStream<T>::Extract(Data& data, std::false_type)
    {
        MemoryRange<typename std::remove_const<T>::type> dataRange(ReinterpretCastMemoryRange<typename std::remove_const<T>::type>(MakeRange(&data, &data + 1)));
        assert(dataRange.size() <= range.size() - offset);
        std::copy(range.begin() + offset, range.begin() + offset + dataRange.size(), dataRange.begin());
        offset += dataRange.size();
    }

    template<class T>
    template<class Manipulator>
    void MemoryStream<T>::Extract(Manipulator& manipulator, std::true_type)
    {
        manipulator.Extract(*this);
    }

    template<class T>
    MemoryRange<T> MemoryStream<T>::Processed() const
    {
        return MakeRange(range.begin(), range.begin() + offset);
    }

    template<class T>
    MemoryRange<T> MemoryStream<T>::Remaining() const
    {
        return MakeRange(range.begin() + offset, range.end());
    }

    template<class T>
    void MemoryStream<T>::Consume(std::size_t size)
    {
        assert(size <= range.size() - offset);
        offset += size;
    }

    inline ForwardStream::ForwardStream(std::size_t size)
        : size(size)
    {}

    template<class T>
    void ForwardStream::Insert(MemoryStream<T>& stream) const
    {
        stream.Consume(size);
    }

    template<class T>
    void ForwardStream::Extract(MemoryStream<T>& stream) const
    {
        stream.Consume(size);
    }

}

#endif
