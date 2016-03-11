#include "services\DataModel\public\DataModel.hpp"
#include "services\DataModel\public\DataModelContent.hpp"
#include "services\DataModel\public\DataModelDebugger.hpp"
#include "services\DataModel\public\DataModelDebuggerContent.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <string>
#include <iostream>

using ::testing::StrictMock;
using ::testing::_;

enum FieldId
{
	FieldTop,
	FieldBottom
};

class TestDebugger : public service::DataModelDebugger
{
public:
    TestDebugger()
        : outputSlot(this, &TestDebugger::ProcessOutput)
        , content(*this)
    {
        OutputSignal += outputSlot;
    }
    std::string output;
    void ProcessInput(const std::string& cmd)
    {
        for(const char* txt = cmd.c_str();*txt;++txt)
            DataModelDebugger::ProcessInput(*txt);
    }
private:
    infra::Slot<TestDebugger, char> outputSlot;
    void ProcessOutput(char c)
    {
        output += c;
    }
    service::DataModelDebuggerContent content;
};

class Observer
{
    void Update()
    {
        cnt++;
    }
    Observer* Self(){return this;}
public:
    int cnt;
    infra::Slot<Observer> counterSlot; 

    Observer() : cnt(0), counterSlot(Self(), &Observer::Update){}
};

TEST(TestDataModel, NotificationUpdated)
{
    service::DataModel::FieldGenericBounded<FieldId, int, 0, 200> fi(FieldBottom, 9);
	
    Observer observerUpdated;
    Observer observerChanged;

    service::DataModel::Reader<int> rd(FieldBottom);
    service::DataModel::Writer<int> wr(FieldBottom);

    rd.UpdateNotification() += observerUpdated.counterSlot;
    rd.ChangedNotification() += observerChanged.counterSlot;

    wr = 123;
    wr = 122;
    ASSERT_EQ(2, observerUpdated.cnt);
    ASSERT_EQ(2, observerChanged.cnt);
    ASSERT_EQ(122, rd);
}

TEST(TestDataModel, NotificationChanged)
{
    service::DataModel::FieldGenericBounded<FieldId, int, 0, 200> fi(FieldBottom, 9);

    Observer observerUpdated;
    Observer observerChanged;

    service::DataModel::Reader<int> rd(FieldBottom);
    service::DataModel::Writer<int> wr(FieldBottom);

    rd.UpdateNotification() += observerUpdated.counterSlot;
    rd.ChangedNotification() += observerChanged.counterSlot;

    wr = 123;
    wr = 123;
    wr = 123;
    ASSERT_EQ(3, observerUpdated.cnt);
    ASSERT_EQ(1, observerChanged.cnt);
    ASSERT_EQ(123, rd);

    wr = 124;
    ASSERT_EQ(4, observerUpdated.cnt);
    ASSERT_EQ(2, observerChanged.cnt);
    ASSERT_EQ(124, rd);
}

TEST(TestDataModel, NewWriter)
{
	service::DataModel::FieldNonVolatileBounded<FieldId, int,0,200> fi(FieldBottom,9);
	
    service::DataModel::Reader<int> rd(FieldBottom);
    for(int i=0;i<10;++i)
    {
        service::DataModel::Writer<int> wr(FieldBottom);
        wr = i;
        ASSERT_EQ(i, rd);
    }
}

TEST(TestDataModel, Content)
{
    service::DataModelContent content;

    service::DataModel::Writer<int> wr(service::DataModelContentFieldId::NumberInt);
    service::DataModel::Reader<int> rd(service::DataModelContentFieldId::NumberInt);

    wr = 100;

    ASSERT_EQ(100, rd);
}

TEST(TestDataModel, SerializeSize)
{
    service::DataModelContent content;
    ASSERT_EQ(service::DataModelContent::NonVolatileSize, service::DataModel::Instance().SerializeSize());
}

TEST(TestDataModel, Serialize)
{
    service::DataModelContent content;
    
    uint8_t buffer[service::DataModelContent::NonVolatileSize];
    service::DataModel::Instance().Serialize(buffer);
    service::DataModel::Writer<uint32_t> wr(service::DataModelContentFieldId::N12);
    service::DataModel::Reader<uint32_t> rd(service::DataModelContentFieldId::N12);
    wr = wr + 1;
    ASSERT_EQ(3312, rd);
    service::DataModel::Instance().Deserialize(
        service::DataModel::InputMemory(buffer, service::DataModelContent::NonVolatileSize));
    ASSERT_EQ(3311, rd);
}

TEST(TestDataModel, SerializeDifferentProtocolVersion)
{
    service::DataModelContent content;

    uint8_t buffer[service::DataModelContent::NonVolatileSize];
    service::DataModel::Instance().Serialize(buffer);
    service::DataModel::Writer<uint32_t> wr(service::DataModelContentFieldId::N12);
    service::DataModel::Reader<uint32_t> rd(service::DataModelContentFieldId::N12);
    wr = wr + 1;

    uint32_t protocolVersion = 0;
    memcpy(&protocolVersion, buffer, sizeof(protocolVersion));
    protocolVersion++;
    memcpy(buffer, &protocolVersion, sizeof(protocolVersion));
    
    ASSERT_EQ(3312, rd);
    service::DataModel::Instance().Deserialize(service::DataModel::InputMemory(buffer, service::DataModelContent::NonVolatileSize));
    ASSERT_EQ(3312, rd);
}

TEST(TestDataModel, SerializeRemovedField)
{
    uint8_t buffer[100];
    uint32_t s;
    {
        service::DataModel::FieldNonVolatile<FieldId, int> fi(FieldBottom, 9);
        s = service::DataModel::Instance().Serialize(buffer);
    }
    service::DataModel::FieldNonVolatile<FieldId, int> fi(FieldTop, 100);
    service::DataModel::Instance().Deserialize(service::DataModel::InputMemory(buffer, s));
    service::DataModel::Reader<int> rd(FieldTop);
    ASSERT_EQ(100, rd);
}

TEST(TestDataModel, InvalidSizeDeserialize)
{
    service::DataModel::FieldNonVolatile<FieldId, int> fi(FieldBottom, 9);
    service::DataModel::Writer<int> wr(FieldBottom);

    wr = 50;
    std::array<uint8_t, 100> buffer;
    uint32_t s = service::DataModel::Instance().Serialize(buffer.data());
    
    wr = 100;
    // find size
    ASSERT_EQ(10, buffer[4]);
    buffer[4] = buffer[4] + 1; // invalid size should be ignored
    service::DataModel::Instance().Deserialize(service::DataModel::InputMemory(buffer.data(), s));
    ASSERT_EQ(100, wr);
}

TEST(TestDataModel, InvalidSizeZeroDeserialize)
{
    service::DataModel::FieldNonVolatile<FieldId, int> fi(FieldBottom, 9);
    service::DataModel::Writer<int> wr(FieldBottom);

    wr = 50;
    std::array<uint8_t, 100> buffer;
    uint32_t s = service::DataModel::Instance().Serialize(buffer.data());

    wr = 100;
    // find size
    ASSERT_EQ(10, buffer[4]);
    buffer[4] = 0; // invalid size should be ignored
    ASSERT_FALSE(service::DataModel::Instance().Deserialize(service::DataModel::InputMemory(buffer.data(), s)));
    ASSERT_EQ(100, wr);
}
TEST(TestDataModel, NotifyChangedNonVolatile)
{
    service::DataModelContent content;

    Observer obs;

    service::DataModel::Instance().NonVolatileFieldChanged += obs.counterSlot;
    
    service::DataModel::Writer<int16_t>  wr(service::DataModelContentFieldId::NumberShort);
    service::DataModel::Writer<uint32_t> wrV(service::DataModelContentFieldId::N12);
    
    ASSERT_EQ(0, obs.cnt);
    wr = 1;
    ASSERT_EQ(0, obs.cnt);
    wrV = 1;
    ASSERT_EQ(1, obs.cnt);
    wrV = 1;
    ASSERT_EQ(1, obs.cnt);

    service::DataModel::Instance().NonVolatileFieldChanged -= obs.counterSlot;
}

TEST(TestDataModel, NoNotifyChangedNonVolatileDuringDeserialization)
{
    service::DataModelContent content;
    Observer obs;
    service::DataModel::Instance().NonVolatileFieldChanged += obs.counterSlot;
    uint8_t buffer[service::DataModelContent::NonVolatileSize];
    service::DataModel::Instance().Serialize(buffer);
    service::DataModel::Writer<uint32_t> wr1(service::DataModelContentFieldId::N12);
    service::DataModel::Reader<uint32_t> rd1(service::DataModelContentFieldId::N12);
    service::DataModel::Writer<uint32_t> wr2(service::DataModelContentFieldId::N13);
    service::DataModel::Reader<uint32_t> rd2(service::DataModelContentFieldId::N13);
	wr1 = wr1 + 1;
	wr2 = wr2 + 1;
	ASSERT_EQ(3312, rd1);
	ASSERT_EQ(14, rd2);
	ASSERT_EQ(2, obs.cnt);
    service::DataModel::Instance().Deserialize(service::DataModel::InputMemory(buffer, service::DataModelContent::NonVolatileSize));
	ASSERT_EQ(3311, rd1);
	ASSERT_EQ(13, rd2);
	ASSERT_EQ(2, obs.cnt);

    service::DataModel::Instance().NonVolatileFieldChanged -= obs.counterSlot;
}

TEST(TestDataModel, 2Writers)
{
    service::DataModelContent content;
  
    service::DataModel::Writer<uint32_t> wr1(service::DataModelContentFieldId::N12);
    service::DataModel::Writer<uint32_t> wr2(service::DataModelContentFieldId::N12);
    
    wr1=4000;
    ASSERT_EQ(4000, wr1);
    ASSERT_EQ(4000, wr2);
    wr2=400;
    ASSERT_EQ(400, wr1);
    ASSERT_EQ(400, wr2);
    ASSERT_TRUE(wr1.Lock());
    ASSERT_TRUE(wr1.Lock());
    ASSERT_FALSE(wr2.Lock());
    wr1=123;
    ASSERT_EQ(123, wr1);
    ASSERT_EQ(123, wr2);
    wr2=1;
    ASSERT_EQ(123, wr1);
    ASSERT_EQ(123, wr2);

    wr1.Unlock();
    wr2.Unlock();
    wr1 = 4000;
    ASSERT_EQ(4000, wr1);
    ASSERT_EQ(4000, wr2);
    wr2 = 400;
    ASSERT_EQ(400, wr1);
    ASSERT_EQ(400, wr2);
}

TEST(TestDataModel, UnknownField)
{
    EXPECT_THROW(service::DataModel::Writer<uint32_t> wr(service::DataModelContentFieldId::N12), std::exception);
    EXPECT_THROW(service::DataModel::Reader<uint32_t> rd(service::DataModelContentFieldId::N12), std::exception);
}

TEST(TestDataModel, Clipping)
{
    service::DataModelContent content;

    service::DataModel::Writer<int> wr(service::DataModelContentFieldId::NumberInt);

    ASSERT_EQ(1000, wr);
    wr = -1000;
    ASSERT_EQ(0, wr);
    wr = 123456;
    ASSERT_EQ(1000, wr);
}

TEST(TestDataModel, StructField)
{
	service::DataModelContent content;

	TestConfig config;
	config.data[0] = 1;
	config.data[1] = 2;
	config.index = 3;
    service::DataModel::Writer<TestConfig> wr(service::DataModelContentFieldId::TestConfig1);

	wr = config;
    TestConfig fromDataModel = wr;

	ASSERT_EQ(config.data[0], fromDataModel.data[0]);
	ASSERT_EQ(config.data[1], fromDataModel.data[1]);
	ASSERT_EQ(config.index,   fromDataModel.index);
}

TEST(TestDataModel, DebuggerPrint)
{
    service::DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("p\n");
    
    //std::cout << debugger.output << std::endl;
    std::string expected = 
    "--------------------\r\n"
    "102 , NofPendingBNPMessages : 0\r\n"
    "101 , MobileConnected : False\r\n"
	"14  , TestConfig1 : -\r\n"
 	"13  , N13        : 13\r\n"
	"12  , N12        : 3311\r\n"
    "11  , N11        : 33\r\n"
	"10  , N10        : 33\r\n"
    "1   , NumberShort : -678\r\n"
    "0   , NumberInt  : 1000\r\n"
    "---End---\r\n";
    ASSERT_EQ(expected, debugger.output);
}

TEST(TestDataModel, DebuggerPrintInfo)
{
    service::DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("P0\n");

    std::string expected =
        "   0, NumberInt  : 1000\r\n";
    ASSERT_EQ(expected, debugger.output);
    debugger.output = "";

    debugger.ProcessInput("l0\n");
    debugger.ProcessInput("P0\n");

    expected =
        "L  0, NumberInt  : 1000\r\n";
    ASSERT_EQ(expected, debugger.output);
    debugger.output = "";

    debugger.ProcessInput("w0 1\n");

    debugger.ProcessInput("P0\n");

    expected =
        "LS 0, NumberInt  : 1\r\n";
    ASSERT_EQ(expected, debugger.output);
}

TEST(TestDataModel, DebuggerPrintFieldName)
{
    service::DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("pNu\n");

    std::cout << debugger.output << std::endl;
    std::string expected =
        "1   , NumberShort : -678\r\n"
        "0   , NumberInt  : 1000\r\n";
    ASSERT_EQ(expected, debugger.output);
}

TEST(TestDataModel, DebuggerPrintFieldId)
{
    service::DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("p10\n");

    //std::cout << debugger.output << std::endl;
    std::string expected = "10  , N10        : 33\r\n";
    ASSERT_EQ(expected, debugger.output);
}

TEST(TestDataModel, DebuggerObserve)
{
    service::DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("a1\n");

    service::DataModel::Writer<int16_t> wr(service::DataModelContentFieldId::NumberShort);
    wr = 123;

    std::string expected = "1   , NumberShort : 123\r\n";
    ASSERT_EQ(expected, debugger.output);
    debugger.output = "";

    debugger.ProcessInput("d 1 \n");
    wr = 321;
    ASSERT_EQ("", debugger.output);
}

TEST(TestDataModel, DebuggerWriteId)
{
    service::DataModelContent content;
    TestDebugger debugger;
    service::DataModel::Reader<int16_t> rd(service::DataModelContentFieldId::NumberShort);
    debugger.ProcessInput("w1 135\n");
    ASSERT_EQ(135, rd);

    debugger.ProcessInput("w  1  513\n");
    ASSERT_EQ(513, rd);
}

TEST(TestDataModel, DebuggerWriteName)
{
    service::DataModelContent content;
    TestDebugger debugger;
    service::DataModel::Reader<int16_t> rd(service::DataModelContentFieldId::NumberShort);
    debugger.ProcessInput("wNumberShort 135\n");
    ASSERT_EQ(135, rd);

    debugger.ProcessInput("w  NumberShort  513\n");
    ASSERT_EQ(513, rd);
}

TEST(TestDataModel, IsSetSameValue)
{
    service::DataModelContent content;
    service::DataModel::Writer<int16_t> wr(service::DataModelContentFieldId::NumberShort);
    service::DataModel::Reader<int16_t> rd(service::DataModelContentFieldId::NumberShort);

    ASSERT_FALSE(wr.IsSet());

    int16_t v = rd;

    wr = v;

    ASSERT_TRUE(wr.IsSet());
}

TEST(TestDataModel, IsSetDifferentValue)
{
    service::DataModelContent content;
    service::DataModel::Writer<int16_t> wr(service::DataModelContentFieldId::NumberShort);
    service::DataModel::Reader<int16_t> rd(service::DataModelContentFieldId::NumberShort);

    ASSERT_FALSE(wr.IsSet());

    int16_t v = rd;
    v++;
    wr = v;

    ASSERT_TRUE(wr.IsSet());
}

TEST(TestDataModel, IsSetAfterDeserialize)
{
    std::array<uint8_t, service::DataModelContent::NonVolatileSize> buffer;
    {
        service::DataModelContent content;
        service::DataModel::Writer<uint32_t> wr(service::DataModelContentFieldId::N12);
        wr = wr + 1;
        ASSERT_TRUE(wr.IsSet());
        service::DataModel::Instance().Serialize(buffer.data());
    }
    {
        service::DataModelContent content;
        service::DataModel::Reader<uint32_t> rd(service::DataModelContentFieldId::N12);
        ASSERT_FALSE(rd.IsSet());
        service::DataModel::Instance().Deserialize(service::DataModel::InputMemory(buffer.data(), buffer.size()));
        ASSERT_TRUE(rd.IsSet());
    }
}

TEST(TestDataModel, ResetToDefaultField)
{
    service::DataModelContent content;
    service::DataModel::Writer<uint32_t> wr(service::DataModelContentFieldId::N12);

    wr = wr + 1;
    wr.ResetToDefault();
    EXPECT_EQ(3311, wr);
}

//FIELD_MINMAX(  10, N10,         uint8_t,    33,     10,  100)
//FIELD(         11, N11,         uint8_t,    33)
//FIELD_P_MINMAX(12, N12,         uint32_t, 3311,0,100000000)
//FIELD_P(       13, N13,         uint32_t, 13)

TEST(TestDataModel, ResetToDefaultFieldNonVolatile)
{
    service::DataModelContent content;
    service::DataModel::Writer<uint8_t> wr1(service::DataModelContentFieldId::N10);
    service::DataModel::Writer<uint8_t> wr2(service::DataModelContentFieldId::N11);
    service::DataModel::Writer<uint32_t> wr3(service::DataModelContentFieldId::N12);
    service::DataModel::Writer<uint32_t> wr4(service::DataModelContentFieldId::N13);

    wr1 = wr1 + 1;
    wr2 = wr2 + 2;
    wr3 = wr3 + 3;
    wr4 = wr4 + 4;

    service::DataModel::Instance().ResetToDefault(true);

    EXPECT_EQ(34, wr1);
    EXPECT_EQ(35, wr2);
    EXPECT_EQ(3311, wr3);
    EXPECT_EQ(13, wr4);
}

TEST(TestDataModel, ResetToDefaultFieldAll)
{
    service::DataModelContent content;
    service::DataModel::Writer<uint8_t> wr1(service::DataModelContentFieldId::N10);
    service::DataModel::Writer<uint8_t> wr2(service::DataModelContentFieldId::N11);
    service::DataModel::Writer<uint32_t> wr3(service::DataModelContentFieldId::N12);
    service::DataModel::Writer<uint32_t> wr4(service::DataModelContentFieldId::N13);

    wr1 = wr1 + 1;
    wr2 = wr2 + 2;
    wr3 = wr3 + 3;
    wr4 = wr4 + 4;

    service::DataModel::Instance().ResetToDefault(false);

    EXPECT_EQ(33, wr1);
    EXPECT_EQ(33, wr2);
    EXPECT_EQ(3311, wr3);
    EXPECT_EQ(13, wr4);
}

TEST(TestDataModel, ReaderWriterId)
{
    service::DataModelContent content;

    service::DataModelContent::Writer<service::DataModelContentFieldId::N10> n10wr;
    service::DataModelContent::Reader<service::DataModelContentFieldId::N10> n10rd;

    service::DataModelContent::IdToType<service::DataModelContentFieldId::N10>::Type value = 99;
    n10wr = value;

    EXPECT_EQ(value, n10rd);
}
