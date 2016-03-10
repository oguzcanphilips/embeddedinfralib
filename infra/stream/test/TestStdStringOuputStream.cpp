#include "gtest/gtest.h"
#include "infra/stream/public/StdStringOutputStream.hpp"
#include <cstdint>

TEST(StdStringOuputStreamTest, StdStringOutputStream)
{
    infra::StdStringOutputStream::WithStorage stream;

    stream << "abcd";

    EXPECT_EQ("abcd", stream.Storage());
}
