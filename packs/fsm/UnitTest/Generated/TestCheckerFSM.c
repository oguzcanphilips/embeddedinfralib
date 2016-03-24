/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : FSM.exe
  Author   : Patrick van Kaam

****************************************************************************
  Copyright (c) 2014 Koninklijke Philips N.V.,
  All Rights Reserved.
***************************************************************************/

#include"TestCheckerFSM.h"
#include<assert.h>

#ifndef DEBUG_FSM
  #define DebugFSM(TXT)
#else
  extern void DebugFSM(const char* txt);
#endif

static TestCheckerFSM_State currentState = TestCheckerFSM_State_Unknown;
static int EvTestCheckerFSM_IsProcessingEvents = 1;
static void StartEventProcessing()
{
  assert(EvTestCheckerFSM_IsProcessingEvents == 0);
  EvTestCheckerFSM_IsProcessingEvents = 1;
}
static void EndEventProcessing()
{
  EvTestCheckerFSM_IsProcessingEvents = 0;
}

static void TestCheckerFSM_Enter_S1(void)
{
  TestCheckerFSM_S1E();
}
static void TestCheckerFSM_Leave_S1(void)
{
  TestCheckerFSM_S1L();
}
static void TestCheckerFSM_Enter_S11(void)
{
  TestCheckerFSM_S11E();
}
static void TestCheckerFSM_Leave_S11(void)
{
  TestCheckerFSM_S11L();
}
static void TestCheckerFSM_Enter_S12(void)
{
  TestCheckerFSM_S12E();
}
static void TestCheckerFSM_Leave_S12(void)
{
  TestCheckerFSM_S12L();
}
static void TestCheckerFSM_Enter_S0(void)
{
  TestCheckerFSM_S0E();
}
static void TestCheckerFSM_Leave_S0(void)
{
  TestCheckerFSM_S0L();
}
TestCheckerFSM_State TestCheckerFSM_GetCurrentState(void)
{
  return currentState;
}

void TestCheckerFSM_Ping(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestCheckerFSM_State_S11:
  {
    int condWhat = TestCheckerFSM_What();
    if(((condWhat)))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      TestCheckerFSM_Leave_S11();
      TestCheckerFSM_A0();
      TestCheckerFSM_A1();
      DebugFSM("TestCheckerFSM_Enter_S12");
      TestCheckerFSM_Enter_S12();
      DebugFSM("TestCheckerFSM_State_S12");
      currentState = TestCheckerFSM_State_S12;
      break;
    }
    if(((!condWhat)))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      TestCheckerFSM_Leave_S11();
      DebugFSM("TestCheckerFSM_Leave_S1");
      TestCheckerFSM_Leave_S1();
      TestCheckerFSM_A0();
      TestCheckerFSM_A2();
      DebugFSM("TestCheckerFSM_Enter_S0");
      TestCheckerFSM_Enter_S0();
      DebugFSM("TestCheckerFSM_State_S0");
      currentState = TestCheckerFSM_State_S0;
      break;
    }
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestCheckerFSM_E(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestCheckerFSM_State_S11:
  {
    int condC1 = TestCheckerFSM_C1();
    int condC2 = TestCheckerFSM_C2();
    int condC3 = TestCheckerFSM_C3();
    if((condC1))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      TestCheckerFSM_Leave_S11();
      TestCheckerFSM_A1();
      DebugFSM("TestCheckerFSM_Enter_S11");
      TestCheckerFSM_Enter_S11();
      DebugFSM("TestCheckerFSM_State_S11");
      currentState = TestCheckerFSM_State_S11;
      break;
    }
    if((!condC1 && (condC2 || condC3)))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      TestCheckerFSM_Leave_S11();
      TestCheckerFSM_A2();
      DebugFSM("TestCheckerFSM_Enter_S11");
      TestCheckerFSM_Enter_S11();
      DebugFSM("TestCheckerFSM_State_S11");
      currentState = TestCheckerFSM_State_S11;
      break;
    }
    break;
  }
  default: break;
  }
  EndEventProcessing();
}

void TestCheckerFSM_SetInitialState(void)
{
  EvTestCheckerFSM_IsProcessingEvents = 1;
  TestCheckerFSM_Enter_S1();
  TestCheckerFSM_Enter_S11();
  DebugFSM("TestCheckerFSM_State_S11");
  currentState = TestCheckerFSM_State_S11;
  EvTestCheckerFSM_IsProcessingEvents = 0;
}
