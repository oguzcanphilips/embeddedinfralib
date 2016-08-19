#include "packs/hex_compiler/application/public/HexOutput.hpp"
#include "infra/stream/public/StdStringOutputStream.hpp"

namespace application
{
    std::string HexLine(uint8_t recordType, uint16_t address, const std::vector<uint8_t>& data)
    {
        infra::StdStringOutputStream::WithStorage result;

        result << ':' << infra::hex << infra::Width(2, '0') << data.size() << infra::Width(4, '0') << address << infra::Width(2, '0') << recordType;

        uint8_t checkSum = static_cast<uint8_t>(data.size()) + static_cast<uint8_t>(address >> 8) + static_cast<uint8_t>(address & 0xff) + recordType;

        for (uint8_t byte : data)
        {
            result << infra::hex << infra::Width(2, '0') << byte;
            checkSum += byte;
        }

        result << infra::hex << infra::Width(2, '0') << static_cast<uint8_t>(256 - checkSum);

        return result.Storage();
    }

    std::vector<std::string> HexOutput(const SparseVector<uint8_t>& data, uint32_t maxBytesPerLine)
    {
        std::vector<std::string> result;

        uint16_t linearAddress = 0;

        SparseVector<uint8_t>::Iterator i = data.begin();
        while (i != data.end())
        {
            std::vector<uint8_t> line;
            uint32_t startAddress = (*i).first;
            uint32_t address = startAddress;
            while (i != data.end() && line.size() != maxBytesPerLine)
            {
                if ((*i).first != address)
                    break;

                line.push_back((*i).second);

                ++i;
                ++address;
            }

            if (!line.empty())
            {
                if ((startAddress >> 16) != linearAddress)
                {
                    linearAddress = startAddress >> 16;
                    result.push_back(HexLine(4, 0, std::vector<uint8_t>{ static_cast<uint8_t>(linearAddress >> 8), static_cast<uint8_t>(linearAddress) }));
                }

                result.push_back(HexLine(0, static_cast<uint16_t>(startAddress), line));
            }
        }

        result.push_back(HexLine(1, 0, std::vector<uint8_t>()));

        return result;
    }
}
