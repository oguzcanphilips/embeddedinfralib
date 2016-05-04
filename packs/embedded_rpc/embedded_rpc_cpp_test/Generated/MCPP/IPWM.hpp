/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
public interface class IPWM
{
public:
  delegate void PwmUpdateDelegate(uint8_t channel, uint16_t value);
  event PwmUpdateDelegate^ PwmUpdateEvent;

  void SetPwm(uint8_t channel, uint16_t value);
  uint16_t GetPwm(uint8_t channel);
  void ResetPwm();
};
