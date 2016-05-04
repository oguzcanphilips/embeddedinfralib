#include <array>
#include "PacketCommunicationBufferedReceive.hpp"
#include "gtest/gtest.h"

class CyclicBufferFixture
    : public testing::Test
{
public:
    CyclicBufferFixture()
        : buffer(array.data(), array.data() + array.size())
    {}

    std::array<uint8_t, 3> array;
    erpc::PacketCommunicationBufferedReceive::CyclicBuffer buffer;
};

TEST_F(CyclicBufferFixture, Creation)
{
    EXPECT_TRUE(buffer.Empty());
    EXPECT_FALSE(buffer.Full());
    EXPECT_EQ(0, buffer.Size());
}

TEST_F(CyclicBufferFixture, PushBack)
{
    buffer.PushBack(5);

    EXPECT_FALSE(buffer.Empty());
    EXPECT_FALSE(buffer.Full());
    EXPECT_EQ(1, buffer.Size());
}

TEST_F(CyclicBufferFixture, PopBack)
{
    buffer.PushBack(5);

    EXPECT_EQ(5, buffer.PopBack());
    EXPECT_TRUE(buffer.Empty());
    EXPECT_FALSE(buffer.Full());
    EXPECT_EQ(0, buffer.Size());
}

TEST_F(CyclicBufferFixture, DoublePushBack)
{
    buffer.PushBack(6);
    buffer.PushBack(7);

    EXPECT_FALSE(buffer.Empty());
    EXPECT_TRUE(buffer.Full());

    EXPECT_EQ(7, buffer.PopBack());
    EXPECT_EQ(6, buffer.PopBack());
}

TEST_F(CyclicBufferFixture, FrontAndBack)
{
    buffer.PushBack(6);
    buffer.PushBack(7);

    EXPECT_EQ(6, buffer.Front());
    EXPECT_EQ(7, buffer.Back());
}
