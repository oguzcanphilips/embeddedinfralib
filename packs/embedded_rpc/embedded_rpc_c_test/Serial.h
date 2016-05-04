#ifndef SERIAL2_H
#define SERIAL2_H

#include "PacketCommunication.h"

void Serial2Init(PacketCommunicationDefinition_t* pc);
void Serial2Write(uint8_t v);
bool Serial2Read(uint8_t* v);

#endif