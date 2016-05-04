/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef IPWMEVENTS_HPP
#define IPWMEVENTS_HPP

#include "PacketCommunication.hpp"
#include "Array.hpp"
#include "PCString.hpp"
namespace erpc
{
class IPWMEvents
{
public:
  virtual ~IPWMEvents(){}


  virtual void PwmUpdate(uint8_t channel, uint16_t value) = 0;
protected:
};
}

#endif /*IPWMEVENTS_HPP*/
