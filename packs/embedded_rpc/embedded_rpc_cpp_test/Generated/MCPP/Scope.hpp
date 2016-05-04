/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef SCOPE_H
#define SCOPE_H

#include "PacketCommunication.hpp"

public ref class Scope : public Serialize
{
public:
  enum class Value
  {
    Low = 0,
    Med = 111,
    High = 112
  };

  Value value;
  virtual void Write(PacketCommunication^ pc)
  {
    pc->Write((int32_t)value);
  }
  virtual bool Read(PacketCommunication^ pc)
  {
      int32_t v;
      if (!pc->Read(v)) return false;
      value = (Value)v;
      return true;
  }
};
#endif
