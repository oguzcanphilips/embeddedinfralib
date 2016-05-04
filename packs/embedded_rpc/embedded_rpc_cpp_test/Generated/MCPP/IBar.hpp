/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#pragma once

#include "PacketCommunication.hpp"
#include "KeyId.hpp"
#include "Array.hpp"
#include "PCString.hpp"
public interface class IBar
{
public:

  KeyId^ GetKeyId();
  void SetKeyId(KeyId^ kid);
  void SetGetKeyId(KeyId^ kid);
};
