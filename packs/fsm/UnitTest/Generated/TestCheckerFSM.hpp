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

class TestCheckerFSM
{
protected:
  TestCheckerFSM();
  // should be called form the constructor of the derived class
  void SetInitialState();
public:
  virtual ~TestCheckerFSM(){}
  // events
  void Ping();
  void E();
  enum State
  {
    TestCheckerFSM_Unknown,
    TestCheckerFSM_S11,
    TestCheckerFSM_S12,
    TestCheckerFSM_S0,
    TestCheckerFSM_TransitionalState0
  };
  State GetCurrentState() const;
protected:
  // conditions
  virtual bool What() = 0;
  virtual bool C1() = 0;
  virtual bool C2() = 0;
  virtual bool C3() = 0;
  // actions
  virtual void A0() = 0;
  virtual void A1() = 0;
  virtual void A2() = 0;
  virtual void S1E() = 0;
  virtual void S1L() = 0;
  virtual void S11E() = 0;
  virtual void S11L() = 0;
  virtual void S12E() = 0;
  virtual void S12L() = 0;
  virtual void S0E() = 0;
  virtual void S0L() = 0;
private:
  void StartEventProcessing();
  void EndEventProcessing();
  void Enter_S1();
  void Leave_S1();
  void Enter_S11();
  void Leave_S11();
  void Enter_S12();
  void Leave_S12();
  void Enter_S0();
  void Leave_S0();
  State currentState;
  // events implementation
  bool mIsProcessingEvents;
};

#endif
