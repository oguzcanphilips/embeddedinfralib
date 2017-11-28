/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : FSM.exe
  Author   : Patrick van Kaam

****************************************************************************
  Copyright (c) 2014 Koninklijke Philips N.V.,
  All Rights Reserved.
***************************************************************************/

#include"TestFSM.hpp"
#include<cassert>

#ifndef DEBUG_FSM
  #define DebugFSM(TXT)
#else
  extern void DebugFSM(const char* txt);
#endif

namespace fsm
{
TestFSM::TestFSM()
: currentState(TestFSM_Unknown)
, EvPing(&TestFSM::PingImpl)
, EvReset(&TestFSM::ResetImpl)
, EvGo(&TestFSM::GoImpl)
, EvReturn(&TestFSM::ReturnImpl)
, EvJustAnEvent(&TestFSM::JustAnEventImpl)
, mPendingEvents(0)
, mIsProcessingEvents(true)
{
}
void TestFSM::Enter_S1()
{
  S1E();
}
void TestFSM::Leave_S1()
{
  S1L();
}
void TestFSM::Enter_S11()
{
  S11E();
}
void TestFSM::Leave_S11()
{
  S11L();
}
void TestFSM::Enter_S111()
{
  S111E();
}
void TestFSM::Leave_S111()
{
  S111L();
}
void TestFSM::Enter_S112()
{
  S112E();
}
void TestFSM::Leave_S112()
{
  S112L();
}
void TestFSM::StartEventProcessing()
{
}
void TestFSM::EndEventProcessing()
{
}
void TestFSM::AddEvent(EventFunc* ev)
{
  if(mPendingEvents == 0) mPendingEvents = ev;
  else
  {
     EventFunc* pos = mPendingEvents;
     for(EventFunc* it = mPendingEvents; it ; it=it->next)
     {
       if(it == ev) return;
       pos = it;
     }
     pos->next = ev;
  }
  ev->next = 0;
  ProcessEvents();
}
void TestFSM::ProcessEvents()
{
  if(!mIsProcessingEvents)
  {
    mIsProcessingEvents = true;
    while(mPendingEvents)
    {
      EventFunc* ef = mPendingEvents;
      mPendingEvents = mPendingEvents->next;
      (this->*ef->func)();
    }
    mIsProcessingEvents = false;
  }
}

TestFSM::State TestFSM::GetCurrentState() const
{
  return currentState;
}

void TestFSM::Ping()
{
  AddEvent(&EvPing);
}
void TestFSM::PingImpl()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_S111:
  {
    bool condWhat = What();
    if((condWhat))
    {
      Pong();
      break;
    }
    if((!condWhat))
    {
      DebugFSM("TestFSM_Leave_S111");
      Leave_S111();
      DebugFSM("TestFSM_Enter_S112");
      Enter_S112();
      DebugFSM("TestFSM_S112");
      currentState = TestFSM_S112;
      break;
    }
    Finger();
    break;
  }
  case TestFSM_S112:
  {
    Finger();
    break;
  }
  case TestFSM_Simple:
  {
    Finger();
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM::Reset()
{
  AddEvent(&EvReset);
}
void TestFSM::ResetImpl()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_S111:
  {
    bool condWhat = What();
    if((condWhat))
    {
      DebugFSM("TestFSM_Leave_S111");
      Leave_S111();
      DebugFSM("TestFSM_Leave_S11");
      Leave_S11();
      DebugFSM("TestFSM_Enter_S11");
      Enter_S11();
      DebugFSM("TestFSM_Enter_S111");
      Enter_S111();
      DebugFSM("TestFSM_S111");
      currentState = TestFSM_S111;
      break;
    }
    DebugFSM("TestFSM_Leave_S111");
    Leave_S111();
    DebugFSM("TestFSM_Leave_S11");
    Leave_S11();
    DebugFSM("TestFSM_Leave_S1");
    Leave_S1();
    DebugFSM("TestFSM_Enter_S1");
    Enter_S1();
    DebugFSM("TestFSM_Enter_S11");
    Enter_S11();
    DebugFSM("TestFSM_Enter_S111");
    Enter_S111();
    DebugFSM("TestFSM_S111");
    currentState = TestFSM_S111;
    break;
  }
  case TestFSM_S112:
  {
    bool condWhat = What();
    if((condWhat))
    {
      DebugFSM("TestFSM_Leave_S112");
      Leave_S112();
      DebugFSM("TestFSM_Leave_S11");
      Leave_S11();
      DebugFSM("TestFSM_Enter_S11");
      Enter_S11();
      DebugFSM("TestFSM_Enter_S111");
      Enter_S111();
      DebugFSM("TestFSM_S111");
      currentState = TestFSM_S111;
      break;
    }
    DebugFSM("TestFSM_Leave_S112");
    Leave_S112();
    DebugFSM("TestFSM_Leave_S11");
    Leave_S11();
    DebugFSM("TestFSM_Leave_S1");
    Leave_S1();
    DebugFSM("TestFSM_Enter_S1");
    Enter_S1();
    DebugFSM("TestFSM_Enter_S11");
    Enter_S11();
    DebugFSM("TestFSM_Enter_S111");
    Enter_S111();
    DebugFSM("TestFSM_S111");
    currentState = TestFSM_S111;
    break;
  }
  case TestFSM_Simple:
  {
    bool condWhat = What();
    if((condWhat))
    {
      DebugFSM("TestFSM_Leave_S11");
      Leave_S11();
      DebugFSM("TestFSM_Enter_S11");
      Enter_S11();
      DebugFSM("TestFSM_Enter_S111");
      Enter_S111();
      DebugFSM("TestFSM_S111");
      currentState = TestFSM_S111;
      break;
    }
    DebugFSM("TestFSM_Leave_S11");
    Leave_S11();
    DebugFSM("TestFSM_Leave_S1");
    Leave_S1();
    DebugFSM("TestFSM_Enter_S1");
    Enter_S1();
    DebugFSM("TestFSM_Enter_S11");
    Enter_S11();
    DebugFSM("TestFSM_Enter_S111");
    Enter_S111();
    DebugFSM("TestFSM_S111");
    currentState = TestFSM_S111;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM::Go()
{
  AddEvent(&EvGo);
}
void TestFSM::GoImpl()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_S111:
  {
    DebugFSM("TestFSM_Leave_S111");
    Leave_S111();
    TriggerReturn();
    DebugFSM("TestFSM_Enter_S112");
    Enter_S112();
    DebugFSM("TestFSM_S112");
    currentState = TestFSM_S112;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM::Return()
{
  AddEvent(&EvReturn);
}
void TestFSM::ReturnImpl()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_S112:
  {
    DebugFSM("TestFSM_Leave_S112");
    Leave_S112();
    DebugFSM("TestFSM_Enter_S111");
    Enter_S111();
    DebugFSM("TestFSM_S111");
    currentState = TestFSM_S111;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestFSM::JustAnEvent()
{
  AddEvent(&EvJustAnEvent);
}
void TestFSM::JustAnEventImpl()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestFSM_S112:
  {
    DebugFSM("TestFSM_Leave_S112");
    Leave_S112();
    DebugFSM("TestFSM_Simple");
    currentState = TestFSM_Simple;
    break;
  }
  case TestFSM_Simple:
  {
    DebugFSM("TestFSM_Enter_S111");
    Enter_S111();
    DebugFSM("TestFSM_S111");
    currentState = TestFSM_S111;
    break;
  }
  default: break;
  }
  EndEventProcessing();
}

void TestFSM::SetInitialState()
{
  mIsProcessingEvents = true;
  Enter_S1();
  Enter_S11();
  Enter_S111();
  DebugFSM("TestFSM_S111");
  currentState = TestFSM_S111;
  mIsProcessingEvents = false;
  ProcessEvents();
}
}
