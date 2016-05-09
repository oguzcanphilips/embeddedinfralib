#pragma once

#include "Generated\CPP\IPWM.hpp"
#include "Generated\CPP\PWMProxy.hpp"

class PWMImpl : public erpc::IPWM
{
private:
  uint16_t mPWMs[2];
public:
  void SetPwm(uint8_t channel, uint16_t value)
  {
    mPWMs[channel] = value;
    PwmUpdate(channel, value);
  }

  uint16_t GetPwm(uint8_t channel)
  {
    return mPWMs[channel];
  }

  void ResetPwm(){}
};


class PWMEvents 
{
    uint16_t mChannel[2];
    void PwmUpdate(uint8_t channel, uint16_t value)
    {
      mChannel[channel] = value;
    }
public:
    infra::Slot<PWMEvents, uint8_t, uint16_t> PwmUpdateSlot;
    PWMEvents(erpc::PWMProxy& p) : PwmUpdateSlot(this, &PWMEvents::PwmUpdate)
    {
        p.PwmUpdateSignal += PwmUpdateSlot;
    }
    
    uint16_t GetPwm(uint8_t index)
    {
      return mChannel[index];
    }
};
