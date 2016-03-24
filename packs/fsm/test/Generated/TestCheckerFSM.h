/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : FSM.exe
  Author   : Patrick van Kaam

****************************************************************************
  Copyright (c) 2014 Koninklijke Philips N.V.,
  All Rights Reserved.
***************************************************************************/

#ifndef TESTCHECKERFSM_H
#define TESTCHECKERFSM_H

typedef enum
{
  TestCheckerFSM_State_Unknown,
  TestCheckerFSM_State_S11,
  TestCheckerFSM_State_S12,
  TestCheckerFSM_State_S0,
  TestCheckerFSM_State_TransitionalState0
} TestCheckerFSM_State;

void TestCheckerFSM_SetInitialState(void);
TestCheckerFSM_State TestCheckerFSM_GetCurrentState(void);
/* events */
void TestCheckerFSM_Ping(void);
void TestCheckerFSM_E(void);
/* conditions */
/*virtual*/ int TestCheckerFSM_What(void);
/*virtual*/ int TestCheckerFSM_C1(void);
/*virtual*/ int TestCheckerFSM_C2(void);
/*virtual*/ int TestCheckerFSM_C3(void);
/* actions */
/*virtual*/ void TestCheckerFSM_A0(void);
/*virtual*/ void TestCheckerFSM_A1(void);
/*virtual*/ void TestCheckerFSM_A2(void);
/*virtual*/ void TestCheckerFSM_S1E(void);
/*virtual*/ void TestCheckerFSM_S1L(void);
/*virtual*/ void TestCheckerFSM_S11E(void);
/*virtual*/ void TestCheckerFSM_S11L(void);
/*virtual*/ void TestCheckerFSM_S12E(void);
/*virtual*/ void TestCheckerFSM_S12L(void);
/*virtual*/ void TestCheckerFSM_S0E(void);
/*virtual*/ void TestCheckerFSM_S0L(void);
#endif
