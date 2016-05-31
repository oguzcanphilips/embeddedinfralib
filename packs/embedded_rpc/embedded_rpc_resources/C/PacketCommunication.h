#ifndef PACKETCOMMUNICATION_H
#define PACKETCOMMUNICATION_H

#include "../PacketCommunicationConfig.h"
#include <stdint.h>

struct PacketCommunicationDefinition;

typedef struct packetCommunicationCallback
{
    uint8_t interfaceId;
    uint32_t registerMask;
    struct packetCommunicationCallback* next;
    void(*Receive)(struct PacketCommunicationDefinition* pc);
} packetCommunicationCallback_t;

typedef struct PacketCommunicationDefinition
{
    void (*ProcessReceive)(    void* self);
    bool (*IsPacketEnded)(     void* self);	
    void (*WriteStartToken)(  void* self);
    void (*WriteEndToken)(         void* self);
    void (*HandleReceiveError)(void* self);
    void (*WriteByte)(         void* self, uint8_t v);
    bool (*ReadByte)(          void* self, uint8_t* v);
#ifdef VALIDATION_CRC
    uint16_t crcWr;
    uint16_t crcRd;
#endif

#ifdef VALIDATION_CHECKSUM
    uint8_t checksumWr;
    uint8_t checksumRd;
#endif
    uint32_t idMask;
    void* self;
    struct PacketCommunicationDefinition* link;
    packetCommunicationCallback_t* callbacks;
} PacketCommunicationDefinition_t;

void CreatePacketCommunication(PacketCommunicationDefinition_t* definition);
void PacketCommunication_ProcessReceive(PacketCommunicationDefinition_t* self);

void PacketCommunication_Register(PacketCommunicationDefinition_t* self, packetCommunicationCallback_t* callback);
void PacketCommunication_Unregister(PacketCommunicationDefinition_t* self, packetCommunicationCallback_t* callback);
void PacketCommunication_Link(PacketCommunicationDefinition_t* self, PacketCommunicationDefinition_t* link);

void PacketCommunication_PacketStart_InterfaceFunction(PacketCommunicationDefinition_t* self, uint8_t interfaceId, uint8_t functionId);
void PacketCommunication_PacketDone_InterfaceFunction(PacketCommunicationDefinition_t* self);
bool PacketCommunication_ReadDone(PacketCommunicationDefinition_t* self);

void PacketCommunication_Write_uint8_t(PacketCommunicationDefinition_t* self, uint8_t v);
void PacketCommunication_Write_uint16_t(PacketCommunicationDefinition_t* self, uint16_t v);
void PacketCommunication_Write_uint32_t(PacketCommunicationDefinition_t* self, uint32_t v);
void PacketCommunication_Write_int8_t(PacketCommunicationDefinition_t* self, int8_t v);
void PacketCommunication_Write_int16_t(PacketCommunicationDefinition_t* self, int16_t v);
void PacketCommunication_Write_int32_t(PacketCommunicationDefinition_t* self, int32_t v);
void PacketCommunication_Write_bool(PacketCommunicationDefinition_t* self, bool v);
void PacketCommunication_Write(PacketCommunicationDefinition_t* self, const uint8_t* data, uint16_t len);

bool PacketCommunication_Read_uint8_t(PacketCommunicationDefinition_t* self, uint8_t* v);
bool PacketCommunication_Read_uint16_t(PacketCommunicationDefinition_t* self, uint16_t* v);
bool PacketCommunication_Read_uint32_t(PacketCommunicationDefinition_t* self, uint32_t* v);
bool PacketCommunication_Read_int8_t(PacketCommunicationDefinition_t* self, int8_t* v);
bool PacketCommunication_Read_int16_t(PacketCommunicationDefinition_t* self, int16_t* v);
bool PacketCommunication_Read_int32_t(PacketCommunicationDefinition_t* self, int32_t* v);
bool PacketCommunication_Read(PacketCommunicationDefinition_t* self, uint8_t* data, uint16_t len);
bool PacketCommunication_Read_bool(PacketCommunicationDefinition_t* self, bool* v);
void PacketCommunication_Receive(PacketCommunicationDefinition_t* self);


#endif
