#ifndef REPORT_H
#define REPORT_H

#include "PacketCommunication.h"

void ReportInit(PacketCommunicationDefinition_t* pc);
void ReportWrite(uint8_t* v, uint16_t l);
bool ReportRead(uint8_t* v);

#endif