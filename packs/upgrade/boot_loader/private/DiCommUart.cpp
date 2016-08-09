#include "infra/util/public/BoundedVector.hpp"
#include "infra/util/public/CrcCcittCalculator.hpp"
#include "packs/upgrade/boot_loader/public/DiCommUart.hpp"

namespace application
{
    const std::array<uint8_t, 2> DiCommUart::startBytes = { 0xfe, 0xff };
    const std::array<uint8_t, 1> null = { 0 };

    DiCommUart::DiCommUart(hal::SynchronousSerialCommunication& communication, hal::TimeKeeper& timeKeeper)
        : communication(communication)
        , timeKeeper(timeKeeper)
    {}

    bool DiCommUart::Initialize()
    {
        communication.SendData(startBytes);

        Operation operation = Operation::initialize;
        communication.SendData(infra::MakeByteRange(operation));

        uint32_t size = 2;
        std::array<uint8_t, 2> length = { uint8_t(size >> 8), uint8_t(size) };
        communication.SendData(length);

        uint8_t version = 1;
        communication.SendData(infra::MakeByteRange(version));
        communication.SendData(null);

        infra::CrcCcittCalculator crcCalculator;
        crcCalculator.Update(infra::MakeByteRange(version));
        crcCalculator.Update(null);

        uint16_t crc = crcCalculator.Result();
        crc = (crc << 8) | (crc >> 8);
        communication.SendData(infra::MakeByteRange(crc));

        infra::BoundedString::WithStorage<0> responseDummy;
        return ReceiveResponse(Operation::initializeResponse, responseDummy);
    }

    bool DiCommUart::GetProps(infra::BoundedConstString port, infra::BoundedString& values)
    {
        communication.SendData(startBytes);

        Operation operation = Operation::getProps;
        communication.SendData(infra::MakeByteRange(operation));

        uint32_t size = port.size() + 1;
        std::array<uint8_t, 2> length = { uint8_t(size >> 8), uint8_t(size) };
        communication.SendData(length);

        communication.SendData(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(port)));
        communication.SendData(null);

        infra::CrcCcittCalculator crcCalculator;
        crcCalculator.Update(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(port)));
        crcCalculator.Update(null);

        uint16_t crc = crcCalculator.Result();
        crc = (crc << 8) | (crc >> 8);
        communication.SendData(infra::MakeByteRange(crc));

        return ReceiveResponse(Operation::response, values) && !values.full();
    }

    bool DiCommUart::PutProps(infra::BoundedConstString port, infra::BoundedConstString values, infra::BoundedString& result)
    {
        communication.SendData(startBytes);

        Operation operation = Operation::putProps;
        communication.SendData(infra::MakeByteRange(operation));

        uint16_t size = static_cast<uint16_t>(port.size() + values.size()) + 2;
        std::array<uint8_t, 2> length = { uint8_t(size >> 8), uint8_t(size) };
        communication.SendData(length);

        communication.SendData(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(port)));
        communication.SendData(null);
        communication.SendData(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(values)));
        communication.SendData(null);

        infra::CrcCcittCalculator crcCalculator;
        crcCalculator.Update(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(port)));
        crcCalculator.Update(null);
        crcCalculator.Update(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(values)));
        crcCalculator.Update(null);

        uint16_t crc = crcCalculator.Result();
        crc = (crc << 8) | (crc >> 8);
        communication.SendData(infra::MakeByteRange(crc));

        return ReceiveResponse(Operation::response, result);
    }

    bool DiCommUart::ReceiveResponse(Operation responseOperation, infra::BoundedString& values)
    {
        timeKeeper.Reset();

        while (true)
        {
            uint8_t firstStart;
            if (!ReceiveData(infra::MakeByteRange(firstStart)))
                return false;
            if (firstStart != 0xfe)
                continue;

            uint8_t secondStart;
            if (!ReceiveData(infra::MakeByteRange(secondStart)))
                return false;
            if (secondStart != 0xff)
                continue;

            Operation operation;
            if (!ReceiveData(infra::MakeByteRange(operation)))
                return false;
            if (operation != responseOperation)
                continue;

            uint16_t size;
            if (!ReceiveData(infra::MakeByteRange(size)))
                return false;
            size = (size << 8) | (size >> 8);

            uint8_t status;
            if (!ReceiveData(infra::MakeByteRange(status)))
                return false;
            --size;

            infra::CrcCcittCalculator crcCalculator;
            crcCalculator.Update(infra::MakeByteRange(status));

            values.resize(std::min<uint32_t>(values.max_size(), size));
            if (!values.empty())
                if (!ReceiveData(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(values))))
                    return false;
            crcCalculator.Update(infra::ReinterpretCastByteRange(infra::MakeRangeFromContainer(values)));
            size -= static_cast<uint16_t>(values.size());
            std::size_t terminatingZero = values.find('\0');
            if (terminatingZero != infra::BoundedString::npos)
                values.erase(terminatingZero);

            for (; size != 0; --size)
            {
                char value;
                if (!ReceiveData(infra::MakeByteRange(value)))
                    return false;
                crcCalculator.Update(infra::MakeByteRange(value));
            }

            uint16_t crc;
            if (!ReceiveData(infra::MakeByteRange(crc)))
                return false;
            crc = (crc << 8) | (crc >> 8);
            crc -= crcCalculator.Result();
            return status == 0 && crc == 0;
        }
    }

    bool DiCommUart::ReceiveData(infra::ByteRange data)
    {
        return communication.ReceiveData(data) && !timeKeeper.Timeout();
    }
}
