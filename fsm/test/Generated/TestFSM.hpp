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

namespace fsm
{
class TestFSM
{
protected:
  TestFSM();
  // should be called form the constructor of the derived class
  void SetInitialState();
public:
  virtual ~TestFSM(){}
  // events
  void Ping();
  void Reset();
  void Go();
  void Return();
  void JustAnEvent();
  enum State
  {
    TestFSM_Unknown,
    TestFSM_S111,
    TestFSM_S112,
    TestFSM_Simple
  };
  State GetCurrentState() const;
protected:
  // conditions
  virtual bool What() = 0;
  // actions
  virtual void Pong() = 0;
  virtual void Finger() = 0;
  virtual void Lower() = 0;
  virtual void TriggerReturn() = 0;
  virtual void S1E() = 0;
  virtual void S1L() = 0;
  virtual void S11E() = 0;
  virtual void S11L() = 0;
  virtual void S111E() = 0;
  virtual void S111L() = 0;
  virtual void S112E() = 0;
  virtual void S112L() = 0;
private:
  void StartEventProcessing();
  void EndEventProcessing();
  void Enter_S1();
  void Leave_S1();
  void Enter_S11();
  void Leave_S11();
  void Enter_S111();
  void Leave_S111();
  void Enter_S112();
  void Leave_S112();
  State currentState;
  // events implementation
  void PingImpl();
  void ResetImpl();
  void GoImpl();
  void ReturnImpl();
  void JustAnEventImpl();
  class EventFunc
  {
    friend class TestFSM;
    void(TestFSM::*func)(void);
    EventFunc* next;
    EventFunc(void(TestFSM::*f)(void)) : func(f){}
  };
  EventFunc EvPing;
  EventFunc EvReset;
  EventFunc EvGo;
  EventFunc EvReturn;
  EventFunc EvJustAnEvent;
  EventFunc* mPendingEvents;
  void AddEvent(EventFunc* ev);
  void ProcessEvents();
  bool mIsProcessingEvents;
};
}

#endif
