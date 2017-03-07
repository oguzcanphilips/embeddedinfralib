#ifndef SERVICES_USB_DEVICE_HPP
#define SERVICES_USB_DEVICE_HPP

#include "hal/interfaces/public/UsbLinkLayer.hpp"
#include "services/usb/public/UsbInterface.hpp"

#ifdef _MSC_VER                                                                                                         //TICS !POR#021
#define alignas(x)
#endif

namespace services
{
    static const uint8_t usbRequestGetStatus = 0;
    static const uint8_t usbRequestClearFeature = 1;
    static const uint8_t usbRequestSetFeature = 3;
    static const uint8_t usbRequestSetAddress = 5;
    static const uint8_t usbRequestGetDescriptor = 6;
    static const uint8_t usbRequestSetDescriptor = 7;
    static const uint8_t usbRequestGetConfiguration = 8;
    static const uint8_t usbRequestSetConfiguration = 9;
    static const uint8_t usbRequestGetInterface = 10;
    static const uint8_t usbRequestSetInterface = 11;
    static const uint8_t usbRequestSynchFrame = 12;

    static const uint8_t usbDescriptorTypeDevice = 1;
    static const uint8_t usbDescriptorTypeConfiguration = 2;
    static const uint8_t usbDescriptorTypeString = 3;
    static const uint8_t usbDescriptorTypeInterface = 4;
    static const uint8_t usbDescriptorTypeEndPoint = 5;
    static const uint8_t usbDescriptorTypeDeviceQualifier = 6;
    static const uint8_t usbDescriptorTypeOtherSpeedConfiguration = 7;

    static const std::size_t fullSpeedMaxPacketSize = 64;

    #define USB_STRING(name, string)                                                                                                         \
        STRING_PROVIDER(name ## StringProvider, string);                                                                                     \
        auto const name ## Storage = services::ExplodeImpl<name ## StringProvider, services::ConstStrLen(name ## StringProvider::String())>::Result(); \
        infra::ConstByteRange name = infra::MakeByteRange(name ## Storage)

    struct UsbConfig
    {
        uint8_t deviceClass;
        uint8_t deviceSubClass;
        uint8_t deviceProtocol;
        
        infra::ConstByteRange manufacturerString;
        infra::ConstByteRange productString;
        infra::ConstByteRange serialNumber;
        infra::ConstByteRange configuration;
        infra::ConstByteRange interfaceString;

        uint16_t vid;
        uint16_t pid;
        uint16_t release;
    };

    class UsbDevice
        : public hal::UsbLinkLayerObserver
    {
    public:
        struct EndPoint
        {
            alignas(4) uint16_t status;
            uint32_t totalLength;
            uint32_t remainingLength;
            uint32_t maxpacket;
        };

    public:
        UsbDevice(hal::UsbLinkLayer& linkLayer, const UsbConfig& config, bool selfPowered = true);
        ~UsbDevice();

        hal::UsbLinkLayer& LinkLayer();
        void RegisterInterface(UsbInterface& interface);

        void SetClassConfig(uint8_t configIndex);
        void ClearClassConfig(uint8_t configIndex);

        virtual void SetupStage(infra::ConstByteRange setup) override;
        virtual void DataOutStage(uint8_t epnum, infra::ConstByteRange data) override;
        virtual void DataInStage(uint8_t epnum, infra::ConstByteRange data) override;

        virtual void Suspend() override;
        virtual void Resume() override;

        virtual void StartOfFrame() override;
        virtual void IsochronousInIncomplete(uint8_t epnum) override;
        virtual void IsochronousOutIncomplete(uint8_t epnum) override;

        void ControlSendData(infra::ConstByteRange data);
        void ControlPrepareReceive(infra::ByteRange data);
        void ControlError();

        bool Configured() const;

    private:
        void StandardDeviceRequest(USBD_SetupReqTypedef* req);
        void StandardInterfaceRequest(USBD_SetupReqTypedef* req);
        void StandardEndPointRequest(USBD_SetupReqTypedef* req);

        void GetDescriptor(uint16_t value, uint16_t length);
        void SetAddress(uint16_t value, uint16_t index, uint16_t length);
        void SetConfig(uint16_t value);
        void GetConfig(uint16_t length);
        void GetStatus();
        void SetFeature(uint16_t value);
        void ClearFeature(uint16_t value);

        USBD_SetupReqTypedef ParseSetupRequest(infra::ConstByteRange setup) const;

        void ControlContinueSendData(infra::ConstByteRange data);
        void ControlContinueReceive(infra::ConstByteRange data);
        void ControlSendStatus();
        void ControlReceiveStatus();

        infra::ByteRange GetConfigurationDescriptor();

    private:
        hal::UsbLinkLayer& linkLayer;
        bool selfPowered;

        const UsbConfig config;

        alignas(4) std::array<uint8_t, 18> deviceDescriptor;

        alignas(4) uint8_t dev_config;
        alignas(4) uint8_t dev_default_config;
        alignas(4) uint16_t dev_config_status;
        hal::UsbSpeed deviceSpeed = hal::UsbSpeed::full;
        std::array<EndPoint, 15> endPointIn;
        std::array<EndPoint, 15> endPointOut;

        enum class EndPointState: uint8_t
        {
            idle,
            setup,
            dataIn,
            dataOut,
            statusIn,
            statusOut,
            stall
        };

        EndPointState ep0_state;
        uint32_t ep0_data_len;

        enum class DeviceState: uint8_t
        {
            defaultState,
            addressed,
            configured,
            suspended
        };

        DeviceState deviceState = DeviceState::defaultState;
        DeviceState oldDeviceState = DeviceState::defaultState;
        bool remoteWakeUp = false;

        alignas(4) std::array<uint8_t, 512> configurationBuffer;

        infra::IntrusiveForwardList<UsbInterface> interfaces;
    };

    ////    Implementation    ////

    template<uint16_t... Characters>
    struct alignas(4) UsbString
    {
        uint8_t length = sizeof...(Characters)* 2 + 2;
        uint8_t type = usbDescriptorTypeString;
        std::array<char16_t, sizeof...(Characters)> data = { { Characters... } };
    };

    inline constexpr uint8_t ConstStrLen(const char16_t* string, uint8_t count = 0)
    {
        return ('\0' == string[0]) ? count : ConstStrLen(string + 1, count + 1);
    }

    template<class StringProvider, uint8_t Size, char16_t... Characters>
    struct ExplodeImpl
    {
        using Result = typename ExplodeImpl<StringProvider, Size - 1, StringProvider::String()[Size - 1], Characters...>::Result;
    };

    template<class StringProvider, char16_t... Characters>
    struct ExplodeImpl<StringProvider, 0, Characters...>
    {
        using Result = UsbString<Characters...>;
    };

    #define STRING_PROVIDER(name, string) struct name { constexpr static const char16_t* String() { return string; } }; //TICS !PRE#004
}

#endif
