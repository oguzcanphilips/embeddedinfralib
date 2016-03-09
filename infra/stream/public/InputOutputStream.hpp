#ifndef INFRA_INPUT_OUTPUT_STREAM_HPP
#define INFRA_INPUT_OUTPUT_STREAM_HPP

#include "infra/util/public/ByteRange.hpp"
#include <cstdlib>
#include <type_traits>

namespace infra
{
    struct SoftFail {};
    const SoftFail softFail;

    template<class T>
    class InputStream
    {
    public:
        InputStream() = default;
        explicit InputStream(SoftFail);

        virtual void Extract(MemoryRange<T> range) = 0;
        virtual void Extract(T& element) = 0;
        virtual void Peek(T& element) = 0;
        virtual void Forward(std::size_t amount) = 0;
        virtual bool Empty() const = 0;

        virtual void ReportFailureCheck(bool hasCheckFailed);
        virtual bool HasFailed() const;
        virtual void ResetFail();

    protected:
        ~InputStream();

    private:
        bool softFailMode = false;
        bool failed = false;
        mutable bool checkedFail = true;
    };

    class OutputStreamWriter
    {
    public:
        virtual void Insert(ConstByteRange range) = 0;
        virtual void Insert(uint8_t element) = 0;
        virtual void Forward(std::size_t amount) = 0;

    protected:
        ~OutputStreamWriter() = default;
    };

    template<class T>
    class IndirectInputStream
        : public InputStream<T>
    {
    public:
        IndirectInputStream(InputStream<T>& stream);
        IndirectInputStream(InputStream<T>& stream, SoftFail);

    public:
        void Extract(MemoryRange<T> range) override;
        void Extract(T& element) override;
        void Peek(T& element) override;
        void Forward(std::size_t amount) override;
        bool Empty() const override;

        void ReportFailureCheck(bool hasCheckFailed) override;
        bool HasFailed() const override;
        void ResetFail() override;

    protected:
        InputStream<T>& stream;
    };

    struct ForwardStream
    {
        explicit ForwardStream(std::size_t amount);

        std::size_t amount;
    };

    template<class Stream>
        Stream& operator>>(Stream& stream, ForwardStream forward);
    template<class Stream>
        Stream& operator<<(Stream& stream, ForwardStream forward);

    ////    Implementation    ////

    template<class T>
    InputStream<T>::InputStream(SoftFail)
        : softFailMode(true)
    {}

    template<class T>
    InputStream<T>::~InputStream()
    {
        assert(checkedFail);
    }

    template<class T>
    void InputStream<T>::ReportFailureCheck(bool hasCheckFailed)
    {
        checkedFail = !softFailMode;
        failed = hasCheckFailed;
        assert(softFailMode || !hasCheckFailed);
    }

    template<class T>
    bool InputStream<T>::HasFailed() const
    {
        checkedFail = true;
        return failed;
    }

    template<class T>
    void InputStream<T>::ResetFail()
    {
        assert(checkedFail);
        failed = false;
    }

    template<class T>
    IndirectInputStream<T>::IndirectInputStream(InputStream<T>& stream)
        : stream(stream)
    {}

    template<class T>
    IndirectInputStream<T>::IndirectInputStream(InputStream<T>& stream, SoftFail)
        : InputStream<T>(softFail)
        , stream(stream)
    {}

    template<class T>
    void IndirectInputStream<T>::Extract(MemoryRange<T> range)
    {
        stream.Extract(range);
    }

    template<class T>
    void IndirectInputStream<T>::Extract(T& element)
    {
        stream.Extract(element);
    }

    template<class T>
    void IndirectInputStream<T>::Peek(T& element)
    {
        stream.Peek(element);
    }

    template<class T>
    void IndirectInputStream<T>::Forward(std::size_t amount)
    {
        stream.Forward(amount);
    }

    template<class T>
    bool IndirectInputStream<T>::Empty() const
    {
        return stream.Empty();
    }

    template<class T>
    void IndirectInputStream<T>::ReportFailureCheck(bool hasCheckFailed)
    {
        stream.ReportFailureCheck(hasCheckFailed);
    }

    template<class T>
    bool IndirectInputStream<T>::HasFailed() const
    {
        return stream.HasFailed();
    }

    template<class T>
    void IndirectInputStream<T>::ResetFail()
    {
        stream.ResetFail();
    }

    inline ForwardStream::ForwardStream(std::size_t amount)
        : amount(amount)
    {}

    template<class Stream>
    Stream& operator>>(Stream& stream, ForwardStream forward)
    {
        stream.Forward(forward.amount);
        return stream;
    }

    template<class Stream>
    Stream& operator<<(Stream& stream, ForwardStream forward)
    {
        stream.Forward(forward.amount);
        return stream;
    }
}

#endif
