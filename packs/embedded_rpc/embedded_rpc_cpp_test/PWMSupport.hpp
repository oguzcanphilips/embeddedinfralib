#pragma once

#include "Generated\CPP\IPWM.hpp"
#include "Generated\CPP\PWMProxy.hpp"

class PWMImpl : public erpc::IPWM
{
private:
  uint16_t mPWMs[2];
  erpc::PCString name;
public:
  void SetPwm(const erpc::Channel& channel, uint16_t value) override
  {
    mPWMs[channel] = value;
    PwmUpdate(channel, value);
  }

  uint16_t GetPwm(const erpc::Channel&  channel) override
  {
    return mPWMs[channel];
  }

  void ResetPwm() override
  {}

  void SetName(const erpc::PCString& name) override
  {
      this->name = name;
  }
  erpc::PCString GetName() override
  {
      return name;
  }
};


class PWMEvents 
{
    uint16_t mChannel[2];
    void PwmUpdate(const erpc::Channel& channel, uint16_t value)
    {
      mChannel[channel] = value;
    }
public:
    infra::Slot<PWMEvents, const erpc::Channel&, uint16_t> PwmUpdateSlot;
    PWMEvents(erpc::PWMProxy& p) : PwmUpdateSlot(this, &PWMEvents::PwmUpdate)
    {
        p.PwmUpdateSignal += PwmUpdateSlot;
    }
    
    uint16_t GetPwm(const erpc::Channel& channel)
    {
      return mChannel[channel];
    }
};
