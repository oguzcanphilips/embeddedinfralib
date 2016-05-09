#include "gtest/gtest.h"
#include "Generated\TestFSM.hpp"
#include "Generated\TestCheckerFSM.hpp"
#include <string>

class Tester : public fsm::TestFSM
{
public:

    bool what = false;
    std::string Calls;
    Tester() : what(false)
    {
        SetInitialState();
    }
protected:
    // conditions
    virtual bool What(){return what;}


    // actions
    virtual void Pong(){Calls += "_P";}
    virtual void Finger(){Calls += "_F";}
    virtual void Lower(){Calls += "_L";}
    virtual void S1E(){Calls += "_S1E";}
    virtual void S1L(){Calls += "_S1L";}
    virtual void S11E(){Calls += "_S11E";}
    virtual void S11L(){Calls += "_S11L";}
    virtual void S111E(){Calls += "_S111E";}
    virtual void S111L(){Calls += "_S111L";}
    virtual void S112E(){Calls += "_S112E";}
    virtual void S112L(){ Calls += "_S112L"; }

    virtual void TriggerReturn()
    {
        Return();
    }
};

class TestChecker : public fsm::TestCheckerFSM
{
public:

    bool what = false;
    bool c1 = false;
    bool c2 = false;
    bool c3 = false;
    std::string Calls;
    TestChecker() : what(false)
    {
        SetInitialState();
    }
protected:
    // conditions
    virtual bool What(){return what;}
    virtual bool C1(){ return c1; }
    virtual bool C2(){ return c2; }
    virtual bool C3(){ return c3; }
    // actions
    virtual void S0E(){Calls += "_S0E";}
    virtual void S0L(){Calls += "_S0L";}
    virtual void S1E(){Calls += "_S1E";}
    virtual void S1L(){Calls += "_S1L";}
    virtual void S11E(){Calls += "_S11E";}
    virtual void S11L(){Calls += "_S11L";}
    virtual void S12E(){Calls += "_S12E";}
    virtual void S12L(){Calls += "_S12L";}
    virtual void A0(){Calls += "_A0";}
    virtual void A1(){Calls += "_A1";}
    virtual void A2(){Calls += "_A2";}
};

TEST(TestFiniteStateMachine, Init)
{
    Tester tester;
    
    ASSERT_EQ("_S1E_S11E_S111E",tester.Calls);
}

TEST(TestFiniteStateMachine, InternalEvent)
{
    Tester tester;
    tester.Calls = "";
    tester.what = true;
    tester.Ping();
    ASSERT_EQ("_P",tester.Calls);
}

TEST(TestFiniteStateMachine, ExternalEvent)
{
    Tester tester;
    tester.Calls = "";
    tester.what = false;
    tester.Ping();
    ASSERT_EQ("_S111L_S112E",tester.Calls);
}

TEST(TestFiniteStateMachine, ParentEvent1)
{
    Tester tester;
    tester.Calls = "";
    tester.what = true;
    tester.Reset();
    ASSERT_EQ("_S111L_S11L_S11E_S111E",tester.Calls);
}

TEST(TestFiniteStateMachine, ParentEvent2)
{
    Tester tester;
    tester.Calls = "";
    tester.what = false;
    tester.Reset();
    ASSERT_EQ("_S111L_S11L_S1L_S1E_S11E_S111E",tester.Calls);
}

TEST(TestFiniteStateMachine, TriggerEventInAction)
{
    Tester tester;
    tester.Calls = "";
    tester.Go();
    ASSERT_EQ("_S111L_S112E_S112L_S111E",tester.Calls);
}

TEST(TestFiniteStateMachine, Checker1)
{
    TestChecker tester;
    tester.Calls = "";
    tester.what = true;
    tester.Ping();
    ASSERT_EQ("_S11L_A0_A1_S12E",tester.Calls);
}

TEST(TestFiniteStateMachine, Checker2)
{
    TestChecker tester;
    tester.Calls = "";
    tester.what = false;
    tester.Ping();
    ASSERT_EQ("_S11L_S1L_A0_A2_S0E",tester.Calls);
}

TEST(TestFiniteStateMachine, Checker3)
{
    TestChecker tester;
    tester.Calls = "";
    tester.c1 = true;
    tester.E();
    ASSERT_EQ("_S11L_A1_S11E", tester.Calls);
}

TEST(TestFiniteStateMachine, Checker4)
{
    TestChecker tester;
    tester.Calls = "";
    tester.c2 = true;
    tester.E();
    ASSERT_EQ("_S11L_A2_S11E", tester.Calls);
}

TEST(TestFiniteStateMachine, Checker5)
{
    TestChecker tester;
    tester.Calls = "";
    tester.c3 = true;
    tester.E();
    ASSERT_EQ("_S11L_A2_S11E", tester.Calls);
}

TEST(TestFiniteStateMachine, Checker6)
{
    TestChecker tester;
    tester.Calls = "";
    tester.c2 = true;
    tester.c3 = true;
    tester.E();
    ASSERT_EQ("_S11L_A2_S11E", tester.Calls);
}
