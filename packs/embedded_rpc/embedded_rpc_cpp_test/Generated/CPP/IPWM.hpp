/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef IPWM_HPP
#define IPWM_HPP

#include "PacketCommunication.hpp"
#include SIGNAL_SLOT_HEADER
#include "Array.hpp"
#include "PCString.hpp"
namespace erpc
{
class IPWM
{
public:
  virtual ~IPWM(){}

  SIGNAL<IPWM, uint8_t/*channel*/, uint16_t/*value*/> PwmUpdateSignal;

  virtual void SetPwm(uint8_t channel, uint16_t value) = 0;
  virtual uint16_t GetPwm(uint8_t channel) = 0;
  virtual void ResetPwm() = 0;
protected:
  void PwmUpdate(uint8_t channel, uint16_t value){PwmUpdateSignal(channel, value);}
};
}

#endif /*IPWM_HPP*/
