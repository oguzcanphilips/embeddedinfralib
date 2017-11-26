/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : FSM.exe
  Author   : Patrick van Kaam

****************************************************************************
  Copyright (c) 2014 Koninklijke Philips N.V.,
  All Rights Reserved.
***************************************************************************/

#include"TestFSM.h"
#include<assert.h>

#ifndef DEBUG_FSM
  #define DebugFSM(TXT)
#else
  extern void DebugFSM(const char* txt);
#endif

static TestFSM_State currentState = TestFSM_State_Unknown;
static int EvTestFSM_IsProcessingEvents = 1;
typedef struct _TestFSM_Event
{
  struct _TestFSM_Event* next;
  void(*func)(void);
} TestFSM_Event;
static TestFSM_Event EvTestFSM_Ping;
static TestFSM_Event EvTestFSM_Reset;
static TestFSM_Event EvTestFSM_Go;
static TestFSM_Event EvTestFSM_Return;
static TestFSM_Event EvTestFSM_JustAnEvent;
static TestFSM_Event* EvTestFSM_PendingEvents = 0;
static void TestFSM_ProcessEvents(void);
static void TestFSM_AddEvent(TestFSM_Event* ev);
  // events implementation
static void TestFSM_PingImpl(void);
static void TestFSM_ResetImpl(void);
static void TestFSM_GoImpl(void);
static void TestFSM_ReturnImpl(void);
static void TestFSM_JustAnEventImpl(void);
static void TestFSM_AddEvent(TestFSM_Event* ev)
{
  if(EvTestFSM_PendingEvents == 0) EvTestFSM_PendingEvents = ev;
  else
  {
    TestFSM_Event* pos = EvTestFSM_PendingEvents;
    TestFSM_Event* it = EvTestFSM_PendingEvents;
    for(; it ; it=it->next)
    {
      if(it == ev) return;
      pos = it;
    }
    pos->next = ev;
  }
  ev->next = 0;
  TestFSM_ProcessEvents();
}
static void TestFSM_ProcessEvents()
{
  if(!EvTestFSM_IsProcessingEvents)
  {
    EvTestFSM_IsProcessingEvents = 1;
    while(EvTestFSM_PendingEvents)
    {
     TestFSM_Event* ef = EvTestFSM_PendingEvents;
      EvTestFSM_PendingEvents = EvTestFSM_PendingEvents->next;
      (*ef->func)();
    }
    EvTestFSM_IsProcessingEvents = 0;
  }
}
static void StartEventProcessing()
{
}
static void EndEventProcessing()
{
}

static void TestFSM_Enter_S1(void)
{
  TestFSM_S1E();
}
static void TestFSM_Leave_S1(void)
{
  TestFSM_S1L();
}
static void TestFSM_Enter_S11(void)
{
  TestFSM_S11E();
}
static void TestFSM_Leave_S11(void)
{
  TestFSM_S11L();
}
static void TestFSM_Enter_S111(void)
{
  TestFSM_S111E();
}
static void TestFSM_Leave_S111(void)
{
  TestFSM_S111L();
}
static void TestFSM_Enter_S112(void)
{
  TestFSM_S112E();
}
static void TestFSM_Leave_S112(void)
{
  TestFSM_S112L();
}
TestFSM_State TestFSM_GetCurrentState(void)
{
  return currentState;
}

void TestFSM_Ping(void)
{
  TestFSM_AddEvent(&EvTestFSM_Ping);
}
void TestFSM_PingImpl(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_State_S111:
  {
    int condWhat = TestFSM_What();
    if((condWhat))
    {
      TestFSM_Pong();
      break;
    }
    if((!condWhat))
    {
      DebugFSM("TestFSM_Leave_S111");
      TestFSM_Leave_S111();
      DebugFSM("TestFSM_Enter_S112");
      TestFSM_Enter_S112();
      DebugFSM("TestFSM_State_S112");
      currentState = TestFSM_State_S112;
      break;
    }
    TestFSM_Finger();
    break;
  }
  case TestFSM_State_S112:
  {
    TestFSM_Finger();
    break;
  }
  case TestFSM_State_Simple:
  {
    TestFSM_Finger();
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM_Reset(void)
{
  TestFSM_AddEvent(&EvTestFSM_Reset);
}
void TestFSM_ResetImpl(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_State_S111:
  {
    int condWhat = TestFSM_What();
    if((condWhat))
    {
      DebugFSM("TestFSM_Leave_S111");
      TestFSM_Leave_S111();
      DebugFSM("TestFSM_Leave_S11");
      TestFSM_Leave_S11();
      DebugFSM("TestFSM_Enter_S11");
      TestFSM_Enter_S11();
      DebugFSM("TestFSM_Enter_S111");
      TestFSM_Enter_S111();
      DebugFSM("TestFSM_State_S111");
      currentState = TestFSM_State_S111;
      break;
    }
    DebugFSM("TestFSM_Leave_S111");
    TestFSM_Leave_S111();
    DebugFSM("TestFSM_Leave_S11");
    TestFSM_Leave_S11();
    DebugFSM("TestFSM_Leave_S1");
    TestFSM_Leave_S1();
    DebugFSM("TestFSM_Enter_S1");
    TestFSM_Enter_S1();
    DebugFSM("TestFSM_Enter_S11");
    TestFSM_Enter_S11();
    DebugFSM("TestFSM_Enter_S111");
    TestFSM_Enter_S111();
    DebugFSM("TestFSM_State_S111");
    currentState = TestFSM_State_S111;
    break;
  }
  case TestFSM_State_S112:
  {
    int condWhat = TestFSM_What();
    if((condWhat))
    {
      DebugFSM("TestFSM_Leave_S112");
      TestFSM_Leave_S112();
      DebugFSM("TestFSM_Leave_S11");
      TestFSM_Leave_S11();
      DebugFSM("TestFSM_Enter_S11");
      TestFSM_Enter_S11();
      DebugFSM("TestFSM_Enter_S111");
      TestFSM_Enter_S111();
      DebugFSM("TestFSM_State_S111");
      currentState = TestFSM_State_S111;
      break;
    }
    DebugFSM("TestFSM_Leave_S112");
    TestFSM_Leave_S112();
    DebugFSM("TestFSM_Leave_S11");
    TestFSM_Leave_S11();
    DebugFSM("TestFSM_Leave_S1");
    TestFSM_Leave_S1();
    DebugFSM("TestFSM_Enter_S1");
    TestFSM_Enter_S1();
    DebugFSM("TestFSM_Enter_S11");
    TestFSM_Enter_S11();
    DebugFSM("TestFSM_Enter_S111");
    TestFSM_Enter_S111();
    DebugFSM("TestFSM_State_S111");
    currentState = TestFSM_State_S111;
    break;
  }
  case TestFSM_State_Simple:
  {
    int condWhat = TestFSM_What();
    if((condWhat))
    {
      DebugFSM("TestFSM_Leave_S11");
      TestFSM_Leave_S11();
      DebugFSM("TestFSM_Enter_S11");
      TestFSM_Enter_S11();
      DebugFSM("TestFSM_Enter_S111");
      TestFSM_Enter_S111();
      DebugFSM("TestFSM_State_S111");
      currentState = TestFSM_State_S111;
      break;
    }
    DebugFSM("TestFSM_Leave_S11");
    TestFSM_Leave_S11();
    DebugFSM("TestFSM_Leave_S1");
    TestFSM_Leave_S1();
    DebugFSM("TestFSM_Enter_S1");
    TestFSM_Enter_S1();
    DebugFSM("TestFSM_Enter_S11");
    TestFSM_Enter_S11();
    DebugFSM("TestFSM_Enter_S111");
    TestFSM_Enter_S111();
    DebugFSM("TestFSM_State_S111");
    currentState = TestFSM_State_S111;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM_Go(void)
{
  TestFSM_AddEvent(&EvTestFSM_Go);
}
void TestFSM_GoImpl(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_State_S111:
  {
    DebugFSM("TestFSM_Leave_S111");
    TestFSM_Leave_S111();
    TestFSM_TriggerReturn();
    DebugFSM("TestFSM_Enter_S112");
    TestFSM_Enter_S112();
    DebugFSM("TestFSM_State_S112");
    currentState = TestFSM_State_S112;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM_Return(void)
{
  TestFSM_AddEvent(&EvTestFSM_Return);
}
void TestFSM_ReturnImpl(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_State_S112:
  {
    DebugFSM("TestFSM_Leave_S112");
    TestFSM_Leave_S112();
    DebugFSM("TestFSM_Enter_S111");
    TestFSM_Enter_S111();
    DebugFSM("TestFSM_State_S111");
    currentState = TestFSM_State_S111;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM_JustAnEvent(void)
{
  TestFSM_AddEvent(&EvTestFSM_JustAnEvent);
}
void TestFSM_JustAnEventImpl(void)
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_State_S112:
  {
    DebugFSM("TestFSM_Leave_S112");
    TestFSM_Leave_S112();
    DebugFSM("TestFSM_State_Simple");
    currentState = TestFSM_State_Simple;
    break;
  }
  case TestFSM_State_Simple:
  {
    DebugFSM("TestFSM_Enter_S111");
    TestFSM_Enter_S111();
    DebugFSM("TestFSM_State_S111");
    currentState = TestFSM_State_S111;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}

void TestFSM_SetInitialState(void)
{
  EvTestFSM_IsProcessingEvents = 1;
  EvTestFSM_Ping.func = &TestFSM_PingImpl;
  EvTestFSM_Reset.func = &TestFSM_ResetImpl;
  EvTestFSM_Go.func = &TestFSM_GoImpl;
  EvTestFSM_Return.func = &TestFSM_ReturnImpl;
  EvTestFSM_JustAnEvent.func = &TestFSM_JustAnEventImpl;
  TestFSM_Enter_S1();
  TestFSM_Enter_S11();
  TestFSM_Enter_S111();
  DebugFSM("TestFSM_State_S111");
  currentState = TestFSM_State_S111;
  EvTestFSM_IsProcessingEvents = 0;
  TestFSM_ProcessEvents();
}
