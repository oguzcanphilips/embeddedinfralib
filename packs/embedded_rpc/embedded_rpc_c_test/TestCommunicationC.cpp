#include "gtest/gtest.h"
#include <string.h>
extern "C"
{
    #include "Generated\C\FooSkeleton.h"
    #include "Generated\C\FooEvents.h"
    #include "FooEventsSkeletonRename.h"
    #include "FooProxyRename.h"

    #include "Generated\C\BarSkeleton.h"
    #include "BarProxyRename.h"

    #include "PacketCommunication.h"
    #include "PacketCommunicationReport.h"
    #include "PacketCommunicationSLIP.h"
    #include "Report.h"
    #include "Serial.h"
    #include "Serial2.h"
    #include "Array.h"    

    extern void PacketCommunication_ResetIdCounterForTesting(void);
}

array_t array;

static uint16_t updatedValue = 0;
static PacketCommunicationReport_t report;
static PacketCommunicationSlip_t slip;
static PacketCommunicationSlip_t slip2;

extern "C" void FooEventsRename_UpdateMe(uint16_t v)
{
    updatedValue = v;
}

static array_t updatedArray;
extern "C" void FooEventsRename_UpdateMeArray(array_t* array)
{
    memcpy(&updatedArray, array, sizeof(updatedArray));
}

class TestCommunicationCM : public ::testing::Test
{
public:
    TestCommunicationCM()
    {
        PacketCommunication_ResetIdCounterForTesting();

        CreatePacketCommunicationReport(&report, ReportWrite, ReportRead,4);
        CreatePacketCommunicationSlip(&slip, SerialWrite, SerialRead);
        CreatePacketCommunicationSlip(&slip2, Serial2Write, Serial2Read);
        ReportInit(&report.packetCommunication);
        SerialInit(&slip.packetCommunication);
        Serial2Init(&slip2.packetCommunication);

        FooSkeleton_Init(&slip.packetCommunication);
        FooSkeleton_Init(&slip2.packetCommunication);
        FooEventsSkeleton_Init(&slip.packetCommunication);
        FooEvents_Init(&slip.packetCommunication);
        FooProxy_Init(&slip.packetCommunication);
        BarSkeleton_Init(&report.packetCommunication);
        BarProxy_Init(&report.packetCommunication);
    }
    ~TestCommunicationCM()
    {
        FooSkeleton_DeInit(&slip.packetCommunication);
        FooSkeleton_DeInit(&slip2.packetCommunication);
        FooEventsSkeleton_DeInit(&slip.packetCommunication);
        FooEvents_DeInit(&slip.packetCommunication);
        FooProxy_DeInit(&slip.packetCommunication);
        BarSkeleton_DeInit(&report.packetCommunication);
        BarProxy_DeInit(&report.packetCommunication);
    }

};

TEST_F(TestCommunicationCM, CallImpl)
{
    FooProxy_DoThis(1234); 
      
    ASSERT_EQ(2468, FooProxy_DoThat(2));
    ASSERT_EQ(1234, updatedValue);
}

TEST_F(TestCommunicationCM, 2CommunicationChannels)
{
    ASSERT_EQ(2468, FooProxy_DoThat(2));

    FooProxy_DeInit(&slip.packetCommunication);
    FooProxy_Init(&slip2.packetCommunication);
    ASSERT_EQ(1234, FooProxy_DoThat(1));
}

TEST_F(TestCommunicationCM, CallArray)
{
    memset(updatedArray.data, 0, sizeof(updatedArray.data));
    memset(updatedArray.data, 0, sizeof(updatedArray.data));

    uint8_t i=0;

    for(i=0;i<sizeof(array.data);++i)
    {
        array.data[i] = i;
    }

    FooProxy_DoThisArray(&array);

    for(i=0;i<sizeof(array.data);++i)
    {
        ASSERT_EQ(array.data[i] * 2, updatedArray.data[i]);
    }
}

TEST_F(TestCommunicationCM, CallPCString)
{
    PCString str;
    PCString strRet;
    strcpy(str.text,"Hello world");

    uint8_t i=0;

    strRet = FooProxy_UpperCase(&str);
    ASSERT_STREQ(strRet.text, "HELLO WORLD");
}

TEST_F(TestCommunicationCM, GeneratedDataType)
{
    keyId_t kid;
    keyId_t kidGet;
    uint16_t i;

    kid.id = 0x12345678;
    kid.scope = Scope_Normal;
    for(i=0;i<10;++i)
    {
        kid.key[0] = i*128;
    }

    BarProxy_SetKeyId(&kid);
    kidGet = BarProxy_GetKeyId();

    ASSERT_EQ(kid.id, kidGet.id);
    ASSERT_EQ(kid.scope, kidGet.scope);
    for(i=0;i<10;++i)
    {
        ASSERT_EQ(kid.key[i], kidGet.key[i]);
    }
}

TEST_F(TestCommunicationCM, Forwarding)
{
    keyId_t kid;
    keyId_t kidGet;
    uint16_t i;

    BarProxy_Init(&slip.packetCommunication);
    BarSkeleton_Init(&slip2.packetCommunication);
    PacketCommunication_Link(&slip.packetCommunication, &slip2.packetCommunication);
        
    kid.id = 0x12345678;
    kid.scope = Scope_Normal;
    for(i=0;i<10;++i)
    {
        kid.key[0] = i*128;
    }

    BarProxy_SetKeyId(&kid);
    kidGet = BarProxy_GetKeyId();

    ASSERT_EQ(kid.id, kidGet.id);
    ASSERT_EQ(kid.scope, kidGet.scope);
    for(i=0;i<10;++i)
    {
        ASSERT_EQ(kid.key[i], kidGet.key[i]);
    }
}


TEST_F(TestCommunicationCM, In_InOut_Out)
{
    uint16_t v;

    Bar_IO_In(100);
    Bar_IO_Out(&v);
    ASSERT_EQ(100,v);

    v=200;
    Bar_IO_InOut(&v);
    ASSERT_EQ(100,v);
    Bar_IO_Out(&v);
    ASSERT_EQ(200,v);
}