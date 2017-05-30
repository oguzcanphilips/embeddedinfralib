#ifndef SERVICES_USB_INTERFACE_CUSTOM_HID_HPP
#define SERVICES_USB_INTERFACE_CUSTOM_HID_HPP

#include "hal/interfaces/UsbCustomHid.hpp"
#include "services/usb/UsbInterface.hpp"
#include "services/usb/UsbDevice.hpp"
#include "infra/util/ByteRange.hpp"
#include <cstdint>

namespace services
{
    #define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE    64

    #define CUSTOM_HID_EPIN_ADDR                 0x81
    #define CUSTOM_HID_EPIN_SIZE                 USBD_CUSTOMHID_OUTREPORT_BUF_SIZE

    #define CUSTOM_HID_EPOUT_ADDR                0x01
    #define CUSTOM_HID_EPOUT_SIZE                USBD_CUSTOMHID_OUTREPORT_BUF_SIZE

    #define USB_CUSTOM_HID_DESC_SIZ              9

    #define CUSTOM_HID_DESCRIPTOR_TYPE           0x21
    #define CUSTOM_HID_REPORT_DESC               0x22

    #define CUSTOM_HID_REQ_SET_PROTOCOL          0x0B
    #define CUSTOM_HID_REQ_GET_PROTOCOL          0x03

    #define CUSTOM_HID_REQ_SET_IDLE              0x0A
    #define CUSTOM_HID_REQ_GET_IDLE              0x02

    #define CUSTOM_HID_REQ_SET_REPORT            0x09
    #define CUSTOM_HID_REQ_GET_REPORT            0x01

    enum CUSTOM_HID_StateTypeDef
    {
        CUSTOM_HID_IDLE = 0,
        CUSTOM_HID_BUSY,
    };

    class UsbInterfaceCustomHid
        : public UsbInterface
        , public hal::UsbCustomHid
    {
    public:
        explicit UsbInterfaceCustomHid(UsbDevice& device);

        virtual void Send(infra::ConstByteRange data) override;
        virtual void OnReceived(infra::Function<void(infra::ConstByteRange)> onReportReceived) override;

        virtual void Configured(uint8_t configIndex) override;
        virtual void Unconfigured(uint8_t configIndex) override;
        virtual void Setup(USBD_SetupReqTypedef  *req) override;
        virtual void EP0_TxSent() override {}
        virtual void EP0_RxReady() override {}
        virtual void DataIn(uint8_t epnum) override;
        virtual void DataOut(uint8_t epnum) override;
        virtual void StartOfFrame() override {}
        virtual void IsoINIncomplete(uint8_t epnum) override {}
        virtual void IsoOUTIncomplete(uint8_t epnum) override {}

        virtual uint8_t NumberOfInterfaces() const override;
        virtual void AddConfigDescriptor(infra::DataOutputStream& stream) const override;

        void SendReport(infra::ConstByteRange report);
        void PrepareRx(uint16_t size);

    protected:
        void OutEvent(uint8_t* data);

    protected:
        infra::Function<void(infra::ConstByteRange)> onReportReceived;                                                  //TICS !INT#002
        std::array<uint8_t, ReportSize> receiveBuffer;                                                                  //TICS !INT#002

    private:
        UsbDevice& device;

        std::array<uint8_t, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE> Report_buf;
        alignas(4) uint8_t Protocol;
        alignas(4) uint8_t IdleState;
        alignas(4) uint8_t AltSetting;
        uint32_t IsReportAvailable;
        volatile CUSTOM_HID_StateTypeDef state = CUSTOM_HID_IDLE;
    };
}

#endif
