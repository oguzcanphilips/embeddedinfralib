#include "gtest/gtest.h"
#include "infra/event/test_helper/public/EventDispatcherFixture.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"
#include "hal/interfaces/test_doubles/public/I2cRegisterAccessMock.hpp"

class RegisterAccessMockTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    RegisterAccessMockTest()
        : master(masterMock)
    {}

    hal::I2cMasterRegisterAccessMock masterMock;
    hal::I2cMaster& master;
};

TEST_F(RegisterAccessMockTest, ReadRegisterInOneRead)
{
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDoneVerifier(hal::Result::complete, 1);
    infra::VerifyingFunctionMock<void(hal::Result)> receiveDataDoneVerifier(hal::Result::complete);
    EXPECT_CALL(masterMock, ReadRegisterMock(3)).WillOnce(testing::Return(std::vector<uint8_t>{ 1, 2, 3, 4 }));

    uint8_t dataRegister = 3;
    std::array<uint8_t, 4> data;
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(dataRegister), hal::Action::repeatedStart, 100000, sendDataDoneVerifier);
    master.ReceiveData(hal::I2cAddress(1), infra::MakeByteRange(data), hal::Action::stop, 100000, receiveDataDoneVerifier);

    EXPECT_EQ((std::array<uint8_t, 4>{ 1, 2, 3, 4 }), data);
}

TEST_F(RegisterAccessMockTest, ReadRegisterInTwoReads)
{
    infra::VerifyingFunctionMock<void(hal::Result)> receiveDataDone1Verifier(hal::Result::complete);
    infra::VerifyingFunctionMock<void(hal::Result)> receiveDataDone2Verifier(hal::Result::complete);
    EXPECT_CALL(masterMock, ReadRegisterMock(3)).WillOnce(testing::Return(std::vector<uint8_t>{ 1, 2, 3, 4 }));

    uint8_t dataRegister = 3;
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(dataRegister), hal::Action::repeatedStart, 100000, [](hal::Result, uint32_t numberOfBytesSent) {});
    
    std::array<uint8_t, 3> data1;
    std::array<uint8_t, 1> data2;
    master.ReceiveData(hal::I2cAddress(1), infra::MakeByteRange(data1), hal::Action::continueSession, 100000, receiveDataDone1Verifier);
    master.ReceiveData(hal::I2cAddress(1), infra::MakeByteRange(data2), hal::Action::stop, 100000, receiveDataDone2Verifier);
    EXPECT_EQ((std::array<uint8_t, 3>{ 1, 2, 3 }), data1);
    EXPECT_EQ((std::array<uint8_t, 1>{ 4 }), data2);
}

TEST_F(RegisterAccessMockTest, WriteRegisterInOneWrite)
{
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDoneVerifier(hal::Result::complete, 5);
    EXPECT_CALL(masterMock, WriteRegisterMock(3, std::vector<uint8_t>{ 1, 2, 3, 4 }));

    std::array<uint8_t, 5> data = { 3, 1, 2, 3, 4 };
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(data), hal::Action::stop, 100000, sendDataDoneVerifier);
}

TEST_F(RegisterAccessMockTest, WriteRegisterInTwoWrites)
{
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDone1Verifier(hal::Result::complete, 1);
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDone2Verifier(hal::Result::complete, 4);
    EXPECT_CALL(masterMock, WriteRegisterMock(3, std::vector<uint8_t>{ 1, 2, 3, 4 }));

    uint8_t dataRegister = 3;
    std::array<uint8_t, 4> data = { 1, 2, 3, 4 };
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(dataRegister), hal::Action::continueSession, 100000, sendDataDone1Verifier);
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(data), hal::Action::stop, 100000, sendDataDone2Verifier);
}

TEST_F(RegisterAccessMockTest, WriteRegisterInThreeWrites)
{
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDone1Verifier(hal::Result::complete, 1);
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDone2Verifier(hal::Result::complete, 3);
    infra::VerifyingFunctionMock<void(hal::Result, uint32_t)> sendDataDone3Verifier(hal::Result::complete, 1);
    EXPECT_CALL(masterMock, WriteRegisterMock(3, std::vector<uint8_t>{ 1, 2, 3, 4 }));

    uint8_t dataRegister = 3;
    std::array<uint8_t, 3> data1 = { 1, 2, 3 };
    std::array<uint8_t, 1> data2 = { 4 };
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(dataRegister), hal::Action::continueSession, 100000, sendDataDone1Verifier);
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(data1), hal::Action::continueSession, 100000, sendDataDone2Verifier);
    master.SendData(hal::I2cAddress(1), infra::MakeByteRange(data2), hal::Action::stop, 100000, sendDataDone3Verifier);
}
