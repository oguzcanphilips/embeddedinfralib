#include "gtest/gtest.h"

#include "Generated\CPP\PWMProxy.hpp"
#include "Generated\CPP\PWMSkeleton.hpp"
#include "LocalPacketCommunicationAscii.hpp"
#include "PWMSupport.hpp"
#include <list>


TEST(TestCommunicationCPPAscii, CallImpl)
{
    LocalPacketCommunicationAscii commA;
    erpc::PWMProxy proxy(commA);
    PWMEvents events(proxy);

    LocalPacketCommunicationAscii commB;

    commA.Link(&commB);
    erpc::PWMSkeleton skeleton(commB);
    commB.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    
    commA.SetMode(LocalPacketCommunicationAscii::OnEndSendReceive);
    proxy.SetPwm(0, 1000);
    commA.SetMode(LocalPacketCommunicationAscii::OnEndSend);
    ASSERT_EQ(1000, proxy.GetPwm(0));
    
    commA.SetMode(LocalPacketCommunicationAscii::OnEndSend);
    proxy.SetName("Hello");
    ASSERT_EQ(erpc::PCString("Hello"), proxy.GetName());

}
