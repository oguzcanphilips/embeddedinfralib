/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef KEYID_H
#define KEYID_H

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
#include "Scope.hpp"

public ref class KeyId: public Serialize
{
public:
  KeyId();

  virtual void Write(PacketCommunication^ packetComm);
  virtual bool Read(PacketCommunication^ packetComm);

  uint32_t id;
  array<uint16_t>^ key; //[10]
  Scope^ scope;
};
#endif
