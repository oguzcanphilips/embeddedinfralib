#ifndef SERIAL_H
#define SERIAL_H

#include "PacketCommunication.h"

void SerialInit(PacketCommunicationDefinition_t* pc);
void SerialWrite(uint8_t v);
bool SerialRead(uint8_t* v);

#endif