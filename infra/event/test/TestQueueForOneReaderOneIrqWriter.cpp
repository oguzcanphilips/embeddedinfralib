#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/event/test_helper/EventDispatcherFixture.hpp"
#include "infra/event/QueueForOneReaderOneIrqWriter.hpp"
#include "infra/util/test_helper/MockCallback.hpp"

class QueueForOneReaderOneIrqWriterTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    infra::MockCallback<void()> callback;
    std::array<uint8_t, 5> buffer;
    infra::Optional < infra::QueueForOneReaderOneIrqWriter> queue;
};

TEST_F(QueueForOneReaderOneIrqWriterTest, add_element)
{
    queue.Emplace(buffer, [this]() { queue->Get(); callback.callback(); });

    queue->AddFromInterrupt(0);
    EXPECT_CALL(callback, callback());
    ExecuteAllActions();

    queue->AddFromInterrupt(1);
    EXPECT_CALL(callback, callback());
    ExecuteAllActions();
}

TEST_F(QueueForOneReaderOneIrqWriterTest, add_range)
{
    queue.Emplace(buffer, [this]() { while (!queue->Empty()) queue->Get(); callback.callback(); });

    std::array<uint8_t, 2> data = {{ 0, 1 }};
    queue->AddFromInterrupt(data);
    EXPECT_CALL(callback, callback());
    ExecuteAllActions();
}

TEST_F(QueueForOneReaderOneIrqWriterTest, consume_1_before_get)
{
    queue.Emplace(buffer, [this]() { });
        
    queue->AddFromInterrupt(0);
    queue->AddFromInterrupt(1);

    queue->Consume(1);
    EXPECT_EQ(1, queue->Get());
}

TEST_F(QueueForOneReaderOneIrqWriterTest, get_ContiguousRange)
{
    queue.Emplace(buffer, [this]() { });

    EXPECT_TRUE(queue->ContiguousRange().empty());

    std::array<uint8_t, 2> data = {{ 0, 1 }};
    queue->AddFromInterrupt(data);
    
    infra::ConstByteRange range = queue->ContiguousRange();
    EXPECT_EQ((std::vector<uint8_t>(data.begin(), data.end())), (std::vector<uint8_t>{ range.begin(), range.end() }));
}

TEST_F(QueueForOneReaderOneIrqWriterTest, get_ContiguousRange_while_queue_is_wrapped)
{
    queue.Emplace(buffer, [this]() { while (!queue->Empty()) queue->Get(); callback.callback(); });

    std::array<uint8_t, 3> data = { { 0, 1, 2 } };
    queue->AddFromInterrupt(data);
    queue->Consume(3);
    queue->AddFromInterrupt(data);

    infra::ConstByteRange range = queue->ContiguousRange();
    EXPECT_EQ((std::vector<uint8_t>(data.begin(), data.begin() + 2)), (std::vector<uint8_t>{ range.begin(), range.end() }));
}
