/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
public interface class IFoo
{
public:

  void DoThis(Array^ array);
  uint32_t DoThat(uint32_t i);
  PCString^ UpperCase(PCString^ str);
};
