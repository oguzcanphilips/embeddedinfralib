#include <gtest/gtest.h>
#include "infra\threading\public\Thread.hpp"
#include "infra\threading\public\Lock.hpp"
#include "Generated\CPP\IPWM.hpp"
#include "Generated\CPP\PWMProxy.hpp"
#include "Generated\CPP\PWMSkeleton.hpp"
#include "Generated\CPP\ITest.hpp"
#include "Generated\CPP\TestProxy.hpp"
#include "Generated\CPP\TestSkeleton.hpp"
#include "PacketCommunicationSLIP.hpp"
#include "PacketCommunicationReport.hpp"
#include "PacketReantrant.hpp"

#include "PWMSupport.hpp"
#include "TestSupport.hpp"
#include "LocalPacketCommunication.hpp"
#include <list>
#include <iostream>


class SerialLocal : public erpc::ISerialIO
{
  mutable infra::Lock mLock;
  SerialLocal* mLink;
  std::list<uint8_t> mRecv;
public:
  SerialLocal():mLink(0){}
  void Send(uint8_t v)
  {
    infra::Lock::ScopedLock lock(mLink->mLock);
    mLink->mRecv.push_back(v);
  }

  bool Receive(uint8_t& v)
  {
    int cnt = 10;
    while(!HasData())
    {
        if(--cnt == 0) return false;
        infra::Thread::Sleep(10);
    }
    infra::Lock::ScopedLock lock(mLock);
    v = mRecv.front();
    mRecv.pop_front();
    return true;
  }

  bool HasData() const
  {
      infra::Lock::ScopedLock lock(mLock);
    return !mRecv.empty();
  }
  
  void Link(SerialLocal* comm)
  {
    comm->mLink = this;
    mLink = comm;
  }
};

class ReportLocal : public erpc::IReportIO
{
    mutable infra::Lock mLock;
  uint16_t mReportLen;
  ReportLocal* mLink;
  std::list<std::vector<uint8_t> > mRecv;
public:
  ReportLocal(uint16_t reportLen):mLink(0),mReportLen(reportLen){}
  void Send(uint8_t* v, uint16_t len)
  {
    infra::Lock::ScopedLock lock(mLink->mLock);
    std::vector<uint8_t> data(v,v+mReportLen);
    mLink->mRecv.push_back(data);
  }

  bool Receive(uint8_t* v)
  {
    int cnt = 10;
    while(!HasData())
    {
        if(--cnt == 0) return false;
        infra::Thread::Sleep(10);
    }
    infra::Lock::ScopedLock lock(mLock);
    memcpy(v, &mRecv.front()[0], mReportLen);
    mRecv.pop_front();
    return true;
  }

  bool HasData() const
  {
      infra::Lock::ScopedLock lock(mLock);
    return !mRecv.empty();
  }
  
  void Link(ReportLocal* comm)
  {
    comm->mLink = this;
    mLink = comm;
  }
};

class PWMApplicationSlip
{
  SerialLocal mSerial;
  erpc::PacketReantrant<erpc::PacketCommunicationSLIP> mComm;
  erpc::PWMSkeleton mSkeleton;
  PWMImpl mPwm;
public:
  PWMApplicationSlip(SerialLocal& serial)
  : mComm(1000, mSerial)
  , mSkeleton(mComm)
  {
    mSkeleton.SetImpl(mPwm);
    mSerial.Link(&serial);
    mComm.Register(mSkeleton);
    mComm.Start();
  }
  ~PWMApplicationSlip()
  {
      mComm.Stop();
  }
};

template<uint16_t R_LEN>
class PWMApplicationReport
{
  ReportLocal mReport;
  erpc::PacketReantrant<erpc::PacketCommunicationReport<R_LEN> > mComm;
  erpc::PWMSkeleton mSkeleton;
  PWMImpl mPwm;
public:
  PWMApplicationReport(ReportLocal& report)
  : mReport(R_LEN)
  , mComm(1000, mReport)
  , mSkeleton(mComm)
  {
    mSkeleton.SetImpl(mPwm);
    mReport.Link(&report);
    mComm.Register(mSkeleton);
    mComm.Start();
  }
  ~PWMApplicationReport()
  {
      mComm.Stop();
  }
};

TEST(TestCommunication, CallImpl)
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

  ASSERT_EQ(1000, proxy.GetPwm(0));
  ASSERT_EQ(2000, proxy.GetPwm(1));

  ASSERT_EQ(1000, events.GetPwm(0));
  ASSERT_EQ(2000, events.GetPwm(1));
}

TEST(TestCommunication, CallImplSecondProxy)
{
  LocalPacketCommunication commA;

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::PWMSkeleton skeleton(commB);
  commB.Register(skeleton);
  PWMImpl pwm;
  skeleton.SetImpl(pwm);
  {
      erpc::PWMProxy proxy(commA);
    PWMEvents events(proxy);
    proxy.SetPwm(0,1000);
    ASSERT_EQ(1000, proxy.GetPwm(0));
  }
  {
      erpc::PWMProxy proxy(commA);
    ASSERT_EQ(1000, proxy.GetPwm(0));
    proxy.SetPwm(0,2000);
    ASSERT_EQ(2000, proxy.GetPwm(0));
  }
}

TEST(TestCommunication, CallArray)
{
  LocalPacketCommunication commA;
  erpc::TestProxy proxy(commA);

  LocalPacketCommunication commB;
  
  commA.Link(&commB);
  erpc::TestSkeleton skeleton;
  commB.Register(skeleton);

  TestImpl impl;
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

TEST(TestCommunication, SLIP)
{
  SerialLocal serial;
  erpc::PacketReantrant<erpc::PacketCommunicationSLIP> comm(1000, serial);
  erpc::PWMProxy proxy(comm);
  PWMEvents events(proxy);  
  
  PWMApplicationSlip application(serial);
  comm.Start();

  proxy.SetPwm(0,1000);
  ASSERT_EQ(1000, proxy.GetPwm(0));
  ASSERT_EQ(1000, events.GetPwm(0));
}

TEST(TestCommunication, Report1)
{
	ReportLocal report(4);
    erpc::PacketReantrant<erpc::PacketCommunicationReport<4> > comm(1000, report);
    erpc::PWMProxy proxy(comm);
	PWMEvents events(proxy);

	PWMApplicationReport<4> application(report);
	comm.Start();

	proxy.SetPwm(0,1000);
	ASSERT_EQ(1000, proxy.GetPwm(0));
	ASSERT_EQ(1000, events.GetPwm(0));
}

TEST(TestCommunication, Report2)
{
	ReportLocal report(16);
    erpc::PacketReantrant<erpc::PacketCommunicationReport<16> > comm(1000, report);
    erpc::PWMProxy proxy(comm);
	PWMEvents events(proxy);

	PWMApplicationReport<16> application(report);
	comm.Start();

	proxy.SetPwm(0,1000);
	ASSERT_EQ(1000, proxy.GetPwm(0));
	ASSERT_EQ(1000, events.GetPwm(0));
}

TEST(TestCommunication, Forward)
{
  SerialLocal serialA;
  SerialLocal serialB;
  serialA.Link(&serialB);
  
  ReportLocal reportA(4);
  ReportLocal reportB(4);
  reportA.Link(&reportB);

  erpc::PacketReantrant<erpc::PacketCommunicationSLIP> pcomm1(1000, serialA);
  erpc::PacketReantrant<erpc::PacketCommunicationSLIP> pcomm2(1000, serialB);
  erpc::PacketReantrant<erpc::PacketCommunicationReport<4> > pcomm3(1000, reportA);
  erpc::PacketReantrant<erpc::PacketCommunicationReport<4> > pcomm4(1000, reportB);

  pcomm2.Link(pcomm3);
  
  TestImpl     impl;
  erpc::TestSkeleton skeleton;
  skeleton.SetImpl(impl);
  pcomm4.Register(skeleton);

  erpc::TestProxy proxy(pcomm1);

  pcomm1.Start();
  pcomm2.Start();
  pcomm3.Start();
  pcomm4.Start();
  ASSERT_EQ(1000, proxy.DoubleOutput(500));
}
