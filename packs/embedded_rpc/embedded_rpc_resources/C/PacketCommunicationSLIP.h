#ifndef PACKETCOMMUNICATIONSLIP_H
#define PACKETCOMMUNICATIONSLIP_H

/* RFC 1055 */
#include "PacketCommunication.h"

typedef struct PacketCommunicationSlip
{
    PacketCommunicationDefinition_t packetCommunication;
    void (*SerialSend)(uint8_t v);
    bool (*SerialReceive)(uint8_t* v);
} PacketCommunicationSlip_t;

void CreatePacketCommunicationSlip(PacketCommunicationSlip_t* definition, void (*SerialSend)(uint8_t v), bool (*SerialReceive)(uint8_t* v));

#endif