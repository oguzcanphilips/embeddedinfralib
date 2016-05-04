/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : InterfaceCodeGen.exe

***************************************************************************/

#ifndef TEST_H
#define TEST_H

#include "PacketCommunication.h"
#include "Array.h"
#include "PCString.h"

/* These functions are only valid for the proxy */
void Test_Init(PacketCommunicationDefinition_t* pc);
void Test_DeInit(PacketCommunicationDefinition_t* pc);


void Test_DoThis(Array* array);
uint16_t Test_DoubleOutput(uint16_t i);
bool Test_IO_In(uint16_t value);
bool Test_IO_Out(uint16_t* value);
void Test_IO_InOut(uint16_t* value);

#endif /*TEST_H*/
