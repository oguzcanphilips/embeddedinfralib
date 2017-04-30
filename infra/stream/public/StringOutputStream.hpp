#ifndef INFRA_STRING_OUTPUT_STREAM_HPP
#define INFRA_STRING_OUTPUT_STREAM_HPP

#include "infra/stream/public/StreamManipulators.hpp"
#include "infra/stream/public/OutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include <cstdint>

namespace infra
{
    class StringOutputStream                                                                            //TICS !OOP#013
        : private StreamWriter
        , public TextOutputStream
    {
    public:
        template<std::size_t Size>
            using WithStorage = infra::WithStorage<StringOutputStream, BoundedString::WithStorage<Size>>;

        explicit StringOutputStream(BoundedString& string);
        StringOutputStream(BoundedString& string, SoftFail);
        StringOutputStream(BoundedString& string, NoFail);
        ~StringOutputStream() = default;

        template<class T>
            ReservedProxy<T> Reserve();

    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;

        virtual const uint8_t* ConstructSaveMarker() const override;
        virtual std::size_t GetProcessedBytesSince(const uint8_t* marker) const override;
        virtual infra::ByteRange SaveState(const uint8_t* marker) override;
        virtual void RestoreState(infra::ByteRange range) override;

    private:
        BoundedString& string;
    };

    ////    Implementation    ////

    template<class T>
    ReservedProxy<T> StringOutputStream::Reserve()
    {
        ByteRange range(ReinterpretCastByteRange(MemoryRange<char>(string.end(), string.end() + sizeof(T))));
        std::size_t spaceLeft = string.max_size() - string.size();
        bool spaceOk = range.size() <= spaceLeft;
        ReportResult(spaceOk);
        if (!spaceOk)
            range.shrink_from_back_to(spaceLeft);

        string.append(range.size(), 0);

        return ReservedProxy<T>(range);
    }
}

#endif
