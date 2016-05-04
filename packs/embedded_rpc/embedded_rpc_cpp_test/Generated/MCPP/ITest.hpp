/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
public interface class ITest
{
public:

  void DoThis(Array^ array);
  uint16_t DoubleOutput(uint16_t i);
  bool IO_In(uint16_t value);
  bool IO_Out(uint16_t% value);
  void IO_InOut(uint16_t% value);
};
