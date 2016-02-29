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

class TestDebugger : public DataModelDebugger
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
    DataModelDebuggerContent content;
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
	DataModel::FieldGenericBounded<FieldId,int,0,200> fi(FieldBottom,9);
	
    Observer observerUpdated;
    Observer observerChanged;

    DataModel::Reader<int> rd(FieldBottom);
    DataModel::Writer<int> wr(FieldBottom);

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
    DataModel::FieldGenericBounded<FieldId, int, 0, 200> fi(FieldBottom, 9);

    Observer observerUpdated;
    Observer observerChanged;

    DataModel::Reader<int> rd(FieldBottom);
    DataModel::Writer<int> wr(FieldBottom);

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
	DataModel::FieldNonVolatileBounded<FieldId, int,0,200> fi(FieldBottom,9);
	
    DataModel::Reader<int> rd(FieldBottom);
    for(int i=0;i<10;++i)
    {
        DataModel::Writer<int> wr(FieldBottom);
        wr = i;
        ASSERT_EQ(i, rd);
    }
}

TEST(TestDataModel, Content)
{
    DataModelContent content;

    DataModel::Writer<int> wr(DataModelContentFieldId::NumberInt);
    DataModel::Reader<int> rd(DataModelContentFieldId::NumberInt);

    wr = 100;

    ASSERT_EQ(100, rd);
}

TEST(TestDataModel, SerializeSize)
{
    DataModelContent content;
    ASSERT_EQ(DataModelContent::NonVolatileSize, DataModel::Instance().SerializeSize());
}

TEST(TestDataModel, Serialize)
{
    DataModelContent content;
    
    uint8_t buffer[DataModelContent::NonVolatileSize];
    DataModel::Instance().Serialize(buffer);
    DataModel::Writer<uint32_t> wr(DataModelContentFieldId::N12);
    DataModel::Reader<uint32_t> rd(DataModelContentFieldId::N12);
    wr = wr + 1;
    ASSERT_EQ(3312, rd);
    DataModel::Instance().Deserialize(
        DataModel::InputMemory(buffer, DataModelContent::NonVolatileSize));
    ASSERT_EQ(3311, rd);
}

TEST(TestDataModel, SerializeDifferentProtocolVersion)
{
    DataModelContent content;

    uint8_t buffer[DataModelContent::NonVolatileSize];
    DataModel::Instance().Serialize(buffer);
    DataModel::Writer<uint32_t> wr(DataModelContentFieldId::N12);
    DataModel::Reader<uint32_t> rd(DataModelContentFieldId::N12);
    wr = wr + 1;

    uint32_t protocolVersion = 0;
    memcpy(&protocolVersion, buffer, sizeof(protocolVersion));
    protocolVersion++;
    memcpy(buffer, &protocolVersion, sizeof(protocolVersion));
    
    ASSERT_EQ(3312, rd);
    DataModel::Instance().Deserialize(DataModel::InputMemory(buffer, DataModelContent::NonVolatileSize));
    ASSERT_EQ(3312, rd);
}

TEST(TestDataModel, SerializeRemovedField)
{
    uint8_t buffer[100];
    uint32_t s;
    {
        DataModel::FieldNonVolatile<FieldId, int> fi(FieldBottom, 9);
        s = DataModel::Instance().Serialize(buffer);
    }
    DataModel::FieldNonVolatile<FieldId, int> fi(FieldTop, 100);
    DataModel::Instance().Deserialize(DataModel::InputMemory(buffer, s));
    DataModel::Reader<int> rd(FieldTop);
    ASSERT_EQ(100, rd);
}

TEST(TestDataModel, InvalidSizeDeserialize)
{
    DataModel::FieldNonVolatile<FieldId, int> fi(FieldBottom, 9);
    DataModel::Writer<int> wr(FieldBottom);

    wr = 50;
    std::array<uint8_t, 100> buffer;
    uint32_t s = DataModel::Instance().Serialize(buffer.data());
    
    wr = 100;
    // find size
    ASSERT_EQ(10, buffer[4]);
    buffer[4] = buffer[4] + 1; // invalid size should be ignored
    DataModel::Instance().Deserialize(DataModel::InputMemory(buffer.data(), s));
    ASSERT_EQ(100, wr);
}

TEST(TestDataModel, InvalidSizeZeroDeserialize)
{
    DataModel::FieldNonVolatile<FieldId, int> fi(FieldBottom, 9);
    DataModel::Writer<int> wr(FieldBottom);

    wr = 50;
    std::array<uint8_t, 100> buffer;
    uint32_t s = DataModel::Instance().Serialize(buffer.data());

    wr = 100;
    // find size
    ASSERT_EQ(10, buffer[4]);
    buffer[4] = 0; // invalid size should be ignored
    ASSERT_FALSE(DataModel::Instance().Deserialize(DataModel::InputMemory(buffer.data(), s)));
    ASSERT_EQ(100, wr);
}
TEST(TestDataModel, NotifyChangedNonVolatile)
{
    DataModelContent content;

    Observer obs;

    DataModel::Instance().NonVolatileFieldChanged += obs.counterSlot;
    
    DataModel::Writer<int16_t>  wr(DataModelContentFieldId::NumberShort);
    DataModel::Writer<uint32_t> wrV(DataModelContentFieldId::N12);
    
    ASSERT_EQ(0, obs.cnt);
    wr = 1;
    ASSERT_EQ(0, obs.cnt);
    wrV = 1;
    ASSERT_EQ(1, obs.cnt);
    wrV = 1;
    ASSERT_EQ(1, obs.cnt);

    DataModel::Instance().NonVolatileFieldChanged -= obs.counterSlot;
}

TEST(TestDataModel, NoNotifyChangedNonVolatileDuringDeserialization)
{
    DataModelContent content;
    Observer obs;
    DataModel::Instance().NonVolatileFieldChanged += obs.counterSlot;
    uint8_t buffer[DataModelContent::NonVolatileSize];
    DataModel::Instance().Serialize(buffer);
    DataModel::Writer<uint32_t> wr1(DataModelContentFieldId::N12);
    DataModel::Reader<uint32_t> rd1(DataModelContentFieldId::N12);
    DataModel::Writer<uint32_t> wr2(DataModelContentFieldId::N13);
    DataModel::Reader<uint32_t> rd2(DataModelContentFieldId::N13);
	wr1 = wr1 + 1;
	wr2 = wr2 + 1;
	ASSERT_EQ(3312, rd1);
	ASSERT_EQ(14, rd2);
	ASSERT_EQ(2, obs.cnt);
    DataModel::Instance().Deserialize(DataModel::InputMemory(buffer, DataModelContent::NonVolatileSize));
	ASSERT_EQ(3311, rd1);
	ASSERT_EQ(13, rd2);
	ASSERT_EQ(2, obs.cnt);

    DataModel::Instance().NonVolatileFieldChanged -= obs.counterSlot;
}

TEST(TestDataModel, 2Writers)
{
    DataModelContent content;
  
    DataModel::Writer<uint32_t> wr1(DataModelContentFieldId::N12);
    DataModel::Writer<uint32_t> wr2(DataModelContentFieldId::N12);
    
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
    EXPECT_THROW(DataModel::Writer<uint32_t> wr(DataModelContentFieldId::N12), std::exception);
    EXPECT_THROW(DataModel::Reader<uint32_t> rd(DataModelContentFieldId::N12), std::exception);
}

TEST(TestDataModel, Clipping)
{
    DataModelContent content;

    DataModel::Writer<int> wr(DataModelContentFieldId::NumberInt);

    ASSERT_EQ(1000, wr);
    wr = -1000;
    ASSERT_EQ(0, wr);
    wr = 123456;
    ASSERT_EQ(1000, wr);
}

TEST(TestDataModel, StructField)
{
	DataModelContent content;

	TestConfig config;
	config.data[0] = 1;
	config.data[1] = 2;
	config.index = 3;
    DataModel::Writer<TestConfig> wr(DataModelContentFieldId::TestConfig1);

	wr = config;
    TestConfig fromDataModel = wr;

	ASSERT_EQ(config.data[0], fromDataModel.data[0]);
	ASSERT_EQ(config.data[1], fromDataModel.data[1]);
	ASSERT_EQ(config.index,   fromDataModel.index);
}

TEST(TestDataModel, DebuggerPrint)
{
    DataModelContent content;
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
    DataModelContent content;
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
    DataModelContent content;
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
    DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("p10\n");

    //std::cout << debugger.output << std::endl;
    std::string expected = "10  , N10        : 33\r\n";
    ASSERT_EQ(expected, debugger.output);
}

TEST(TestDataModel, DebuggerObserve)
{
    DataModelContent content;
    TestDebugger debugger;

    debugger.ProcessInput("a1\n");

    DataModel::Writer<int16_t> wr(DataModelContentFieldId::NumberShort);
    wr = 123;

    std::string expected = "1   , NumberShort : 123\r\n";
    ASSERT_EQ(expected, debugger.output);
    debugger.output = "";

    debugger.ProcessInput("dX1X\n");
    wr = 321;
    ASSERT_EQ("", debugger.output);
}

TEST(TestDataModel, DebuggerWriteId)
{
    DataModelContent content;
    TestDebugger debugger;
    DataModel::Reader<int16_t> rd(DataModelContentFieldId::NumberShort);
    debugger.ProcessInput("w1 135\n");
    ASSERT_EQ(135, rd);

    debugger.ProcessInput("w  1  513\n");
    ASSERT_EQ(513, rd);
}

TEST(TestDataModel, DebuggerWriteName)
{
    DataModelContent content;
    TestDebugger debugger;
    DataModel::Reader<int16_t> rd(DataModelContentFieldId::NumberShort);
    debugger.ProcessInput("wNumberShort 135\n");
    ASSERT_EQ(135, rd);

    debugger.ProcessInput("w  NumberShort  513\n");
    ASSERT_EQ(513, rd);
}

TEST(TestDataModel, IsSetSameValue)
{
    DataModelContent content;
    DataModel::Writer<int16_t> wr(DataModelContentFieldId::NumberShort);
    DataModel::Reader<int16_t> rd(DataModelContentFieldId::NumberShort);

    ASSERT_FALSE(wr.IsSet());

    int16_t v = rd;

    wr = v;

    ASSERT_TRUE(wr.IsSet());
}

TEST(TestDataModel, IsSetDifferentValue)
{
    DataModelContent content;
    DataModel::Writer<int16_t> wr(DataModelContentFieldId::NumberShort);
    DataModel::Reader<int16_t> rd(DataModelContentFieldId::NumberShort);

    ASSERT_FALSE(wr.IsSet());

    int16_t v = rd;
    v++;
    wr = v;

    ASSERT_TRUE(wr.IsSet());
}

TEST(TestDataModel, IsSetAfterDeserialize)
{
    std::array<uint8_t, DataModelContent::NonVolatileSize> buffer;
    {
        DataModelContent content;
        DataModel::Writer<uint32_t> wr(DataModelContentFieldId::N12);
        wr = wr + 1;
        ASSERT_TRUE(wr.IsSet());
        DataModel::Instance().Serialize(buffer.data());
    }
    {
        DataModelContent content;
        DataModel::Reader<uint32_t> rd(DataModelContentFieldId::N12);
        ASSERT_FALSE(rd.IsSet());
        DataModel::Instance().Deserialize(DataModel::InputMemory(buffer.data(), buffer.size()));
        ASSERT_TRUE(rd.IsSet());
    }
}

TEST(TestDataModel, ResetToDefaultField)
{
    DataModelContent content;
    DataModel::Writer<uint32_t> wr(DataModelContentFieldId::N12);

    wr = wr + 1;
    wr.ResetToDefault();
    EXPECT_EQ(3311, wr);
}
/*
FIELD_MINMAX(  10, N10,         uint8_t,    33,     10,  100)
FIELD(         11, N11,         uint8_t,    33)
FIELD_P_MINMAX(12, N12,         uint32_t, 3311,0,100000000)
FIELD_P(       13, N13,         uint32_t, 13)
*/
TEST(TestDataModel, ResetToDefaultFieldNonVolatile)
{
    DataModelContent content;
    DataModel::Writer<uint8_t> wr1(DataModelContentFieldId::N10);
    DataModel::Writer<uint8_t> wr2(DataModelContentFieldId::N11);
    DataModel::Writer<uint32_t> wr3(DataModelContentFieldId::N12);
    DataModel::Writer<uint32_t> wr4(DataModelContentFieldId::N13);

    wr1 = wr1 + 1;
    wr2 = wr2 + 2;
    wr3 = wr3 + 3;
    wr4 = wr4 + 4;

    DataModel::Instance().ResetToDefault(true);

    EXPECT_EQ(34, wr1);
    EXPECT_EQ(35, wr2);
    EXPECT_EQ(3311, wr3);
    EXPECT_EQ(13, wr4);
}

TEST(TestDataModel, ResetToDefaultFieldAll)
{
    DataModelContent content;
    DataModel::Writer<uint8_t> wr1(DataModelContentFieldId::N10);
    DataModel::Writer<uint8_t> wr2(DataModelContentFieldId::N11);
    DataModel::Writer<uint32_t> wr3(DataModelContentFieldId::N12);
    DataModel::Writer<uint32_t> wr4(DataModelContentFieldId::N13);

    wr1 = wr1 + 1;
    wr2 = wr2 + 2;
    wr3 = wr3 + 3;
    wr4 = wr4 + 4;

    DataModel::Instance().ResetToDefault(false);

    EXPECT_EQ(33, wr1);
    EXPECT_EQ(33, wr2);
    EXPECT_EQ(3311, wr3);
    EXPECT_EQ(13, wr4);
}

TEST(TestDataModel, ReaderWriterId)
{
    DataModelContent content;

    DataModelContent::Writer<DataModelContentFieldId::N10> n10wr;
    DataModelContent::Reader<DataModelContentFieldId::N10> n10rd;

    DataModelContent::IdToType<DataModelContentFieldId::N10>::Type value = 99;
    n10wr = value;

    EXPECT_EQ(value, n10rd);
}
