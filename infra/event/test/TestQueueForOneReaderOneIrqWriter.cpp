#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "infra/event/test_helper/public/EventDispatcherFixture.hpp"
#include "infra/event/public/QueueForOneReaderOneIrqWriter.hpp"
#include "infra/util/test_helper/public/MockCallback.hpp"

class QueueForOneReaderOneIrqWriterTest
    : public testing::Test
    , public infra::EventDispatcherFixture
{
public:
    QueueForOneReaderOneIrqWriterTest()
        : queue(buffer, [this]() { queue.Get(); callback.callback(); })
    {}

    infra::MockCallback<void()> callback;
    std::array<uint8_t, 2> buffer;
    infra::QueueForOneReaderOneIrqWriter queue;
};

TEST_F(QueueForOneReaderOneIrqWriterTest, add_element)
{
    queue.AddFromInterrupt(0);
    EXPECT_CALL(callback, callback());
    ExecuteAllActions();

    queue.AddFromInterrupt(1);
    EXPECT_CALL(callback, callback());
    ExecuteAllActions();
}
