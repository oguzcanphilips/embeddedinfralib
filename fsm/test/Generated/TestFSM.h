/***************************************************************************

  THIS CODE IS GENERATED. DO NOT CHANGE OR CHECK-IN.             

  Tool     : FSM.exe
  Author   : Patrick van Kaam

****************************************************************************
  Copyright (c) 2014 Koninklijke Philips N.V.,
  All Rights Reserved.
***************************************************************************/

#ifndef TESTFSM_H
#define TESTFSM_H

typedef enum
{
  TestFSM_State_Unknown,
  TestFSM_State_S111,
  TestFSM_State_S112,
  TestFSM_State_Simple
} TestFSM_State;

void TestFSM_SetInitialState(void);
TestFSM_State TestFSM_GetCurrentState(void);
/* events */
void TestFSM_Ping(void);
void TestFSM_Reset(void);
void TestFSM_Go(void);
void TestFSM_Return(void);
void TestFSM_JustAnEvent(void);
/* conditions */
/*virtual*/ int TestFSM_What(void);
/* actions */
/*virtual*/ void TestFSM_Pong(void);
/*virtual*/ void TestFSM_Finger(void);
/*virtual*/ void TestFSM_Lower(void);
/*virtual*/ void TestFSM_TriggerReturn(void);
/*virtual*/ void TestFSM_S1E(void);
/*virtual*/ void TestFSM_S1L(void);
/*virtual*/ void TestFSM_S11E(void);
/*virtual*/ void TestFSM_S11L(void);
/*virtual*/ void TestFSM_S111E(void);
/*virtual*/ void TestFSM_S111L(void);
/*virtual*/ void TestFSM_S112E(void);
/*virtual*/ void TestFSM_S112L(void);
#endif
