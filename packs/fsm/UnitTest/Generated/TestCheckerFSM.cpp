/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : FSM.exe
  Author   : Patrick van Kaam

****************************************************************************
  Copyright (c) 2014 Koninklijke Philips N.V.,
  All Rights Reserved.
***************************************************************************/

#include"TestCheckerFSM.hpp"
#include<cassert>

#ifndef DEBUG_FSM
  #define DebugFSM(TXT)
#else
  extern void DebugFSM(const char* txt);
#endif

TestCheckerFSM::TestCheckerFSM()
: currentState(TestCheckerFSM_Unknown)
, mIsProcessingEvents(true)
{
}
void TestCheckerFSM::Enter_S1()
{
  S1E();
}
void TestCheckerFSM::Leave_S1()
{
  S1L();
}
void TestCheckerFSM::Enter_S11()
{
  S11E();
}
void TestCheckerFSM::Leave_S11()
{
  S11L();
}
void TestCheckerFSM::Enter_S12()
{
  S12E();
}
void TestCheckerFSM::Leave_S12()
{
  S12L();
}
void TestCheckerFSM::Enter_S0()
{
  S0E();
}
void TestCheckerFSM::Leave_S0()
{
  S0L();
}
void TestCheckerFSM::StartEventProcessing()
{
  assert(!mIsProcessingEvents);
  mIsProcessingEvents = true;
}
void TestCheckerFSM::EndEventProcessing()
{
  mIsProcessingEvents = true;
}
TestCheckerFSM::State TestCheckerFSM::GetCurrentState() const
{
  return currentState;
}

void TestCheckerFSM::Ping()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestCheckerFSM_S11:
  {
    bool condWhat = What();
    if(((condWhat)))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      Leave_S11();
      A0();
      A1();
      DebugFSM("TestCheckerFSM_Enter_S12");
      Enter_S12();
      DebugFSM("TestCheckerFSM_S12");
      currentState = TestCheckerFSM_S12;
      break;
    }
    if(((!condWhat)))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      Leave_S11();
      DebugFSM("TestCheckerFSM_Leave_S1");
      Leave_S1();
      A0();
      A2();
      DebugFSM("TestCheckerFSM_Enter_S0");
      Enter_S0();
      DebugFSM("TestCheckerFSM_S0");
      currentState = TestCheckerFSM_S0;
      break;
    }
    break;
  }
  default: break;
  }
  EndEventProcessing();
}


void TestCheckerFSM::E()
{
  StartEventProcessing();
  switch(currentState)
  {
  case TestCheckerFSM_S11:
  {
    bool condC1 = C1();
    bool condC2 = C2();
    bool condC3 = C3();
    if((condC1))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      Leave_S11();
      A1();
      DebugFSM("TestCheckerFSM_Enter_S11");
      Enter_S11();
      DebugFSM("TestCheckerFSM_S11");
      currentState = TestCheckerFSM_S11;
      break;
    }
    if((!condC1 && (condC2 || condC3)))
    {
      DebugFSM("TestCheckerFSM_Leave_S11");
      Leave_S11();
      A2();
      DebugFSM("TestCheckerFSM_Enter_S11");
      Enter_S11();
      DebugFSM("TestCheckerFSM_S11");
      currentState = TestCheckerFSM_S11;
      break;
    }
    break;
  }
  default: break;
  }
  EndEventProcessing();
}

void TestCheckerFSM::SetInitialState()
{
  mIsProcessingEvents = true;
  Enter_S1();
  Enter_S11();
  DebugFSM("TestCheckerFSM_S11");
  currentState = TestCheckerFSM_S11;
  mIsProcessingEvents = false;
}
