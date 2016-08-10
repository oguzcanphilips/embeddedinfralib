#include "gmock/gmock.h"
#include "hal/synchronous_interfaces/test_doubles/public/SynchronousSerialCommunicationMock.hpp"
#include "packs/upgrade/boot_loader/public/DiCommUart.hpp"

class TimeKeeperStub
    : public hal::TimeKeeper
{
public:
    virtual bool Timeout() override
    {
        return false;
    }

    virtual void Reset() override
    {}
};

class TimeKeeperMock
    : public hal::TimeKeeper
{
public:
    virtual bool Timeout() override
    {
        return TimeoutMock();
    }

    virtual void Reset() override
    {
        ResetMock();
    }

    MOCK_METHOD0(TimeoutMock, bool());
    MOCK_METHOD0(ResetMock, void());
};

class DiCommUartTest
    : public testing::Test
{
public:
    DiCommUartTest()
        : diComm(communication, timeKeeper)
    {}

    testing::StrictMock<hal::SynchronousSerialCommunicationMock> communication;
    TimeKeeperStub timeKeeper;
    application::DiCommUart diComm;
    testing::InSequence s;

    struct Ok {};

    void ExpectInitialize()
    {
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{0xfe, 0xff }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1 }));     // Initialize
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 2 }));  // Length
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0x2e, 0x3e }));
    }

    void ExpectPutProps()
    {
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xfe, 0xff }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 3 }));     // PutProps
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 12 })); // Length
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 'p', 'o', 'r', 't' }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 'v', 'a', 'l', 'u', 'e', 's' }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0x2d, 0x40 }));
    }

    void ExpectGetProps()
    {
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xfe, 0xff }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 4 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 5 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 'p', 'o', 'r', 't' }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xf3, 0x43 }));
    }

    infra::BoundedString::WithStorage<0> ignoredResult;
};

TEST_F(DiCommUartTest, InitializeSendsRequest)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xfe, 0xff }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1 }));     // Initialize
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 2 }));  // Length
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0x2e, 0x3e })).WillOnce(testing::Throw<Ok>(Ok()));
    EXPECT_THROW(diComm.Initialize(), Ok);
}

TEST_F(DiCommUartTest, InitializeParsesResponse)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.Initialize());
}

TEST_F(DiCommUartTest, InitializeResponseSearchesForStart)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.Initialize());
}

TEST_F(DiCommUartTest, InitializeResponseSearchesForSecondStart)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.Initialize());
}

TEST_F(DiCommUartTest, InitializeResponseSearchesForResponse)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 9 })));  // Not a response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.Initialize());
}

TEST_F(DiCommUartTest, InitializeFailsWhenResponseFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, InitializeResponseChecksCrc)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 0 })));  // CRC
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataFirstStartFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 0xfe })));
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataSecondStartFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 0xff })));
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataOperationFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataLengthFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataStatusFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataValuesFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, WhenReceiveDataCrcFailsInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(false, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_FALSE(diComm.Initialize());
}

TEST_F(DiCommUartTest, PutPropsSendsRequest)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xfe, 0xff }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 3 }));     // PutProps
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 12 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 'p', 'o', 'r', 't' }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 'v', 'a', 'l', 'u', 'e', 's' }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0x2d, 0x40,  })).WillOnce(testing::Throw<Ok>(Ok()));
    EXPECT_THROW(diComm.PutProps("port", "values", ignoredResult), Ok);
}

TEST_F(DiCommUartTest, PutPropsParsesResponse)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.PutProps("port", "values", ignoredResult));
}

TEST_F(DiCommUartTest, PutPropsResponseSearchesForStart)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.PutProps("port", "values", ignoredResult));
}

TEST_F(DiCommUartTest, PutPropsResponseSearchesForSecondStart)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.PutProps("port", "values", ignoredResult));
}

TEST_F(DiCommUartTest, PutPropsResponseSearchesForResponse)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 2 })));  // Not a response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_TRUE(diComm.PutProps("port", "values", ignoredResult));
}

TEST_F(DiCommUartTest, PutPropsFailsWhenResponseFails)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    EXPECT_FALSE(diComm.PutProps("port", "values", ignoredResult));
}

TEST_F(DiCommUartTest, PutPropsResponseChecksCrc)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 0 })));  // CRC
    EXPECT_FALSE(diComm.PutProps("port", "values", ignoredResult));
}

TEST_F(DiCommUartTest, PutPropsParsesValues)
{
    ExpectPutProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 4 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ '{', '}', 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xd2, 0x3c  })));  // CRC
    infra::BoundedString::WithStorage<10> values;
    EXPECT_TRUE(diComm.PutProps("port", "values", values));
    EXPECT_EQ("{}", values);
}

TEST_F(DiCommUartTest, GetPropsSendsRequest)
{
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xfe, 0xff }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 4 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 5 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 'p', 'o', 'r', 't' }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
    EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0xf3, 0x43 })).WillOnce(testing::Throw<Ok>(Ok()));
    infra::BoundedString::WithStorage<128> values;
    EXPECT_THROW(diComm.GetProps("port", values), Ok);
}

TEST_F(DiCommUartTest, GetPropsReceivesResponse)
{
    ExpectGetProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 2 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x1d, 0x0f })));  // CRC
    infra::BoundedString::WithStorage<128> values;
    EXPECT_TRUE(diComm.GetProps("port", values));
    EXPECT_EQ("", values);
}

TEST_F(DiCommUartTest, GetPropsReceivesNonEmptyString)
{
    ExpectGetProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 4 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 'a', 'b', 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x45, 0xd3 })));  // CRC
    infra::BoundedString::WithStorage<128> values;
    EXPECT_TRUE(diComm.GetProps("port", values));
    EXPECT_EQ("ab", values);
}

TEST_F(DiCommUartTest, GetPropsFailsOnNonZeroStatus)
{
    ExpectGetProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 4 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 1 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 'a', 'b', 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x45, 0xd3 })));  // CRC
    infra::BoundedString::WithStorage<128> values;
    EXPECT_FALSE(diComm.GetProps("port", values));
}

TEST_F(DiCommUartTest, GetPropsFailsOnReceivePartialString)
{
    ExpectGetProps();

    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xff })));
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 7 })));  // Response
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0, 4 })));  // Length
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Status
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 'a' })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 'b' })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0 })));  // Values
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0x45, 0xd3 })));  // CRC
    infra::BoundedString::WithStorage<0> values;
    EXPECT_FALSE(diComm.GetProps("port", values));
}

class DiCommUartTimeoutTest
    : public testing::Test
{
public:
    DiCommUartTimeoutTest()
        : diComm(communication, timeKeeper)
    {}

    testing::StrictMock<hal::SynchronousSerialCommunicationMock> communication;
    testing::StrictMock<TimeKeeperMock> timeKeeper;
    application::DiCommUart diComm;
    testing::InSequence s;

    struct Ok {};

    void ExpectInitialize()
    {
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{0xfe, 0xff }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1 }));     // Initialize
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0, 2 }));  // Length
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 1 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0 }));
        EXPECT_CALL(communication, SendDataMock(std::vector<uint8_t>{ 0x2e, 0x3e  }));
    }
};

TEST_F(DiCommUartTimeoutTest, OnTimeoutInitializeFails)
{
    ExpectInitialize();

    EXPECT_CALL(timeKeeper, ResetMock());
    EXPECT_CALL(communication, ReceiveDataMock()).WillOnce(testing::Return(std::make_pair(true, std::vector<uint8_t>{ 0xfe })));
    EXPECT_CALL(timeKeeper, TimeoutMock()).WillOnce(testing::Return(true));
    EXPECT_FALSE(diComm.Initialize());
}

