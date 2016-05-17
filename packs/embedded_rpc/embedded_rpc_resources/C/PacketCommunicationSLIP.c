#include "PacketCommunicationSLIP.h"
#include "../PacketCommunicationDefines.h"
#include <string.h>

static uint8_t ReadInternal(void* self)
{
  uint8_t r = SLIP_END;
  PacketCommunicationSlip_t* me = (PacketCommunicationSlip_t*)self;
  if(me->SerialReceive(&r))
  {
    return r;
  }
  return SLIP_END;
}

static void ProcessReceive(void* self)
{
    PacketCommunicationSlip_t* me = (PacketCommunicationSlip_t*)self;
	uint8_t v;
	do
	{
    	v = ReadInternal(self);
  	}
  	while(v!=SLIP_BEGIN && v!=SLIP_END);
  	if(v==SLIP_BEGIN) PacketCommunication_Receive(&me->packetCommunication);
}

static bool IsPacketEnded(void* self)
{
    return ReadInternal(self) == SLIP_END;
}

static void PackedEndToken(void* self)
{
    PacketCommunicationSlip_t* me = (PacketCommunicationSlip_t*)self;
    me->SerialSend(SLIP_END);
}

static void WriteByte(void* self, uint8_t data)
{
    PacketCommunicationSlip_t* me = (PacketCommunicationSlip_t*)self;
	switch(data)
    {
    case SLIP_BEGIN: me->SerialSend(SLIP_ESC); me->SerialSend(SLIP_ESC_BEGIN); break;
    case SLIP_END:   me->SerialSend(SLIP_ESC); me->SerialSend(SLIP_ESC_END);   break;
    case SLIP_ESC:   me->SerialSend(SLIP_ESC); me->SerialSend(SLIP_ESC_ESC);   break;
    default:    me->SerialSend(data);  break;
    }
}

static bool ReadByte(void* self, uint8_t* v)
{
  *v = ReadInternal(self);
  if(SLIP_END == *v || SLIP_BEGIN == *v) 
  {
    return false;
  }
  if(SLIP_ESC == *v)
  {
    switch(ReadInternal(self)) 
    {
    case SLIP_ESC_BEGIN: *v = SLIP_BEGIN; break;
    case SLIP_ESC_END:   *v = SLIP_END;   break;
    case SLIP_ESC_ESC:   *v = SLIP_ESC;   break;
    default:
      return false;
    }
  }
  return true;
}

static void PacketStartToken(void* self)
{
    PacketCommunicationSlip_t* me = (PacketCommunicationSlip_t*)self;
    me->SerialSend(SLIP_BEGIN);
}

static void HandleReceiveError(void* self)
{
    self = self;
}

void CreatePacketCommunicationSlip(PacketCommunicationSlip_t* definition, void (*serialSend)(uint8_t v), bool (*serialReceive)(uint8_t* v))
{
    memset(definition, sizeof(*definition),0);

    CreatePacketCommunication(&definition->packetCommunication);

    definition->SerialSend = serialSend;
    definition->SerialReceive = serialReceive;
    definition->packetCommunication.self = definition;

    definition->packetCommunication.ProcessReceive = ProcessReceive;
	definition->packetCommunication.IsPacketEnded = IsPacketEnded;
    definition->packetCommunication.PacketStartToken = PacketStartToken;
    definition->packetCommunication.PackedEndToken = PackedEndToken;
    definition->packetCommunication.HandleReceiveError = HandleReceiveError;
    definition->packetCommunication.WriteByte = WriteByte;
    definition->packetCommunication.ReadByte = ReadByte;
}