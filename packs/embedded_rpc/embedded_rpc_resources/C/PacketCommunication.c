#include "PacketCommunication.h"

#ifdef VALIDATION_CRC
#include "../CRC.h"
#endif

#include <string.h>

static uint32_t idMaskCounter = 1;
static packetCommunicationCallback_t* callbacks = 0;

void PacketCommunication_ResetIdCounterForTesting(void)
{
    idMaskCounter = 1;
}

void CreatePacketCommunication(PacketCommunicationDefinition_t* definition)
{
    memset(definition, sizeof(*definition),0);
    while(idMaskCounter==0)
    {
        // out of Ids, mask overflowed
    }
    definition->idMask = idMaskCounter;
    idMaskCounter <<=1;
#ifdef VALIDATION_CRC
    definition->crcWr = 0;
    definition->crcRd = 0;
#endif

#ifdef VALIDATION_CHECKSUM
    definition->checksumWr = 0;
    definition->checksumRd = 0;
#endif
}

void PacketCommunication_ProcessReceive(PacketCommunicationDefinition_t* self)
{
    self->ProcessReceive(self->self);
}

static void WriteInternal(PacketCommunicationDefinition_t* self, uint8_t b)
{
	if(self == 0) return;
#ifdef VALIDATION_CRC
    CRC_Update(&self->crcWr, b);
#endif
#ifdef VALIDATION_CHECKSUM
    self->checksumWr += b;
#endif
    self->WriteByte(self->self, b);
}

static bool ReadInternal(PacketCommunicationDefinition_t* self, uint8_t* b)
{
	if(self == 0) return false;
    if(!self->ReadByte(self->self, b))
    {
        return false;
    }
#ifdef VALIDATION_CRC
    CRC_Update(&self->crcRd, *b);
#endif
#ifdef VALIDATION_CHECKSUM
    checksumRd += *b;
#endif
    return true;
}

void PacketCommunication_Register(PacketCommunicationDefinition_t* self, packetCommunicationCallback_t* callback)
{
    packetCommunicationCallback_t* it;
	
	if(self == 0) return;
    callback->registerMask |= self->idMask;
    for(it = callbacks; it; it = it->next)
    {
        if(it==callback) 
        {
            return;
        }
    }
    callback->next = callbacks;
    callbacks = callback;
}

void PacketCommunication_Unregister(PacketCommunicationDefinition_t* self, packetCommunicationCallback_t* callback)
{
    packetCommunicationCallback_t* it;
    packetCommunicationCallback_t* itPrev;

	if(self == 0) return;
    callback->registerMask &= ~self->idMask;
    if(callback->registerMask) return;
    if(callbacks == 0) return;
    if(callbacks == callback)
    {
        callbacks = callbacks->next;
        return;
    }
    
    itPrev = callbacks;
    for(it = itPrev->next; it; it = it->next)
    {
        if(it==callback)
        {
            itPrev->next = it->next;
            return;
        }
        itPrev = it;
    }
}

void PacketCommunication_Link(PacketCommunicationDefinition_t* self, PacketCommunicationDefinition_t* link)
{
	if(self == 0 || link == 0) return;
    self->link = link;
    link->link = self;
}

void PacketCommunication_PacketStart_InterfaceFunction(PacketCommunicationDefinition_t* self, uint8_t interfaceId, uint8_t functionId)
{
	if(self == 0) return;
#ifdef VALIDATION_CRC
    self->crcWr = 0;
#endif
#ifdef VALIDATION_CHECKSUM
    self->checksumWr = 0;
#endif
    self->PacketStart(self->self);
    WriteInternal(self, interfaceId);
    WriteInternal(self, functionId);
}

void PacketCommunication_PacketDone_InterfaceFunction(PacketCommunicationDefinition_t* self)
{
	if(self == 0) return;
#ifdef VALIDATION_CRC
    self->WriteByte(self->self, self->crcWr >> 8);
    self->WriteByte(self->self, self->crcWr & 0xff);
#endif
#ifdef VALIDATION_CHECKSUM
    self->WriteByte(self->self, self->checksumWr);
#endif
    self->PacketEnd(self->self);
}

void PacketCommunication_Write_uint8_t(PacketCommunicationDefinition_t* self, uint8_t v)
{
    WriteInternal(self, v);
}

void PacketCommunication_Write_uint16_t(PacketCommunicationDefinition_t* self, uint16_t v)
{
    WriteInternal(self, v >> 8);
    WriteInternal(self, v & 0xff);
}

void PacketCommunication_Write_uint32_t(PacketCommunicationDefinition_t* self, uint32_t v)
{
    WriteInternal(self, v >> 24);
    WriteInternal(self, (v >> 16) & 0xff);
    WriteInternal(self, (v >> 8) & 0xff);
    WriteInternal(self, v & 0xff);
}

void PacketCommunication_Write_int8_t(PacketCommunicationDefinition_t* self, int8_t v)   
{ 
    WriteInternal(self, (uint8_t)v);
}

void PacketCommunication_Write_int16_t(PacketCommunicationDefinition_t* self, int16_t v) 
{ 
    PacketCommunication_Write_uint16_t(self, (uint16_t)v);
}

void PacketCommunication_Write_int32_t(PacketCommunicationDefinition_t* self, int32_t v) 
{ 
    PacketCommunication_Write_uint32_t(self, (uint32_t)v);
}

void PacketCommunication_Write_bool(PacketCommunicationDefinition_t* self, bool v)
{ 
    WriteInternal(self, (uint8_t)(v ? 1 : 0));
}

void PacketCommunication_Write(PacketCommunicationDefinition_t* self, const uint8_t* data, uint16_t len)
{
    while(len)
    {
        WriteInternal(self, *data);
        data++;
        len--;
    }
}

bool PacketCommunication_Read_uint8_t(PacketCommunicationDefinition_t* self, uint8_t* v)
{
    return ReadInternal(self, v);
}

bool PacketCommunication_Read_uint16_t(PacketCommunicationDefinition_t* self, uint16_t* v)
{
    uint8_t c;
    if(!ReadInternal(self, &c)) return false;
    *v = c;
    *v <<= 8;
    if(!ReadInternal(self, &c)) return false;
    *v |= c;
    return true;
}

bool PacketCommunication_Read_uint32_t(PacketCommunicationDefinition_t* self, uint32_t* v)
{
    uint8_t c;
    if(!ReadInternal(self, &c)) return false;
    *v = c;
    *v <<= 8;
    if(!ReadInternal(self, &c)) return false;
    *v |= c;
    *v <<= 8;
    if(!ReadInternal(self, &c)) return false;
    *v |= c;
    *v <<= 8;
    if(!ReadInternal(self, &c)) return false;
    *v |= c;
    return true;
}

bool PacketCommunication_Read(PacketCommunicationDefinition_t* self, uint8_t* data, uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;++i)
    {
        if(!ReadInternal(self, data+i)) return false;
    }
    return true;
}

bool PacketCommunication_Read_bool(PacketCommunicationDefinition_t* self, bool* v)
{
    uint8_t c;
    if(!ReadInternal(self, &c)) return false;
    *v = c ? true : false;
    return true;
}

bool PacketCommunication_Read_int8_t(PacketCommunicationDefinition_t* self, int8_t* v)      
{ 
    return ReadInternal(self, (uint8_t*)v);
}

bool PacketCommunication_Read_int16_t(PacketCommunicationDefinition_t* self, int16_t* v)   
{ 
    return PacketCommunication_Read_uint16_t(self, (uint16_t*)v);
}

bool PacketCommunication_Read_int32_t(PacketCommunicationDefinition_t* self, int32_t* v)   
{
    return PacketCommunication_Read_uint32_t(self, (uint32_t*)v);
}


void PacketCommunication_Receive(PacketCommunicationDefinition_t* self)
{
    uint8_t interfaceId;
    packetCommunicationCallback_t* it;
	if(self == 0) return;
#ifdef VALIDATION_CRC
    self->crcRd = 0;
#endif
#ifdef VALIDATION_CHECKSUM
    checksumRd = 0;
#endif
    if(!ReadInternal(self, &interfaceId)) return;
    for(it = callbacks; it; it = it->next)
    {
        if(it->interfaceId == interfaceId) 
        {
            if(it->registerMask & self->idMask)
            {
                it->Receive(self);
                return;
            }
        }
    }
    // not handled, forward to link
    if(self->link)
    {
        PacketCommunicationDefinition_t* linked = self->link;
        uint8_t data;

        linked->PacketStart(linked);
        linked->WriteByte(linked, interfaceId);
        while(self->ReadByte(self, &data))
        {
            linked->WriteByte(linked, data);
        }
        linked->PacketEnd(linked);
    }
}

bool PacketCommunication_ReadDone(PacketCommunicationDefinition_t* self)
{
#if defined(VALIDATION_CRC)
    uint8_t dummy;
	if(self == 0) return false;
    if(!ReadInternal(self, &dummy) || !ReadInternal(self, &dummy)) return false;
    return self->IsPacketEnded(self) && (self->crcRd == 0);
#elif defined(VALIDATION_CHECKSUM)
    uint8_t checksum;
	if(self == 0) return false;
    if(!self->ReadByte(self->self, &checksum)) return false;
    return self->IsPacketEnded(self) && (checksumRd == checksum);
#else
    return self->IsPacketEnded(self);
#endif
}

