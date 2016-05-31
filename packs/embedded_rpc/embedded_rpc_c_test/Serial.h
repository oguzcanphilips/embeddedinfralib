#ifndef SERIAL_H
#define SERIAL_H

#include "PacketCommunication.h"

void Serial1Init(PacketCommunicationDefinition_t* pc);
void Serial1Write(uint8_t v);
bool Serial1Read(uint8_t* v);

void Serial2Init(PacketCommunicationDefinition_t* pc);
void Serial2Write(uint8_t v);
bool Serial2Read(uint8_t* v);

#endif