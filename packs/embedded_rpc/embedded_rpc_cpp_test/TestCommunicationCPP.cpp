#include "gtest/gtest.h"

#include "infra\threading\public\Lock.hpp"
#include "infra\threading\public\Thread.hpp"
#include "Generated\CPP\IPWM.hpp"
#include "Generated\CPP\PWMProxy.hpp"
#include "Generated\CPP\PWMSkeleton.hpp"
#include "Generated\CPP\IFoo.hpp"
#include "Generated\CPP\FooProxy.hpp"
#include "Generated\CPP\FooSkeleton.hpp"
#include "Generated\CPP\IBar.hpp"
#include "Generated\CPP\BarProxy.hpp"
#include "Generated\CPP\BarSkeleton.hpp"
#include "Generated\CPP\TestProxy.hpp"
#include "Generated\CPP\TestSkeleton.hpp"
#include "LocalPacketCommunication.hpp"
#include "LocalPacketCommunicationMT.hpp"
#include "PacketReantrant.hpp"
#include "PWMSupport.hpp"
#include "TestSupport.hpp"
#include <list>

class BarImpl : public erpc::IBar
{
public:
  erpc::KeyId mKid;
  void SetKeyId(const erpc::KeyId& v)
  {
      mKid = v;
  }
  erpc::KeyId GetKeyId()
  {
      return mKid;
  }
  void SetGetKeyId(erpc::KeyId& kid)
  {
      erpc::KeyId tmp = mKid;
      mKid = kid;
      kid = tmp;
  }
};

class FooImpl : public erpc::IFoo
{
public:
    erpc::Array mData;
    void DoThis(const erpc::Array& array)
  {
    mData = array;
  }
  uint32_t DoThat(uint32_t i)
  {
    return i*2;
  }
  erpc::PCString UpperCase(const erpc::PCString& str)
  {
      erpc::PCString res;
    uint8_t i=0;
    while(str.text[i])
    {
        if(str.text[i]>='a' && str.text[i]<='z')
        {
            res.text[i] = str.text[i]-32;
        }
        else
        {
            res.text[i] = str.text[i];
        }
        i++;
    }
    res.text[i] = 0;
    return res;
  }

  erpc::Scope NextScope(const erpc::Scope& s)
  {
      if (s == erpc::Scope::Low)
          return erpc::Scope::Med;
      if (s == erpc::Scope::Med)
          return erpc::Scope::High;
      return erpc::Scope::Low;
  }

};

TEST(TestCommunicationCPP, CallImpl)
{
  LocalPacketCommunication commA;
  erpc::PWMProxy proxy(commA);
  PWMEvents events(proxy);

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::PWMSkeleton skeleton(commB);
  commB.Register(skeleton);
  PWMImpl pwm;
  skeleton.SetImpl(pwm);

  proxy.SetPwm(0,1000);
  proxy.SetPwm(1,2000);

  ASSERT_EQ(1000, pwm.GetPwm(0));
  ASSERT_EQ(2000, pwm.GetPwm(1));

  ASSERT_EQ(1000, events.GetPwm(0));
  ASSERT_EQ(2000, events.GetPwm(1));

  ASSERT_EQ(1000, proxy.GetPwm(0));
  ASSERT_EQ(2000, proxy.GetPwm(1));
}

TEST(TestCommunicationCPP, 2ChannelCommunication)
{
  LocalPacketCommunication commA;
  LocalPacketCommunication commB;
  commA.Link(&commB);

  LocalPacketCommunication commA2;
  LocalPacketCommunication commB2;
  commA2.Link(&commB2);

  erpc::FooProxy proxy(commA);
  erpc::FooProxy proxy2(commA2);

  erpc::FooSkeleton skeleton;
  commB.Register(skeleton);
  commB2.Register(skeleton);

  FooImpl impl;
  skeleton.SetImpl(impl);

  ASSERT_EQ(10, proxy.DoThat(5));
  ASSERT_EQ(12, proxy2.DoThat(6));
}

TEST(TestCommunicationCPP, CallImplMT)
{
  bool verbose = true;
  erpc::PacketReantrant<LocalPacketCommunicationMT> commA(10000);
  erpc::PWMProxy proxy(commA);
  PWMEvents events(proxy);

  erpc::PacketReantrant<LocalPacketCommunicationMT> commB(10000);
  commA.Link(&commB);

  commA.Start();
  commB.Start();
  erpc::PWMSkeleton skeleton(commB);
  commB.Register(skeleton);
  PWMImpl pwm;
  skeleton.SetImpl(pwm);

  proxy.SetPwm(0,1000);
  proxy.SetPwm(1,2000);

  ASSERT_EQ(1000, proxy.GetPwm(0));
  for(int i=0;i<10;++i)
  {
    ASSERT_EQ(2000, proxy.GetPwm(1));
  }

  ASSERT_EQ(1000, pwm.GetPwm(0));
  ASSERT_EQ(2000, pwm.GetPwm(1));

  ASSERT_EQ(1000, events.GetPwm(0));
  ASSERT_EQ(2000, events.GetPwm(1));
}

TEST(TestCommunicationCPP, CallArray)
{
  LocalPacketCommunication commA;
  erpc::FooProxy proxy(commA);

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::FooSkeleton skeleton;
  commB.Register(skeleton);

  FooImpl impl;
  skeleton.SetImpl(impl);

  erpc::Array array;
  for(int i=0;i<10;++i) array[i] = i;
  proxy.DoThis(array);

  for(int i=0;i<10;++i)
  {
    ASSERT_EQ(i, array[i]);
    ASSERT_EQ(i, impl.mData[i]);
  }
}

TEST(TestCommunicationCPP, CallString)
{
  LocalPacketCommunication commA;
  erpc::FooProxy proxy(commA);

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::FooSkeleton skeleton;
  commB.Register(skeleton);

  FooImpl impl;
  skeleton.SetImpl(impl);

  erpc::PCString str("Hello World");

  erpc::PCString res = proxy.UpperCase(str);
  ASSERT_STREQ("HELLO WORLD", res.text);
}

TEST(TestCommunicationCPP, GeneratedDataType)
{
  LocalPacketCommunication commA;
  erpc::BarProxy proxy(commA);

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::BarSkeleton skeleton;
  commB.Register(skeleton);

  BarImpl impl;
  skeleton.SetImpl(impl);

  erpc::KeyId kid;
  kid.id = 0x12345678;
  kid.scope = erpc::Scope::Med;

  for(uint16_t i=0;i<10;++i)
  {
      kid.key[i] = i*128;
  }
  proxy.SetKeyId(kid);
  erpc::KeyId kidGet = proxy.GetKeyId();

  ASSERT_EQ(kid.id, kidGet.id);
  ASSERT_EQ(kid.scope, kidGet.scope);
  for(int i=0;i<10;++i)
  {
      ASSERT_EQ(kid.key[i], kidGet.key[i]);
  }
}

TEST(TestCommunicationCPP, GeneratedDataTypeInOut)
{
  LocalPacketCommunication commA;
  erpc::BarProxy proxy(commA);

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::BarSkeleton skeleton;
  commB.Register(skeleton);

  BarImpl impl;
  skeleton.SetImpl(impl);

  erpc::KeyId kid[2];
  for(int i=0;i<2;++i)
  {
      kid[i].id = 0x12345678+i;
      kid[i].scope = i == 0 ? erpc::Scope::Med : erpc::Scope::Low;

      for(uint16_t j=0;j<10;++j)
      {
          kid[i].key[j] = j*128+i;
      }
      }
  proxy.SetKeyId(kid[0]);

  erpc::KeyId kidGet = proxy.GetKeyId();

  ASSERT_EQ(kid[0].id, kidGet.id);
  ASSERT_EQ(kid[0].scope, kidGet.scope);
  for(int i=0;i<10;++i)
  {
      ASSERT_EQ(kid[0].key[i], kidGet.key[i]);
  }
  kidGet = kid[1];
  proxy.SetGetKeyId(kidGet);

  ASSERT_EQ(kid[0].id, kidGet.id);
  ASSERT_EQ(kid[0].scope, kidGet.scope);
  for(int i=0;i<10;++i)
  {
      ASSERT_EQ(kid[0].key[i], kidGet.key[i]);
  }


  kidGet = proxy.GetKeyId();

  ASSERT_EQ(kid[1].id, kidGet.id);
  ASSERT_EQ(kid[1].scope, kidGet.scope);
  for(int i=0;i<10;++i)
  {
      ASSERT_EQ(kid[1].key[i], kidGet.key[i]);
  }
}

TEST(TestCommunicationCPP, In_InOut_Out)
{
  LocalPacketCommunication commA;
  LocalPacketCommunication commB;
  commA.Link(&commB);

  erpc::TestSkeleton skeleton;
  commB.Register(skeleton);
  TestImpl impl;
  skeleton.SetImpl(impl);

  erpc::TestProxy proxy(commA);

  
  proxy.IO_In(100);

  uint16_t v = 0;
  proxy.IO_Out(v);
  ASSERT_EQ(100,v);

  v = 200;
  proxy.IO_InOut(v);
  ASSERT_EQ(100,v);
  proxy.IO_Out(v);
  ASSERT_EQ(200,v);
}

class CallbackSpy : public erpc::PacketCommunication::Callback
{
public:
    CallbackSpy(uint8_t interfaceId)
        : Callback(interfaceId)
        , ReceiveCalled(false)
    {}

    void Receive(erpc::PacketCommunication&){ ReceiveCalled = true; }

    bool ReceiveCalled;
};

TEST(TestCommunicationCPP, Comm1KeepsCallbacksIfComm2Destructed)
{
    const uint8_t interfaceId = 123;

    LocalPacketCommunication* comm1 = new LocalPacketCommunication();
    LocalPacketCommunication* comm2 = new LocalPacketCommunication();

    CallbackSpy callback(interfaceId);
    comm1->Register(callback);

    delete comm2;

    comm1->Link(comm1);
    comm1->PacketStart();
    comm1->WriteByte(interfaceId);
    comm1->ProcessReceive();

    ASSERT_TRUE(callback.ReceiveCalled);

    delete comm1;
}
