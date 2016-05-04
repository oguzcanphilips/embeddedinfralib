/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#include "KeyId.hpp"

KeyId::KeyId()
: id(uint32_t())
, scope(gcnew Scope())
{
  {
    key = gcnew array<uint16_t>(10);
    for(uint32_t i=0;i<10;++i) key[i] = uint16_t();
  }
}

void KeyId::Write(PacketCommunication^ packetComm)
{
  packetComm->Write(id);
  {
    for(uint32_t i=0;i<10;++i) packetComm->Write(key[i]);
  }
  packetComm->Write(scope);
}

bool KeyId::Read(PacketCommunication^ packetComm)
{
  if(!packetComm->Read(id)) return false;
  {
    for(uint32_t i=0;i<10;++i) if(!packetComm->Read(key[i])) return false;
  }
  if(!packetComm->Read(scope)) return false;
  return true;
}
