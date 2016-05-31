#ifndef REPORT_H
#define REPORT_H

#include "PacketCommunication.h"

void Report1Init(PacketCommunicationDefinition_t* pc);
void Report1Write(uint8_t* v, uint16_t l);
bool Report1Read(uint8_t* v);

void Report2Init(PacketCommunicationDefinition_t* pc);
void Report2Write(uint8_t* v, uint16_t l);
bool Report2Read(uint8_t* v);

#endif