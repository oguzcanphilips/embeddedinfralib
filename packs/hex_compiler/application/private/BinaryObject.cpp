#include "packs/hex_compiler/application/public/BinaryObject.hpp"
#include "infra/stream/public/StdStringInputStream.hpp"

namespace application
{
    LineException::LineException(const std::string& file, int line)
        : file(file)
        , line(line)
    {}

    IncorrectCrcException::IncorrectCrcException(const std::string& file, int line)
        : LineException(file, line)
    {}

    NoEndOfFileException::NoEndOfFileException(const std::string& file, int line)
        : LineException(file, line)
    {}

    DataAfterEndOfFileException::DataAfterEndOfFileException(const std::string& file, int line)
        : LineException(file, line)
    {}

    UnknownRecordException::UnknownRecordException(const std::string& file, int line)
        : LineException(file, line)
    {}

    RecordTooShortException::RecordTooShortException(const std::string& file, int line)
        : LineException(file, line)
    {}

    RecordTooLongException::RecordTooLongException(const std::string& file, int line)
        : LineException(file, line)
    {}

    void BinaryObject::AddHex(const std::vector<std::string>& data, uint32_t offset, const std::string& fileName)
    {
        linearAddress = 0;
        endOfFile = false;
        this->offset = offset;

        int lineNumber = 0;
        for (auto& line : data)
        {
            ++lineNumber;
            if (!line.empty())
                AddLine(line, fileName, lineNumber);
        }

        if (!endOfFile)
            throw NoEndOfFileException(fileName, lineNumber);
    }

    void BinaryObject::AddBinary(const std::vector<uint8_t>& data, uint32_t offset, const std::string& fileName)
    {
        for (auto byte: data)
        {
            memory.Insert(byte, offset);
            ++offset;
        }
    }

    const SparseVector<uint8_t>& BinaryObject::Memory() const
    {
        return memory;
    }

    void BinaryObject::AddLine(const std::string& line, const std::string& fileName, int lineNumber)
    {
        if (endOfFile)
            throw DataAfterEndOfFileException(fileName, lineNumber);

        LineContents lineContents(line, fileName, lineNumber);
        switch (lineContents.recordType)
        {
            case 0:
                for (std::size_t i = 0; i != lineContents.data.size(); ++i)
                    memory.Insert(lineContents.data[i], linearAddress + offset + lineContents.address + i);
                break;
            case 1:
                endOfFile = true;
                break;
            case 2:
                linearAddress = (lineContents.data[0] * 256 + lineContents.data[1]) << 4;
                break;
            case 3:
                // Ignore Start Segment Address because in hex file, the entrypoint of the program is not interesting
                break;
            case 4:
                assert(lineContents.data.size() == 2);
                linearAddress = (lineContents.data[0] * 256 + lineContents.data[1]) << 16;
                break;
            case 5:
                // Ignore Start Linear Address because in hex file, the entrypoint of the program is not interesting
                break;
            default:
                throw UnknownRecordException(fileName, lineNumber);
        }
    }
    
    BinaryObject::LineContents::LineContents(std::string line, const std::string& fileName, int lineNumber)
    {
        infra::StdStringInputStream stream(line, infra::softFail);

        char colon;
        stream >> colon;

        uint8_t size;
        stream >> infra::hex >> infra::Width(2, '0') >> size >> infra::Width(4, '0') >> address >> infra::Width(2, '0') >> recordType;
        uint8_t sum = size + address + (address >> 8) + recordType;

        if (stream.HasFailed())
            throw RecordTooShortException(fileName, lineNumber);

        for (std::size_t i = 0; i != size; ++i)
        {
            uint8_t byte;
            stream >> infra::hex >> infra::Width(2) >> byte;
            sum += byte;
            data.push_back(byte);
        }

        uint8_t checksum;
        stream >> infra::hex >> infra::Width(2) >> checksum;
        sum += checksum;

        if (stream.HasFailed())
            throw RecordTooShortException(fileName, lineNumber);
        if (sum != 0)
            throw IncorrectCrcException(fileName, lineNumber);
        if (!line.empty())
            throw RecordTooLongException(fileName, lineNumber);
    }
}
