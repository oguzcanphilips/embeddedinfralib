/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef ITEST_HPP
#define ITEST_HPP

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
namespace erpc
{
class ITest
{
public:
  virtual ~ITest(){}


  virtual void DoThis(const Array& array) = 0;
  virtual uint16_t DoubleOutput(uint16_t i) = 0;
  virtual bool IO_In(uint16_t value) = 0;
  virtual bool IO_Out(uint16_t& value) = 0;
  virtual void IO_InOut(uint16_t& value) = 0;
protected:
};
}

#endif /*ITEST_HPP*/
