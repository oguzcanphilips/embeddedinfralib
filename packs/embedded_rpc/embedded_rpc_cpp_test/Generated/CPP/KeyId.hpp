/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef KEYID_HPP
#define KEYID_HPP

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
#include "Scope.hpp"

namespace erpc
{
class KeyId: public Serialize
{
public:
  KeyId();

  void Write(PacketCommunication& packetComm) const;
  bool Read(PacketCommunication& packetComm);

  uint32_t id;
  uint16_t key[10];
  Scope scope;
};
}

#endif /*KEYID_HPP*/
