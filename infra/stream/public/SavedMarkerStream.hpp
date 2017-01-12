#ifndef INFRA_SAVED_MARKER_STREAM_HPP
#define INFRA_SAVED_MARKER_STREAM_HPP

#include "infra/stream/public/OutputStream.hpp"

namespace infra
{
    class SavedMarkerTextStream
        : private StreamWriter
        , public TextOutputStream
    {
    public:
        SavedMarkerTextStream(TextOutputStream stream, const uint8_t* marker);
        ~SavedMarkerTextStream();

    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual void Forward(std::size_t amount) override;

        virtual const uint8_t* ConstructSaveMarker() const override;
        virtual infra::ByteRange SaveState(const uint8_t* marker) override;
        virtual void RestoreState(infra::ByteRange range) override;

    private:
        TextOutputStream stream;
        infra::ByteRange streamRange;
    };

    class SavedMarkerDataStream
        : private StreamWriter
        , public DataOutputStream
    {
    public:
        SavedMarkerDataStream(DataOutputStream stream, const uint8_t* marker);
        ~SavedMarkerDataStream();

    private:
        virtual void Insert(ConstByteRange range) override;
        virtual void Insert(uint8_t element) override;
        virtual void Forward(std::size_t amount) override;

        virtual const uint8_t* ConstructSaveMarker() const override;
        virtual infra::ByteRange SaveState(const uint8_t* marker) override;
        virtual void RestoreState(infra::ByteRange range) override;

    private:
        DataOutputStream stream;
        infra::ByteRange streamRange;
    };
}

#endif