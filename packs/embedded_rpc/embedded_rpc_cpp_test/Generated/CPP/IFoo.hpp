/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef IFOO_HPP
#define IFOO_HPP

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
namespace erpc
{
class IFoo
{
public:
  virtual ~IFoo(){}


  virtual void DoThis(const Array& array) = 0;
  virtual uint32_t DoThat(uint32_t i) = 0;
  virtual PCString UpperCase(const PCString& str) = 0;
protected:
};
}

#endif /*IFOO_HPP*/
