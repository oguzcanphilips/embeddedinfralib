#include "hex_compiler_application/HexOutput.hpp"
#include "gtest/gtest.h"

TEST(HexOutputTest, EmptyContents)
{
    application::SparseVector<uint8_t> memory;
    EXPECT_EQ(std::vector<std::string>{":00000001ff"}, application::HexOutput(memory));
}

TEST(HexOutputTest, OneByte)
{
    application::SparseVector<uint8_t> memory;
    memory.Insert(1, 0);
    EXPECT_EQ((std::vector<std::string>{":0100000001fe", ":00000001ff"}), application::HexOutput(memory));
}

TEST(HexOutputTest, TwoBytes)
{
    application::SparseVector<uint8_t> memory;
    memory.Insert(1, 0);
    memory.Insert(2, 1);
    EXPECT_EQ((std::vector<std::string>{":020000000102fb", ":00000001ff"}), application::HexOutput(memory));
}

TEST(HexOutputTest, OneByteAtNonZeroAddress)
{
    application::SparseVector<uint8_t> memory;
    memory.Insert(1, 1);
    EXPECT_EQ((std::vector<std::string>{":0100010001fd", ":00000001ff"}), application::HexOutput(memory));
}

TEST(HexOutputTest, MultipleLines)
{
    application::SparseVector<uint8_t> memory;
    memory.Insert(1, 0);
    memory.Insert(2, 2);
    EXPECT_EQ((std::vector<std::string>{":0100000001fe", ":0100020002fb", ":00000001ff"}), application::HexOutput(memory));
}

TEST(HexOutputTest, LimitLength)
{
    application::SparseVector<uint8_t> memory;
    memory.Insert(1, 0);
    memory.Insert(2, 1);
    memory.Insert(3, 2);
    EXPECT_EQ((std::vector<std::string>{":020000000102fb", ":0100020003fa", ":00000001ff"}), application::HexOutput(memory, 2));
}

TEST(HexOutputTest, HighAddress)
{
    application::SparseVector<uint8_t> memory;
    memory.Insert(1, 0x10000);
    EXPECT_EQ((std::vector<std::string>{":020000040001f9", ":0100000001fe", ":00000001ff"}), application::HexOutput(memory));
}
