#include "packs/hex_compiler/application/public/HexOutput.hpp"
#include "infra/stream/public/StdStringOutputStream.hpp"

namespace application
{
    namespace
    {
        class HexOutputHelper
        {
        public:
            HexOutputHelper(const SparseVector<uint8_t>& data, uint32_t maxBytesPerLine);

            std::vector<std::string> Output();

        private:
            void WriteBlock();
            void UpdateLinearAddress(uint32_t startAddress);
            std::string HexLine(uint8_t recordType, uint16_t address, const std::vector<uint8_t>& data) const;
            void OutputHexBytes(infra::TextOutputStream& stream, const std::vector<uint8_t>& data, uint8_t& checkSum) const;
            void WriteLastLine();
            void WriteLine(uint32_t startAddress, const std::vector<uint8_t>& line);

        private:
            const SparseVector<uint8_t>& data;
            SparseVector<uint8_t>::Iterator dataBlock;
            uint32_t maxBytesPerLine;
            uint16_t linearAddress = 0;
            std::vector<std::string> result;
        };

        HexOutputHelper::HexOutputHelper(const SparseVector<uint8_t>& data, uint32_t maxBytesPerLine)
            : data(data)
            , dataBlock(data.begin())
            , maxBytesPerLine(maxBytesPerLine)
        {
            while (dataBlock != data.end())
                WriteBlock();

            WriteLastLine();
        }

        std::vector<std::string> HexOutputHelper::Output()
        {
            return result;
        }

        void HexOutputHelper::WriteBlock()
        {
            std::vector<uint8_t> line;
            uint32_t startAddress = (*dataBlock).first;
            uint32_t address = startAddress;
            while (dataBlock != data.end() && line.size() != maxBytesPerLine && (*dataBlock).first == address)
            {
                line.push_back((*dataBlock).second);

                ++dataBlock;
                ++address;
            }

            UpdateLinearAddress(startAddress);
            WriteLine(startAddress, line);
        }

        void HexOutputHelper::UpdateLinearAddress(uint32_t startAddress)
        {
            if ((startAddress >> 16) != linearAddress)
            {
                linearAddress = startAddress >> 16;
                result.push_back(HexLine(4, 0, std::vector<uint8_t>{ static_cast<uint8_t>(linearAddress >> 8), static_cast<uint8_t>(linearAddress) }));
            }
        }

        std::string HexOutputHelper::HexLine(uint8_t recordType, uint16_t address, const std::vector<uint8_t>& data) const
        {
            infra::StdStringOutputStream::WithStorage result;

            result << ':' << infra::hex << infra::Width(2, '0') << data.size() << infra::Width(4, '0') << address << infra::Width(2, '0') << recordType;
            uint8_t checkSum = static_cast<uint8_t>(data.size()) + static_cast<uint8_t>(address >> 8) + static_cast<uint8_t>(address & 0xff) + recordType;
            OutputHexBytes(result, data, checkSum);
            result << infra::hex << infra::Width(2, '0') << static_cast<uint8_t>(256 - checkSum);

            return result.Storage();
        }

        void HexOutputHelper::OutputHexBytes(infra::TextOutputStream& stream, const std::vector<uint8_t>& data, uint8_t& checkSum) const
        {
            for (uint8_t byte : data)
            {
                stream << infra::hex << infra::Width(2, '0') << byte;
                checkSum += byte;
            }
        }

        void HexOutputHelper::WriteLastLine()
        {
            result.push_back(HexLine(1, 0, std::vector<uint8_t>()));
        }

        void HexOutputHelper::WriteLine(uint32_t startAddress, const std::vector<uint8_t>& line)
        {
            result.push_back(HexLine(0, static_cast<uint16_t>(startAddress), line));
        }
    }

    std::vector<std::string> HexOutput(const SparseVector<uint8_t>& data, uint32_t maxBytesPerLine)
    {
        HexOutputHelper helper(data, maxBytesPerLine);
        return helper.Output();
    }
}
