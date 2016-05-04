/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
public interface class IPWMEvents
{
public:

  void PwmUpdate(uint8_t channel, uint16_t value);
};
