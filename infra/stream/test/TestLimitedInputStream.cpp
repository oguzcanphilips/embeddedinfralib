#include "gmock/gmock.h"
#include "infra/stream/LimitedInputStream.hpp"
#include "infra/stream/test/StreamReaderMock.hpp"

TEST(LimitedInputStreamTest, Extract)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 2);

    std::array<uint8_t, 4> data;
    EXPECT_CALL(reader, Extract(infra::Head(infra::MakeRange(data), 2)));
    limitedReader.Extract(data);
    EXPECT_TRUE(reader.Failed());
}

TEST(LimitedInputStreamTest, ExtractOne)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 1);

    EXPECT_CALL(reader, ExtractOne()).WillOnce(testing::Return(4));
    EXPECT_EQ(4, limitedReader.ExtractOne());
    EXPECT_FALSE(reader.Failed());

    EXPECT_EQ(0, limitedReader.ExtractOne());
    EXPECT_TRUE(reader.Failed());
}

TEST(LimitedInputStreamTest, Peek_with_data_available)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 1);

    EXPECT_CALL(reader, Peek()).WillOnce(testing::Return(4));
    EXPECT_EQ(4, limitedReader.Peek());
    EXPECT_FALSE(reader.Failed());
}

TEST(LimitedInputStreamTest, Peek_with_no_data_available)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 0);

    EXPECT_EQ(0, limitedReader.Peek());
    EXPECT_TRUE(reader.Failed());
}

TEST(LimitedInputStreamTest, ExtractContiguousRange)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 2);

    std::array<uint8_t, 2> data;
    EXPECT_CALL(reader, ExtractContiguousRange(2)).WillOnce(testing::Return(infra::MakeRange(data)));
    EXPECT_EQ(infra::MakeRange(data), limitedReader.ExtractContiguousRange(4));
    EXPECT_FALSE(reader.Failed());

    EXPECT_TRUE(limitedReader.Empty());
}

TEST(LimitedInputStreamTest, Available)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 2);

    EXPECT_CALL(reader, Available()).WillOnce(testing::Return(4));
    EXPECT_EQ(2, limitedReader.Available());

    EXPECT_CALL(reader, Available()).WillOnce(testing::Return(1));
    EXPECT_EQ(1, limitedReader.Available());
}

TEST(LimitedInputStreamTest, Empty)
{
    testing::StrictMock<StreamReaderMock> reader(infra::softFail);
    infra::LimitedStreamReader limitedReader(reader, 2);

    EXPECT_CALL(reader, Empty()).WillOnce(testing::Return(false));
    EXPECT_FALSE(limitedReader.Empty());

    EXPECT_CALL(reader, Empty()).WillOnce(testing::Return(true));
    EXPECT_TRUE(limitedReader.Empty());
}
