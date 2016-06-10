#include "gtest/gtest.h"

#include "Generated\CPP\PWMProxy.hpp"
#include "Generated\CPP\PWMSkeleton.hpp"
#include "LocalPacketCommunicationAscii.hpp"
#include "PWMSupport.hpp"
#include <list>
class PacketCommunicationAsciiIO : public erpc::PacketCommunicationAscii
{
public:
    std::string output;

    void Write(const std::string& line)
    {
        for (const char* str = line.c_str(); *str; str++)
        {
            fifoIn.push_back(static_cast<uint8_t>(*str));
        }
        fifoIn.push_back('\n');
        ProcessReceive();
    }
    void ProcessReceive()
    {
        while (!fifoIn.empty())
            Receive();
    }
protected:
    void WriteByte(uint8_t b) override
    {
        if (resetOnNextWrite)
        {
            resetOnNextWrite = false;
            output = "";
        }

        if (b == '\n')
            resetOnNextWrite = true;
        else
            output += b;
    }
    bool ReadByte(uint8_t& b) override
    {
        if (fifoIn.empty())
            return false;
        b = fifoIn.front();
        fifoIn.pop_front();
        return true;
    }
private:
    bool resetOnNextWrite = false;
    std::list<uint8_t> fifoIn;
};


TEST(TestCommunicationCPPAscii, CallImplViaProxy)
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
    proxy.SetPwm(erpc::Channel::Low, 1000);
    commA.SetMode(LocalPacketCommunicationAscii::OnEndSend);
    ASSERT_EQ(1000, proxy.GetPwm(erpc::Channel::Low));
    
    commA.SetMode(LocalPacketCommunicationAscii::OnEndSend);
    proxy.SetName("Hello");
    ASSERT_EQ(erpc::PCString("Hello"), proxy.GetName());
}

TEST(TestCommunicationCPPAscii, CallImpl)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);

    comm.Write("PWM.SetName(\"Hello!\")");
    EXPECT_EQ("", comm.output);
    comm.Write("PWM.GetName()");
    EXPECT_EQ("PWM.GetName(\"Hello!\")", comm.output);

    comm.Write("PWM.SetPwm(Channel.Low,123)");
    comm.Write("PWM.GetPwm(Channel.Low)");
    EXPECT_EQ("PWM.GetPwm(123)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplHex)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);

    comm.Write("PWM.SetPwm(0x00,0x123)");
    comm.Write("PWM.GetPwm(0x00)");
    EXPECT_EQ("PWM.GetPwm(291)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplHexOutputHexAndDec)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("PWM.SetPwm(0x00,0x123)");

    comm.Write("$HEX");
    comm.Write("PWM.GetPwm(0x00)");
    EXPECT_EQ("PWM.GetPwm(0x0123)", comm.output);
    comm.Write("$DEC");
    comm.Write("PWM.GetPwm(0x00)");
    EXPECT_EQ("PWM.GetPwm(291)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplTrim)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("$TRIM");
    comm.Write("PWM.SetPwm(0,123)");
    comm.Write("PWM.GetPwm(0)");
    EXPECT_EQ("123", comm.output);
    comm.Write("$!TRIM");
    comm.Write("PWM.SetPwm(0,123)");
    comm.Write("PWM.GetPwm(0)");
    EXPECT_EQ("PWM.GetPwm(123)", comm.output);
}

TEST(TestCommunicationCPPAscii, UnknownCommand)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("$BlaBla");
    EXPECT_EQ("$ERROR:Unknown command (BLABLA)", comm.output);
}

TEST(TestCommunicationCPPAscii, UnknownCommandTooLong)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("$BlaBlaBlaBlaBlaBla");
    EXPECT_EQ("$ERROR:Unknown command (BLABLABLA)", comm.output);
} 

TEST(TestCommunicationCPPAscii, KnownCommand)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("$HEX");
    EXPECT_EQ("", comm.output);
    comm.Write("$Hex");
    EXPECT_EQ("", comm.output);
    comm.Write("$hex");
    EXPECT_EQ("", comm.output);
    comm.Write("$dec");
    EXPECT_EQ("", comm.output);
    comm.Write("$trim");
    EXPECT_EQ("", comm.output);
    comm.Write("$!trim");
    EXPECT_EQ("", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplWithSpaces)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);

    comm.Write("PWM.SetName(  \" Hello! \"  )");
    EXPECT_EQ("", comm.output);
    comm.Write("PWM.GetName(  )");
    EXPECT_EQ("PWM.GetName(\" Hello! \")", comm.output);

    comm.Write("PWM.SetPwm(   Channel.Low   , 123 )");
    comm.Write("PWM.GetPwm(   Channel.Low   )");
    EXPECT_EQ("PWM.GetPwm(123)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplWithUnknownEnum)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("PWM.SetPwm(   Channel_.Low   , 123 )");
    EXPECT_EQ("$ERROR:Unknown enum (Channel_)", comm.output);
    comm.Write("PWM.SetPwm(   Channel.Low_   , 123 )");
    EXPECT_EQ("$ERROR:Unknown enum field (Low_)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplWithUnknownInterface)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("PWM_.SetPwm(Channel.Low, 123)");
    EXPECT_EQ("$ERROR:Unknown interface (PWM_)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplWithUnknownFunction)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);
    comm.Write("PWM.SetPwm_(Channel.Low, 123)");
    EXPECT_EQ("$ERROR:Unknown function (SetPwm_)", comm.output);
}

TEST(TestCommunicationCPPAscii, CallImplWithMissingInput)
{
    PacketCommunicationAsciiIO comm;
    erpc::PWMSkeleton skeleton(comm);
    comm.Register(skeleton);
    PWMImpl pwm;
    skeleton.SetImpl(pwm);

    comm.Write("PWM.SetPwm(0,");
    EXPECT_EQ("$ERROR:Unexpected end of line", comm.output);
    comm.output = "";
    comm.Write("PWM.SetPwm(0,1");
    EXPECT_EQ("$ERROR:Unexpected end of line", comm.output);
    comm.output = "";
    comm.Write("PWM.SetPwm(0,1 ");
    EXPECT_EQ("$ERROR:Unexpected end of line", comm.output);
    comm.output = "";
    comm.Write("PWM.SetPw");
    EXPECT_EQ("$ERROR:Syntax error", comm.output);
    comm.output = "";
}
