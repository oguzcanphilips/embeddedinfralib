#ifndef SERVICES_USB_INTERFACE_VIRTUAL_COM_PORT_HPP
#define SERVICES_USB_INTERFACE_VIRTUAL_COM_PORT_HPP

#include "hal/interfaces/SerialCommunication.hpp"
#include "services/usb/UsbInterface.hpp"
#include "services/usb/UsbDevice.hpp"
#include "services/usb/UsbEndPoint.hpp"
#include "infra/util/ByteRange.hpp"
#include <cstdint>

namespace services
{
#define CDC_IN_EP                                   0x81  /* EP1 for data IN */
#define CDC_OUT_EP                                  0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                                  0x82  /* EP2 for CDC commands */

    class UsbInterfaceVirtualComPort
        : public UsbInterface
        , public hal::SerialCommunication
    {
    public:
        explicit UsbInterfaceVirtualComPort(UsbDevice& device);

        // Implementation of SerialCommunication
        virtual void SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion) override;
        virtual void ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived) override;

        // Implementation of UsbInterface
        virtual void Configured(uint8_t configIndex) override;
        virtual void Unconfigured(uint8_t configIndex) override;
        virtual void Setup(USBD_SetupReqTypedef  *req) override;
        virtual void EP0_TxSent() override {}
        virtual void EP0_RxReady() override;
        virtual void DataIn(uint8_t epnum) override;
        virtual void DataOut(uint8_t epnum) override;
        virtual void StartOfFrame() override {}
        virtual void IsoINIncomplete(uint8_t epnum) override {}
        virtual void IsoOUTIncomplete(uint8_t epnum) override {}
        virtual uint8_t NumberOfInterfaces() const override;
        virtual void AddConfigDescriptor(infra::DataOutputStream& stream) const override;

    private:
        void Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);

    private:
        static const uint8_t cdcSendEncapsulatedCommand = 0;
        static const uint8_t cdcGetEncapsulatedResponse = 1;
        static const uint8_t cdcSetCommFeature = 2;
        static const uint8_t cdcGetCommFeature = 3;
        static const uint8_t cdcClearCommFeature = 4;
        static const uint8_t cdcSetLineCoding = 32;
        static const uint8_t cdcGetLineCoding = 33;
        static const uint8_t cdcSetControlLineState = 34;
        static const uint8_t cdcSendBreak = 35;

        UsbDevice& device;
        infra::Function<void(infra::ConstByteRange data)> dataReceived;

        alignas(4) std::array<uint8_t, fullSpeedMaxPacketSize> data;

        uint8_t cmdOpCode = 0xFF;
        uint8_t cmdLength;

        struct LineCoding
        {
            uint32_t bitrate;
            uint8_t format;
            uint8_t paritytype;
            uint8_t datatype;
        };

        LineCoding lineCoding;

        class ConfiguredVirtualComPort
        {
        public:
            class DataEndPointOut
                : public UsbBulkEndPointOut
            {
            public:
                DataEndPointOut(UsbDevice& device, uint8_t endPointNumber, const infra::Function<void(infra::ConstByteRange data)>& dataReceived);      //TICS !OLC#020

                virtual void DataOut() override;

            private:
                void PrepareReceive();

            private:
                infra::Function<void(infra::ConstByteRange data)> dataReceived;
                alignas(4) std::array<uint8_t, fullSpeedMaxPacketSize> rxBuffer;
            };

            class DataEndPointIn
                : public UsbBulkEndPointIn
            {
            public:
                DataEndPointIn(UsbDevice& device, uint8_t endPointNumber);

                void SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion);

                virtual void DataIn() override;

            private:
                infra::Function<void()> onTransmitComplete;
                bool sending = false;
            };

        public:
            ConfiguredVirtualComPort(UsbDevice& device, infra::Function<void(infra::ConstByteRange data)> dataReceived);

        public:
            DataEndPointOut dataEndPointOut;                                                                            //TICS !INT#002
            DataEndPointIn dataEndPointIn;                                                                              //TICS !INT#002
        };

        infra::Optional<ConfiguredVirtualComPort> configured;
    };
}

#endif
