/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "PacketCommunication.hpp"

namespace erpc
{
class Scope : public Serialize
{
public:
  enum Value
  {
    Low = 0,
    Med = 111,
    High = 112
  };
  Scope() : value((Value)0){}
  Scope(const Value& v) : value(v){}
  operator Value&(){return value;}
  operator Value() const {return value;}

  virtual void Write(PacketCommunication& pc) const
  {
    pc.Write((int32_t)value);
  }
  virtual bool Read(PacketCommunication& pc)
  {
      int32_t v;
      if (!pc.Read(v)) return false;
      value = (Value)v;
      return true;
  }
private:
  Value value;
};
}

#endif /*SCOPE_HPP*/
