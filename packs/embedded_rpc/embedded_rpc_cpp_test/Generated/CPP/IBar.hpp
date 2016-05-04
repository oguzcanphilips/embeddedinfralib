/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef IBAR_HPP
#define IBAR_HPP

#include "PacketCommunication.hpp"
#include "KeyId.hpp"
#include "Array.hpp"
#include "PCString.hpp"
namespace erpc
{
class IBar
{
public:
  virtual ~IBar(){}


  virtual KeyId GetKeyId() = 0;
  virtual void SetKeyId(const KeyId& kid) = 0;
  virtual void SetGetKeyId(KeyId& kid) = 0;
protected:
};
}

#endif /*IBAR_HPP*/
