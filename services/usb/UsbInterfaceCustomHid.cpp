#include "services/usb/UsbInterfaceCustomHid.hpp"
#include "infra/event/EventDispatcher.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace services
{
    namespace
    {
        static const std::array<uint8_t, 38> customHidReportDescriptor =
        {
            0x06, 0xFF, 0xFF, // 04|2   , Usage Page (vendor defined?)
            0x09, 0x01,        // 08|1   , Usage      (vendor defined
            0xA1, 0x01,        // A0|1   , Collection (Application)
            // IN report
            0x09, 0x02,        // 08|1   , Usage      (vendor defined)
            0x09, 0x03,        // 08|1   , Usage      (vendor defined)
            0x15, 0x00,        // 14|1   , Logical Minimum(0 for signed byte?)
            0x26, 0xFF, 0x00,//24|1   , Logical Maximum(255 for signed byte?)
            0x75, 0x08,        // 74|1   , Report Size(8) = field size in bits = 1 byte
            // 94|1   , ReportCount(size) = repeat count of previous item
            0x95, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE,
            0x81, 0x02,        // 80|1   , IN report (Data,Variable, Absolute)
            // OUT report
            0x09, 0x04,        // 08|1   , Usage      (vendor defined)
            0x09, 0x05,        // 08|1   , Usage      (vendor defined)
            0x15, 0x00,        // 14|1   , Logical Minimum(0 for signed byte?)
            0x26, 0xFF, 0x00,//24|1   , Logical Maximum(255 for signed byte?)
            0x75, 0x08,        // 74|1   , Report Size(8) = field size in bits = 1 byte
            // 94|1   , ReportCount(size) = repeat count of previous item
            0x95, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE,
            0x91, 0x02,     // 90|1   , OUT report (Data,Variable, Absolute)
            0xC0            // C0|0   , End Collection
        };

        static const std::array<uint8_t, 9> interfaceDescriptor =
        {
            0x09, /*bLength: Interface Descriptor size*/
            usbDescriptorTypeInterface,/*bDescriptorType: Interface descriptor type*/
            0x00, /*bInterfaceNumber: Number of Interface*/
            0x00, /*bAlternateSetting: Alternate setting*/
            0x02, /*bNumEndpoints*/
            0x03, /*bInterfaceClass: HID*/
            0x00, /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
            0x00, /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
            0, /*iInterface: Index of string descriptor*/
        };

        static const std::array<uint8_t, 9> customHidDescriptor =
        {
            0x09, /*bLength: CUSTOM_HID Descriptor size*/
            CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
            0x10, /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number ?? 0x11??*/
            0x01, 0x00, /*bCountryCode: Hardware target country*/
            0x01, /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
            0x22, /*bDescriptorType*/
            sizeof(customHidReportDescriptor),/*wItemLength: Total length of Report descriptor*/
            0x00,
        };

        static const std::array<uint8_t, 14> endPointDescriptors =
        {
            0x07, /*bLength: Endpoint Descriptor size*/
            usbDescriptorTypeEndPoint, /*bDescriptorType:*/
            CUSTOM_HID_EPIN_ADDR, /*bEndpointAddress: Endpoint Address (IN)*/
            0x03, /*bmAttributes: Interrupt endpoint*/
            CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
            0x00, 0x01, /*bInterval: Polling Interval (1 ms)*/

            0x07, /* bLength: Endpoint Descriptor size */
            usbDescriptorTypeEndPoint, /* bDescriptorType: */
            CUSTOM_HID_EPOUT_ADDR, /*bEndpointAddress: Endpoint Address (OUT)*/
            0x03, /* bmAttributes: Interrupt endpoint */
            CUSTOM_HID_EPOUT_SIZE, /* wMaxPacketSize: 2 Bytes max  */
            0x00, 0x01, /* bInterval: Polling Interval (1 ms) */
        };
    }

    UsbInterfaceCustomHid::UsbInterfaceCustomHid(UsbDevice& device)
        : onReportReceived([](infra::ConstByteRange) {})
        , device(device)
    {
        device.RegisterInterface(*this);
    }

    void UsbInterfaceCustomHid::Send(infra::ConstByteRange data)
    {
        SendReport(infra::ConstByteRange(data.begin(), data.begin() + ReportSize));
    }

    void UsbInterfaceCustomHid::OnReceived(infra::Function<void(infra::ConstByteRange)> onReportReceived)
    {
        this->onReportReceived = onReportReceived;
    }

    void UsbInterfaceCustomHid::Configured(uint8_t configIndex)
    {
        device.LinkLayer().OpenEndPoint(CUSTOM_HID_EPIN_ADDR, hal::UsbEndPointType::interrupt, CUSTOM_HID_EPIN_SIZE);
        device.LinkLayer().OpenEndPoint(CUSTOM_HID_EPOUT_ADDR, hal::UsbEndPointType::interrupt, CUSTOM_HID_EPOUT_SIZE);

        state = CUSTOM_HID_IDLE;

        device.LinkLayer().PrepareReceive(CUSTOM_HID_EPOUT_ADDR, Report_buf);
    }

    void UsbInterfaceCustomHid::Unconfigured(uint8_t configIndex)
    {
        device.LinkLayer().CloseEndPoint(CUSTOM_HID_EPIN_ADDR);
        device.LinkLayer().CloseEndPoint(CUSTOM_HID_EPOUT_ADDR);
    }

    void UsbInterfaceCustomHid::Setup(USBD_SetupReqTypedef *req)
    {
        switch (req->bmRequest & usbRequestTypeMask)
        {
            case usbRequestTypeClass:
                switch (req->bRequest)
                {

                case CUSTOM_HID_REQ_SET_PROTOCOL:
                    Protocol = static_cast<uint8_t>(req->wValue);
                    break;

                case CUSTOM_HID_REQ_GET_PROTOCOL:
                    device.ControlSendData(infra::MakeByteRange(Protocol));
                    break;

                case CUSTOM_HID_REQ_SET_IDLE:
                    IdleState = static_cast<uint8_t>(req->wValue >> 8);
                    break;

                case CUSTOM_HID_REQ_GET_IDLE:
                    device.ControlSendData(infra::MakeByteRange(IdleState));
                    break;

                case CUSTOM_HID_REQ_SET_REPORT:
                    IsReportAvailable = 1;
                    device.ControlPrepareReceive(infra::ByteRange(Report_buf.data(), Report_buf.data() + static_cast<uint8_t>(req->wLength)));

                    break;
                default:
                    device.ControlError();
                    break;
                }
                break;

            case usbRequestTypeStandard:
                switch (req->bRequest)
                {
                    case usbRequestGetDescriptor:
                    {
                        infra::ConstByteRange data;

                        if (req->wValue >> 8 == CUSTOM_HID_REPORT_DESC)
                            data = infra::MakeByteRange(customHidReportDescriptor);
                        else if (req->wValue >> 8 == CUSTOM_HID_DESCRIPTOR_TYPE)
                            data = infra::MakeByteRange(customHidDescriptor);

                        data.shrink_from_back_to(req->wLength);
                        device.ControlSendData(data);
                        break;
                    }
                    case usbRequestGetInterface:
                        device.ControlSendData(infra::MakeByteRange(AltSetting));
                        break;

                    case usbRequestSetInterface:
                        AltSetting = static_cast<uint8_t>(req->wValue);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    void UsbInterfaceCustomHid::DataIn(uint8_t epnum)
    {
        /* Ensure that the FIFO is empty before a new transfer, this condition could
        be caused by  a new transfer before the end of the previous transfer */
        //USBD_LL_FlushEP(pdev, epnum);
        state = CUSTOM_HID_IDLE;
    }

    void UsbInterfaceCustomHid::DataOut(uint8_t epnum)
    {
        if (epnum == CUSTOM_HID_EPOUT_ADDR)
        {
            /* Get the received data buffer and update the counter */
            /* USB data will be immediately processed, this allow next USB traffic being NAKed till the end of the application Xfer */
            if (device.Configured())
            {
                /* USB data will be immediately processed, this allows next USB traffic to be NAKed till the end of the application Xfer */
                OutEvent(Report_buf.data());
            }
        }
    }

    uint8_t UsbInterfaceCustomHid::NumberOfInterfaces() const
    {
        return 1;
    }

    void UsbInterfaceCustomHid::AddConfigDescriptor(infra::DataOutputStream& stream) const
    {
        stream << interfaceDescriptor << customHidDescriptor << endPointDescriptors;
    }

    void UsbInterfaceCustomHid::SendReport(infra::ConstByteRange report)
    {
        if (device.Configured())
        {
            state = CUSTOM_HID_BUSY;
            device.LinkLayer().Transmit(CUSTOM_HID_EPIN_ADDR, report);

            while (state != CUSTOM_HID_IDLE)
            {}
        }
    }

    void UsbInterfaceCustomHid::PrepareRx(uint16_t size)
    {
        device.LinkLayer().PrepareReceive(CUSTOM_HID_EPOUT_ADDR, infra::ByteRange(Report_buf.data(), Report_buf.data() + size));
    }

    void UsbInterfaceCustomHid::OutEvent(uint8_t* data)
    {
        std::copy(data, data + ReportSize, receiveBuffer.begin());

        infra::EventDispatcher::Instance().Schedule([this]()
        {
            onReportReceived(receiveBuffer);
            PrepareRx(USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
        });
    }
}
