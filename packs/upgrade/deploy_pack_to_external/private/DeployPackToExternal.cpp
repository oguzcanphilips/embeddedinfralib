#include "packs/upgrade/deploy_pack_to_external/public/DeployPackToExternal.hpp"

namespace application
{
    DeployPackToExternal::DeployPackToExternal(hal::Flash& from_, hal::Flash& to_, hal::GpioPin& statusLedPin)
        : from(from_)
        , to(to_)
        , statusLed(statusLedPin, std::chrono::milliseconds(100))
    {
        statusLed.Set(0x01, 16);
        sequencer.Load([this]() {
            sequencer.Step([this]() { from.ReadBuffer(infra::MakeByteRange(header), 0, [this]() { sequencer.Continue(); }); });
            sequencer.Execute([this]() { sizeToDo = header.signedContentsLength + sizeof(UpgradePackHeaderPrologue) + header.signatureLength; });
            sequencer.If([this]() { return header.status != UpgradePackStatus::readyToDeploy; });
                sequencer.Execute([this]() { statusLed.Set(0xff, 16); });
            sequencer.ElseIf([this]() { return sizeToDo <= from.TotalSize(); });
                sequencer.Step([this]() { to.EraseAll([this]() { sequencer.Continue(); }); });
                sequencer.While([this]() { return sizeToDo != 0; });
                    sequencer.Step([this]()
                    {
                        currentSize = std::min(buffer.size(), sizeToDo);
                        from.ReadBuffer(buffer, currentReadAddress, [this]() { sequencer.Continue(); });
                    });
                    sequencer.Step([this]()
                    {
                        to.WriteBuffer(buffer, currentWriteAddress, [this]() { sequencer.Continue(); });
                    });
                    sequencer.Execute([this]()
                    {
                        currentReadAddress += currentSize;
                        currentWriteAddress += currentSize;
                        sizeToDo -= currentSize;
                    });
                sequencer.EndWhile();
                sequencer.Step([this]()
                {
                    static const uint8_t statusDeployed = 0xf8;
                    from.WriteBuffer(infra::MakeByteRange(statusDeployed), 0, [this]() { sequencer.Continue(); });
                });
                sequencer.Execute([this]() { statusLed.Set(5, 16); });
            sequencer.Else();
                sequencer.Execute([this]() { statusLed.Set(5, 4); });
            sequencer.EndIf();
        });
    }
}
